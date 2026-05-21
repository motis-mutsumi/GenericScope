#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include "transfer/scopetransferbasic.h"
#include "transfer/scopecontroltransfer.h"
#include "transfer/uart/scopeuart.h"
#include "transfer/udp/scopeudp.h"

/**
 * @brief 设备管理器
 *
 * 管理传感器设备的连接和数据采集，支持串口/UDP 切换
 */
class DeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    bool connectDevice();
    void disconnectDevice();
    bool isConnected() const;
    bool sendCommand(const QByteArray &command);
    void startPolling();
    void stopPolling();

signals:
    void connectionChanged(bool connected);
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);

private slots:
    void onTransferStatusChanged(ScopeTransferStatus status);
    void onTransferDataReceived(const QByteArray &data);
    void onTransferError(const QString &error);
    void onPollingDataReceived(const QByteArray &data);

private:
    bool connectUart();
    bool connectUdp();

private:
    ScopeTransferBasic   *m_transfer;
    ScopeControlTransfer *m_controlTransfer;
};

#endif // DEVICEMANAGER_H
