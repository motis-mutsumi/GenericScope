#include "scopeuart.h"
#include "protocol/protocolparser.h"
#include "protocol/protocolmanager.h"
#include <iostream>
#include <process.h>
#include <QDebug>

using namespace std;

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

    UartInfo info_data = getUartInfo();

    // Windows串口命名规则：COM10及以上需要使用 "\\\\.\\COMN" 格式
    std::string portName = info_data.port_name;
    if (portName.find("\\\\.\\") == std::string::npos) {
        // 如果端口名不包含 "\\\\.\\", 则添加前缀
        portName = "\\\\.\\" + portName;
    }

    if(info_data.async)
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
    SetCommTimeouts(m_hcom, &timeouts);

    // 配置串口参数
    DCB dcb;
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

    if(info_data.async)
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
            CloseHandle(m_ovWrite.hEvent);
            m_ovWrite.hEvent = NULL;
            goto err;
        }

        m_hthread = (HANDLE)_beginthreadex(NULL, 0, &comRecv, this, 0, NULL);
        if(m_hthread != INVALID_HANDLE_VALUE)
        {
            m_open = true;
            return Ok;
        }
        else
        {
            CloseHandle(m_hcom);
            return Error;
        }

    }

    m_open = true;
    return Ok;

err:
    CloseHandle(m_hcom);
    return Error;
}

ScopeTransferStatus ScopeUart::close()
{
    if (m_open)
    {
        m_open = false;

        if (m_hcom != INVALID_HANDLE_VALUE)
        {
            PurgeComm(m_hcom, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
        }

        if (NULL != m_hthread)
        {
            WaitForSingleObject(m_hthread, 3000);//等待线程结束
            CloseHandle(m_hthread);
            m_hthread = NULL;
        }

        if (NULL != m_ovWrite.hEvent)
        {
            CloseHandle(m_ovWrite.hEvent);
            m_ovWrite.hEvent = NULL;
        }
        if (NULL != m_ovRead.hEvent)
        {
            CloseHandle(m_ovRead.hEvent);
            m_ovRead.hEvent = NULL;
        }

        if (m_hcom != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_hcom);
            m_hcom = INVALID_HANDLE_VALUE;
        }
    }

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
        ScopeTransferStatus ret = writeData(cmd, write_len);
        if (ret != Ok)
        {
            return ret;
        }
    }

    ScopeTransferStatus status = Ok;
    UartInfo info_data = getUartInfo();

    if (!info_data.async)
    {
        return readData(rx_data, read_len);
    }

    return status;
}

