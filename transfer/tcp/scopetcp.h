#ifndef SCOPETCP_H
#define SCOPETCP_H

#include "scopeimagetransfer.h"
#include "scopecontroltransfer.h"
#include <shared_mutex>
#include <string>

#pragma pack(push, 1)
struct TcpInfo
{
    std::string ip;
    uint16_t port;
};
#pragma pack(pop)
class TRANSFER_EXPORT ScopeTcp : public ScopeImageTransfer, public ScopeControlTransfer
{
public:
    ScopeTcp();

    // override ScopeTransferBasic
    virtual ScopeTransferStatus open() override;
    virtual ScopeTransferStatus close() override;

    // override ScopeControlTransfer
    virtual ScopeTransferStatus readData(uint8_t *data, uint32_t read_len, uint8_t *cmd = nullptr, int write_len = 0) override;

    // override ScopeImageTransfer
    virtual ScopeTransferStatus startStream() override;
    virtual ScopeTransferStatus stopStream() override;

    void setTcpInfo(const TcpInfo &info) noexcept
    {
        std::unique_lock<std::shared_mutex> lock(m_dataMutex);
        m_infoData = info;
    }
    TcpInfo getTcpInfo() const noexcept
    {
        std::shared_lock<std::shared_mutex> lock(m_dataMutex);
        return m_infoData;
    }

private:
    int initTcp();
    int deinitTcp();
    int openClientSocket();
    int closeClientSocket();

private:
    mutable std::shared_mutex m_dataMutex;
    std::mutex m_clientSocketMutex;
    uint64_t m_clientSocket = NULL;
    std::atomic<bool> m_clientSocketOpen = false;
    TcpInfo m_infoData;
};

#endif // SCOPETCP_H
