#include "mainwindow.h"
#include "ui_mainwindow.h"

typedef enum{
    CMD_UPDATE_REQ = 0x91,
    CMD_PKT_REQ,
    CMD_PKT_RSP,
    CMD_UPDATE_STA,
    CMD_DEV_RESET,
}UPDATE_CMD;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 初始化 HID 库
    if (hid_init()) {
        qDebug() << "Failed to initialize HID library";
        return;
    }

    //初始化连接按钮显示
    ui->btn_connect->setText("连接");
    ui->btn_connect->setStyleSheet("QPushButton { color: white; background-color: red; }");

    // 每 1ms 检查一次收到的数据
    readTimer = new QTimer(this);
    connect(readTimer, &QTimer::timeout, this, &MainWindow::readHIDData);
    readTimer->start(1);

    //搜索设备
    populateUSBDevices();

    // 连接信号和槽
    connect(ui->comboBox_device, SIGNAL(currentIndexChanged(int)),
            this, SLOT(device_connect()));
}

MainWindow::~MainWindow()
{
    if (deviceHandle) {
        hid_close(deviceHandle);
        deviceHandle = nullptr;
    }

    // 停止定时器
    readTimer->stop();

    // 退出 HID 库
    hid_exit();

    delete ui;
}

//搜索hid设备,并显示在下拉框(combox)
void MainWindow::populateUSBDevices()
{
    hid_device_info *devs = hid_enumerate(0, 0);
    if (!devs) {
        qDebug() << "No HID devices found";
        ui->comboBox_device->clear();
        return;
    }

    ui->comboBox_device->clear();

    hid_device_info *dev = devs;
    int index = 1;
    while (dev) {
        QString deviceInfo = QString("%1.%2, VID: 0x%3, PID: 0x%4, Usage Page: 0x%5, Usage: 0x%6")
                                .arg(index++)
                                .arg(QString::fromWCharArray(dev->product_string))
                                .arg(dev->vendor_id, 4, 16, QLatin1Char('0'))
                                .arg(dev->product_id, 4, 16, QLatin1Char('0'))
                                .arg(dev->usage_page, 4, 16, QLatin1Char('0'))
                                .arg(dev->usage, 4, 16, QLatin1Char('0'));

        if (dev->path) {
            ui->comboBox_device->addItem(deviceInfo, QVariant(QString::fromLocal8Bit(dev->path)));
        } else {
            qDebug() << "Device path is null for device:" << QString::fromWCharArray(dev->product_string);
        }

        dev = dev->next;
    }

    hid_free_enumeration(devs);
}


void MainWindow::device_connect()
{
    int res;
    uint8_t reportDescriptor[256]; // 缓冲区大小
    uint8_t data[256];
    QString new_str;

    if (deviceHandle) {
        // 如果已经连接，执行断开逻辑
        hid_close(deviceHandle);
        deviceHandle = nullptr;
        ui->btn_connect->setText("连接");
        ui->btn_connect->setStyleSheet("QPushButton { color: white; background-color: red; }");
        new_str = "设备已断开连接!";
        ui->textEdit_info->append(new_str);
        qDebug() << "Device disconnected";
        return;
    }

    // 获取选中的设备路径
    int currentIndex = ui->comboBox_device->currentIndex();
    if (currentIndex == -1) {
        qDebug() << "No device selected";
        return;
    }

    QString devicePath = ui->comboBox_device->itemData(currentIndex).toString();
    if (devicePath.isEmpty()) {
        qDebug() << "Device path is empty";
        return;
    }

    qDebug() << "Selected device path:" << devicePath;

    // 打开设备
    deviceHandle = hid_open_path(devicePath.toStdString().c_str());
    if (deviceHandle == nullptr) {
        qDebug() << "Failed to open device";
        return;
    }

    ui->btn_connect->setText("已连接");
    ui->btn_connect->setStyleSheet("QPushButton { color: white; background-color: green; }");
    qDebug() << "Device connected";

    // 获取 Report Descriptor
    res = hid_get_report_descriptor(deviceHandle, reportDescriptor, sizeof(reportDescriptor));
    if (res < 0) {
        qDebug() << "Error retrieving Report Descriptor:" << QString::fromWCharArray(hid_error(deviceHandle));

        // 将新信息追加到 textEdit_info 中
        QString new_str = "Report Descriptor获取失败！！";
        ui->textEdit_info->append(new_str);
        return;
    } else {
        //获取HID设备report_id以及可接收的最大数据,有1byte是report_id
        for (int i = 0; i < res; i++) {
            if(reportDescriptor[i] == 0x85){
                reporter_id = reportDescriptor[i+1];
            } else if(reportDescriptor[i] == 0x95){
                reporter_count = reportDescriptor[i+1]+1;
            }
        }
    }

    //先读取一次数据,看看设备是否支持访问,不支持访问的时候直接断开
    hid_set_nonblocking(deviceHandle, 1);  //读数据的时候非阻塞
    res = hid_read(deviceHandle, data, reporter_count);
    if (res < 0) {
        hid_close(deviceHandle);
        deviceHandle = nullptr;
        ui->btn_connect->setText("连接");
        ui->btn_connect->setStyleSheet("QPushButton { color: white; background-color: red; }");
        new_str = "当前连接设备被拒绝访问,请重新连接其他设备!";
        ui->textEdit_info->append(new_str);
        qDebug() << "Device disconnected";

    } else {
        new_str = "设备连接成功!\n";

        // 格式化 reporter_id 和 reporter_count 为十六进制字符串
        new_str += QString("Reporter ID: 0x%1\nReporter Count: 0x%2")
                            .arg(reporter_id, 2, 16, QChar('0'))
                            .arg(reporter_count, 2, 16, QChar('0'));

        ui->textEdit_info->append(new_str);
    }
}

