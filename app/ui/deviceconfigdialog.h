#ifndef DEVICECONFIGDIALOG_H
#define DEVICECONFIGDIALOG_H

#include <QDialog>

class DeviceManager;

namespace Ui {
class DeviceConfigDialog;
}

/**
 * @brief 设备配置对话框
 *
 * 参考 DS_RVision 设备配置界面
 * 功能包括：
 * - 设备基本信息查询
 * - 通信参数配置
 * - 传感器参数配置
 * - 设备校准和重置
 */
class DeviceConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceConfigDialog(DeviceManager *deviceManager, QWidget *parent = nullptr);
    ~DeviceConfigDialog();

private slots:
    // 工具栏操作
    void onRefresh();
    void onHorizontalCalibration();
    void onReset();
    void onRestart();

    // 协议切换
    void onProtocolChanged();

    // 参数写入
    void onWriteOutputParams();
    void onWriteGyroParams();
    void onWriteAccelParams();

    // 设备连接状态变化
    void onDeviceConnectionChanged(bool connected);

private:
    void setupConnections();
    void loadDeviceInfo();
    void updateConnectionStatus(bool connected);

    Ui::DeviceConfigDialog *ui;
    DeviceManager *m_deviceManager;
    bool m_isConnected;
};

#endif // DEVICECONFIGDIALOG_H
