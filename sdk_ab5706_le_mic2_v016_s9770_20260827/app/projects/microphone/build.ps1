<#
.SYNOPSIS
    中科蓝讯 SDK 命令行构建脚本（脱离 Code::Blocks GUI）。
    工程：AB5706 LE MIC（v016 s9770），产品目录 projects\microphone。

.DESCRIPTION
    解析本目录 app.cbp，复现 Code::Blocks 的 Debug 构建流程：
    ExtraCommands(before) -> 预处理 ram.ld/app.xm -> 编译所有 .c -> 链接 app.rv32 -> ExtraCommands(after)。
    编译选项、头文件路径、链接库、前后置命令均直接取自 app.cbp（单一来源），
    构建产物路径与 Code::Blocks 完全一致，可与 GUI 构建混用。

    本文件含中文注释，必须保留 UTF-8 BOM（PS 5.1 无 BOM 会乱码）。
    改脚本时只调整标注 ">>> 按工程调整" 的位置，其余逻辑勿动。

.PARAMETER ToolchainRoot
    RV32 工具链根目录（含 bin\ 下 riscv32-elf-*.exe）。不指定则按优先级自动探测：
    1. 环境变量 RV32_TOOLCHAIN_ROOT
    2. Code::Blocks 全局配置 %APPDATA%\codeblocks\default.conf 中 master_path
       （本工程 app.cbp 的编译器 ID 为 riscv32-v3；default.conf 的注册块名是
        riscv32_v3（下划线），脚本会把 ID 归一化后查同名块，再以 riscv32_v3 兜底）
    3. 本机历史安装路径（按需修改，须与 cbp 编译器 ID 配套的版本）
    4. 旧默认 C:\Program Files (x86)\RV32-Toolchain\RV32-V3

.PARAMETER Clean
    清理中间与产物。仅清理，不构建。

.PARAMETER Rebuild
    先清理再构建。

.EXAMPLE
    ./build.ps1
    ./build.ps1 -Clean
    ./build.ps1 -Rebuild
    ./build.ps1 -ToolchainRoot D:\...\RV32-V3
#>
[CmdletBinding()]
param(
    [string]$ToolchainRoot,
    [switch]$Clean,
    [switch]$Rebuild
)

$ErrorActionPreference = 'Stop'

# ----------------------------------------------------------------------
# 定位脚本所在目录（projects\<产品>）并切换为工作目录，
# 使相对路径基准与 app.cbp 一致（Code::Blocks 构建时 CWD 即工程目录）。
# ----------------------------------------------------------------------
$ProjDir = $PSScriptRoot
Set-Location $ProjDir
$CbpFile  = Join-Path $ProjDir 'app.cbp'
$ObjDir   = Join-Path $ProjDir 'Output\obj'
$BinDir   = Join-Path $ProjDir 'Output\bin'
$AppRv32  = Join-Path $BinDir 'app.rv32'

function Write-Stage([string]$msg) { Write-Host "`n==== $msg ====" -ForegroundColor Cyan }
function Write-OK([string]$msg)    { Write-Host "  [OK] $msg" -ForegroundColor Green }
function Fail([string]$msg) {
    Write-Host "  [FAIL] $msg" -ForegroundColor Red
    exit 1
}

# 原生命令统一经 Invoke-Native 执行。bat/编译器向 stderr 写提示文本（如 copy_tone.bat 的
# 资源清理提示、gcc 警告）时，若本进程输出被重定向（*> 、2>&1 或 VSCode 任务管道），
# PowerShell 5.1 会把它包装成 NativeCommandError；叠加全局 $ErrorActionPreference='Stop'
# 会把构建掐死在半路（已实际踩坑：-Rebuild 删完 obj 树后夭折，GUI 构建随之报
# "can't create *.o"）。此处临时降级 EAP，成败一律由各调用点既有的 $LASTEXITCODE 判定。
function Invoke-Native {
    $ErrorActionPreference = 'Continue'
    $exe = $args[0]
    $rest = @()
    if ($args.Count -gt 1) {
        foreach ($a in @($args | Select-Object -Skip 1)) {
            # 数组参数摊平一层（CFLAGS/INCLUDES/obj 列表/库列表），保持与原 & 调用相同的实参形态
            if ($a -is [System.Collections.IEnumerable] -and $a -isnot [string] -and $a -isnot [System.Char]) {
                $rest += @($a)
            } else {
                $rest += $a
            }
        }
    }
    & $exe @rest
}

