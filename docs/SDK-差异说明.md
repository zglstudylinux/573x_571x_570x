# 我方 SDK 与客户 SDK 差异说明

本文对比我方三套工程与客户量产工程（`client/` 目录）的实际差异，用于回答「我们改了什么」和「客户把 SDK 改成了什么样」两个问题。

对比基准：

| 角色 | 我方工程 | 客户工程 |
| --- | --- | --- |
| TX（发射端 / 领夹麦） | `sdk_ab573x_le_mic2_v013_s8047_20260129/app` | `client/app_5726A_TX/app` |
| RX（接收端 / 适配器） | `sdk_ab5706_le_mic2_v016_s9770_20260827/app` | `client/app_5706A_RX/app` |
| TX+RX（同一固件兼作两端） | `sdk_ab571x_le_mic2_v015_s8952_20260516/app` | **无对应客户工程**（见第四节） |

前两套的芯片族、编译器 ID（`riscv32-v3`）、`-march`、链接库集合、无线链路参数（`WIRELESS_CON_VERS=6`、48K / 120 samples / frame 25 / retry 3 / tx_interval 6）完全一致，可直接对比。

第三套（571x）是不同硅族，编译器 ID 为 `riscv32`（RV32-V1，gcc 6.1.0）、`-march=rv32imacxbs1`，与前两套不可直接对比，单独在第四节说明。

---

## 变更记录

| 阶段 | 变更内容 |
| --- | --- |
| 第一阶段 | TX 开启 YLCRN_L1_32K；RX 开启 USB MIC；三键改为降噪控制；USB 打印口调整 |
| 第二阶段 | 排查 USB 不枚举，确认硬件 D+/D- 问题；RX 打印口改 PE7 并只保留拔插检测打印 |
| 第三阶段 | 571x 切方案、开 USB MIC、按键切换降噪算法（DNR_FRE / DNN_L1） |
| **当前** | **573x TX 由 YLCRN_L1_32K 改为 AINS4**（见 1.2 第二阶段），按键逻辑同步切换；571x 的 `ylcrn_L3.c` 已还原为原版 |


## 一、TX 端（573x）我方改动

以下为我方在**客户原始配置基础上**为本次需求做的改动。

### 1.1 UART0 打印口改为 PB3

`app/projects/microphone/config_ab5706a_le_mic.h`

```diff
-#define UART0_PRINTF_SEL                PRINTF_NONE//PRINTF_PB3 //选择UART打印信息输出IO，或关闭打印信息输出
+#define UART0_PRINTF_SEL                PRINTF_PB3 //选择UART打印信息输出IO，或关闭打印信息输出
```

客户原本关闭了打印（`PRINTF_NONE`），我方按需求打开并复用 PB3。注意 PB3 与 `USBDP`、`SD0MAP_G2` 同 IO，且 `header/config_extra.h` 在 `PRINTF_PB3` 时会强制关掉 `FUNC_USBDEV_EN` 与 `MUSIC_UDISK_EN`——TX 不需要 USB，可接受。

### 1.2 降噪算法：先开 YLCRN_L1_32K，后改为 AINS4

`app/projects/microphone/config_ab5706a_le_mic.h`

> 这一节经历过一次变更。最初按客户方案开启 YLCRN_L1_32K，后续应测试要求改为 AINS4。下面按时间顺序记录，**当前代码状态是 AINS4**。

**第一阶段：开启 YLCRN_L1_32K（已被下一阶段取代）**

```diff
-#define WIRELESS_MIC_YLCRN_L1_32K_EN            0               //发射端YLCRN_L1_32K降噪算法使能
+#define WIRELESS_MIC_YLCRN_L1_32K_EN            1               //发射端YLCRN_L1_32K降噪算法使能
```

```diff
-#define YLCRN_L1_32K_LEVEL                    	50//  100        //1~32768 越小值降噪越深
+#define YLCRN_L1_32K_LEVEL                       100            //1~32768 越小值降噪越深
```

客户把降噪深度设成 **50**（比 SDK 默认 100 更深）。我方曾保留默认 100。

`WIRELESS_MIC_SRC_EN` 不需要手改——`header/config_extra.h:707` 检测到 YLCRN 时会自动置 1（`ylcrn_L1_32k` 需要跑 32K 采样率）。

**第二阶段（当前状态）：关闭 YLCRN_L1_32K，改开 AINS4**

```diff
-#define WIRELESS_MIC_YLCRN_L1_32K_EN            1               //发射端YLCRN_L1_32K降噪算法使能 ，开此降噪需要打开WIRELESS_MIC_SRC_EN宏
-#define WIRELESS_MIC_AINS4_EN                   0               //AINS4算法使能  (发射端处理，默认开启后主频会抬至160M)
+#define WIRELESS_MIC_YLCRN_L1_32K_EN            0               //发射端YLCRN_L1_32K降噪算法使能 ，开此降噪需要打开WIRELESS_MIC_SRC_EN宏
+#define WIRELESS_MIC_AINS4_EN                   1               //AINS4算法使能  (发射端处理，默认开启后主频会抬至160M)
```

YLCRN_L1_32K 关闭后 `WIRELESS_MIC_SRC_EN` 也随之回落为 0（不再需要 32K→48K 重采样）。

两种算法的关键差异：

| 项目 | YLCRN_L1_32K | AINS4 |
| --- | --- | --- |
| 类型 | AI 大模型 | 传统+AI 混合 |
| code+rodata | 42.6K | 22.2K |
| buf | 34K | 38.5K |
| 算法耗时 | 15ms/20ms | 4ms/10ms |
| 等级参数 | `YLCRN_L1_32K_LEVEL`，1~32767 | `AINS4_NOISE_NT`，1~32767 |
| 等级语义 | 值越小降噪越深 | 值越小底噪越小（降噪越深） |
| `param_set` 是否用入参 | **是** | **是** |
| 是否需要 SRC | 是（32K 处理） | 否 |
| 固件体积 | 420 KB | 412 KB |

### 1.3 P/P、NEXT、PREV 按键改为降噪控制

客户 TX 这三个键在 DEVICE 模式下分别发 `MSG_MUSIC_PLAY_PAUSE`（实际无处理函数，是空按键）、`MSG_VOL_DOWN`、`MSG_VOL_UP`。我方改为降噪开关与等级调节。

> 三条消息名（`MSG_NR_TOGGLE` / `MSG_NR_LEVEL_UP` / `MSG_NR_LEVEL_DOWN`）保持不变，算法从 YLCRN_L1_32K 换成 AINS4 时只替换了实现函数，见 1.3.3。

#### 新增消息定义

`app/modules/key_scan.h`

```diff
     MSG_RECORD_START,               //开始录音
     MSG_RECORD_PAUSE,               //暂停录音
     MSG_RECORD_STOP,                //结束录音
     MSG_DELETE_FILE,
+
+    MSG_NR_TOGGLE,                  //YLCRN降噪算法开/关
+    MSG_NR_LEVEL_UP,                //YLCRN降噪等级加（值越大降噪越浅）
+    MSG_NR_LEVEL_DOWN,              //YLCRN降噪等级减（值越小降噪越深）
 
     MSG_MUSIC_PLAY          = 0x80, //播放
```

取值 0x18 / 0x19 / 0x1A，落在 `MSG_DELETE_FILE`(0x17) 与 `MSG_MUSIC_PLAY`(0x80) 之间的空闲区，已校验无重值。