void MainWindow::readHIDData()
{
    if (!deviceHandle) {
        return;
    }

    uint8_t buffer[256];
    int res = hid_read(deviceHandle, buffer, reporter_count);
    if (res == 0){
        return;
    } else if (res > 0) {
        QByteArray data(reinterpret_cast<char *>(buffer), res);
        processHIDData(data, res);
    } else if (res < 0) {
        qDebug() << "Error reading from HID device:" << QString::fromWCharArray(hid_error(deviceHandle));
    }
}

void MainWindow::processHIDData(const QByteArray &data, int len)
{
    qDebug() <<"processHIDData,len:"<<len;
    qDebug() << "Received HID data:" << data.toHex();

    // 解析数据
    int offset = 0;

    // 解析 reporter_id
    uint8_t id = static_cast<uint8_t>(data[offset++]);

    // 解析 cmd
    uint8_t cmd = static_cast<uint8_t>(data[offset++]);

    // 解析 len
    uint8_t payload_len = static_cast<uint8_t>(data[offset++]);

    qDebug() << "Reporter ID:" << id;
    qDebug() << "Command:" << cmd;
    qDebug() << "Length:" << payload_len;

    if (cmd == CMD_PKT_REQ) {
        // 解析 file_addr (4字节)
        uint32_t file_addr = static_cast<uint8_t>(data[offset]) |
                             (static_cast<uint8_t>(data[offset + 1]) << 8) |
                             (static_cast<uint8_t>(data[offset + 2]) << 16) |
                             (static_cast<uint8_t>(data[offset + 3]) << 24);
        offset += 4;

        // 解析 pkt_size (4字节)
        uint32_t pkt_size = static_cast<uint8_t>(data[offset]) |
                            (static_cast<uint8_t>(data[offset + 1]) << 8) |
                            (static_cast<uint8_t>(data[offset + 2]) << 16) |
                            (static_cast<uint8_t>(data[offset + 3]) << 24);
        offset += 4;

        //发数据包给下位机
        packet_response(fot_filePath, fot_file_size, file_addr, pkt_size);

        qDebug() << "Parsed HID Data:";
        qDebug() << "File Address:" << QString("0x%1").arg(file_addr, 8, 16, QChar('0'));
        qDebug() << "PKT SIZE:" << QString("0x%1").arg(pkt_size, 8, 16, QChar('0'));

    }  else if (cmd == CMD_UPDATE_STA) {
        uint8_t update_obj = static_cast<uint8_t>(data[offset]);     // 0: adapter, 1: device
        uint8_t status = static_cast<uint8_t>(data[offset + 1]);     // 0: 升级成功，其他：失败原因

        QString statusMessage;
        if (update_obj == 0) {
            // Adapter
            if (status == 0) {
                statusMessage = "Adapter 升级成功！";
            } else {
                statusMessage = "Adapter 升级失败！";
            }
        } else if (update_obj == 1) {
            // Device
            if (status == 0) {
                statusMessage = "Device 升级成功！";
            } else {
                statusMessage = "Device 升级失败！";
            }
        } else {
            statusMessage = QString("未知升级对象：%1").arg(update_obj);
        }

        QString new_str = QString("\n%1").arg(statusMessage);
        ui->textEdit_info->append(new_str);

        qDebug() << "Update Status:" << statusMessage;
    }  else if (cmd == CMD_DEV_RESET) {
        QString reset_info = "设备已复位!";
        ui->textEdit_info->append(reset_info);
    } else {
        QString cmd_info = QString("RX ERROR CMD: 0x%1").arg(cmd, 2, 16, QChar('0'));
        ui->textEdit_info->append(cmd_info);
    }
}


