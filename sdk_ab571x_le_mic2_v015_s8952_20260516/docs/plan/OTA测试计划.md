# AB5712F 无线麦工程 OTA 升级测试计划

> 原则：本文档所有结论均来自仓库内代码/文档证据（附 file:line），未经证实的点一律标注【未确认】，不做任何推测。
> 工程：`app/projects/microphone`（AB5712F，CONFIG_CLIP_LE_MIC 领夹麦方案，见 `config.h:26-27`）
> 编写日期：2026-09-22

---

## 1. 背景：当前固件各升级路径的实际状态

仓库内存在 5 条升级相关代码路径，当前配置（`config_clip_mic.h`）下的状态如下：

| 路径 | 代码 | 状态 | 依据 |
|---|---|---|---|
| A. USB HID 升级（上位机 usb_tool） | `app/modules/usb_device/usb_device_update.c` | **死代码（宏关闭）** | `ADAPTER_USB_UPDATE_EN=0`（config_clip_mic.h:137），文件整体包在 `#if ADAPTER_USB_UPDATE_EN`（usb_device_update.c:13） |
| B. BLE/SPP FOTA | `app/modules/bluetooth/app/app_fota/app_fota.c` | **死代码（宏关闭）** | `ADAPTER_AB_FOT_EN=0`、`ADAPTER_NORMAL_LE_EN=0`（config_clip_mic.h:142-143），文件整体包在 `#if ADAPTER_AB_FOT_EN`（app_fota.c:3） |
| C. 经典蓝牙 OTA（库内建） | `app/functions/sfunc_bt_ota.c` | **已编译但无调用者** | `FUNC_BT_EN=1`（config_clip_mic.h:15），但全仓库 grep 不到 `sfunc_bt_ota` 的调用处；触发方式【未确认】 |
| D. ab_mate App OTA | `app/modules/bluetooth/app/ab_mate/ab_mate_ota.c` | **未参与编译** | app.cbp 中 ab_mate 只有 .h（app.cbp:275-279），无对应 .c 编译单元 |
| E. USB/SD 卡测试盒升级 | `app/functions/func_update.c` | **死代码且未参与编译** | 依赖 `USB_SD_UPDATE_EN`（func_update.c:5），该宏在 app/ 下无任何定义处；func_update.c 也不在 app.cbp（只有 func_update.h，app.cbp:139） |

结论：**当前固件没有任何升级路径处于可工作状态**，测试前必须先打开对应宏并重新构建。推荐从路径 A 开始（协议文档与上位机工具齐全，见 §2）。

---

## 2. 路径 A：USB HID 升级（推荐首选）

### 2.1 协议与工具对应关系（已核实）

固件 `usb_device_update.c` 与 `docs/peripheral/OTA/HID升级协议.docx`、`docs/peripheral/OTA/USB_TOOL1.0源码` 三者协议逐条吻合：

| 项 | 协议文档 | 固件实现 | usb_tool 源码 |
|---|---|---|---|
| 传输 | USB HID，65 字节/次（Report ID + Cmd + Len + Data） | `usb_device_hid_send_user(reporter_id=0x55, ...)`（usb_device_update.c:172,187,221,237） | hidapi 枚举所有 HID 设备、自动解析 Report ID（mainwindow.cpp，不按 VID/PID 过滤） |
| UPDATE_REQ 0x91 | FILE_SIZE(4B)+FILE_HASH(4B)+UPDATE_OBJ(1B，0=adapter，1=device) | 解析 0x91（usb_device_update.c:269-277），file_size/file_hash/role 字段一致 | MD5 前 4 字节作 HASH；"升级adapter端"/"升级device端"两个按钮 |
| PAKET_REQ 0x92 / PAKET_RSP 0x93 | 下位机拉数据，512B/包 | `CMD_PKT_REQ`/`CMD_PKT_RSP`（usb_device_update.c:169-191），512B 缓存 | 按 0x92 请求回 0x93 |
| UPDATE_STA 0x94 / DEV_RESET 0x95 | 状态上报（0=成功）/复位 | 对应实现（同文件） | 进度条/复位按钮 |

### 2.2 环境准备

1. 工具：`docs/peripheral/OTA/usb_tool1.1_20241205/usb_tool.exe`（Qt 发布包，无 readme，与 1.0 源码同名，应为其编译产物——此对应关系【未确认】，建议先装 1.0 源码版比对界面）。
2. 测试固件：`./build.ps1 -Rebuild` 产出 `Output/bin/app.bin`。
3. 硬件连接：板卡 USB 口与 PC 连接。**板卡 USB（DP/DM）引脚接法及是否需要跳线/按键进升级模式【未确认】**——测试前先查 `docs/sch/` 原理图确认。
4. 【未确认】设备上电枚举后的 USB 描述符形态（HID 复合设备？）——用 Windows 设备管理器确认，若无法枚举，检查 `app/modules/usb_device/usb_enum.c` 与 `usb_device_user.c` 的描述符配置。

### 2.3 测试步骤

**步骤 1：打开宏并构建**
- 将 `config_clip_mic.h:137` `ADAPTER_USB_UPDATE_EN` 改为 1。
- `./build.ps1 -Rebuild`。
- 检查 `Output/bin/map.txt`：代码空间必须 < 492K（config_clip_mic.h:39-40 注释：超了需改 `FLASH_SIZE` 为 2M 才支持）。
- 【未确认】打开该宏后是否连带要求打开其它宏（如 USB device 模式选择宏）——以编译告警/链接结果为准，必要时对照 `config_broadcast.h:133` 等其他方案的相同宏位。