#### 按键映射

`app/projects/microphone/port/port_key.c`，DEVICE 与 ADAPTER 两张表都改（TX 实际角色由配置工具的 `wireless_adapter_en` / `wireless_device_en` 决定，只改一张表角色一变就失效）：

```diff
 //DEVICE模式按键消息
 const u8 device_key_msg_tbl[KEY_TBL_MAX_NB][KEY_MSG_MAX_IDX] = {
                 //单击,                 长按下,                 HOLD,               长按抬起,               双击,                   三击                四击            五击
+#if WIRELESS_MIC_AINS4_EN
+    //PP 开关AINS4降噪，NEXT/PREV 调降噪量（双击保留音量调节）
+    [KEY_1] =   {MSG_NR_TOGGLE,         MSG_NO,                 MSG_PWR_HOLD,       MSG_NO,                 MSG_NO,                 MSG_NO,             MSG_NO,         MSG_NO},
+    [KEY_2] =   {MSG_NR_LEVEL_DOWN,     MSG_PAIRING,            MSG_NO,             MSG_NO,                 MSG_VOL_DOWN,           MSG_NO,             MSG_NO,         MSG_NO},
+    [KEY_3] =   {MSG_NR_LEVEL_UP,       MSG_PAIRING,            MSG_NO,             MSG_NO,                 MSG_VOL_UP,             MSG_NO,             MSG_NO,         MSG_NO},
+#else
     [KEY_1] =   {MSG_MUSIC_PLAY_PAUSE,  MSG_NO,                 MSG_PWR_HOLD,       MSG_NO,                 MSG_NO,                 MSG_NO,             MSG_NO,         MSG_NO},
     [KEY_2] =   {MSG_VOL_DOWN,          MSG_PAIRING,            MSG_NO,             MSG_NO,                 MSG_VOL_DOWN,           MSG_NO,             MSG_NO,         MSG_NO},
     [KEY_3] =   {MSG_VOL_UP,            MSG_PAIRING,            MSG_NO,             MSG_NO,                 MSG_VOL_UP,             MSG_NO,             MSG_NO,         MSG_NO},
+#endif
 };
```

`adapter_key_msg_tbl` 同样处理（客户原表带 `#if WIRELESS_CON_AND_BT_PAIRING_EN` 分支，我方在其外套了降噪分支）。两张表都改是必要的：TX 实际角色由配置工具的 `wireless_adapter_en` / `wireless_device_en` 决定，只改一张表角色一变就失效。

行为对照：

| 按键 | 短按 | 双击 | 长按 |
| --- | --- | --- | --- |
| P/P（KEY_1） | AINS4 降噪开/关 | — | 关机（`MSG_PWR_HOLD`，未改） |
| PREV（KEY_2） | 降噪量减（降噪更深） | 音量减 | 配对 |
| NEXT（KEY_3） | 降噪量加（降噪变浅） | 音量加 | 配对 |

#### 1.3.1 控制逻辑实现（当前：AINS4）

`app/projects/microphone/plugin/plugin.c`

开关部分按客户 `MSG_DNN_ONOFF` 的写法：**未连接直接返回 → 标志位取反 → 直接下发 mute**，不加额外的静音保护。`ains4_mic_mute_set(1)` 内部自带 `kick_proc_done` 自旋等待（`modules/voice/ains4.c:379`），会等算法线程退出后才返回，所以直接调用是安全的。

```c
#if WIRELESS_MIC_AINS4_EN
//ains4_noise_nt：1~32767，20*log10(nt/32768) 即降噪量。值越小底噪越小（降噪越深），越大越浅。
static s16 ains4_noise_cur = AINS4_NOISE_NT;

#define AINS4_NOISE_MAX         32767
#define AINS4_NOISE_MIN         1

//PP短按：开/关AINS4降噪（取反标志位后直接下发mute）
void plugin_ains4_toggle(void)
{
    static u8 ains4_mute_flag = 0;

    if (!wireless_cb.connected_sta) {
        return;
    }

    ains4_mute_flag = !ains4_mute_flag;
    ains4_mic_mute_set(ains4_mute_flag);

    printf("AINS4 %s (noise=%d)\n", ains4_mute_flag ? "OFF" : "ON", ains4_noise_cur);
}

//NEXT短按：降噪量加（底噪变大，降噪变浅）
void plugin_ains4_level_up(void)
{
    s16 nt = ains4_noise_cur;
    s16 step = (nt < 200) ? 50 : (nt < 500) ? 100 : (nt < 2000) ? 500 : (nt < 8000) ? 2000 : 8000;

    if (!wireless_cb.connected_sta) {
        return;
    }

    nt += step;
    if (nt > AINS4_NOISE_MAX) { nt = AINS4_NOISE_MAX; }

    ains4_mic_param_set(0, nt);
    ains4_noise_cur = nt;
    printf("AINS4 noise = %d\n", nt);
}

//PREV短按：降噪量减（底噪变小，降噪更深）
void plugin_ains4_level_down(void)
{
    s16 nt = ains4_noise_cur;
    s16 step = (nt <= 200) ? 50 : (nt <= 500) ? 100 : (nt <= 2000) ? 500 : (nt <= 8000) ? 2000 : 8000;

    if (!wireless_cb.connected_sta) {
        return;
    }

    nt -= step;
    if (nt < AINS4_NOISE_MIN) { nt = AINS4_NOISE_MIN; }

    ains4_mic_param_set(0, nt);
    ains4_noise_cur = nt;
    printf("AINS4 noise = %d\n", nt);
}
#endif
```

等级调节之所以能真正生效，是因为 `ains4_mic_param_set(mode, ains4_noise_nt)` 内部确实使用传入的 `ains4_noise_nt` 赋给 `ains4_cb.denoiseBound`（573x 的 `modules/voice/ains4.c:237`）。这点与 571x 的 `ylcrn_L3_mic_param_set` 不同——后者忽略入参、硬编码降噪量。

步进是分段的：200 以下 +50，500 以下 +100，2000 以下 +500，8000 以下 +2000，再往上 +8000，两端都有明显听感差异。

#### 1.3.2 早先的 YLCRN_L1_32K 版本（已被取代，仅作记录）

```c
#if WIRELESS_MIC_YLCRN_L1_32K_EN
static s16 ylcrn_level_cur = YLCRN_L1_32K_LEVEL;

#define YLCRN_LEVEL_MAX         32767   //API形参是s16，32768会溢出成负数
#define YLCRN_LEVEL_MIN         1

static void ylcrn_level_apply(s16 level)
{
    if (level > YLCRN_LEVEL_MAX) { level = YLCRN_LEVEL_MAX; }
    if (level < YLCRN_LEVEL_MIN) { level = YLCRN_LEVEL_MIN; }

    mic_alg_mute_en(1);
    delay_5ms(2);
    ylcrn_L1_32k_mic_param_set(level);      //内部会重跑ns_init下发新参数
    delay_5ms(2);
    mic_alg_mute_en(0);

    ylcrn_level_cur = level;
    printf("YLCRN level = %d\n", level);
}
...
#endif
```

这一版加 `mic_alg_mute_en` + `delay_5ms` 是因为 `ylcrn_L1_32k_mic_param_set()` 会重跑 `ns_init()` 重新初始化算法，过程中需要屏蔽输出。AINS4 不需要这一层，因为它的 `mute_set` 自带线程等待。

