# OTA（USB HID 升级）测试记录

> 工程：`app/projects/microphone`（CONFIG_CLIP_LE_MIC，AB5712F）
> 原理与代码说明：`docs/peripheral/OTA/OTA原理与代码说明.md`
> 测试日期：2026-09-22。结论：**adapter 端 USB HID OTA 升级全链路验证通过 ✅**（T1 通道打通 / T2 复位加载 / T3 自编译固件交叉升级闭环）

## 1. 测试环境

| 项 | 内容 |
|---|---|
| 固件 | `ADAPTER_USB_UPDATE_EN=1`，reporter_id=0x03，4 参 `usb_device_hid_send_user(..., FOTA_DATA)` |
| 上位机 | `docs/peripheral/OTA/usb_tool1.1_20241205/usb_tool.exe` |
| 升级文件 | `Output/bin/rx_ota.fot`（0x51400 字节，原厂工具生成，MD5 前 4 字节 A3C247C0） |
| 连接 | USB 线（设备枚举为 Composite Device，VID 0x0C21/PID 0x0124），usb_tool 选 **Usage Page 0xff90（Reporter ID 0x03）** 的 "USB Mirophone" 条目 |
| 串口 | PB3，`PRINTF_PB3`（config_clip_mic.h），观察升级打印 |
| 注意 | Reporter ID **0x41**（0xff82 页）连接无效——发 0x91 后设备虽解析但不回 0x92，进度 0%。**必须用 0x03** |

## 2. 测试原理

设备固件内置 HID 升级服务器（`usb_device_update.c`）：上位机发 0x91（带文件大小+MD5 hash+升级对象）启动升级，之后设备按 512B/包用 0x92 拉数据（0x93 回），全部写入 flash（FOTA 双备份，库内 `fot_write` 校验）后回 0x94 成功状态；上位机发 0x95 触发设备 1.5 秒后软复位加载新固件。升级全程不需要按键/跳线，插 USB 即可。

## 3. 测试步骤与结果

### T1 升级主流程 ✅

1. 烧录含升级功能的固件，USB 连 PC，usb_tool 连 0xff90 条目。
2. 加载 rx_ota.fot，点【升级adapter端】。
3. usb_tool 显示进度条走完，日志输出 **"Adapter 升级成功！"**（收到 0x94 sta=0）。

串口实际打印（完整关键字序列）：

```
usb_dev_update_init adapter
file_size: 51400
file_hash: a3c247c0
adapter ota            ← 每收到一包0x93打一次（共约百余行，略）
-->addr: 0x51000, len: 0x200
adapter ota ...（最后一包）
-->addr: 0x51200, len: 0x200
-->fot update ok       ← 写入+校验通过，回0x94
```

`-->addr` 从 0 递增到 0x51200 共 0x51400 字节 = 文件大小，无缺口；`fot_get_err()==0`。

### T2 复位加载 ✅

usb_tool 点【重启设备】（发 0x95）：

```
usb_dev_update_reset
ADAPTER RESET          ← 1.5s 后软复位
Hello Platform / startup
SDK : v0150, LIBS: v0150
WDT reset
func_adapter ... 正常进入适配器模式
EVT_PC_INSERT          ← USB 重新枚举成功
```

设备复位后重新枚举、正常启动运行。

### T3 版本交叉升级闭环 ✅（2026-09-22）

目的：验证"PC 收到我们自编译固件转的 FOT → 升级后新固件真正生效"，即完整闭环。

1. [main.c:24](../../app/projects/microphone/main.c) 增加版本标识打印 `APP_VER: OTA-TEST-V2 (__DATE__ __TIME__)`，重新构建得 `app_ota_test_v2.dcf`（CODE SIZE 328KB）。
2. 原厂 FOT 打包工具将 dcf 转为 `OTA-TEST-V2.fot`（0x51400 字节，Hash B6A17E7C）。
3. usb_tool 连 0xff90（Report ID 0x03）升级 → 工具显示"Adapter 升级成功！"→ 点【重启设备】。

