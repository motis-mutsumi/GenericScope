#include "scopeuart.h"
#include "protocol/protocolparser.h"
#include "protocol/protocolmanager.h"
#include <iostream>
#include <process.h>
#include <QDebug>

using namespace std;

namespace {
void closeWinHandle(HANDLE &handle)
{
    if (handle != NULL && handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(handle);
    }
    handle = INVALID_HANDLE_VALUE;
}

void closeEventHandle(HANDLE &handle)
{
    if (handle != NULL)
    {
        CloseHandle(handle);
    }
    handle = NULL;
}
}

ScopeUart::ScopeUart()
{
}

ScopeUart::~ScopeUart()
{
    close();
}

// ============================================================================
// 协议解析相关实现 (新增)
// ============================================================================

void ScopeUart::setProtocol(const QString &protocolName)
{
    ProtocolManager *manager = ProtocolManager::instance();

    if (!manager->hasProtocol(protocolName)) {
        qWarning() << "ScopeUart: Protocol not found:" << protocolName;
        return;
    }

    // 创建协议解析器
    m_parser = manager->createParser(protocolName);
    m_currentProtocolName = protocolName;

    qDebug() << "ScopeUart: Protocol set to" << protocolName;
}

QString ScopeUart::currentProtocol() const
{
    return m_currentProtocolName;
}

void ScopeUart::setParseResultCallback(std::function<void(const ParseResult&)> callback)
{
    m_parseResultCallback = callback;
}

size_t ScopeUart::getBufferSize() const
{
    std::lock_guard<std::mutex> lock(m_bufferMutex);
    return m_receiveBuffer.size();
}

void ScopeUart::clearBuffer()
{
    std::lock_guard<std::mutex> lock(m_bufferMutex);
    m_receiveBuffer.clear();
    qDebug() << "ScopeUart: Receive buffer cleared";
}