`plugin.h` 增加对应声明：

```diff
 void key_knob_process(u16 adc_val, const u8 *knob_level, u8 *key_val);
 void magic_voice_switch(void);
+
+#if WIRELESS_MIC_AINS4_EN
+//AINS4降噪的运行中开关与降噪量调节（PP/NEXT/PREV按键调用）
+void plugin_ains4_toggle(void);
+void plugin_ains4_level_up(void);
+void plugin_ains4_level_down(void);
+#endif
```

#### 消息分发

`app/projects/message/msg_device.c` 与 `app/projects/message/msg_adapter.c` 各加三个 case，都转发到上面的 plugin 接口：

```diff
+#if WIRELESS_MIC_AINS4_EN
+    //PP键开关AINS4降噪，NEXT/PREV键调降噪量，实现在plugin.c
+    case MSG_NR_TOGGLE:
+        plugin_ains4_toggle();
+        break;
+
+    case MSG_NR_LEVEL_UP:
+        plugin_ains4_level_up();
+        break;
+
+    case MSG_NR_LEVEL_DOWN:
+        plugin_ains4_level_down();
+        break;
+#endif // WIRELESS_MIC_AINS4_EN
```

### 1.4 我方 TX 相对客户未采纳的定制

客户 TX 里有一批与本次降噪需求无关的产品定制，我方**没有**引入，列出以便判断是否需要同步：

| 客户文件 | 定制内容 | 我方状态 |
| --- | --- | --- |
| `functions/func.c` | ZCD_M13 电量灯显逻辑（`__ledseg_scan` / `ledseg_disp_bat` / `power_set`，充电常亮、连接亮 3 秒等） | 未引入 |
| `functions/func.c` | `func_info[FUNC_IDLE]` 挂 `idle_key_msg_tbl` | 未引入 |
| `functions/func.h` | 新增 `idle_key_msg_tbl` 声明 | 未引入 |
| `functions/func_charge.c` | `sys_cb.charge_in` 充电在线标志、ZCD_M13 充满断电 | 未引入 |
| `modules/charge/charge.c`、`charge_box.c` | `vusb_reset_en(0)`、充电仓调试打印 | 未引入（仅注释差异） |
| `modules/voice/echo.c` | `ZCD_M13` 下 echo 初始挡位改为 0 | 未引入 |
| `modules/voice/echo.h` | 仅空行 | 无需引入 |
| `modules/voice/dnn_L3_32k.c` | `denoiseBound` 1000 → 500 | 未引入（我方不开 DNN_L3_32K） |
| `modules/voice/dnr_fre.c` | `denoiseBound` 16384→32768、`enr_nr_thr` -56→-30 | 未引入（我方不开 DNR_FRE） || `modules/warning/res_play.c` | 注释掉提示音期间的 `mic_alg_mute_en` 与 `wireless_tx_mic_mute_cmd0` | 未引入 |
| `modules/wireless/mic_proc.c` | mute 时改为清 PCM 而非发空帧 | 未引入 |
| `modules/wireless/wireless.c` | `WIRELESS_MIC_DNR_FRE_EN` 的 mute 分支 | 未引入 |
| `modules/bluetooth/bt/hfhs.c` | 新增 `get_vbat_percent()` 电量百分比换算 | 未引入 |
| `libs/bt8910/liba/api_uart.h` | `huart_t` 增加 `txbuf_size` / `txbuf` 字段（配套库有改动） | 未引入 |
| `libs/bt8910/liba/libbtstack.a` | 客户替换过的库 | 用我方 SDK 自带库 |
| `projects/microphone/cpu/huart.c` | 约 500 行大规模改写 | 未引入 |
| `projects/microphone/cpu/key/pwrkey.c` | 调试打印（已被注释） | 未引入 |
| `projects/microphone/cpu/lowpwr.c` | `CHANGE_EN_VUSB` VUSB 唤醒 | 未引入 |
| `projects/microphone/header/config_extra.h` | `HUART_EN` 1→0、`PWRON_FRIST_BAT_EN` 1→0 | 未引入 |
| `system/system.c` / `system.h` | `MSG_SYS_500MS` 1 秒定时、`sys_cb` 新增 10 个字段、`sys_cb1` 结构体、VUSB 唤醒标志 | 未引入 |
| `projects/message/msg_idle.c` | IDLE 模式屏蔽 PWR 长按 | 未引入 |

另有客户工程独有的 Code::Blocks 辅助文件 `app.depend`、`app.layout`，属 IDE 缓存，无需同步。我方独有的是 `build.ps1`（命令行构建脚本）。

---

## 二、RX 端（5706）差异

### 2.1 我方改动：开启 USB MIC 上行

`app/projects/microphone/config_ab5732e_rx.h`

```diff
-#define ADAPTER_USB_MIC_RX_EN                   0               //接收端是否打开usb mic
+#define ADAPTER_USB_MIC_RX_EN                   1               //接收端是否打开usb mic
```

与客户一致。耳机输出 `ADAPTER_DAC_OUTPUT_EN` 本就是 1，无需改。

### 2.2 我方为 USB 枚举关闭的调试通道

有三条路径会把 VUSB 引脚抢走，全部关闭。

```diff
-#define UART0_PRINTF_SEL                PRINTF_VUSB             //选择UART打印信息输出IO，或关闭打印信息输出
+#define UART0_PRINTF_SEL                PRINTF_PE7              //UART0打印输出到PE7（调试用）。不能用PRINTF_VUSB(USB VBUS脚)、PB3(USBDP)、PB2(LEDC)、PB4(5706的UART0只支持RX不支持TX)
```

打印口最终选了 **PE7**：它既不与 USB 冲突，也不被本工程的 ADKEY（`ADKEY_EN=0`）和 PWM（`PWM_RGB_EN=0`）占用，因此可以做调试打印。详见 3.7。

```diff
-#define EQ_DBG_IN_UART                  1           //是否使能UART在线调节EQ
+#define EQ_DBG_IN_UART                  0           //RX需USB声卡，HUART会抢占VUSB脚，关闭
```

`EQ_DBG_IN_UART=1` 会通过 `include/config_extra.h:65` 强制 `HUART_EN=1`，而 HUART 的引脚由 `xcfg_cb.huart_sel` 决定，其枚举里 `9 = VUSB`。只要配置工具里开了 HUART 调试，VUSB 就被 HUART 接管。RX 不需要在线调 EQ，直接关掉最稳。

```diff
-#define EFFECT_DBG_ADJUST_IN_UART       1          //是否使能UART在线调试音效
+#define EFFECT_DBG_ADJUST_IN_UART       0          //是否使能UART在线调试音效，该功能依赖EQ_DBG_IN_UART
```

`EFFECT_DBG_ADJUST_EN` 保持 1（关闭它会让 `effect_table.c` 引用的六个回调失去定义、链接失败），只关依赖 HUART 的在线通道。

配套在 `modules/tool/toolkit.c` 补一个空实现——`toolkit_effect.c` 的回调无条件引用 `toolkit_ack_fail()`，而该函数原本只在 `EFFECT_DBG_ADJUST_IN_UART` 下定义：

