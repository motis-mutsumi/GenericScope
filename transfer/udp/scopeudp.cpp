#include "scopeudp.h"
#include <QDebug>

ScopeUdp::ScopeUdp(QObject *parent)
    : QObject(parent)
    , ScopeControlTransfer()
{
}

ScopeUdp::~ScopeUdp()
{
    close();
}

ScopeTransferStatus ScopeUdp::open()
{
    if (m_open) {
        return Ok;
    }

    UdpInfo info = getUdpInfo();

    m_socket = new QUdpSocket(this);
    connect(m_socket, &QUdpSocket::readyRead, this, &ScopeUdp::onReadyRead);

    if (!m_socket->bind(QHostAddress::AnyIPv4, info.localPort)) {
        qWarning() << "ScopeUdp: Failed to bind port" << info.localPort << m_socket->errorString();
        delete m_socket;
        m_socket = nullptr;
        return Error;
    }

    m_open = true;
    qDebug() << "ScopeUdp: Listening on port" << info.localPort
             << "remote" << QString::fromStdString(info.remoteIp) << ":" << info.remotePort;
    return Ok;
}

ScopeTransferStatus ScopeUdp::close()
{
    if (m_open) {
        m_open = false;
        if (m_socket) {
            m_socket->close();
            m_socket->deleteLater();
            m_socket = nullptr;
        }
    }
    return Ok;
}

ScopeTransferStatus ScopeUdp::readData(uint8_t * /*data*/, uint32_t /*read_len*/,
                                        uint8_t * /*cmd*/, int /*write_len*/)
{
    // 异步 UDP：数据由 onReadyRead 推送，不支持同步读
    return Ok;
}

ScopeTransferStatus ScopeUdp::writeData(const uint8_t *data, uint32_t write_len)
{
    if (!m_open || !m_socket) {
        return Error;
    }

    UdpInfo info = getUdpInfo();
    QHostAddress addr(QString::fromStdString(info.remoteIp));
    qint64 sent = m_socket->writeDatagram(
        reinterpret_cast<const char *>(data),
        static_cast<qint64>(write_len),
        addr,
        info.remotePort);

    return (sent == static_cast<qint64>(write_len)) ? Ok : Error;
}

void ScopeUdp::onReadyRead()
{
    while (m_socket && m_socket->hasPendingDatagrams()) {
        QByteArray buf;
        buf.resize(static_cast<int>(m_socket->pendingDatagramSize()));
        m_socket->readDatagram(buf.data(), buf.size());

        if (m_dataCallback && !buf.isEmpty()) {
            m_dataCallback(reinterpret_cast<uint8_t *>(buf.data()),
                           static_cast<size_t>(buf.size()));
        }
    }
}