串口打印（复位后新固件生效）：

```
-->addr: 0x51200, len: 0x200
-->fot update ok
usb_dev_update_reset
ADAPTER RESET
Hello Platform / startup
Hello BT893X: 88010039
SDK : v0150, LIBS: v0150
APP_VER: OTA-TEST-V2 (Sep 22 2026 15:04:40)   ← 新固件标识出现，交叉升级闭环成功
WDT reset
func_adapter ... 正常运行
EVT_PC_INSERT
```

**结论：自编译固件 → FOT → USB HID OTA → 设备运行新固件，全链路验证通过。**

## 4. 判定标准（通过/失败）

| 检查点 | 通过标准 | 本次结果 |
|---|---|---|
| 0x91 应答 | 串口打印 `usb_dev_update_init adapter` + size/hash 与工具显示一致 | ✅ |
| 传输完整 | `-->addr` 最终值 + 0x200 == file_size，中途无 `USB_DEV_ERR`/`fot_get_err:N` | ✅ |
| 写入校验 | `-->fot update ok`，工具收到升级成功 | ✅ |
| 复位 | `ADAPTER RESET` 后正常启动、USB 重新枚举 | ✅ |

## 5. 遗留测试项（待做）

| # | 用例 | 状态 |
|---|---|---|
| L1 | ~~版本交叉验证~~ | ✅ 已完成，见 T3 |
| L2 | 篡改 FOT 文件后升级：确认失败行为（固件侧未见 hash 校验分支，预期靠库 CRC） | 未做 |
| L3 | 升级中途拔 USB 线：设备不死机、可重新升级（无断点续传，预期整包重来） | 未做 |
| L4 | 升级 device 端（UPDATE_OBJ=1）：需 `ADAPTER_AB_FOT_DEVICE_SUPPORT` 及成对环境 | ✅ 流程打通+双端同版本复测通过，见 §7/§8 |
| L5 | 升级后全功能回归（无线收发/蓝牙/按键/LED） | 未做 |
| L6 | ~~确认升级固件生效~~ | ✅ 已完成，见 T3（APP_VER 打印验证） |

## 6. 历史问题记录（供排查参考）

| 现象 | 根因 | 解决 |
|---|---|---|
| 进度条 0%，串口只见 `usb_dev_update_init` 无后续 | 固件用 Report ID 0x55 发 0x92，描述符中无此 ID，Windows 拒发 | reporter_id 改 0x03（usb_device_update.c） |
| `usb_device_hid_send_user` 3/4 参不匹配，第 4 参为栈垃圾 | 头文件声明少一个 `int type` 参数 | usb_device_update.h:19 补全，调用传 `FOTA_DATA` |
| usb_tool 设备列表多个条目选不对 | 复合设备 4 个 HID 接口（0x06/0x07/0x03/0x41） | 选 VID 0x0c21 且 Usage Page 0xff90 的条目 |
| 原厂首次给的 libplatform.a 无 HID 符号 | 发错库（HID 在 libdrivers.a）；后发的 libdrivers.a 与存量库逐字节相同（仅打包时间戳不同） | 无需替换，当前库已支持 |
| 打开 `ADAPTER_AB_FOT_DEVICE_SUPPORT=1` 后链接失败：`region 'comm' overflowed by 3588 bytes` | FOTA（含 device 同步升级）代码+BSS 增加约 8KB，comm RAM（原 109k）放不下 | ram.ld 增加 `#elif ADAPTER_AB_FOT_DEVICE_SUPPORT` 分支，`__comm_ram_size` 109k→113k（从空余较多的 adapter 区借 4k，region `com_adapter` 实际只用约 43KB/188KB，见 map.txt `__buf_end_fot`=0x3742c < 0x59000） |

## 7. L4：升级 device 端（发射端麦克风）——固件已就绪，待成对测试

### 7.1 代码链路（已核实，全部有代码依据）

device 端升级 = **适配器把收到的 FOT 文件通过无线链路（BLE user channel）转发给已连接的发射端麦**：