```diff
 AT(.text.effect_adjust_by_tool_exit)WEAK
 void toolkit_exit(void)
 {

 }
+#else
+//未开UART在线调试时，toolkit_effect.c的回调仍然引用本函数，补空实现避免链接失败
+AT(.text.toolkit)WEAK
+void toolkit_ack_fail(void)
+{
+}
 #endif
```

### 2.3 我方与客户剩余差异

```diff
 #define ADAPTER_HUART_INPUT_EN
-#define ADAPTER_HUART_INPUT_EN                  1               //适配器是否支持HUART输入数字信号,和麦数据混音后输出
+#define ADAPTER_HUART_INPUT_EN                  0               //适配器是否支持HUART输入数字信号,和麦数据混音后输出
```

客户 RX 的其他定制：

| 客户文件 | 定制内容 | 我方状态 |
| --- | --- | --- |
| `functions/func.c` | `MSG_SYS_500MS` 调 `led_conn_sta_scan()` 连接状态灯 | 未引入 |
| `functions/func_adapter.c` | `ADAPTER_HUART_INPUT_EN` 启动前强制 `SYS_240M` 主频（失败则 `WDT_RST`）、`led_bt_init()`、打开 `delay_ms` 随机延迟 | 未引入 |
| `include/config_define.h` | 打印口枚举差异 | 未改 |
| `modules/wireless/wireless_proc.c` | 无线流程差异 | 未改 |
| `system/system.c` / `system.h` | 系统层差异 | 未改 |

> 我方现在 `UART0_PRINTF_SEL` 也是 `PRINTF_NONE`，与客户一致（原因见第三节）。`ADAPTER_HUART_INPUT_EN` 的不一致源于客户给 HUART 数字音频输入加了 240M 主频保底，我方未开该功能。

---

## 三、RX 端 USB 声卡不枚举的问题

> **最终结论：硬件问题。D+ 与 D- 同时接通后设备即正常枚举。**
> 下面记录的排查过程是定位过程中排除的软件因素，其修复（关闭 VUSB 占用等）本身也是必要且正确的，但它们不是这次不枚举的根因。

### 3.1 现象

RX 烧录后插 USB 到电脑，设备管理器无新设备，USB 声卡（录音端）不可用。

### 3.2 已确认的事实

USB 相关代码**确实被编译进去**了，不是宏没打开的问题。在产物里查得到符号：

```
ude_info_init       T
usb_detect          T
usb_device_enter    T
```

配置侧 `ADAPTER_USB_MIC_RX_EN=1` → `UDE_MIC_EN=1`、`UDE_ENUM_TYPE = 0x0C`（HID + MIC），`USB_SUPPORT_EN` 非 0，`plugin_init()` 里的 `CLKGAT0 |= BIT(15)` 也已生效。

皮肤链路是通的：`system.c` 每 5ms 调 `usb_detect()` → 检测到 `USB_PC_CONNECTED` → `msg_enqueue(EVT_PC_INSERT)` → `msg_adapter.c` 的 `EVT_PC_INSERT` 分支 → `ude_info_init()` + `usb_device_enter(UDE_ENUM_TYPE)`。

### 3.3 排查中发现并修复的软件隐患：VUSB 引脚被两处调试通道占用

VUSB 是 USB 插入检测（VBUS）用的引脚。SDK 里有**两条**路径会把它接管成 UART/GPIO：

**路径一：`UART0_PRINTF_SEL = PRINTF_VUSB`**（`system/system.c:246`）

```c
#elif (UART0_PRINTF_SEL == PRINTF_VUSB)
    FUNCMCON0 = URX0MAP_TX | UTX0MAP_VUSB;          //RX0 Map To TX0, TX0 Map to G8
```

**路径二：HUART 调试**（`bsp/bsp_huart.c:53`）

```c
if((xcfg_cb.huart_sel == HUART_TR_VUSB)){
     if(!sys_cb.vusb_uart_flag){
        PWRCON0 |= BIT(30);                             //Enable VUSB GPIO
        sys_cb.vusb_uart_flag = 1;
     }
}
```

`PWRCON0 BIT(30)` 是 VUSB 的 GPIO 输入使能位（`port_mute.c` 里的注释：「输入使能位，1：使能」）。一旦置位，VUSB 就不再是纯 VBUS 检测输入，`usbchk_connect()` 读到的电平不可靠，`usb_detect()` 判不出 `USB_PC_CONNECTED`，`EVT_PC_INSERT` 不产生，枚举流程根本不启动。

**`HUART_EN` 的开启条件**（`include/config_extra.h:65`）是：

```c
#if (TEST_INTF_SEL == INTF_HUART) || EQ_DBG_IN_UART || BT_SCO_DUMP_TX_EN || QTEST_EN || FUNC_LE_DUT_EN
    #define HUART_EN                    1
#endif
```

而 `EQ_DBG_IN_UART` 默认是 1——所以即使关了打印，只要配置工具的 `xcfg.bin` 里 `huart_en=1` 且 `huart_sel=9(VUSB)`，VUSB 仍会被抢。

客户 RX 的 `PRINTF_NONE` 是必要但不充分的对照证据。

### 3.4 已实施的修复

`app/projects/microphone/config_ab5732e_rx.h`：

```diff
-#define UART0_PRINTF_SEL                PRINTF_VUSB             //选择UART打印信息输出IO，或关闭打印信息输出
+#define UART0_PRINTF_SEL                PRINTF_NONE             //RX需USB声卡，VUSB脚必须让给USB，不能用PRINTF_VUSB
```

```diff
-#define EQ_DBG_IN_UART                  1           //是否使能UART在线调节EQ
+#define EQ_DBG_IN_UART                  0           //RX需USB声卡，HUART会抢占VUSB脚，关闭
```

```diff
-#define EFFECT_DBG_ADJUST_IN_UART       1          //是否使能UART在线调试音效,该功能需要打开EFFECT_DBG_ADJUST_EN和EQ_DBG_IN_UART
+#define EFFECT_DBG_ADJUST_IN_UART       0          //是否使能UART在线调试音效，该功能依赖EQ_DBG_IN_UART
```

`modules/tool/toolkit.c`（补链接兜底）：

```diff
 AT(.text.effect_adjust_by_tool_exit)WEAK
 void toolkit_exit(void)
 {

 }
+#else
+//未开UART在线调试时，toolkit_effect.c的回调仍然引用本函数，补空实现避免链接失败
+AT(.text.toolkit)WEAK
+void toolkit_ack_fail(void)
+{
+}
 #endif
```

改动后代码体积从 196 KB 降到 192 KB，说明 HUART 调试通道确实被排除。

验证宏：

```
UART0_PRINTF_SEL         = PRINTF_NONE
EQ_DBG_IN_UART           = 0
EFFECT_DBG_ADJUST_IN_UART= 0
HUART_EN                 = 未定义
ADAPTER_USB_MIC_RX_EN    = 1
UDE_MIC_EN               = 1
WIRELESS_MIC_ROLE        = 1
```

验证符号（产物中均存在）：`usb_detect`、`ude_info_init`、`usb_device_enter`、`usb_mic_in_init`、`usb_mic_in_audio_input`、`usb_mic_in_audio_output_callback_set`、`dac0_out_init`。

### 3.5 USB 声卡数据通路

`functions/func_adapter.c` 的 `mic_rx_cfg_tbl` 里，DAC 输出与 USB MIC 输入是并行的两个模块，所以「耳机能听到」和「USB 能录到」是同一路音频的两个出口，无需额外配置：