void MainWindow::packet_response(QString filePath, uint32_t file_size, uint32_t file_addr, uint32_t paket_size)
{
    if (file_size == 0) {
        qDebug() << "Invalid file size.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << filePath;
        return;
    }

    // 移动到偏移地址
    if (!file.seek(file_addr)) {
        qDebug() << "Failed to seek to file address:" << file_addr;
        file.close();
        return;
    }

    uint8_t data[256];     // HID 数据包缓冲区
    data[0] = reporter_id; // reporter_id
    data[1] = CMD_PKT_RSP; // 数据包相应CMD

    uint32_t bytesRemaining = paket_size; // 剩余要发送的字节数
    uint32_t bytesSent = file_addr; // 已发送的字节数（初始化为偏移地址）

    while (bytesRemaining > 0) {
        // 计算本次发送的 payload 长度
        uint8_t payload_len = qMin(bytesRemaining, static_cast<uint32_t>(reporter_count - 3));
        data[2] = payload_len; // 设置 payload_len

        // 读取文件数据到缓冲区
        QByteArray fileData = file.read(payload_len);
        if (fileData.size() != payload_len) {
            qDebug() << "Failed to read expected bytes from file.";
            break;
        }

        // 填充数据包的 payload
        memcpy(&data[3], fileData.data(), payload_len);

        // 发送数据包
        int res = hid_write(deviceHandle, data, reporter_count);
        if (res == -1) {
            qDebug() << "Failed to send data:" << hid_error(deviceHandle);
            break;
        }

        // 更新已发送字节数和剩余字节数
        bytesSent += payload_len;
        bytesRemaining -= payload_len;

        // 更新进度条
        float progress = (static_cast<float>(bytesSent) / file_size) * 100.0f;
        ui->update_progressBar->setValue(static_cast<int>(progress));
        qDebug() << QString("Progress: %1%").arg(progress, 0, 'f', 1); // 保留 1 位小数

        // 调试信息
        qDebug() << "Sent packet:" << QString("reporter_id=%1, CMD=%2, payload_len=%3")
                                            .arg(data[0])
                                            .arg(data[1])
                                            .arg(data[2]);
    }

    file.close();
    qDebug() << "File data sent successfully.";
}

void MainWindow::on_btn_connect_clicked()
{
    device_connect();
}

void MainWindow::on_btn_get_file_clicked()
{
    fot_filePath = QFileDialog::getOpenFileName(nullptr, "打开文件", "", "所有文件 (*.*)");

    if (fot_filePath.isEmpty()) {
        fot_file_size = 0;
        fot_file_hash = 0;
        QMessageBox::information(this, "提示", "读取文件失败！");
        return;
    }

    QFile file(fot_filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << fot_filePath;
        return;
    }

    // 获取文件大小
    qint64 size = file.size();
    if (size > std::numeric_limits<uint32_t>::max()) {
        qDebug() << "File size exceeds uint32_t limit.";
        return;
    }
    fot_file_size = static_cast<uint32_t>(size);


    // 计算文件哈希值
    QCryptographicHash hash(QCryptographicHash::Md5); // 使用 MD5 哈希
    if (!hash.addData(&file)) {
        qDebug() << "Failed to hash file data.";
        return;
    }

    QByteArray hashResult = hash.result();
    if (hashResult.size() < 4) {
        qDebug() << "Hash result is too short.";
        return;
    }

    // 提取前4字节作为 uint32_t 的哈希值
    fot_file_hash = static_cast<uint32_t>(
        (static_cast<uint8_t>(hashResult[0]) << 24) |
        (static_cast<uint8_t>(hashResult[1]) << 16) |
        (static_cast<uint8_t>(hashResult[2]) << 8)  |
        (static_cast<uint8_t>(hashResult[3]))
    );

    file.close();

    // 格式化新信息
    QString new_info = QString("File Path: %1\nFile Size: 0x%2\nFile Hash: 0x%3")
                        .arg(fot_filePath)
                        .arg(QString("%1").arg(fot_file_size, 8, 16, QChar('0')).toUpper()) // 16进制文件大小
                        .arg(QString("%1").arg(fot_file_hash, 8, 16, QChar('0')).toUpper()); // 16进制文件哈希

    // 追加新信息到原有内容
    new_info += "\n";
    ui->textEdit_info->append(new_info);

//    qDebug() << "File Size (Hex):" << QString("0x%1").arg(fot_file_size, 8, 16, QChar('0')).toUpper();
//    qDebug() << "File Hash (Hex):" << QString("0x%1").arg(fot_file_hash, 8, 16, QChar('0')).toUpper();
}