**步骤 2：功能用例（升级 adapter 端，UPDATE_OBJ=0）**

| # | 用例 | 操作 | 预期 |
|---|---|---|---|
| A1 | 版本一致升级 | usb_tool 加载当前 `app.bin`，点"升级adapter端" | 进度条走完，UPDATE_STA=0，设备按 0x95 复位重启 |
| A2 | 版本交叉升级 | 改 `config_clip_mic.h:489` `SW_VERSION`（只能数字 0-9，"ota需要转码"）为 V0.0.2 后重新构建，用新 app.bin 升级旧固件 | 升级成功，复位后新版本生效 |
| A3 | 升级后功能回归 | A2 完成后做基本功能检查（无线麦收发、蓝牙、按键、LED） | 功能正常 |
| A4 | HASH 校验失败 | 人为篡改升级文件几个字节后升级 | 【未确认】预期设备拒绝（固件如何校验 file_hash：代码中 hash 字段被解析，但拒绝行为分支【未确认】），设备不重启、原固件仍可运行 |
| A5 | 中途断开 | 升级进行中拔 USB 线 | 设备不死机；重新插入后可再次发起升级。【未确认】是否有断点续传（USB HID 路径代码中未见断点续传逻辑，FOTA 路径才有 `fot_breakpoint_info_read`，api_update.h:36） |
| A6 | 文件超尺寸 | 加载大于剩余 flash 空间的文件 | 【未确认】预期失败且原固件不受影响 |
| A7 | 重复升级 | A1 成功后立即再次发起 | 第二次可正常完成或被正确拒绝（以实际行为记录） |

**步骤 3：升级 device 端（UPDATE_OBJ=1）**
- 0x91 的 UPDATE_OBJ=1 表示通过 adapter 转发升级对端麦（协议文档"0=adapter，1=device"）。
- 单机（仅麦端板卡）测试该用例预期无意义；需**适配器+麦成对环境**。成对环境下的转发链路（USB HID → 2.4G/BLE → 麦端 FOTA）是否打通依赖 `ADAPTER_AB_FOT_DEVICE_SUPPORT`（usb_device_update.c 中 `fot_tws_update_data_write` 路径，usb_device_update.c:116）与麦端 FOTA 支持。【未确认】本工程麦端是否具备被升级的接收侧实现。
- **结论：device 端升级用例列入 §5 待确认问题清单，确认前不执行。**

### 2.4 通过标准

- A1/A2/A3 全部通过；A4/A5/A6 设备均不变砖（复位或重新上电后原固件可恢复运行）。
- 全程记录：升级总耗时、失败时 UPDATE_STA 值。

### 2.5 回退

- 【未确认】`Output/bin/updater.bin`（SDK 自带的升级引导镜像，postbuild.bat 不生成它）是否参与 USB HID 升级路径的失败回退。若升级失败变砖，用原厂烧录工具（XCode/download.xm 流程）重新烧录——具体烧录工具用法【未确认】，见 §5。

---

## 3. 路径 B：BLE/SPP FOTA（待确认后再测）

代码存在且实现完整（BLE 认证 magic `0xCC,0xAA,0x55,0xEE,0x12,0x19,0xE4`，app_fota.c:61；命令 0x91/0x92/0xA0/0x20/0x90，512B 块，app_fota.c:49-60），但：

- 需打开 `ADAPTER_AB_FOT_EN=1` + `ADAPTER_NORMAL_LE_EN=1`（config_clip_mic.h:142-143），并确认编译后代码 < 492K。
- **配套上位机（手机 APP 或 PC 端 FOTA 工具）仓库内未找到**【未确认】——ab_mate 相关 .c 未编译（§1 路径 D），`ab_mate_ota.c` 是否就是预期上位机对接端、中科蓝讯"Mate APP"是否可用于本方案，需向原厂确认。
- 若确认 APP 可用，测试用例参照路径 A 的 A1-A7 结构重写（增加 BLE 断连重连续传用例，`app_fota_breakpoint_info_read` app_fota.c:184 表明支持断点续传）。

## 4. 路径 C：经典蓝牙 OTA（待确认后再测）

`sfunc_bt_ota.c` 已编译（FUNC_BT_EN=1），流程为 `ota_enter()` → 等 `BT_STA_OTA` → `ota_exit()`（sfunc_bt_ota.c:8-25），`ota_enter/exit` 由闭源库提供（app/libs/bt8930/api_update.h:56-57）。但全仓库无任何调用者，触发入口（按键？APP 命令？库内部自动进入？）【未确认】。列入 §5 问题清单。

## 5. 需向原厂/硬件确认的问题清单

1. 板卡 USB 升级的接线方式与进入升级模式的操作（是否需要按键组合）。
2. `updater.bin` 在各升级路径中的角色，以及失败回退机制。
3. USB HID 路径中 `file_hash` 的校验算法与失败行为（协议文档说 MD5 前 4 字节，固件侧校验分支未在代码中看到）。
4. UPDATE_OBJ=1（升级对端 device）的完整链路前提：麦端被升级侧实现是否在当前工程配置中存在。
5. BLE FOTA 的官方上位机是什么（Mate APP？），与 `ab_mate_ota.c` 未编译的关系。
6. 经典蓝牙 OTA（`sfunc_bt_ota`）的触发入口。
7. `SW_VERSION`"ota需要转码"的具体转码规则（注释在 config_clip_mic.h:489，规则未在源码中找到）。
8. flash 烧录/回退用的原厂工具名称与用法（XCode？烧录器 + download.xm？）。
