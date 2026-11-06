#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDebug>
#include <QTimer>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>

#include <hidapi/hidapi.h>
#include <libusb/libusb.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void device_connect(); //连接设备
    void readHIDData();    // 定时读取数据
    void processHIDData(const QByteArray &data, int len); // 处理接收到的数据
    void packet_response(QString filePath, uint32_t file_size, uint32_t file_addr, uint32_t paket_size);

    void on_btn_connect_clicked();

    void on_btn_get_file_clicked();

    void on_btn_update_adpt_clicked();

    void on_btn_update_dev_clicked();

    void on_btn_dev_reset_clicked();

private:
    Ui::MainWindow *ui;

    void populateUSBDevices(); // 枚举所有 HID 设备并显示到 QComboBox

    //HID设备句柄
    hid_device *deviceHandle = nullptr;
    //HID报告描述符相关
    size_t reporter_count;
    uint8_t reporter_id;

    //hid定时读取数据
    QTimer *readTimer; // 用于定时读取数据

    //FOT文件路径
    QString fot_filePath;
    uint32_t fot_file_size;
    uint32_t fot_file_hash;

};
#endif // MAINWINDOW_H