# ----------------------------------------------------------------------
# 工具链探测
# ----------------------------------------------------------------------
function Find-ToolchainRoot([string]$Override) {
    if ($Override) { return $Override }

    # 1. 环境变量
    if ($env:RV32_TOOLCHAIN_ROOT -and (Test-Path (Join-Path $env:RV32_TOOLCHAIN_ROOT 'bin\riscv32-elf-gcc.exe'))) {
        return $env:RV32_TOOLCHAIN_ROOT
    }

    # 2. Code::Blocks 全局配置：按 app.cbp 的编译器 ID 查同名注册块的 MASTER_PATH
    #    一个 ID 绑定一套工具链（riscv32 -> RV32-V1，riscv32_v2 -> RV32-V2，
    #    riscv32_v3 -> RV32-V3），选错不报错但产物与 GUI 不一致，详见 SKILL.md
    #    "工具链关键差异"。
    #    注意本族 cbp 写的 ID 带连字符（riscv32-v3），default.conf 的块名用下划线
    #    （riscv32_v3），必须先归一化，否则查不到块、退到本机路径兜底。
    #    块名字面量自带后缀保护：<riscv32> 不会误匹配 <riscv32_v2>/<riscv32_v3>，
    #    闭合标签非贪婪匹配停在本块自己的 </ID>。
    $cbConf = Join-Path $env:APPDATA 'codeblocks\default.conf'
    if (Test-Path $cbConf) {
        $conf = Get-Content $cbConf -Raw
        $blkNames = @('riscv32_v3')
        try {
            if (Test-Path $CbpFile) {
                [xml]$cbpXml = Get-Content $CbpFile
                $cbpCompiler = @($cbpXml.CodeBlocks_project_file.Project.Option |
                    Where-Object { $_.compiler } | Select-Object -First 1).compiler
                if ($cbpCompiler) {
                    foreach ($cand in @($cbpCompiler, ($cbpCompiler -replace '-', '_'))) {
                        if ($blkNames -notcontains $cand) { $blkNames = @($cand) + $blkNames }
                    }
                }
            }
        } catch { }
        foreach ($blkName in $blkNames) {
            if ($conf -match ('(?s)<' + $blkName + '>(.*?)</' + $blkName + '>')) {
                $block = $Matches[1]
                if ($block -match '<MASTER_PATH>\s*<str>\s*<!\[CDATA\[(.*?)\]\]>\s*</str>') {
                    $mp = $Matches[1].Trim()
                    if ($mp -and (Test-Path (Join-Path $mp 'bin\riscv32-elf-gcc.exe'))) {
                        return $mp
                    }
                }
            }
        }
    }

    # 3. 本机历史安装路径  >>> 按工程调整：换成当前机器的实际安装路径
    #    （多版本并存时必须是与 cbp 编译器 ID 配套的那一套）
    $legacy = 'D:\software_download\work\rv32-toolchain_v1.5.9\rv32-toolchain_v1.5.9\install\RV32-V3'
    if (Test-Path (Join-Path $legacy 'bin\riscv32-elf-gcc.exe')) { return $legacy }

    # 4. 旧默认路径
    $old = 'C:\Program Files (x86)\RV32-Toolchain\RV32-V3'
    if (Test-Path (Join-Path $old 'bin\riscv32-elf-gcc.exe')) { return $old }

    return $null
}

$ToolchainRoot = Find-ToolchainRoot $ToolchainRoot
if (-not $ToolchainRoot -or -not (Test-Path (Join-Path $ToolchainRoot 'bin\riscv32-elf-gcc.exe'))) {
    Write-Host "未找到 RV32 工具链。请用 -ToolchainRoot 指定，或设置环境变量 RV32_TOOLCHAIN_ROOT。" -ForegroundColor Red
    Write-Host "示例: ./build.ps1 -ToolchainRoot D:\...\RV32-V3" -ForegroundColor Yellow
    exit 1
}
$BinToolchain = Join-Path $ToolchainRoot 'bin'
$Gcc     = Join-Path $BinToolchain 'riscv32-elf-gcc.exe'
$Ld      = Join-Path $BinToolchain 'riscv32-elf-ld.exe'
$Objcopy = Join-Path $BinToolchain 'riscv32-elf-objcopy.exe'
$Xmaker  = Join-Path $BinToolchain 'riscv32-elf-xmaker.exe'

