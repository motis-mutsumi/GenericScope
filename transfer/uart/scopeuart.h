#ifndef SCOPEUART_H
#define SCOPEUART_H

#include "scopecontroltransfer.h"
#include "windows.h"
#include <string>
#include <mutex>
#include <shared_mutex>
#include <functional>

#pragma pack(push, 1)
struct UartInfo
{
    std::string port_name;
    DCB dcb;
    bool async = false;
};
#pragma pack(pop)
class TRANSFER_EXPORT ScopeUart : public ScopeControlTransfer
{
public:
    ScopeUart();
    ~ScopeUart();

    // override ScopeTransferBasic
    virtual ScopeTransferStatus open() override;
    virtual ScopeTransferStatus close() override;

    // override ScopeControlTransfer
    virtual ScopeTransferStatus readData(uint8_t *data, uint32_t read_len, uint8_t *cmd, int write_len) override;

    void setUartInfo(const UartInfo &info) noexcept
    {
        std::unique_lock<std::shared_mutex> lock(m_dataMutex);
        m_infoData = info;
    }

    UartInfo getUartInfo() const noexcept
    {
        std::shared_lock<std::shared_mutex> lock(m_dataMutex);
        return m_infoData;
    }

    void setDataCallBackFunction(std::function<void(uint8_t*,size_t)> func){
          m_xferCallBackFunction = func;
     }

    ScopeTransferStatus writeData(uint8_t *data, uint32_t write_len);
    ScopeTransferStatus readData(uint8_t *data, uint32_t read_len);

private:
    mutable std::shared_mutex m_dataMutex;
    UartInfo m_infoData;
    HANDLE m_hcom = INVALID_HANDLE_VALUE;
    HANDLE m_hthread = INVALID_HANDLE_VALUE;
    OVERLAPPED m_ovWrite;
    OVERLAPPED m_ovRead;

    std::function<void(uint8_t*, size_t)> m_xferCallBackFunction;
    friend unsigned int __stdcall comRecv(void* param);
};

#endif // SCOPEUART_H