```c
#if ADAPTER_DAC_OUTPUT_EN
    {DAC_OUT_TYPE,              1,   dac0_out_init,      dac0_out_audio_input,      dac0_out_audio_output_callback_set},
#endif

#if ADAPTER_USB_MIC_RX_EN
    {USB_MIC_IN_TYPE,           1,   usb_mic_in_init,    usb_mic_in_audio_input,    usb_mic_in_audio_output_callback_set},
#endif
```

### 3.6 第二轮排查：关掉 VUSB 占用后仍不枚举

第一轮修复（关 `PRINTF_VUSB` / `EQ_DBG_IN_UART`）后插拔仍无反应，且**客户 SDK 同样不枚举**，说明 VUSB 占用只是必要条件之一，不是全部原因。逐层核对代码与库符号后，确认以下链路都是完整的：

| 环节 | 状态 | 证据 |
| --- | --- | --- |
| 编译期开关 | 已开 | `ADAPTER_USB_MIC_RX_EN=1`、`UDE_MIC_EN=1`、`USB_SUPPORT_EN=1` |
| 枚举类型 | 正确 | `UDE_ENUM_TYPE = 0x0C`（HID bit2 + MIC bit3） |
| USB 驱动 | 已链接 | `usb_device_enter`(T)、`usb_device_process`(T)、`ude_tmr_isr`(T)、`pc_remove`(T) |
| 描述符表 | 已链接 | `desc_config_audio_mic`、`desc_config_hid`、`desc_config_audio_speaker` 等全部解析 |
| 子模块初始化 | 已链接 | `ude_init`、`uda_init`、`udh_init`、`ude_run_loop_execute` |
| USB 控制器初始化 | **库内真实实现** | `libdrivers.a:usb_comm.o` 的 `usb_init` 为 W 弱符号，但段大小 **0x8c = 140 字节**，是真实初始化代码，不是空壳 |
| 应用层空实现是否覆盖 | **未覆盖** | `strong_symbol.c:287` 的空 `usb_init`/`usb_isr` 只在 `!USB_SUPPORT_EN` 时编译，当前该值为 1，不参与编译 |
| 调用链 | 完整 | `usb_detect()` 每 5ms → 判 `USB_PC_CONNECTED` → `EVT_PC_INSERT` → `ude_info_init()` + `usb_device_enter()` |

排查中一个容易误判的点：产物里 `usb_init` 显示为 `W`，看起来像"空实现"。实际上 `W` 只是**符号属性**（库内以 weak 定义，允许应用层覆盖），`usb_comm.o` 里它有 140 字节的真实代码。同理 `usb_isr` 有 24 字节。所以 USB 初始化并没有缺失。

链路既然完整，问题就落在**硬件与运行期状态**上。为此新增两处串口诊断（见 3.7），用来区分「VBUS 检测不到」和「检测到但枚举失败」两种情况。

### 3.7 新增的串口诊断

RX 打印口改为 **PE7**，插拔 USB 时可在串口看到状态变化。

选 PE7 的原因（5706 的 UART0 TX 只支持 PA7/PB2/PB3/PE7/PE0/VUSB 六个，PB4 只能作 RX 不能作 TX）：

| 候选 | 是否可用 | 原因 |
| --- | --- | --- |
| `PB4` | **不可用** | 5706 只有 `URX0MAP_PB4`（RX），没有对应的 `UTX0MAP_PB4`（TX） |
| `VUSB` | **不可用** | 正是要排查的 USB VBUS 脚 |
| `PB2` | **不可用** | 被 `LEDC_MAPPING = LEDCMAP_PB2` 占用，且 `LED_DISP_EN=1` |
| `PB3` | **不可用** | 与 `USBDP` 同 IO，会直接干扰 USB 数据线 |
| `PA7` / `PE0` | 可用但未选 | 未与 USB 冲突，但在本板可能另有用途 |
| **`PE7`** | **采用** | `ADKEY_EN=0`（ADKEY 未启用）、`PWM_RGB_EN=0`（LPWM 未启用），无冲突 |

配置改动：

```diff
-#define UART0_PRINTF_SEL                PRINTF_NONE
+#define UART0_PRINTF_SEL                PRINTF_PE7    //不能用PRINTF_VUSB(USB VBUS脚)、PB3(USBDP)、PB2(LEDC)、PB4(5706的UART0只支持RX不支持TX)
```

```diff
+#define USB_DET_DBG_EN                  1             //USB插入检测状态变化打印(调试用，量产可关)
```

诊断点，`system/device.c` 的 `usb_detect()` 里加拔插状态打印（该函数 5ms 调一次，只在状态变化时输出，不刷屏）：

```c
#if USB_DET_DBG_EN
    {
        static u8 last_sta = 0xFF;
        if (usb_sta != last_sta) {
            last_sta = usb_sta;
            if (usb_sta == USB_PC_CONNECTED) {
                printf("[USB] PLUG IN (PC)\n");
            } else if (usb_sta == USB_UDISK_CONNECTED) {
                printf("[USB] PLUG IN (UDISK)\n");
            } else {
                printf("[USB] PLUG OUT\n");
            }
        }
    }
#endif
```

打印用英文，避免串口工具编码差异导致乱码。

**问题定位结果**：末端的 USB 枚举问题最终确认为**硬件问题**——D+ 与 D- 同时接通后设备即可正常枚举。软件侧 USB 链路本身是完整的，上述打印仅作拔插确认用途。

**判读方法**：

- 插线后**完全没有 `[USB] PLUG` 输出** → VBUS 没被检测到，查硬件：USB 线是否带 VBUS、板上 VUSB 分压电阻、D+/D- 是否都接通。
- 出现 `[USB] PLUG IN (PC)` 即表示 USB 链路工作正常，设备应被主机枚举。
- 反复出现 PLUG IN / PLUG OUT 交替 → VBUS 电平不稳，查分压与滤波电容。

> PE7 的串口参数与其他 UART0 打印一致（波特率由库内固定，常见 1.5 Mbps；若乱码依次试 115200 / 921600）。

### 3.8 排查清单

1. **先看串口有没有 `[USB] PLUG`**（见 3.7 的判读方法）。没有输出就不是软件问题，直接查硬件。
2. **确认 D+ / D- 都接通**。本次问题的实际原因就是 D+ 与 D- 未同时接上，两者都接通后设备即正常枚举。
3. **确认模式**：RX 必须进入 ADAPTER 模式，`adapter_cb.init_flag` 才为 1；否则 `usb_detect()` 在 `adapter_usb_init_en()` 处直接 return（`system/device.c:35`）。检查配置工具的 `wireless_adapter_en` 是否勾选。
4. **确认配置工具没再开 HUART**：`xcfg.bin` 里的 `huart_en` 必须是 0。这一项不在源码里，重新导出配置时容易忽略。
5. **确认 VBUS 硬件**：USB 线是否带 VBUS 供电，板上 VUSB 分压是否正常。
6. **确认时钟**：USB 需要 48M 时钟域。客户 RX 在 HUART 场景下强制拉过 `SYS_240M`（`func_adapter.c`），可作参考。
7. **看主机侧**：用 USB 总线分析仪或 Linux `dmesg`/`lsusb` 判断是「设备没拉 D+」还是「拉了但枚举失败」，两者排查方向不同。

