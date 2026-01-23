#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include "transfer/scopetransferbasic.h"
#include "transfer/scopecontroltransfer.h"

/**
 * @brief 设备管理器
 *
 * 管理传感器设备的连接和数据采集
 */
class DeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    /**
     * @brief 连接设备
     * @return 是否成功
     */
    bool connectDevice();

    /**
     * @brief 断开设备
     */
    void disconnectDevice();

    /**
     * @brief 检查是否已连接
     * @return 连接状态
     */
    bool isConnected() const;

    /**
     * @brief 发送命令
     * @param command 命令数据
     * @return 响应数据
     */
    QByteArray sendCommand(const QByteArray &command);

    /**
     * @brief 启动自动轮询
     */
    void startPolling();

    /**
     * @brief 停止自动轮询
     */
    void stopPolling();

signals:
    /**
     * @brief 连接状态改变信号
     * @param connected 是否已连接
     */
    void connectionChanged(bool connected);

    /**
     * @brief 数据接收信号
     * @param data 接收到的数据
     */
    void dataReceived(const QByteArray &data);

    /**
     * @brief 错误信号
     * @param error 错误信息
     */
    void errorOccurred(const QString &error);

private slots:
    void onTransferStatusChanged(ScopeTransferStatus status);
    void onTransferDataReceived(const QByteArray &data);
    void onTransferError(const QString &error);
    void onPollingDataReceived(const QByteArray &data);

private:
    ScopeTransferBasic *m_transfer;
    ScopeControlTransfer *m_controlTransfer;
};

#endif // DEVICEMANAGER_H