# 把工具链 bin 前置进 PATH，使 prebuild.bat / postbuild.bat / copy_tone.bat 等的裸名工具调用可解析
$env:Path = $BinToolchain + [System.IO.Path]::PathSeparator + $env:Path
Write-Host "工具链: $ToolchainRoot" -ForegroundColor DarkGray
Write-Host "工程目录: $ProjDir" -ForegroundColor DarkGray

# 仓库根（projects\<产品> 的上两级，即 SDK 根），.o 相对它存放，与 Code::Blocks 布局一致
$AppRoot = (Resolve-Path (Join-Path $ProjDir '..\..')).Path

# ----------------------------------------------------------------------
# obj 目录骨架：本脚本与 Code::Blocks GUI 共用 Output\obj / Output\bin，
# 产物逐字节一致，先后交错构建互不影响。唯一风险是目录被清后未及重建——
# GUI 并行编译遇目录缺失会报 "can't create *.o: No such file or directory"。
# 因此 Clean 后与每次构建伊始都按 cbp 词法预建完整目录骨架（不要求源文件已
# 存在：effect.c 等可能由 prebuild 生成），脚本无论在哪一步被打断，
# GUI 都能立即正常构建。
# ----------------------------------------------------------------------
function New-ObjSkeleton {
    try {
        if (-not (Test-Path $CbpFile)) { return }
        [xml]$xml = Get-Content $CbpFile
        foreach ($u in @($xml.CodeBlocks_project_file.Project.Unit)) {
            $isCC = $false
            foreach ($opt in @($u.Option)) { if ($opt.compilerVar -eq 'CC') { $isCC = $true; break } }
            if (-not $isCC -or -not $u.filename) { continue }
            $srcAbs = [System.IO.Path]::GetFullPath((Join-Path $ProjDir $u.filename))
            $objRel = $srcAbs.Substring($AppRoot.Length + 1) -replace '\.c$', '.o'
            New-Item -ItemType Directory -Force -Path (Split-Path (Join-Path $ObjDir $objRel) -Parent) | Out-Null
        }
        # 厂商 postbuild.bat 的 echo 1 占位目录（bat 内 %cd% 经 cd %~dp0 + cd ..\.. 落在产品目录），
        # 不预建则从零构建时 cmd 会刷"系统找不到指定的路径"（无 || goto err，不影响产物）
        $postbat = Join-Path $BinDir 'postbuild.bat'
        if (Test-Path $postbat) {
            $prodLeaf = Split-Path $ProjDir -Leaf
            foreach ($m in [regex]::Matches((Get-Content $postbat -Raw), '(?i)echo 1 >\s*"?%cd%\\Output\\obj\\([^"\r\n]+)"?')) {
                $objRel = $m.Groups[1].Value.Replace('%%~nxa', $prodLeaf)
                New-Item -ItemType Directory -Force -Path (Split-Path (Join-Path $ObjDir $objRel) -Parent) | Out-Null
            }
        }
    } catch {
        Write-Host "  [WARN] obj 目录骨架预建失败（不影响本次构建）: $_" -ForegroundColor Yellow
    }
}

# ----------------------------------------------------------------------
# 清理
# ----------------------------------------------------------------------
function Invoke-Clean {
    Write-Stage 'Clean'
    if (Test-Path $ObjDir) {
        Remove-Item $ObjDir -Recurse -Force
        Write-OK "已删除 Output\obj"
    } else {
        Write-Host '  Output\obj 不存在，跳过'
    }
    # 按本工程 postbuild.bat 实际产物清理（objcopy -> app.bin，xmaker -> app.dcf，
    # ld -Map -> map.txt；objdump 缺失时不产 app.lst）
    $binCleanups = @('app.rv32','app.bin','app.dcf','app.lst','appxm.o','map.txt')
    foreach ($f in $binCleanups) {
        $p = Join-Path $BinDir $f
        if (Test-Path $p) { Remove-Item $p -Force; Write-OK "已删除 $f" }
    }
    Write-Host '保留: res/xcfg/copy_tone 等资源与生成配置' -ForegroundColor DarkGray
    New-ObjSkeleton
    Write-OK '已重建 Output\obj 目录骨架（随后 GUI 构建可立即使用）'
}