> 量产时把 `USB_DET_DBG_EN` 改回 0、`UART0_PRINTF_SEL` 改回 `PRINTF_NONE` 即可。

---

## 四、571x（AB5712F，同时作 TX 和 RX）

`sdk_ab571x_le_mic2_v015_s8952_20260516` 与其他两个 SDK 结构差异较大，单独说明。它**没有对应的客户工程**，改动完全按需求自定。

### 4.1 该 SDK 的结构特点

| 项目 | 571x 情况 | 与其他 SDK 的差异 |
| --- | --- | --- |
| 编译器 | `riscv32` → RV32-V1（gcc 6.1.0） | 573x/5706 用 `riscv32-v3` → RV32-V3（gcc 14.2.0） |
| march | `rv32imacxbs1` | 新式 `rv32imc_zba_zbb_zbc_zbs_zca_zcb_zcmp_xbs1` |
| TX/RX 角色 | **同一固件兼作两者**，运行时由 `wireless_role_is_adapter()` 判定 | 573x/5706 分开编 |
| 按键表 | 单一 `wireless_mic_key_msg_tbl`（TX/RX 共用） | 573x 有 `device_key_msg_tbl` + `adapter_key_msg_tbl` 两张 |
| 算法体系 | YLCRN_L3/L2/16K、DNN_L1/L3、AINS4、DNR_FRE 等 | 573x 是 YLCRN_L1_32K、AINS4 |
| USB 枚举类型 | U盘 + HID + MIC（`0x0D`），SDK 硬编码 `UDE_STORAGE_EN=1` | 5706 是 HID + MIC（`0x0C`） |

### 4.2 切换方案到 CONFIG_WIRELESS_LE_MIC

`app/projects/microphone/config.h`

```diff
-#define USER_CONFIG                 CONFIG_CLIP_LE_MIC
+#define USER_CONFIG                 CONFIG_WIRELESS_LE_MIC
```

原配置是领夹麦方案（`config_clip_mic.h`），按需求改为无线话筒方案（`config_wireless_mic.h`）。

### 4.3 修复 SDK 自带的注释吞宏缺陷

`app/projects/microphone/config_wireless_mic.h:133`

```diff
-#define ADAPTER_USB_MIC_RX_EN                   0               //接收端是否打开usb mic#if FUNC_TEST_PRINT//调试打印
+#define ADAPTER_USB_MIC_RX_EN                   1               //接收端是否打开usb mic
```

原文件这一行的注释**没有换行**，把同一行尾部的 `#if FUNC_TEST_PRINT` 一起吞进了注释。因为 `#if` 从未生效，SDK 恰好还能正常编译；但一旦有人在这行后面补功能或调整注释，被吞掉的 `#if` 就会以不同面貌暴露出来，所以改这一行时顺手把注释补完整。

`include/config_extra.h:614` 也有同样的问题（`//设置设备为speaker#if FUNC_TEST_PRINT//调试打印`）。该行本次未改动，仅记录。

### 4.4 开启 USB MIC

`app/projects/microphone/config_wireless_mic.h`

见 4.3 的 diff（`ADAPTER_USB_MIC_RX_EN` 0 → 1）。开启后：

```
USB_SUPPORT_EN   = ADAPTER_USB_MIC_RX_EN = 1
UDE_MIC_EN       = 1
UDE_ENUM_TYPE    = UDE_STORAGE_EN*1 + UDE_SPEAKER_EN*2 + UDE_HID_EN*4 + UDE_MIC_EN*8
                 = 1 + 0 + 4 + 8 = 0x0D
```

### 4.5 给 usb_detect() 加 TX/RX 角色闸门

`app/functions/func_adapter.c`

571x 的 `usb_detect()` 原本没有模式保护——TX 角色下插 USB 也会投递 `EVT_PC_INSERT`，虽然 DEVICE 模式没有对应 case 会丢弃，但 USB 状态机会空转。照 5706 的做法加闸门：

```diff
 AT(.com_text.usb_detect)
 void usb_detect(void)
 {
 
     if(!sys_cb.bt_is_inited){
         return;
     }
+#if ADAPTER_USB_MIC_RX_EN
+    //本SDK同一固件兼作TX/RX，只有ADAPTER(RX)角色才需要USB声卡，TX角色不做USB检测
+    if(!adapter_cb.init_flag){
+        return;
+    }
+#endif
 	u8 usb_sta, usb_sta_old;
```

### 4.6 降噪算法盘点与容量实测

`modules/voice/` 下共 25 个算法模块，其中降噪（NR）类 14 个。**已接入 TX 链路表**（`functions/func_device.c` 的 `mic_tx_cfg_tbl`）的有：

| 算法 | 开关宏 | code+rodata | buf | 备注 |
| --- | --- | --- | --- | --- |
| YLCRN_L3（客户原方案） | `WIRELESS_MIC_YLCRN_L3_EN` | — | — | NPU 100K，12.8ms/20ms |
| YLCRN_L2 | `WIRELESS_MIC_YLCRN_L2_EN` | 14K | 18K | NPU 100K，6.2ms/10ms |
| YLCRN_16K | `WIRELESS_MIC_YLCRN_16K_EN` | — | — | 6.3/15ms |
| AINS4 | `WIRELESS_MIC_AINS4_EN` | 22.2K | 38.5K | 复用 NPU RAM |
| DNN_L1 | `WIRELESS_MIC_DNN_L1_EN` | 5K | 16K | 2.5ms |
| DNN_L3 | `WIRELESS_MIC_DNN_L3_EN` | 57K | 22.8K | 7.7ms/10ms |
| DNR_FRE | `WIRELESS_MIC_DNR_FRE_EN` | 8.4K | 6.7K | 500us/2.5ms |

**关键约束**：该 SDK 的算法代码运行时从 flash 搬到 **RAM（`com_device` 段）** 执行，容量极紧（`FLASH_CODE_SIZE` 450K）。实测各种组合：

| 组合 | 结果 |
| --- | --- |
| YLCRN_L3（原配置） | 376 KB 通过 |
| **DNR_FRE + DNN_L1** | **384 KB 通过（当前采用）** |
| DNR_FRE + DNN_L1 + AINS4 | 404 KB 通过 |
| DNR_FRE + YLCRN_L2 | 376 KB 通过 |
| DNR_FRE + DNN_L1 + DNN_L3 | 溢出 3,944 B |
| DNR_FRE + DNN_L1 + YLCRN_16K | 溢出 52,864 B |
| YLCRN_L3 + DNN_L1 + DNR_FRE | 溢出 74,400 B |

另外 **AINS4 与 YLCRN 系列共用 NPU RAM**（各需 100K），运行时互切需要先 `npu_exit()`，因此没有纳入切换集合。

结论：**无法把所有算法编进同一固件做运行时切换**。只能选一组能放下的组合编进去。

### 4.7 按键切换降噪算法

当前编入 **DNR_FRE + DNN_L1** 两个算法（均不占 NPU，可共存）。

**配置**（`config_wireless_mic.h`）：

```diff
+#define WIRELESS_MIC_DNN_L1_EN                  1
-#define WIRELESS_MIC_YLCRN_L3_EN                1
+#define WIRELESS_MIC_YLCRN_L3_EN                0
-#define WIRELESS_MIC_DNR_FRE_EN                 0
+#define WIRELESS_MIC_DNR_FRE_EN                 1
```

