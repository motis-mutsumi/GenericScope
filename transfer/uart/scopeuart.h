#ifndef SCOPEUART_H
#define SCOPEUART_H

#include "scopecontroltransfer.h"
#include "windows.h"
#include <string>
#include <mutex>
#include <shared_mutex>
#include <functional>
#include <QSharedPointer>
#include <QByteArray>

// 前向声明
class ProtocolParser;
struct ParseResult;

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

    // ========================================================================
    // 协议解析相关接口 (新增)
    // ========================================================================

    /**
     * @brief 设置协议解析器
     * @param protocolName 协议名称（从ProtocolManager加载）
     */
    void setProtocol(const QString &protocolName);

    /**
     * @brief 获取当前协议名称
     */
    QString currentProtocol() const;

    /**
     * @brief 设置协议解析结果回调
     * @param callback 回调函数
     */
    void setParseResultCallback(std::function<void(const ParseResult&)> callback);

    /**
     * @brief 获取接收缓冲区大小
     */
    size_t getBufferSize() const;

    /**
     * @brief 清空接收缓冲区
     */
    void clearBuffer();

private:
    mutable std::shared_mutex m_dataMutex;
    UartInfo m_infoData;
    HANDLE m_hcom = INVALID_HANDLE_VALUE;
    HANDLE m_hthread = INVALID_HANDLE_VALUE;
    OVERLAPPED m_ovWrite;
    OVERLAPPED m_ovRead;

    std::function<void(uint8_t*, size_t)> m_xferCallBackFunction;

    // 协议解析相关成员变量 (新增)
    QSharedPointer<ProtocolParser> m_parser;           // 协议解析器
    QString m_currentProtocolName;                     // 当前协议名称
    QByteArray m_receiveBuffer;                        // 接收缓冲区
    std::function<void(const ParseResult&)> m_parseResultCallback;  // 解析结果回调
    mutable std::mutex m_bufferMutex;                  // 缓冲区互斥锁

    friend unsigned int __stdcall comRecv(void* param);
};

#endif // SCOPEUART_H