ScopeTransferStatus ScopeUart::open()
{
    if (m_open)
    {
        return Ok;
    }

    const UartInfo info_data = getUartInfo();

    // Windows串口命名规则：COM10及以上需要使用 "\\\\.\\COMN" 格式
    std::string portName = info_data.port_name;
    if (portName.find("\\\\.\\") == std::string::npos) {
        // 如果端口名不包含 "\\\\.\\", 则添加前缀
        portName = "\\\\.\\" + portName;
    }

    if (info_data.async)
    {
        m_hcom = CreateFileA(
               portName.c_str(),
               GENERIC_READ | GENERIC_WRITE,
               0,
               NULL,
               OPEN_EXISTING,
               FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
               NULL);

    }
    else
    {
        m_hcom = CreateFileA(portName.c_str(),
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
    }

    if (m_hcom == INVALID_HANDLE_VALUE)
    {
        return Error;
    }

    // 设置超时
    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 200;
    timeouts.ReadTotalTimeoutMultiplier = 100;
    timeouts.ReadTotalTimeoutConstant = 5000;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    if (!SetCommTimeouts(m_hcom, &timeouts))
    {
        goto err;
    }

    // 配置串口参数
    DCB dcb = {0};
    if (!GetCommState(m_hcom, &dcb))
    {
        goto err;
    }

    dcb.BaudRate = info_data.dcb.BaudRate;
    dcb.ByteSize = info_data.dcb.ByteSize;
    dcb.Parity = info_data.dcb.Parity;
    dcb.StopBits = info_data.dcb.StopBits;
    if (!SetCommState(m_hcom, &dcb))
    {
        goto err;
    }

    if (info_data.async)
    {
        ZeroMemory(&m_ovWrite, sizeof(m_ovWrite));
        m_ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (m_ovWrite.hEvent == NULL)
        {
            goto err;
        }

        ZeroMemory(&m_ovRead, sizeof(m_ovRead));
        m_ovRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (m_ovRead.hEvent == NULL)
        {
            goto err;
        }

        m_hthread = (HANDLE)_beginthreadex(NULL, 0, &comRecv, this, 0, NULL);
        if (m_hthread == NULL || m_hthread == INVALID_HANDLE_VALUE)
        {
            goto err;
        }

    }

    m_open = true;
    return Ok;

err:
    closeEventHandle(m_ovWrite.hEvent);
    closeEventHandle(m_ovRead.hEvent);
    closeWinHandle(m_hthread);
    closeWinHandle(m_hcom);
    return Error;
}

ScopeTransferStatus ScopeUart::close()
{
    m_open = false;

    if (m_hcom != INVALID_HANDLE_VALUE)
    {
        PurgeComm(m_hcom, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
    }

    if (m_hthread != NULL && m_hthread != INVALID_HANDLE_VALUE)
    {
            WaitForSingleObject(m_hthread, 3000);//等待线程结束
    }
    closeWinHandle(m_hthread);

    closeEventHandle(m_ovWrite.hEvent);
    closeEventHandle(m_ovRead.hEvent);
    closeWinHandle(m_hcom);

    return Ok;
}

ScopeTransferStatus ScopeUart::readData(uint8_t *rx_data, uint32_t read_len, uint8_t *cmd, int write_len)
{
    if (!m_open)
    {
        return Error;
    }

    if (write_len > 0 && cmd)
    {
        const ScopeTransferStatus writeStatus = writeData(cmd, write_len);
        if (writeStatus != Ok)
        {
            return writeStatus;
        }
    }

    const UartInfo info_data = getUartInfo();

    if (!info_data.async)
    {
        return readData(rx_data, read_len);
    }

    return Ok;
}

ScopeTransferStatus ScopeUart::writeData(uint8_t *data, uint32_t write_len)
{
    if (!m_open || m_hcom == INVALID_HANDLE_VALUE || data == nullptr || write_len == 0)
    {
        return Error;
    }

    DWORD dw_send = 0;
    PurgeComm(m_hcom, PURGE_TXCLEAR | PURGE_TXABORT);

    const bool isAsync = getUartInfo().async;
    if (isAsync)
    {
        DWORD dw_error;

        if (ClearCommError(m_hcom, &dw_error, NULL))
        {
            PurgeComm(m_hcom, PURGE_TXABORT | PURGE_TXCLEAR);
        }

        if (!WriteFile(m_hcom, data, (DWORD)write_len, &dw_send, &m_ovWrite))
        {
            if (GetLastError() == ERROR_IO_PENDING)
            {
                while (!GetOverlappedResult(m_hcom, &m_ovWrite, &dw_send, FALSE))
                {
                    if (GetLastError() == ERROR_IO_INCOMPLETE)
                    {
                        continue;
                    }
                    else
                    {
                        ClearCommError(m_hcom, &dw_error, NULL);
                        return Error;
                    }
                }
            }
            else
            {
                ClearCommError(m_hcom, &dw_error, NULL);
                return Error;
            }
        }

        if (dw_send != write_len)
        {
            return Error;
        }
    }
    else
    {
        uint32_t total_send = 0;
        while (total_send < write_len)
        {
            const bool writeOk = WriteFile(m_hcom, data + total_send, write_len - total_send, &dw_send, NULL);
            if (!writeOk)
            {
                cout << "send failed!" << endl;
                Sleep(10);
                break;
            }
            else
            {
                total_send += dw_send;
                if (total_send < write_len)
                    Sleep(1);
            }
        }

        if (total_send != write_len)
        {
            return Error;
        }
    }

    return Ok;
}

ScopeTransferStatus ScopeUart::readData(uint8_t *data, uint32_t len)
{
    DWORD read_len = 0;
    DWORD total_len = len;

    while (len != 0)
    {
        read_len = 0;
        bool ret = ReadFile(m_hcom, (void *)(data + total_len - len), len, &read_len, NULL);
        if (ret != TRUE)
        {
            return Error;
        }

        if (read_len > len || read_len == 0) // TODO
        {
            return Error;
        }

        len -= read_len;
    }

    return len == 0 ? Ok : Error;
}

unsigned int __stdcall comRecv(void* param)
{
    if (param == nullptr)
    {
        return 0;
    }

    // 新实现：底层串口线程仅做“原始字节透传”，不再按旧0x55协议预拆包。
    // 之前的预拆包会破坏当前协议（例如帧头 AA 55 01 01 22 00）并导致上层CRC大量误报。
    {
        ScopeUart *objRaw = static_cast<ScopeUart*>(param);
        auto &ov_read = objRaw->m_ovRead;
        auto &h_com = objRaw->m_hcom;

        static const DWORD kReadChunkSize = 512;
        char readBuf[kReadChunkSize];
        DWORD dw_error = 0;

        ClearCommError(h_com, &dw_error, NULL);

        while (objRaw->m_open)
        {
            DWORD dw_read = 0;
            ResetEvent(ov_read.hEvent);

            if (!ReadFile(h_com, readBuf, kReadChunkSize, &dw_read, &ov_read))
            {
                if ((dw_error = GetLastError()) == ERROR_IO_PENDING)
                {
                    while (objRaw->m_open && !GetOverlappedResult(h_com, &ov_read, &dw_read, FALSE))
                    {
                        if ((dw_error = GetLastError()) == ERROR_IO_INCOMPLETE)
                        {
                            std::this_thread::sleep_for(std::chrono::milliseconds(1));
                            continue;
                        }
                        ClearCommError(h_com, &dw_error, NULL);
                        dw_read = 0;
                        break;
                    }
                }
                else
                {
                    ClearCommError(h_com, &dw_error, NULL);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                }
            }

            if (!objRaw->m_open)
            {
                return 0;
            }

            if (dw_read == 0)
            {
                continue;
            }

            if (objRaw->m_xferCallBackFunction != NULL)
            {
                objRaw->m_xferCallBackFunction(reinterpret_cast<uint8_t *>(readBuf), dw_read);
            }

            if (objRaw->m_parser && objRaw->m_parseResultCallback)
            {
                QByteArray newData(readBuf, static_cast<int>(dw_read));
                {
                    std::lock_guard<std::mutex> lock(objRaw->m_bufferMutex);
                    objRaw->m_receiveBuffer.append(newData);
                }

                bool continueParsing = true;
                while (continueParsing)
                {
                    QByteArray dataToParse;
                    {
                        std::lock_guard<std::mutex> lock(objRaw->m_bufferMutex);
                        dataToParse = objRaw->m_receiveBuffer;
                    }

                    if (dataToParse.isEmpty()) {
                        break;
                    }

                    ParseResult result = objRaw->m_parser->parse(dataToParse);
                    if (result.success)
                    {
                        objRaw->m_parseResultCallback(result);

                        std::lock_guard<std::mutex> lock(objRaw->m_bufferMutex);
                        if (result.consumedBytes > 0 && result.consumedBytes <= objRaw->m_receiveBuffer.size())
                        {
                            objRaw->m_receiveBuffer.remove(0, result.consumedBytes);
                        }
                        else
                        {
                            qWarning() << "ScopeUart: Invalid consumedBytes:" << result.consumedBytes
                                       << ", buffer size:" << objRaw->m_receiveBuffer.size();
                            objRaw->m_receiveBuffer.clear();
                            continueParsing = false;
                        }
                    }
                    else
                    {
                        continueParsing = false;

                        std::lock_guard<std::mutex> lock(objRaw->m_bufferMutex);
                        static constexpr int kMaxBufferSize = 4096;
                        if (objRaw->m_receiveBuffer.size() > kMaxBufferSize)
                        {
                            qWarning() << "ScopeUart: Buffer overflow ("
                                       << objRaw->m_receiveBuffer.size() << " bytes), clearing old data";
                            objRaw->m_receiveBuffer.clear();
                        }
                    }
                }
            }
        }

        return 0;
    }
}