**新增消息**（`app/modules/key/key_scan.h`）：

```diff
     MSG_RECORD_START,               //开始录音
     MSG_RECORD_PAUSE,               //暂停录音
     MSG_RECORD_STOP,                //结束录音
     MSG_DELETE_FILE,
+
+    MSG_NR_TOGGLE,                  //YLCRN降噪算法开/关
+    MSG_NR_LEVEL_UP,                //YLCRN降噪等级加（降噪变浅）
+    MSG_NR_LEVEL_DOWN,              //YLCRN降噪等级减（降噪更深）
 
     MSG_MUSIC_PLAY          = 0x80, //播放
```

**按键表**（`app/projects/microphone/port/port_key.c`，`wireless_mic_key_msg_tbl`）：

```diff
 const u8 wireless_mic_key_msg_tbl[KEY_TBL_MAX_NB][KEY_MSG_MAX_IDX] = {
                 //单击,                 长按下,                 HOLD,               长按抬起,               双击,                   三击
+#if WIRELESS_MIC_DNR_FRE_EN || WIRELESS_MIC_DNN_L1_EN
+    //PP 循环切换降噪算法(关闭/各算法)，VOL-/VOL+ 前后切换（原音量功能保留在 HOLD 上）
+    [KEY_1] =   {MSG_NR_TOGGLE,         MSG_NO,                 MSG_KEY_PP_HOLD,    MSG_NO,                 MSG_KEY_PP_DOUBLE,      MSG_KEY_PP_THREE},
+    [KEY_2] =   {MSG_NR_LEVEL_DOWN,     MSG_NO,                 MSG_MUSIC_PREV,     MSG_NO,                 MSG_KEY_USER_DOUBLE,    MSG_KEY_USER_THREE},
+    [KEY_3] =   {MSG_NR_LEVEL_UP,       MSG_NO,                 MSG_MUSIC_NEXT,     MSG_NO,                 MSG_NO,                 MSG_NO},
+#else
     [KEY_1] =   {MSG_KEY_PP_KU,         MSG_NO,                 MSG_KEY_PP_HOLD,    MSG_NO,                 MSG_KEY_PP_DOUBLE,      MSG_KEY_PP_THREE},
     [KEY_2] =   {MSG_VOL_DOWN,          MSG_NO,                 MSG_MUSIC_PREV,     MSG_NO,                 MSG_KEY_USER_DOUBLE,    MSG_KEY_USER_THREE},
     [MSG_VOL_UP, ...]
+#endif
 };
```

注意这里和 573x 不同：571x 的 VOL+/VOL- **短按原本是音量加减**，我方改成了降噪切换；音量功能在 HOLD（长按）上仍然保留（`MSG_MUSIC_PREV` / `MSG_MUSIC_NEXT`）。

**切换实现**（`app/projects/microphone/plugin/plugin.c`）：

```c
#if defined(WIRELESS_MIC_DNR_FRE_EN) || defined(WIRELESS_MIC_DNN_L1_EN)
#define NR_SWITCH_EN    1
#else
#define NR_SWITCH_EN    0
#endif

#if NR_SWITCH_EN
typedef struct {
    const char *name;
    void (*mute_set)(uint8_t);
    uint8_t (*mute_get)(void);
} nr_alg_t;

static const nr_alg_t nr_alg_tbl[] = {
#if WIRELESS_MIC_DNR_FRE_EN
    { "DNR_FRE", dnr_fre_mic_mute_set, dnr_fre_mic_mute_get },
#endif
#if WIRELESS_MIC_DNN_L1_EN
    { "DNN_L1",  dnn_L1_mic_mute_set,  dnn_L1_mic_mute_get  },
#endif
};

#define NR_ALG_NB       ((int)(sizeof(nr_alg_tbl)/sizeof(nr_alg_tbl[0])))
static int nr_alg_cur = -1;         //-1表示全部关闭

static void nr_alg_mute_all(void)
{
    for (int i = 0; i < NR_ALG_NB; i++) {
        nr_alg_tbl[i].mute_set(1);
    }
}

//上电默认全关，避免多个算法同时处理同一路音频
void plugin_nr_init(void)
{
    nr_alg_cur = -1;
    nr_alg_mute_all();
}

//PP短按：在「关闭 -> DNR_FRE -> DNN_L1 -> 关闭」之间循环
void plugin_nr_toggle(void)
{
    nr_alg_mute_all();
    nr_alg_cur++;
    if (nr_alg_cur >= NR_ALG_NB) {
        nr_alg_cur = -1;
    }
    if (nr_alg_cur < 0) {
        printf("NR: OFF (all bypass)\n");
    } else {
        nr_alg_tbl[nr_alg_cur].mute_set(0);
        printf("NR: %s ON\n", nr_alg_tbl[nr_alg_cur].name);
    }
}

void plugin_nr_next(void) { ... }   //VOL+：向前切
void plugin_nr_prev(void) { ... }   //VOL-：向后切
#endif
```

**上电初始化**（`app/functions/func_device.c`）——两个算法的 `init` 后 `mute` 字段默认都是 0（启用），不显式关闭会导致上电后两个算法同时处理音频，所以在链路初始化之后挂了一个 hook：

```diff
 void func_device_init(void)
 {
     if (!device_cb.init_flag) {
         device_cb.init_flag = 1;
 #if WIRELESS_MIC_TX_EN
         ws_link_list_init(mic_tx_cfg_tbl, ...);
 #endif
+
+#if WIRELESS_MIC_DNR_FRE_EN || WIRELESS_MIC_DNN_L1_EN
+        //算法实例已建立，把降噪默认置于全关，之后由按键选择
+        plugin_nr_init();
+#endif
```

**行为对照**：

| 按键 | 短按 | 长按/HOLD | 双击 |
| --- | --- | --- | --- |
| PP | 循环切换：`OFF → DNR_FRE → DNN_L1 → OFF` | `MSG_KEY_PP_HOLD`（保留） | 原功能（保留） |
| VOL- | 切到上一个算法 | `MSG_MUSIC_PREV`（保留） | 原功能（保留） |
| VOL+ | 切到下一个算法 | `MSG_MUSIC_NEXT`（保留） | — |

串口输出：`NR: DNR_FRE ON` / `NR: DNN_L1 ON` / `NR: OFF (all bypass)`。

### 4.8 571x 与客户方案的两处算法差异

调研中确认了两个会影响"照抄客户方案"的实质问题：

**第一，`ylcrn_L3_mic_param_set()` 忽略入参。** `modules/voice/ylcrn_L3.c:153` 的函数体把 `denoiseBound`/`denoiseBoundLow`/`denoiseBoundHigh` 全部硬编码为 `1800`，形参 `gtcrn_L3_nt` 从头到尾没有被使用。也就是说客户那个方案**根本无法在运行时调降噪等级**。我方曾尝试改成使用入参，后按要求**已完全还原为原版**，不改厂商默认算法参数。

**第二，`YLCRN_L2` 同样忽略入参**（`denoiseBound = 0` 硬编码）。真正使用入参的只有 `ains4_mic_param_set()`、`dnn_L1_mic_param_set()`、`ains3_mic_param_set()` 等。

---

## 五、构建与烧录速查

