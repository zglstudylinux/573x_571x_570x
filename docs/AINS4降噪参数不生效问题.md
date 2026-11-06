# AINS4 降噪量配置不生效的原因与修复

记录一次实际踩坑：573x TX 端开启 AINS4 后，`AINS4_NOISE_NT` 改成任何值都不起作用，按 NEXT/PREV 调等级也没反应。本文说明原因、排查过程、修复方法和最终改动。

适用工程：`sdk_ab573x_le_mic2_v013_s8047_20260129`（AB573x，TX 端）

---

## 一、现象

在 `config_ab5706a_le_mic.h` 里把降噪量改成任意值：

```c
#define AINS4_NOISE_NT    500      // 改成 1000、12000、30000 都一样
```

编译烧录后，听到的降噪效果**始终不变**。用按键调等级时串口会打印 `AINS4 noise = 700` 之类的数字，但听感毫无变化。

---

## 二、原因

### 2.1 直接原因：函数忽略了入参

`modules/voice/ains4.c` 的 `ains4_mic_param_set()` 是唯一设置降噪量的入口，但它的 `ains4_noise_nt` 形参**在函数体里一次都没有被使用**：

```c
void ains4_mic_param_set(u8 mode ,s16 ains4_noise_nt)
{
    ...
    if(mode==1){
        ains4_cb.denoiseBound = 30000;        // 硬编码
    }
    else{                                     // enc，本工程走这条
        if(ains4_cb.yuan_en==1){
            ains4_cb.denoiseBound = 6000;     // 硬编码
        }
        else{
            ains4_cb.denoiseBound = 6000;     // 硬编码
        }
        ains4_cb.music_lev = 11;
        ...
```

`ains4_noise_nt` 在整个函数体里只出现在签名中，所以传什么都一样，`denoiseBound` 恒为 6000（本工程 `mode=0`）。

### 2.2 函数自己的注释与实现不符

函数头注释是这样写的：

```c
//mode 0：enc模式; mode 1:弱降噪模式；
//若mode=1,ains4_noise_nt 为30000，nr_level为0；
//若mode=0,ains4_noise_nt 可设，再根据ains4_noise_nt来选择nr_level，即轻 中 深三档；
//ains4_noise_nt:范围为1~32768，越小降噪效果越明显(1:-90dB,32768:0dB)
```

注释声称 `mode=0` 时会「根据 `ains4_noise_nt` 选择 nr_level（轻中深三档）」，但**这段选档逻辑在代码里根本不存在**。`grep nr_level` 只能搜到这两行注释本身。

这是厂商代码与自身文档不一致的缺陷，不是我们的用法问题——按注释传参是完全合理的。

### 2.3 配置宏的注释也有误

`config_ab5706a_le_mic.h`：

```c
#define AINS4_NOISE_NT    500    //0~32768   最好每500一个step，值越大底噪越大，值越小底噪越小
```

这里的 `0~32768` 下限是错的。按 `20*log10(nt/32768)` 计算，`nt=0` 会得到 `log10(0)`，数学上是负无穷，算法无法处理。函数注释写的 `1~32768` 才是正确范围。

### 2.4 排查方法（可复用）

判断某个算法的「参数是否真的生效」，用这两招最可靠：

**第一招，看形参在函数体里有没有被引用。**

```bash
# 找到函数体，检查形参名出现次数
awk '/^void ains4_mic_param_set/,/^}/' modules/voice/ains4.c | grep -c 'ains4_noise_nt'
# 输出 1 说明只在签名出现，完全没用；输出 >1 才可能生效
```

**第二招，比较改动前后产物的代码大小。**

```bash
NM=<toolchain>/bin/riscv32-elf-nm.exe
"$NM" -S Output/bin/app.rv32 | grep ains4_mic_param_set
# 改动前 000001be（446 字节）
# 改动后 000001c2（450 字节）——多了 4 字节范围判断，说明代码真的变了
```

如果改了参数宏但代码大小和 md5 完全不变，说明这个参数没进代码路径。

> 同类问题在 571x 的 `ylcrn_L3_mic_param_set()`、`ylcrn_L2_mic_param_set()` 上同样存在（都忽略入参），见 `SDK-差异说明.md` 第 4.8 节。571x 按需求未做修改，保持厂商原版。

---

## 三、修复方法

只改一处：`modules/voice/ains4.c`，`ains4_mic_param_set()` 的 `mode==0` 分支。

### 3.1 改动内容

