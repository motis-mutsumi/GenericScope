#ifndef SCOPEUDP_H
#define SCOPEUDP_H

#include "scopecontroltransfer.h"
#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <functional>
#include <shared_mutex>
#include <string>

#pragma pack(push, 1)
struct UdpInfo
{
    std::string remoteIp;
    uint16_t    remotePort = 0;
    uint16_t    localPort  = 0;   // 本地监听端口
};
#pragma pack(pop)

/**
 * @brief UDP 传输层
 *
 * 使用 Qt QUdpSocket 实现异步 UDP 接收，
 * 接收到数据后通过回调透传给上层（与 ScopeUart 接口一致）。
 *
 * 注意：QUdpSocket 必须在同一线程中创建和使用（Qt 信号槽机制），
 * 因此 ScopeUdp 继承 QObject，open() 在主线程调用。
 */
class TRANSFER_EXPORT ScopeUdp : public QObject, public ScopeControlTransfer
{
    Q_OBJECT

public:
    explicit ScopeUdp(QObject *parent = nullptr);
    ~ScopeUdp();

    // override ScopeTransferBasic
    virtual ScopeTransferStatus open()  override;
    virtual ScopeTransferStatus close() override;

    // override ScopeControlTransfer
    virtual ScopeTransferStatus readData(uint8_t *data, uint32_t read_len,
                                          uint8_t *cmd = nullptr, int write_len = 0) override;

    void setUdpInfo(const UdpInfo &info) noexcept
    {
        std::unique_lock<std::shared_mutex> lock(m_dataMutex);
        m_infoData = info;
    }

    UdpInfo getUdpInfo() const noexcept
    {
        std::shared_lock<std::shared_mutex> lock(m_dataMutex);
        return m_infoData;
    }

    ScopeTransferStatus writeData(const uint8_t *data, uint32_t write_len);

    void setDataCallBackFunction(std::function<void(uint8_t*, size_t)> func)
    {
        m_dataCallback = func;
    }

private slots:
    void onReadyRead();

private:
    mutable std::shared_mutex m_dataMutex;
    UdpInfo   m_infoData;
    QUdpSocket *m_socket = nullptr;
    std::function<void(uint8_t*, size_t)> m_dataCallback;
};

#endif // SCOPEUDP_H