if ($Clean) { Invoke-Clean; exit 0 }
if ($Rebuild) { Invoke-Clean }

# 确保输出目录存在
New-Item -ItemType Directory -Force -Path $ObjDir | Out-Null
New-Item -ItemType Directory -Force -Path $BinDir | Out-Null
New-ObjSkeleton

# ----------------------------------------------------------------------
# 解析 app.cbp：编译选项 / include / 链接库 / ExtraCommands / 源文件
# （与 Code::Blocks 保持单一来源，工程改动后脚本无需同步修改）
# ----------------------------------------------------------------------
Write-Stage '解析 app.cbp'
if (-not (Test-Path $CbpFile)) { Fail "找不到工程文件 $CbpFile" }
[xml]$cbp = Get-Content $CbpFile
$project = $cbp.CodeBlocks_project_file.Project
if (-not $project) { Fail 'app.cbp 格式异常: 未找到 Project 节点' }

$ProjectName = @($project.Option | Where-Object { $_.title } | Select-Object -First 1).title
if (-not $ProjectName) { $ProjectName = 'app' }

$CFLAGS   = @($project.Compiler.Add | ForEach-Object { $_.option }    | Where-Object { $_ })
$INCDIRS  = @($project.Compiler.Add | ForEach-Object { $_.directory } | Where-Object { $_ })
$INCLUDES = @($INCDIRS | ForEach-Object { '-I' + $_ })
$LDOPTS   = @($project.Linker.Add   | ForEach-Object { $_.option }    | Where-Object { $_ })
$LSEARCH  = @($project.Linker.Add   | ForEach-Object { $_.directory } | Where-Object { $_ })
$LIBNAMES = @($project.Linker.Add   | ForEach-Object { $_.library }   | Where-Object { $_ })
$BeforeCmds = @($project.ExtraCommands.Add | ForEach-Object { $_.before } | Where-Object { $_ })
$AfterCmds  = @($project.ExtraCommands.Add | ForEach-Object { $_.after }  | Where-Object { $_ })

# 链接库解析：<Add library="xxx"/> 按 -L 搜索目录展开为完整 .a 路径；带路径的用原值
# 库名可能带或不带 lib 前缀（cbp 里常见写作 libplatform），两种候选都尝试
$LIBS = New-Object System.Collections.Generic.List[string]
foreach ($lib in $LIBNAMES) {
    if ($lib -match '\.a$') {
        $p = [System.IO.Path]::GetFullPath((Join-Path $ProjDir $lib))
        if (-not (Test-Path $p)) { Fail "链接库不存在: $lib" }
        $LIBS.Add($lib)
    } else {
        $candidates = @((($lib -replace '^lib', '') + '.a'), ($lib + '.a'))
        $found = $null
        foreach ($d in $LSEARCH) {
            foreach ($name in $candidates) {
                $cand = [System.IO.Path]::GetFullPath((Join-Path (Join-Path $ProjDir $d) $name))
                if (Test-Path $cand) { $found = $cand; break }
            }
            if ($found) { break }
        }
        if (-not $found) { Fail "无法解析链接库 -l$lib（在 $($LSEARCH -join ', ') 中均未找到）" }
        # 转成相对工程目录的路径（库在 SDK 根下，即 ..\..\ 前缀），使 map.txt 内路径表现形式与 Code::Blocks 一致
        $LIBS.Add((Join-Path '..\..' $found.Substring($AppRoot.Length + 1)))
    }
}
Write-OK "编译选项 $($CFLAGS.Count) 项 / include $($INCDIRS.Count) 项 / 链接库 $($LIBS.Count) 个"

