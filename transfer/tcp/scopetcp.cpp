#include "scopetcp.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <iostream>

using namespace std;

ScopeTcp::ScopeTcp()
{
}

ScopeTransferStatus ScopeTcp::open()
{
    if (initTcp() != 0)
    {
        return Error;
    }

    if (openClientSocket() != 0)
    {
        return Error;
    }

    m_open = true;
    return Ok;
}

ScopeTransferStatus ScopeTcp::close()
{
    closeClientSocket();
    deinitTcp();
    m_open = false;
    return Ok;
}

ScopeTransferStatus ScopeTcp::readData(uint8_t *rx_data, uint32_t read_len, uint8_t *cmd, int write_len)
{
    if (!m_clientSocketOpen)
    {
        return Error;
    }

    if (!m_open)
    {
        return Error;
    }

    if (cmd && write_len > 0)
    {
        int index = 0;
        while (index != write_len)
        {
            int sendlength = send(m_clientSocket, (const char *)cmd + index, write_len - index, 0);
            if (sendlength == -1)
            {
                // disconnect
                m_open = false;
                cout << "send failed!" << endl;
                // connect
                openClientSocket();
                Sleep(10);
                break;
            }
            else
            {
                index += sendlength;
                if (index != write_len)
                    Sleep(1);
            }
        }

        if (index != write_len || !m_open)
        {
            return Error;
        }
    }

    memset(rx_data, 0, read_len);

    int rx_len = 0;
    int rx_total_len = 0;
    uint8_t *rx_ptr = rx_data;
    do
    {
        rx_len = recv(m_clientSocket, (char *)rx_ptr, read_len, 0);
        if (rx_len < 0)
        {
            int error = WSAGetLastError();
            if (error == EAGAIN || error == EWOULDBLOCK)
            {
                Sleep(10); // 短暂等待后重试
                continue;
            }
            break;
        }
        else if (rx_len == 0)
        {
            break;
        }
        rx_total_len = rx_total_len + rx_len;
        rx_ptr += rx_len;

        if (rx_len == read_len)
            Sleep(20);
    } while (rx_len != read_len);

    return read_len == rx_total_len ? Ok : Error;
}

int ScopeTcp::initTcp()
{
    // init socket
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        cout << "WSAStartup failed!\n";
        return -1;
    }
    return 0;
}

int ScopeTcp::deinitTcp()
{
    WSACleanup();
    return 0;
}

int ScopeTcp::openClientSocket()
{
    closeClientSocket();

    m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 设置IPV4，流式传输，TCP协议可为0
    if (INVALID_SOCKET == m_clientSocket)
    {
        cout << "socket failed!" << endl;
        return -1;
    }

    TcpInfo info_data = getTcpInfo();

    // set socket to non-block
    int iMode = 1;
    int ret = ioctlsocket(m_clientSocket, FIONBIO, (u_long FAR *)&iMode);
    if (ret == SOCKET_ERROR)
    {
        cout << "ioctlsocket failed!" << endl;
        return ret;
    }

    // set socket address
    SOCKADDR_IN serve_addr;
    serve_addr.sin_family = AF_INET;
    serve_addr.sin_port = htons(info_data.port);
    inet_pton(AF_INET, info_data.ip.c_str(), &serve_addr.sin_addr.S_un.S_addr);

    m_clientSocketOpen = true;

    UINT64 time_out = 5000;
    DWORD64 timeTick = GetTickCount64();
    while (m_clientSocketOpen)
    {
        DWORD64 timeTick2 = GetTickCount64();
        if (timeTick2 - timeTick > time_out)
        {
            cout << "connect timeout!" << endl;
            break;
        }
        // 连接服务器
        ret = connect(m_clientSocket, (LPSOCKADDR)&serve_addr, sizeof(serve_addr));

        // 处理连接错误
        if (SOCKET_ERROR == ret)
        {
            Sleep(1);
            int nErrCode = WSAGetLastError();
            if (WSAEWOULDBLOCK == nErrCode || // not connect
                WSAEINVAL == nErrCode)
            {
                continue;
            }
            else if (WSAEISCONN == nErrCode) // connect
            {
                return 0;
                break;
            }
            else // other reason, connect failed
            {
                continue;
            }
        }

        if (ret == 0) // connect success
        {
            return 0;
        }
    }
    m_clientSocketOpen = false;
    return -2;
}

int ScopeTcp::closeClientSocket()
{
    if (m_clientSocket != NULL)
    {
        closesocket(m_clientSocket);
        m_clientSocket = NULL;
        m_clientSocketOpen = false;
    }
    return Ok;
}
