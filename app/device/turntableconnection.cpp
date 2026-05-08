#include "turntableconnection.h"
#include "config/config.h"
#include "transfer/udp/scopeudp.h"

#include <QMetaObject>

TurntableConnection::TurntableConnection(QObject *parent)
    : QObject(parent)
    , m_udp(nullptr)
{
}

TurntableConnection::~TurntableConnection()
{
    if (m_udp) {
        m_udp->close();
        m_udp->deleteLater();
        m_udp = nullptr;
    }
}

bool TurntableConnection::open()
{
    Config *cfg = Config::instance();
    return open(cfg->turntable.udpRemoteIp,
                cfg->turntable.udpRemotePort,
                cfg->turntable.udpLocalPort);
}

bool TurntableConnection::open(const QString &remoteIp, int remotePort, int localPort)
{
    close();

    auto *udp = new ScopeUdp(this);

    UdpInfo info;
    info.remoteIp = remoteIp.toStdString();
    info.remotePort = static_cast<uint16_t>(remotePort);
    info.localPort = static_cast<uint16_t>(localPort);
    udp->setUdpInfo(info);

    udp->setDataCallBackFunction([this](uint8_t *data, size_t len) {
        const QByteArray bytes(reinterpret_cast<const char *>(data), static_cast<int>(len));
        QMetaObject::invokeMethod(this, [this, bytes]() {
            emit dataReceived(bytes);
        }, Qt::QueuedConnection);
    });

    if (udp->open() != Ok) {
        setLastError(QStringLiteral("无法绑定转台 UDP 本地端口 %1").arg(localPort));
        udp->deleteLater();
        emit errorOccurred(m_lastError);
        return false;
    }

    m_udp = udp;
    m_lastError.clear();
    emit connectionChanged(true);
    return true;
}

void TurntableConnection::close()
{
    if (!m_udp) {
        return;
    }

    const bool wasConnected = m_udp->isOpen();
    m_udp->close();
    m_udp->deleteLater();
    m_udp = nullptr;

    if (wasConnected) {
        emit connectionChanged(false);
    }
}

bool TurntableConnection::isConnected() const
{
    return m_udp && m_udp->isOpen();
}

bool TurntableConnection::sendFrame(const QByteArray &frame)
{
    if (!isConnected()) {
        setLastError(QStringLiteral("转台 UDP 未连接"));
        return false;
    }

    if (m_udp->writeData(reinterpret_cast<const uint8_t *>(frame.constData()),
                         static_cast<uint32_t>(frame.size())) != Ok) {
        setLastError(QStringLiteral("转台 UDP 发送失败"));
        emit errorOccurred(m_lastError);
        return false;
    }

    m_lastError.clear();
    return true;
}

QString TurntableConnection::lastError() const
{
    return m_lastError;
}

void TurntableConnection::setLastError(const QString &error)
{
    m_lastError = error;
}
