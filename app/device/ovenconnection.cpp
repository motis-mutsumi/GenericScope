#include "ovenconnection.h"
#include "config/config.h"
#include "transfer/uart/scopeuart.h"

#include <QMetaObject>
#include <windows.h>

OvenConnection::OvenConnection(QObject *parent)
    : QObject(parent)
    , m_uart(nullptr)
{
}

OvenConnection::~OvenConnection()
{
    if (m_uart) {
        m_uart->close();
        delete m_uart;
        m_uart = nullptr;
    }
}

bool OvenConnection::open()
{
    Config *cfg = Config::instance();
    return open(cfg->oven.port,
                cfg->oven.baudRate,
                cfg->oven.dataBits,
                cfg->oven.stopBits,
                cfg->oven.parity,
                cfg->oven.timeout);
}

bool OvenConnection::open(const QString &port, int baudRate, int dataBits, int stopBits,
                          const QString &parity, int timeout)
{
    Q_UNUSED(timeout)
    close();

    auto *uart = new ScopeUart();

    UartInfo info;
    info.port_name = port.toStdString();
    info.async = true;

    ZeroMemory(&info.dcb, sizeof(DCB));
    info.dcb.DCBlength = sizeof(DCB);
    info.dcb.BaudRate = baudRate;
    info.dcb.ByteSize = static_cast<BYTE>(dataBits);
    info.dcb.StopBits = (stopBits == 2) ? TWOSTOPBITS : ONESTOPBIT;

    if (parity == QStringLiteral("Odd")) {
        info.dcb.Parity = ODDPARITY;
        info.dcb.fParity = TRUE;
    } else if (parity == QStringLiteral("Even")) {
        info.dcb.Parity = EVENPARITY;
        info.dcb.fParity = TRUE;
    } else {
        info.dcb.Parity = NOPARITY;
        info.dcb.fParity = FALSE;
    }

    uart->setUartInfo(info);
    uart->setDataCallBackFunction([this](uint8_t *data, size_t len) {
        const QByteArray bytes(reinterpret_cast<const char *>(data), static_cast<int>(len));
        QMetaObject::invokeMethod(this, [this, bytes]() {
            emit dataReceived(bytes);
        }, Qt::QueuedConnection);
    });

    if (uart->open() != Ok) {
        setLastError(QStringLiteral("无法打开烘箱串口 %1").arg(port));
        delete uart;
        emit errorOccurred(m_lastError);
        return false;
    }

    m_uart = uart;
    m_lastError.clear();
    emit connectionChanged(true);
    return true;
}

void OvenConnection::close()
{
    if (!m_uart) {
        return;
    }

    const bool wasConnected = m_uart->isOpen();
    m_uart->close();
    delete m_uart;
    m_uart = nullptr;

    if (wasConnected) {
        emit connectionChanged(false);
    }
}

bool OvenConnection::isConnected() const
{
    return m_uart && m_uart->isOpen();
}

bool OvenConnection::sendCommand(const QByteArray &command)
{
    if (!isConnected()) {
        setLastError(QStringLiteral("烘箱串口未连接"));
        return false;
    }

    if (m_uart->writeData(reinterpret_cast<uint8_t *>(const_cast<char *>(command.constData())),
                          static_cast<uint32_t>(command.size())) != Ok) {
        setLastError(QStringLiteral("烘箱串口发送失败"));
        emit errorOccurred(m_lastError);
        return false;
    }

    m_lastError.clear();
    return true;
}

QString OvenConnection::lastError() const
{
    return m_lastError;
}

void OvenConnection::setLastError(const QString &error)
{
    m_lastError = error;
}