//升级adapter端按钮
void MainWindow::on_btn_update_adpt_clicked()
{
    uint8_t data[128] = {0};

    if(!deviceHandle){
        QMessageBox::information(this, "提示", "设备未连接!");
        return;
    }

    if(fot_filePath.isEmpty()){
        QMessageBox::information(this, "提示", "未导入升级文件!");
        return;
    }


    // 获取文件名（从完整路径中提取文件名）
    QFileInfo fileInfo(fot_filePath);
    QString fileName = fileInfo.fileName(); // 获取文件名

    // 弹出确认框提示文件名
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认升级文件",
                                  QString("升级文件名为: %1\n是否继续？").arg(fileName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        // 如果用户点击取消，直接返回
        return;
    }

    int offset = 0;
    data[offset++] = reporter_id;
    data[offset++] = CMD_UPDATE_REQ;
    data[offset++] = 4+4+1;

    // 添加 file_size (4字节)
    data[offset++] = (fot_file_size >> 0) & 0xFF;
    data[offset++] = (fot_file_size >> 8) & 0xFF;
    data[offset++] = (fot_file_size >> 16) & 0xFF;
    data[offset++] = (fot_file_size >> 24) & 0xFF;

    // 添加 file_hash (4字节)
    data[offset++] = (fot_file_hash >> 0) & 0xFF;
    data[offset++] = (fot_file_hash >> 8) & 0xFF;
    data[offset++] = (fot_file_hash >> 16) & 0xFF;
    data[offset++] = (fot_file_hash >> 24) & 0xFF;

    data[offset++] = 0; //update_obj,0:adapter,1:device

    // 发送数据
    int res = hid_write(deviceHandle, data, reporter_count);
    if (res == -1) {
        qDebug() << "Failed to send data:" << hid_error(deviceHandle);
    } else {
        qDebug() << "Data sent successfully, bytes:" << res;
    }
}

//升级device端按钮
void MainWindow::on_btn_update_dev_clicked()
{
    uint8_t data[128] = {0};

    if(!deviceHandle){
        QMessageBox::information(this, "提示", "设备未连接!");
        return;
    }

    if(fot_filePath.isEmpty()){
        QMessageBox::information(this, "提示", "未导入升级文件!");
        return;
    }

    // 获取文件名（从完整路径中提取文件名）
    QFileInfo fileInfo(fot_filePath);
    QString fileName = fileInfo.fileName(); // 获取文件名

    // 弹出确认框提示文件名
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认升级文件",
                                  QString("升级文件名为: %1\n是否继续？").arg(fileName),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        // 如果用户点击取消，直接返回
        return;
    }

    int offset = 0;
    data[offset++] = reporter_id;
    data[offset++] = CMD_UPDATE_REQ;
    data[offset++] = 4+4+1;

    // 添加 file_size (4字节)
    data[offset++] = (fot_file_size >> 0) & 0xFF;
    data[offset++] = (fot_file_size >> 8) & 0xFF;
    data[offset++] = (fot_file_size >> 16) & 0xFF;
    data[offset++] = (fot_file_size >> 24) & 0xFF;

    // 添加 file_hash (4字节)
    data[offset++] = (fot_file_hash >> 0) & 0xFF;
    data[offset++] = (fot_file_hash >> 8) & 0xFF;
    data[offset++] = (fot_file_hash >> 16) & 0xFF;
    data[offset++] = (fot_file_hash >> 24) & 0xFF;

    data[offset++] = 1; //update_obj,0:adapter,1:device

    // 发送数据
    int res = hid_write(deviceHandle, data, reporter_count);
    if (res == -1) {
        qDebug() << "Failed to send data:" << hid_error(deviceHandle);
    } else {
        qDebug() << "Data sent successfully, bytes:" << res;
    }
}

//设备复位按钮
void MainWindow::on_btn_dev_reset_clicked()
{
    uint8_t data[128] = {0};

    if(!deviceHandle){
        QMessageBox::information(this, "提示", "设备未连接!");
        return;
    }

    // 弹出确认框提示文件名
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "设备复位",
                                  QString("是否确认设备复位？"),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        // 如果用户点击取消，直接返回
        return;
    }

    int offset = 0;
    data[offset++] = reporter_id;
    data[offset++] = CMD_DEV_RESET;
    data[offset++] = 0;

    // 发送数据
    int res = hid_write(deviceHandle, data, reporter_count);
    if (res == -1) {
        qDebug() << "Failed to send data:" << hid_error(deviceHandle);
    } else {
        qDebug() << "Data sent successfully, bytes:" << res;
    }
}