```diff
 	else{                                             //enc
-		if(ains4_cb.yuan_en==1){
-			ains4_cb.denoiseBound		    = 6000;  //
-		}
-		else{
-			ains4_cb.denoiseBound		    = 6000;
-		}
+		//原版此处把 denoiseBound 写成固定 6000，完全忽略入参 ains4_noise_nt，
+		//导致运行中无法调降噪量。改为使用入参，有效范围 1~32767（受 s16 形参所限）。
+		//传 <=0 时回退到原默认值 6000，保持对既有调用者的兼容。
+		s16 noise_nt = ains4_noise_nt;
+		if (noise_nt <= 0) {
+			noise_nt = 6000;                //原硬编码值，作为默认
+		} else if (noise_nt > 32767) {      //形参是 s16，上限 32767（传 32768 会先溢出成 -32768）
+			noise_nt = 32767;
+		}
+		ains4_cb.denoiseBound		    = noise_nt;
 		ains4_cb.music_lev				 = 11;
 		ains4_cb.overdrive_adapt_en     = 0;
 		ains4_cb.gain_ceil				= 32767;
 		ains4_cb.noise_min_en			= 0;
 	}
```

### 3.2 三个设计决定

**为什么可以合并那个 if-else？**

`ains4_cb.yuan_en` 在函数开头第 247 行被硬赋值为 0：

```c
ains4_cb.yuan_en = 0;          // 恒为 0
```

两个分支赋的是同一个值 6000，所以合并成一个赋值不影响行为。

**为什么上限取 32767 而不是 32768？**

这里有个容易搞错的细节：

| 项目 | 类型 | 能存的最大值 |
| --- | --- | --- |
| `ains4_mic_param_set` 的形参 `ains4_noise_nt` | **`s16`** | 32767 |
| `ains4_cb_t.denoiseBound` 字段 | `s32` | 2^31-1 |

字段是 `s32`，本身能存 32768；但**形参是 `s16`**，调用方传 32768 进去时已经溢出成 -32768 了，函数内部再判断也来不及。所以瓶颈在形参，上限取 32767。

（如果确实需要用到 32768 = 0dB 这一档，得改 `ains4.h` 和 `ains4.c` 的签名把形参改成 `s32`，影响面更大，本次没做。）

**为什么加 `<= 0` 回退？**

`ains4_mic_param_set()` 的既有调用者不止一处：

| 调用点 | 传的值 |
| --- | --- |
| `ains4.c:153`（上电初始化） | `AINS4_NOISE_NT` |
| `ains4.c:23`（注释里的示例流程） | `1000` |
| `plugin.c`（NEXT/PREV 调等级） | 运行时变量 |

加回退是为了保证任何人传 0 或负数时仍有确定行为，而不是把 `denoiseBound` 置成非法值。

### 3.3 没改的部分

以下全部保持厂商原值，一个都没动：

- `mode==1` 分支：`denoiseBound = 30000`
- `music_lev`（11 / 6）
- `overdrive_adapt_en`、`overdrive`、`gain_ceil`、`noise_min_en`
- `factor`、`lquantile_sm`、`quan_gap`、`spp_thr`、`ai_vad_hard_thres`、`priorModelPars0` 等全部参数

只把 `denoiseBound` 这一个赋值从常量换成了入参。

---

## 四、配套改动

### 4.1 默认值统一为单一来源

`ains4.c` 修好后，参数会真的生效，于是暴露了一个新的不一致：上电初始化和按键调节分别用了不同的值。

```
上电：ains4_mic_init() → ains4_mic_param_set(0, AINS4_NOISE_NT)
按键：plugin.c 的 ains4_noise_cur（初值另定）
```

如果 `AINS4_NOISE_NT` 是 500 而 plugin 记录 6000，上电后算法实际是 500，第一次按键却从 6000 起算，串口打印的数值与生效值会脱节，测试时容易误判。

**处理**：`config_ab5706a_le_mic.h` 的 `AINS4_NOISE_NT` 改为 6000，`plugin.c` 直接引用它。

`config_ab5706a_le_mic.h`：

```diff
-#define AINS4_NOISE_NT                          500            //0~32768   最好每500一个step，值越大底噪越大，值越小底噪越小
+//降噪量：1~32767（不能为0，0 会让 20*log10(nt/32768) 变成 log10(0)）。值越小降噪越深。
+//6000 是厂商原版硬编码的默认值，取此值可保持与厂商默认听感一致。
+//（原注释写「0~32768」有误，实际下限是 1）
+#define AINS4_NOISE_NT                          6000
```

`plugin/plugin.c`：

```c
//初值直接取 AINS4_NOISE_NT，与 ains4_mic_init() 上电时用的值保持同源，避免打印值与实际生效值脱节
static s16 ains4_noise_cur = AINS4_NOISE_NT;
```

### 4.2 为什么默认取 6000 而不是保留 500

因为**厂商原版实际生效的就是硬编码的 6000**。如果让 500 生效，默认降噪会突然大幅变强（500 对应 -36.3dB，6000 对应 -14.7dB，差 21.6dB），无法与之前的测试听感做对照。

用 6000 作默认，是「本次修复不改变默认行为、只让参数变得可调」的最小改动。

想换默认档位，改 `AINS4_NOISE_NT` 一个宏即可，上电和按键会同时跟着变。