| 工程 | 构建命令（在 `app/projects/microphone` 下） | 产物 | 代码体积 |
| --- | --- | --- | --- |
| TX 573x | `./build.ps1` / `./build.ps1 -Rebuild` | `Output/bin/app.dcf` | 412 KB（上限 492K） |
| RX 5706 | `./build.ps1` / `./build.ps1 -Rebuild` | `Output/bin/app.dcf` | 192 KB |
| TX/RX 571x | `./build.ps1` / `./build.ps1 -Rebuild` | `Output/bin/app.dcf` | 384 KB（上限 450K） |

工具链：573x/5706 用 RV32-V3（gcc 14.2.0），571x 用 **RV32-V1（gcc 6.1.0）**，脚本按 `app.cbp` 的编译器 ID 自动选择，不要手动指定成 V3。

烧录参数：TX 用配置工具选 `wireless_mic_emit`（BT8910），RX 用 `-D AB5700`，与 postbuild.bat 内部一致。571x 的 TX/RX 角色靠配置工具的 `wireless_device_en` / `wireless_adapter_en` 选择。

串口打印位置：

| 工程 | 打印口 | 可观测内容 |
| --- | --- | --- |
| TX 573x | PB3 | 见 6.1，随算法版本变化 |
| RX 5706 | PE7 | `[USB] PLUG IN (PC)` / `PLUG OUT`，仅拔插检测 |
| TX/RX 571x | VUSB | `NR: DNR_FRE ON` / `NR: DNN_L1 ON` / `NR: OFF` |

> 注意 573x 的 PB3 与 `USBDP` 同 IO，571x 的 VUSB 与 USB VBUS 同引脚。两个 TX 都不需要 USB 声卡，因此可以占用；但如果后续要在这些口上插 USB，必须先把打印改到别的引脚。

---

## 六、573x 两套降噪方案的切换与编译

573x 的 TX 有两套降噪实现，放在不同 git 分支上，切换时不需要手改代码。

| 分支 | 降噪算法 | 配置取值 | 代码体积 | app.dcf md5 |
| --- | --- | --- | --- | --- |
| `main` | **AINS4** | `YLCRN_L1_32K_EN=0`、`AINS4_EN=1`、`SRC_EN=0` | 412 KB | `e35dd5ca926dc75949990043637ebc4b` |
| `feat/573x-ylcrn` | **YLCRN_L1_32K** | `YLCRN_L1_32K_EN=1`、`AINS4_EN=0`、`SRC_EN=1` | 420 KB | `4b9a8c27300ce83a4afca450b8c3239a` |

两个分支的差异只集中在 6 个文件，且全部在 `sdk_ab573x_le_mic2_v013_s8047_20260129/` 内，另外两个 SDK 不受影响：

- `app/projects/microphone/config_ab5706a_le_mic.h`（算法开关与等级）
- `app/projects/microphone/plugin/plugin.c` / `plugin.h`（开关与等级调节实现）
- `app/projects/microphone/port/port_key.c`（三键映射）
- `app/projects/message/msg_device.c` / `msg_adapter.c`（消息分发）

### 6.1 切换步骤

切换前先确认工作区干净，否则 git 会拒绝：

```bash
cd D:/Code/work/client/573x_and_571x_5706/deepseek
git status --short          # 有输出说明有未提交改动
```

如果有未提交改动，二选一（**别直接丢弃**，见下方「常见问题」）：

```bash
git stash push -u -m "临时保存"
# 或
git commit -am "wip"
```

切到 AINS4 版：

```bash
git checkout main
```

切到 YLCRN 版：

```bash
git checkout feat/573x-ylcrn
```

切回后如果之前 stash 过，恢复改动：

```bash
git stash pop
```

### 6.2 切换后必须重新编译

**这一步不能省。** 产物（`app.rv32` / `app.bin` / `app.dcf`）在 `.gitignore` 里，不随分支切换变化——磁盘上留着的永远是上一次编译出来的那份。切了分支不重编，烧录的还是旧算法的固件。

```bash
cd sdk_ab573x_le_mic2_v013_s8047_20260129/app/projects/microphone
powershell -ExecutionPolicy Bypass -NoProfile -File build.ps1
```

要彻底排除旧 .o 的干扰，用 `-Rebuild` 从零构建：

```bash
powershell -ExecutionPolicy Bypass -NoProfile -File build.ps1 -Rebuild
```

脚本每次都会全量编译所有源文件，所以普通构建和 `-Rebuild` 的产物是一致的；`-Rebuild` 额外会先清空 `Output/obj` 和产物文件，适合怀疑有脏数据时使用。

### 6.3 确认当前烧的是哪个版本

编译后核对产物，最可靠的是查符号：

```bash
cd sdk_ab573x_le_mic2_v013_s8047_20260129/app/projects/microphone
NM="D:/software_download/work/rv32-toolchain_v1.5.9/rv32-toolchain_v1.5.9/install/RV32-V3/bin/riscv32-elf-nm.exe"

"$NM" Output/bin/app.rv32 | grep -c plugin_ains4   # AINS4 版输出 3
"$NM" Output/bin/app.rv32 | grep -c plugin_ylcrn   # YLCRN 版输出 3
```

两个数字一个有值一个为 0，就说明是哪个版本。也可以看 `CODE SIZE`（AINS4 = 412 KB，YLCRN = 420 KB）或直接比对 md5（见本节开头的表）。

### 6.4 两个版本的串口输出与按键行为

按键行为两版完全一致：

| 按键 | 短按 | 双击 | 长按 |
| --- | --- | --- | --- |
| P/P | 降噪开/关 | — | 关机 |
| PREV | 降噪等级减（降噪更深） | 音量减 | 配对 |
| NEXT | 降噪等级加（降噪变浅） | 音量加 | 配对 |

串口打印（PB3）随版本不同：

| 版本 | 开关时打印 | 调等级时打印 |
| --- | --- | --- |
| AINS4 | `AINS4 ON (noise=500)` / `AINS4 OFF (noise=500)` | `AINS4 noise = 700` |
| YLCRN | `YLCRN ON (level=100)` / `YLCRN OFF (level=100)` | `YLCRN level = 150` |

等级参数的语义相反，调按键时要留意：

| 版本 | 参数 | 范围 | 默认 | 值的方向 |
| --- | --- | --- | --- | --- |
| AINS4 | `ains4_noise_nt` | 1~32767 | 500 | **值越小降噪越深** |
| YLCRN | `YLCRN_L1_32K_LEVEL` | 1~32767 | 100 | **值越小降噪越深** |

两版都是「值越小降噪越深」，所以 NEXT（等级加）在两个版本里都是**让降噪变浅**，按键手感一致。

### 6.5 常见问题

**`git checkout` 报错 `Your local changes would be overwritten`**

有未提交改动。先 `git stash push -u -m "说明"` 保存，切换后再 `git stash pop`。**不要用 `git checkout -- .` 或 `git reset --hard`**，那会直接丢掉改动。

**切了分支但听感没变化**

忘了重新编译，或者烧的是旧 `app.dcf`。按 6.3 查符号确认。

**`git stash pop` 报冲突**

说明切换后目标分支的同一文件也被改过。用 `git stash show -p` 看内容，手工合并，或先 `git stash branch <新分支名>` 把改动落到独立分支上再处理。

**两套都想留着随时切**

这正是当前的做法：`main` = AINS4，`feat/573x-ylcrn` = YLCRN。不要在某一个分支上反复改，那样会丢历史。