1. 上位机 0x91 的 role 字节=1（`usb_dev_update_init`，usb_device_update.c:279 `param[11]`）。
2. 适配器检查 `wireless_get_status()`（无线麦已连接，否则回 0x94 ERR）→ `usb_fot_device_wakeup()`（usb_device_user.c:101）发 `{0x80,0x10,0x01}` 唤醒从机进入 FOTA。
3. `fot_tws_update_file_hash_set(file_hash)` 下发文件 hash（app_fota.c:1308）；从机对比 flash 中记录的 hash/版本决定断点续传或从 0 开始（app_fota.c:866 `fot_reply_update_request`）。
4. 适配器每收满 512B：`fot_tws_update_data_write()`（app_fota.c:1286）按 ≤240B 分帧经 `gap_user_channel_send_buf()`（libbtstack）发出；从机 `fot_tws_recv_proc()`（app_fota.c:793）拼包 → `app_fota_write()` → `fot_write()`（libplatform，双备份写 flash）→ 回 0xB0 应答，适配器再拉下一包。
5. 全部传完：0x94 通知上位机 → `usb_dev_update_reset` 中 `fot_tws_sync_update_done()` 通知从机重启（usb_device_update.c:254）→ 从机校验通过后加载新固件。

关键接收侧：libbtstack.a `gap_proc.o` 未定义引用 `tws_set_fot_data`，即 BLE user channel 收到的数据由 app_fota.c:845 接管——链路闭合，无需额外接线。

### 7.2 本次固件改动

| 文件 | 改动 |
|---|---|
| config_clip_mic.h:143-144 | `ADAPTER_AB_FOT_EN` 0→1，`ADAPTER_AB_FOT_DEVICE_SUPPORT` 0→1 |
| ram.ld:20 | 新增 `#elif ADAPTER_AB_FOT_DEVICE_SUPPORT` 分支，`__comm_ram_size` 109k→113k |
| usb_device_update.h:35-36 | 补 `fot_set_base_offset()`/`fot_get_down_length()` 声明（libplatform 提供，头文件原本没有，main.c:47 原为隐式声明） |
| modules.h:38 | 引入 `usb_device/usb_device_update.h`，使 main.c 等可见上述声明 |

编译通过：CODE SIZE 344KB（< 450K 上限），app.dcf 424.58KB。

### 7.3 测试前提与步骤（待执行）

**运行角色由烧录时的配置参数决定**（与编译无关）：`xcfg_init()`（system.c:694，实现在 libplatform）从 flash 参数区读入 `.setting` 烧录值 → `wireless_mic_role_init()`（wireless_proc.c:221）按 `xcfg_cb.wireless_adapter_en/device_en` 置 `cfg_wireless_role` → `func_run()`（func.c:238）选 `func_adapter()`/`func_device()`。`wireless_mic_emit.setting` 中 `wireless_adapter_en=False / wireless_device_en=True`，正确烧录后应打印 **`load_code(0)`**（wireless_proc.c:250）。

**开机日志判别（关键）**：
- `load_code(1)` + `func_adapter` → 参数区仍是 adapter 配置（本次实测如此，烧录时未生效或未擦参数区）；
- `load_code(0)` + `func_device` → device 端，才是待测角色。