---

## 五、参数含义速查

`ains4_noise_nt` 与降噪深度的换算：`dB = 20*log10(nt / 32768)`

| `ains4_noise_nt` | 降噪量 | 听感参考 |
| --- | --- | --- |
| 1 | -90.3 dB | 最深，人声可能都被压掉 |
| 10 | -70.3 dB | 极深 |
| 100 | -50.3 dB | 很深 |
| 500 | -36.3 dB | 深 |
| 1000 | -30.3 dB | 较深 |
| 2000 | -24.3 dB | 中等 |
| **6000** | **-14.7 dB** | **当前默认（与厂商原版一致）** |
| 12000 | -8.7 dB | 较轻 |
| 24000 | -2.7 dB | 很轻 |
| 32767 | ≈0 dB | 几乎不降噪 |

**取值范围：1 ~ 32767**，值越小降噪越深。不能取 0。

按键调节（`plugin.c`，步进按当前值分段）：

| 当前值区间 | NEXT/PREV 步进 |
| --- | --- |
| < 500 | 100 |
| 500 ~ 2000 | 250 |
| 2000 ~ 6000 | 500 |
| 6000 ~ 12000 | 1000 |
| 12000 ~ 24000 | 2000 |
| > 24000 | 4000 |

从默认 6000 出发：按 PREV 降到 1 需 **19 次**，按 NEXT 升到 32767 需 **15 次**。低段步进小、便于细调，高段步进大、能快速拉开差距。

（若觉得按键次数偏多，可以把低段步进调大，或在 `plugin.c` 里加入长按连续调节。）

---

## 六、验证

### 6.1 代码层面

```bash
cd sdk_ab573x_le_mic2_v013_s8047_20260129/app/projects/microphone
powershell -ExecutionPolicy Bypass -NoProfile -File build.ps1
```

`ains4_mic_param_set` 的代码大小变化（446 → 450 字节）证明新增的范围判断进入了产物：

```bash
NM="D:/software_download/work/rv32-toolchain_v1.5.9/rv32-toolchain_v1.5.9/install/RV32-V3/bin/riscv32-elf-nm.exe"
"$NM" -S Output/bin/app.rv32 | grep ains4_mic_param_set
# 000115f8 000001c2 W ains4_mic_param_set     ← 0x1c2 = 450
```

产物 md5 演变：

| 版本 | app.dcf md5 | 说明 |
| --- | --- | --- |
| 修复前 | `e35dd5ca926dc75949990043637ebc4b` | 参数被忽略，`denoiseBound` 恒 6000 |
| 修复后（`AINS4_NOISE_NT` 仍 500） | `64bc4d557c9aa85032559f16382615bd` | 参数生效，但上电/按键不同源 |
| 最终（统一 6000） | `f015c85f1d3f7e632ebc22583076cff7` | 单一来源 |

代码体积 412 KB（上限 492K）。

### 6.2 实测层面

串口接 **PB3**，每次按键会打印当前生效值：

```
AINS4 ON (noise=6000)      ← 按下 P/P 开降噪
AINS4 noise = 6500         ← 按 NEXT，降噪变浅
AINS4 noise = 6000         ← 按 PREV，降噪变深
AINS4 OFF (noise=6000)     ← 按 P/P 关降噪
```

建议按这几个点对比听感，确认参数真的起作用：

| 操作 | 预期听感 |
| --- | --- |
| 开机默认 6000 | 与修复前的听感一致 |
| PREV 降到 1000 左右 | 降噪明显加深 |
| PREV 一路降到 1 | 最深（-90dB），可能闷到听不清人声 |
| NEXT 升到 24000 以上 | 几乎不降噪，底噪明显 |

如果从 6000 降到 1 全程听不出区别，说明参数仍未生效，回到第二节的排查方法重查。

---

## 七、注意事项

**这是对厂商算法文件的修改。** `modules/voice/ains4.c` 里已加注释说明原版行为和改动原因。后续升级此 SDK 时，需要重新应用这处改动，否则等级调节会再次失效。

升级后可以用这条命令快速确认是否被覆盖回原版：

```bash
grep -n 'ains4_cb.denoiseBound' modules/voice/ains4.c
```

判断依据：

| 状态 | 输出特征 |
| --- | --- |
| 修复仍在 | 有 `= noise_nt;`，且 266/269 位置的 6000 硬编码已消失（共 4 行匹配） |
| 已被覆盖回原版 | 出现两处 `= 6000;` 硬编码（252/255 的 30000 + 266/269 的 6000，共 5 行匹配） |

**`mode=1`（弱降噪模式）未修。** 该分支的 `denoiseBound` 仍为硬编码 30000，因为它本来就不接受可调参数（注释里写明 mode=1 时 `ains4_noise_nt` 固定为 30000）。本工程用的是 `mode=0`（`ains4.c:153` 和 `plugin.c` 都传 0）。
