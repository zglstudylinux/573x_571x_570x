# AGENTS.md — AB5712F 无线麦 SDK 工程

中科蓝讯（Bluetrum）AB5712F 无线麦克风 SDK（v015 s8952），RISC-V（RV32）裸机固件，用 Code::Blocks 工程组织（`app.cbp`），本仓库已配置脱离 GUI 的命令行构建。

## 构建命令

```powershell
# 在 app/projects/microphone 下执行（或用 VSCode 任务 build / rebuild / clean）
powershell -ExecutionPolicy Bypass -File app/projects/microphone/build.ps1
./build.ps1 -Rebuild     # 清理后构建
./build.ps1 -Clean
```

- `build.ps1` 解析 `app.cbp`（编译选项/头文件路径/链接库/前后置命令的**单一来源**），产物输出到 `app/projects/microphone/Output/bin/`（app.rv32 → app.bin），与 GUI 构建完全一致，可混用。
- 工具链自动探测顺序：环境变量 `RV32_TOOLCHAIN_ROOT` → Code::Blocks 配置 `%APPDATA%\codeblocks\default.conf` → 兜底路径。可用 `-ToolchainRoot` 指定（含 `bin\riscv32-elf-*.exe`）。
- **`build.ps1` 必须保留 UTF-8 BOM**（PS 5.1 无 BOM 中文注释乱码）。改脚本时只调整标注 `>>> 按工程调整` 的位置，其余逻辑勿动。
- 无 lint / 单元测试；验证方式是构建通过（固件项目无自动化测试）。

## 产品配置切换（重要）

`app/projects/microphone/config.h` 中 `USER_CONFIG` 选择方案（0–6），再 include 对应 `config_*.h`：

- 当前激活：`CONFIG_CLIP_LE_MIC`（领夹麦）→ `config_clip_mic.h`
- 其余：wireless_mic / broadcast / low_latency / stereo_mic / headset / interphone

改配置去对应的 `config_*.h`，不要动 `config.h` 的选择逻辑（除非明确要求切换方案）。`FLASH_CODE_SIZE`（当前 450K）限制程序空间，注意 1MB 内置 flash 减去 params/res2 的余量。

## 目录结构

- `app/projects/microphone/` — 产品工程（app.cbp、config_*.h、main.c、effect.c、message/、plugin/、port/、build.ps1、Output/ 产物）
- `app/` — SDK 各层：`bsp/`（外设驱动）、`modules/`、`functions/`、`os/`、`system/`、`include/`、`libs/`（闭源库）、`3rd-party/`
- `docs/` — `SDK/`、`datasheet/`（AB5712F 数据手册、用户手册 PDF）、`sch/`（原理图）、`peripheral/`（OTA 等）

## 约定与注意事项

- 中文注释、GBK 兼容环境常见；新文件遵循周边代码风格，注释密度与现有一致。
- 配置大量使用 `#define FUNC_XXX_EN 0/1` 宏开关，改动前先确认宏的取值来源（有的来自 `xcfg_cb.*` 运行时配置）。
- `Output/obj/` 及最终产物已被 .gitignore，但 `Output/bin/Settings/*.setting` 等配置文件是入库的，会被工具改写——提交前检查 diff 是否为预期改动。
- 硬件相关改动前先查 `docs/datasheet/` 的数据手册与 `docs/sch/` 原理图；UART 打印口由 `UART0_PRINTF_SEL` 决定（当前 PRINTF_PB3）。
- 中科蓝讯 SDK 构建问题可参考 skill：`bluetrum-sdk-build`。