原厂确认（Q1/Q2 与代码核对结果）：
- **Q1 需要两块板：正确**。role=1 时 PC 仍插 adapter（dongle），adapter 断开音频链路（`ble_audio_ctr_cb(0)`，usb_device_update.c:291）后经 TWS 无线命令通道转发，从机（device/麦克风）写自己的 flash。`usb_fot_device_wakeup()`（usb_device_user.c:101）要求 TWS 已连接，未连接直接 ERR——与原厂说法一致。两块板都要上电。
- **Q2 adapter 自己不会被升级：正确**。role=1 走 `usb_dev_update_process()` 的 device 分支（usb_device_update.c:99-143），只调 `fot_tws_update_data_write()` 转发，不调 `fot_write()` 写本地。两端都要更新时先 role=0 再 role=1（两次升级会话）。
- **"升级前先按键进升级模式（fot_check）"【在本工程不成立/未接线】**：`fot_check()`/`fot_update_en_switch()`（bsp_usbfot.c）是进入 USB Storage 升级模式的逻辑，全工程搜不到任何按键调用 `fot_update_en_switch()`——按键表（port_key.c:72 无线麦表）映射的 `MSG_KEY_USER_DOUBLE/THREE` 在整个工程无处理分支。USB HID 0x91 role=1 流程也从不查询 `fot_update_en_flag`。**该说法可能对应原厂另一版代码（如对讲机方案），需向原厂确认本 v015 s8952 是否需要、按哪个键。**

步骤：
1. 适配器（adapter 板）烧录本节固件（保留 adapter setting）；发射端板用 `wireless_mic_emit` 配置烧录，**开机串口必须看到 `load_code(0)` + `func_device`**，若仍打印 `load_code(1)` 说明配置参数没烧进去（换烧录工具重新烧录参数区/勾选擦除参数）。
2. 两块板上电并完成无线配对（device 端日志出现连接成功 / adapter 端 `wireless_get_status()!=0`）。
3. USB 连 adapter 板，usb_tool 连 0xff90（Report ID 0x03）条目。
4. 加载发射端麦对应的 FOT 文件（由发射端固件 dcf 转换，**不是**适配器的 app.dcf），点【升级device端】（UPDATE_OBJ=1）。
5. 判定：
   - adapter 串口出现 `usb_dev_update_init device` + file_size/file_hash；
   - 转发持续推进，无 `USB_FOT_ERR`、无 `fot_get_err:N`；
   - 工具显示升级成功；0x95 复位后两端重启、发射端麦运行新固件（用版本打印/行为差异确认，同 T3 思路）。
6. 失败排查入口：
   - 0x91 后立即 ERR → adapter 与 device 无线未连接（`wireless_get_status()==0`）；
   - 进度停在中间不动 → 从机未回 0xB0 应答，多半是发射端固件没有 FOTA 接收侧（见前提【未确认】）。

### 7.4 需向原厂确认的问题清单

1. 发射端（wireless_mic_emit 角色）固件是否内置 FOTA 接收侧（app_fota 接收逻辑）？出厂固件是否已打开？（实测：已打开，role=1 升级流程可走通）
2. "role=1 升级前按键进升级模式"是否适用于本版本（本工程 `fot_update_en_switch()` 无任何调用者）？（实测：不需要按键）
3. 发射端单/双麦（一拖二）时 `gap_user_channel_send_buf(0/1,...)` 双索引发包的行为（从机各自应答？）。
4. device 端升级的 FOT 是否必须用**发射端工程**编译的 dcf 生成？用 adapter 同源码 dcf 升 device 是否受支持？（见 §8.2 连不回问题，最可疑点）
5. `fot_write` 双备份区与参数区（link info/校准）的 flash 布局关系，升级是否会波及参数区？

## 8. L4 实测结果（2026-09-22 第二轮）

### 8.1 测试通过的部分 ✅

- 烧录工具里取消"保留参数区"、用 `wireless_mic_emit` 配置重烧后，发射端开机打印 **`load_code(0)` + `func_device`**，角色正确。
- usb_tool 选 0xff90（Report ID 0x03），加载 `OTA-TEST-V2.fot`，点【升级device端】：
  - adapter 串口：`device ota`（每包一次）持续到 `update:51400, 51400` + `USB_UDP_FINISH`（收满 0x51400 字节，与文件大小一致）；
  - usb_tool：**"Device 升级成功！"**；
  - 0x95 复位后：adapter 打 `ADAPTER RESET` 重启，device 端先打 `-->addr: 0x51200, len: 0x200`（尾包）后复位，重启后打印 **`load_code(0)` + `func_device`**，且出现 `APP_VER: OTA-TEST-V2` —— **device 端确实运行了升级后的新固件**。