# Code::Blocks 宏展开
function Expand-Macros([string]$cmd) {
    $cmd = $cmd.Replace('$(PROJECT_DIR)',       ($ProjDir + '\'))
    $cmd = $cmd.Replace('$(PROJECT_NAME)',      $ProjectName)
    $cmd = $cmd.Replace('$(TARGET_OUTPUT_DIR)', 'Output\bin\')
    $cmd = $cmd.Replace('$(TARGET_OBJECT_DIR)', 'Output\obj\')
    $cmd = $cmd.Replace('$compiler', $Gcc)
    $cmd = $cmd.Replace('$options', ($CFLAGS -join ' '))
    $cmd = $cmd.Replace('$includes', ($INCLUDES -join ' '))
    return $cmd
}

# 命令行分词：按空白切分，保留引号内的整体
function Split-CmdLine([string]$s) {
    [regex]::Matches($s, '"[^"]*"|[^\s]+') | ForEach-Object { $_.Value.Trim('"') }
}

# 执行一条 ExtraCommands 命令：$compiler 开头的走 gcc 分词调用，其余交 cmd /c
function Invoke-CbCommand([string]$raw) {
    $expanded = Expand-Macros $raw
    if ($raw -match '^\s*\$compiler\b') {
        $tokens = Split-CmdLine $expanded
        Invoke-Native $Gcc $tokens[1..($tokens.Count - 1)]
        if ($LASTEXITCODE -ne 0) { Fail "命令失败: $expanded (exit $LASTEXITCODE)" }
    } else {
        Invoke-Native cmd /c "`"$expanded`""
        if ($LASTEXITCODE -ne 0) { Fail "命令失败: $expanded (exit $LASTEXITCODE)" }
    }
}

# ----------------------------------------------------------------------
# 1) ExtraCommands before：照 cbp 原样逐条执行（含 copy_tone.xm 预处理与 prebuild）。
#    注意顺序敏感：copy_tone.xm -> copy_tone.bat 生成后才执行，prebuild 里 xmaker
#    消费 res.xm / xcfg.xm。before 必须先于源文件清单解析（源文件可能由 prebuild 生成）。
# ----------------------------------------------------------------------
Write-Stage 'ExtraCommands (before)'
foreach ($cmd in $BeforeCmds) { Invoke-CbCommand $cmd }
Write-OK 'before 命令执行完成（含 prebuild）'

# ----------------------------------------------------------------------
# 2) 源文件清单：仅取 compilerVar="CC" 的 Unit
#    （ram.ld/app.xm/download.xm 等为特殊 Unit，单独处理或不处理）
# ----------------------------------------------------------------------
Write-Stage '解析源文件清单'
$units = $project.Unit
if (-not $units) { Fail 'app.cbp 未包含任何 Unit' }

$Sources = New-Object System.Collections.Generic.List[object]
foreach ($u in $units) {
    if (-not $u.filename) { continue }
    $ccOpt = $null
    foreach ($opt in @($u.Option)) {
        if ($opt.compilerVar -eq 'CC') { $ccOpt = $opt; break }
    }
    if (-not $ccOpt) { continue }
    $rel = $u.filename
    # 词法归一化（不要求文件已存在），再校验存在
    $srcAbs = [System.IO.Path]::GetFullPath((Join-Path $ProjDir $rel))
    if (-not (Test-Path $srcAbs)) { Fail "源文件不存在: $rel (-> $srcAbs)" }
    # .o 相对仓库根（SDK 根）存放，与 Code::Blocks 一致：
    #   ..\..\functions\func.c -> Output\obj\functions\func.o
    #   .\config.c             -> Output\obj\projects\<产品>\config.o
    $relRoot = $srcAbs.Substring($AppRoot.Length + 1)
    $objRel  = $relRoot -replace '\.c$', '.o'
    $objPath = Join-Path $ObjDir $objRel
    # 个别 Unit 自带 buildCommand（如 toolkit_effect.c 追加 -Wno-address-of-packed-member），
    # 必须按 Unit 覆盖，否则与 GUI 的编译参数不一致
    $unitCmd = $null
    if ($ccOpt.buildCommand -and $ccOpt.buildCommand.Trim()) { $unitCmd = $ccOpt.buildCommand }
    $Sources.Add([pscustomobject]@{ Src = $srcAbs; Obj = $objPath; Rel = $relRoot; Cmd = $unitCmd })
}
Write-OK "共 $($Sources.Count) 个源文件待编译"

# ----------------------------------------------------------------------
# 3) 预处理 ram.ld -> Output\obj\ram.o（链接脚本，对应 Unit buildCommand）
# ----------------------------------------------------------------------
Write-Stage '预处理链接脚本 ram.ld -> ram.o'
$ramLd  = Join-Path $ProjDir 'ram.ld'
$ramObj = Join-Path $ObjDir 'ram.o'
if (-not (Test-Path $ramLd)) { Fail "缺少 $ramLd" }
Invoke-Native $Gcc $CFLAGS $INCLUDES '-E' '-P' '-x' 'c' '-c' $ramLd '-o' $ramObj
if ($LASTEXITCODE -ne 0) { Fail "预处理 ram.ld 失败 (exit $LASTEXITCODE)" }
Write-OK 'ram.o 已生成'

# ----------------------------------------------------------------------
# 4) 预处理 app.xm -> Output\bin\appxm.o（postbuild 的 xmaker 输入）
# ----------------------------------------------------------------------
Write-Stage '预处理 app.xm -> appxm.o'
$appXm  = Join-Path $BinDir 'app.xm'
$appxmO = Join-Path $BinDir 'appxm.o'
if (-not (Test-Path $appXm)) { Fail "缺少 $appXm" }
Invoke-Native $Gcc $CFLAGS $INCLUDES '-E' '-P' '-x' 'c' '-c' $appXm '-o' $appxmO
if ($LASTEXITCODE -ne 0) { Fail "预处理 app.xm 失败 (exit $LASTEXITCODE)" }
Write-OK 'appxm.o 已生成'

# ----------------------------------------------------------------------
# 5) 编译所有 .c -> .o（每次全量编译，与 GUI 产物可互覆盖）
# ----------------------------------------------------------------------
Write-Stage "编译 $($Sources.Count) 个源文件"
$idx = 0
foreach ($s in $Sources) {
    $idx++
    $dir = Split-Path $s.Obj -Parent
    if (-not (Test-Path $dir)) { New-Item -ItemType Directory -Force -Path $dir | Out-Null }
    Write-Host ("  [{0,3}/{1}] {2}" -f $idx, $Sources.Count, $s.Rel) -ForegroundColor DarkGray
    if ($s.Cmd) {
        $expanded = $s.Cmd.Replace('$file', $s.Src).Replace('$object', $s.Obj)
        $expanded = Expand-Macros $expanded
        $tokens = Split-CmdLine $expanded
        Invoke-Native $Gcc $tokens[1..($tokens.Count - 1)]
    } else {
        Invoke-Native $Gcc $CFLAGS $INCLUDES '-c' $s.Src '-o' $s.Obj
    }
    if ($LASTEXITCODE -ne 0) { Fail "编译失败: $($s.Rel) (exit $LASTEXITCODE)" }
}
Write-OK '全部源文件编译完成'

# ----------------------------------------------------------------------
# 6) 链接 -> app.rv32
#    链接参数用相对产品目录的路径，与 Code::Blocks 编译 log 一致
# ----------------------------------------------------------------------
Write-Stage '链接 app.rv32'
$objListRel = $Sources | ForEach-Object { Join-Path 'Output\obj' ($_.Rel -replace '\.c$', '.o') }
$ldOptsExp  = $LDOPTS | ForEach-Object { Expand-Macros $_ }
# 参数顺序对齐 Code::Blocks：<.o 列表> <链接选项(-T ram.o --gc-sections -Map=...)> <库>
Invoke-Native $Ld '-o' 'Output\bin\app.rv32' $objListRel $ldOptsExp ($LIBS | ForEach-Object { $_ })
if ($LASTEXITCODE -ne 0) { Fail "链接失败 (exit $LASTEXITCODE)" }
Write-OK ("app.rv32 已生成 ({0:N2} KB)" -f ((Get-Item $AppRv32).Length / 1KB))

# ----------------------------------------------------------------------
# 7) ExtraCommands after：postbuild（生成 app.bin/app.dcf/download 镜像）
# ----------------------------------------------------------------------
Write-Stage 'ExtraCommands (after)'
foreach ($cmd in $AfterCmds) { Invoke-CbCommand $cmd }

Write-Stage '构建成功'
$dcf = Join-Path $BinDir 'app.dcf'
$bin = Join-Path $BinDir 'app.bin'
Write-Host ("  产物: app.rv32 ({0:N2} KB)" -f ((Get-Item $AppRv32).Length / 1KB)) -ForegroundColor Green
if (Test-Path $bin) { Write-Host ("        app.bin ({0:N2} KB)" -f ((Get-Item $bin).Length / 1KB)) -ForegroundColor Green }
if (Test-Path $dcf) { Write-Host ("        app.dcf ({0:N2} KB)" -f ((Get-Item $dcf).Length / 1KB)) -ForegroundColor Green }
Write-Host '  提示: 本机 RV32-V3 无 objdump 且 postbuild 里的绝对路径不存在，app.lst 跳过（非致命）' -ForegroundColor DarkGray
exit 0