ScopeTransferStatus ScopeUart::writeData(uint8_t *data, uint32_t write_len)
{
    if (!m_open || m_hcom == INVALID_HANDLE_VALUE || data == nullptr || write_len == 0)
    {
        return Error;
    }

    DWORD dw_send = 0;
    PurgeComm(m_hcom, PURGE_TXCLEAR | PURGE_TXABORT);

    if(getUartInfo().async)
    {
        DWORD dw_send = 0;
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
            bool flag = WriteFile(m_hcom, data + total_send, write_len - total_send, &dw_send, NULL);
            if (!flag)
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
    static const uint32_t buffer_size = 100;
    static const unsigned char frame_header = {0x55};
    static const unsigned char length_padding = 1;
    static const uint16_t header_len = 1;
    static const uint16_t crc_len  = 2;
    ScopeUart *obj = static_cast<ScopeUart*>(param);

    auto& ov_read = obj->m_ovRead;
    auto& h_com = obj->m_hcom;

    DWORD dw_error;

    char *p_read_buf = (char *)malloc(buffer_size);
    char *p_next = p_read_buf;

    if (p_read_buf == NULL)
    {
        obj->m_open = FALSE;
        return 0;
    }

    // 数据接收
    DWORD dw_want_read = 1;
    DWORD dw_read = 0;

    if (ClearCommError(h_com, &dw_error, NULL))
    {
        PurgeComm(h_com, PURGE_TXCLEAR | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_RXABORT);
    }

    enum ReadType{
        Header,
        Length,
        Body
    };

    ReadType read_type = Header;

    auto reset_param = [&](){
        p_next = p_read_buf;
        dw_want_read = 1;
        read_type = Header;
    };

    memset(p_read_buf, 0, buffer_size);
    while (obj->m_open)
    {
        bool success_ = true;

        if (!ReadFile(h_com, p_next, dw_want_read, &dw_read, &ov_read))
        {
            if ((dw_error = GetLastError()) == ERROR_IO_PENDING)
            {
                while(!GetOverlappedResult(h_com, &ov_read, &dw_read, FALSE))
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(50));
                    if(!obj->m_open)
                    {
                        success_ = false;
                        free(p_read_buf);
                        return 0;
                    }

                    if ((dw_error = GetLastError()) == ERROR_IO_INCOMPLETE)
                    {
                        continue;
                    }
                    else
                    {
                        success_ = false;
                        ClearCommError(h_com, &dw_error, NULL);
                        break;
                    }
                }

            }
            else
            {
                success_ = false;
                ClearCommError(h_com, &dw_error, NULL);
                break;
            }
        }

        if(!obj->m_open)
        {
            success_ = false;
            free(p_read_buf);
            return 0;
        }

        if(success_ && dw_read == dw_want_read)
        {
            if (dw_read == 1 && read_type == Header)
            {
                if (((p_next[0] == (char)frame_header)))
                {
                    dw_want_read = length_padding;
                    p_next = p_read_buf + 1;
                    read_type = Length;
                }
            }
            else if (read_type == Length)
            {
                dw_want_read = *p_next + crc_len;//check length
                p_next = p_read_buf + length_padding + header_len;
                read_type = Body;
                if(dw_want_read > buffer_size - header_len - crc_len)
                {
                    reset_param();
                }
                //size range limit
            }
            else if (read_type == Body)
            {
                // 原始回调（保持向后兼容）
                if (obj->m_xferCallBackFunction != NULL)
                {
                    obj->m_xferCallBackFunction((uint8_t *)p_read_buf, dw_read + length_padding + header_len);
                }

                // 协议解析（新增）
                if (obj->m_parser && obj->m_parseResultCallback)
                {
                    // 将数据添加到缓冲区
                    QByteArray newData((const char*)p_read_buf, dw_read + length_padding + header_len);

                    {
                        std::lock_guard<std::mutex> lock(obj->m_bufferMutex);
                        obj->m_receiveBuffer.append(newData);
                    }

                    // 尝试解析完整帧
                    bool continueParsing = true;
                    while (continueParsing)
                    {
                        QByteArray dataToParse;
                        {
                            std::lock_guard<std::mutex> lock(obj->m_bufferMutex);
                            dataToParse = obj->m_receiveBuffer;
                        }

                        if (dataToParse.isEmpty()) {
                            break;
                        }

                        // 解析数据
                        ParseResult result = obj->m_parser->parse(dataToParse);

                        if (result.success)
                        {
                            // 解析成功，发送结果
                            obj->m_parseResultCallback(result);

                            // 从缓冲区移除已解析的数据
                            {
                                std::lock_guard<std::mutex> lock(obj->m_bufferMutex);
                                if (result.consumedBytes > 0 && result.consumedBytes <= obj->m_receiveBuffer.size())
                                {
                                    obj->m_receiveBuffer.remove(0, result.consumedBytes);
                                }
                                else
                                {
                                    // 异常情况：consumedBytes不合理，清空缓冲区
                                    qWarning() << "ScopeUart: Invalid consumedBytes:" << result.consumedBytes
                                               << ", buffer size:" << obj->m_receiveBuffer.size();
                                    obj->m_receiveBuffer.clear();
                                    continueParsing = false;
                                }
                            }
                        }
                        else
                        {
                            // 解析失败 - 数据可能不完整，继续等待
                            continueParsing = false;

                            // 缓冲区溢出保护
                            {
                                std::lock_guard<std::mutex> lock(obj->m_bufferMutex);
                                const int MAX_BUFFER_SIZE = 4096;
                                if (obj->m_receiveBuffer.size() > MAX_BUFFER_SIZE)
                                {
                                    qWarning() << "ScopeUart: Buffer overflow ("
                                               << obj->m_receiveBuffer.size() << " bytes), clearing old data";
                                    obj->m_receiveBuffer.clear();
                                }
                            }
                        }
                    }
                }

                reset_param();
            }
        }
        else
        {
            reset_param();
        }
    }

    free(p_read_buf);

    //obj->m_shutDownDeviceCallBackFunction();
    return 0;
}