- 中途 `ERROR ======> timer_thread miss: pcm` 数次：device OTA 转发期间音频 PCM 线程超时（升级占用 CPU/关了音频链路），属升级过程正常现象，不影响升级结果。

**结论：role=1（adapter 中转升级 device 端）流程打通 ✅，无需按键进升级模式（本工程）。**

### 8.2 升级成功后无线连不回 ❌（新问题）

现象：两端复位后反复 `WIRELESS_CONNECTED → WIRELESS_DISCONNECT`（adapter 侧，con_sta 在 0/1/2 间循环+chan_scan 换频点）、device 侧持续 `WIRELESS_CONNECT_FAIL, addr 6f 17 10 e7 42 41`。

本地分析（基于代码，最终结论需原厂确认）：
- 两端 `WIRELESS_FEAT=0x4682`（V7 机制、D2A、hop_v1、link_nb=2）、codec、`WS_NAME=LE_MIC` 全一致，组队特性匹配不是问题；
- `WIRELESS_CON_BONDING_EN=0`（feat bit5=0），非组队绑定模式；但 adapter 升级后打印 `adapter, init(0): 0, 1`——**flash 里有 1 条历史 link info**（升级配对前留下）；
- 本次给 device 升的 FOT 是 **adapter 工程的 dcf** 转的（OTA-TEST-V2.fot，与 adapter 同源码）。device 端运行角色由参数区决定没错，但**发射端出厂固件与本工程编译出的 emit 代码可能存在差异**（算法/参数/配置工具版本），升级后 device 行为变化导致链路建立后立即掉线；
- 也不能排除升级过程中 flash 参数区（校准/链接信息）受影响——`fot_write` 的双备份区域与参数区的位置关系【未确认】。

待办/需原厂确认：
1. device 端是否只能用**发射端自己工程编译的 dcf** 转 FOT 升级？用 adapter 同源码 dcf 升 device 是否受支持？（当前最可疑）
2. `fot_write` 双备份区与参数区（含 link info、校准）的 flash 布局，升级是否会碰参数区？
3. 恢复方法：重新用线烧回原厂发射端固件+emit 配置后，组队是否恢复正常（用于区分"固件内容问题"还是"参数区被破坏"）。

### 8.3 复测：同一 FOT 分别升级 adapter 和 device，双端正常 ✅（2026-09-22 第三轮）

方法：用同一个 `OTA-Emit-Test-V1.fot`（19:34:31 编译，带 `APP_VER` 打印）先 role=0 升 adapter，再 role=1 升 device，两端升到同一版本。

结果：
- adapter 串口：`adapter ota` → `-->addr: 0x55200, len: 0x200` → `-->fot update ok`，复位后打印 `APP_VER: OTA-Emit-Test-V1` + `load_code(1)` + `func_adapter`；
- device 串口：尾包 `-->addr: 0x55200` 后复位，重启打印 `APP_VER: OTA-Emit-Test-V1` + `load_code(0)` + `func_device`；
- **两端无线组队恢复正常**：adapter `WIRELESS_CONNECTED,0 0` + `RX: pdu=(50B, 622)`（收音频），device `WIRELESS_CONNECTED,0 1` + `TX: pdu=(50B, 622)`（发音频），链路稳定不再掉线。

### 8.4 §8.2"连不上"的结论（存疑保留）

双端都升到**同一份固件**后恢复正常，说明 §8.2 的掉线大概率是**两端固件版本不一致**：当时只升了 device（跑的是本工程编译的新 emit 代码），adapter 还跑着旧版（T3 的 vendor 版）固件，两端无线实现存在差异，链路建立后随即掉线。但"版本不一致必然掉线"还是"升级过程碰了参数、重烧时顺带恢复"两种解释无法本地区分【未确认】。

**实践结论：跨端升级时两端务必用同一份源码编译的同版本固件；role=1 升完 device 后如果连不上，先把 adapter 也升到同版本再排查。**
