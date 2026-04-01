#include "devicemanager.h"
#include "config/config.h"
#include "transfer/uart/scopeuart.h"
#include "transfer/udp/scopeudp.h"
#include "common_component/log/logmanager.h"

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
    , m_transfer(nullptr)
    , m_controlTransfer(nullptr)
{
}

DeviceManager::~DeviceManager()
{
    disconnectDevice();
}

bool DeviceManager::connectDevice()
{
    if (isConnected()) {
        return true;
    }

    Config *cfg = Config::instance();
    const QString type = cfg->device.type.toUpper();

    if (type == "UDP") {
        return connectUdp();
    }
    if (type == "UART") {
        return connectUart();
    }

    LOG_ERROR(QString("Unsupported device type: %1").arg(cfg->device.type));
    emit errorOccurred(QString("Unsupported device type: %1").arg(cfg->device.type));
    return false;
}

bool DeviceManager::connectUart()
{
    Config *cfg = Config::instance();

    ScopeUart *uart = new ScopeUart();

    UartInfo info;
    info.port_name = cfg->device.port.toStdString();
    info.async = true;

    ZeroMemory(&info.dcb, sizeof(DCB));
    info.dcb.DCBlength = sizeof(DCB);
    info.dcb.BaudRate = cfg->device.baudRate;
    info.dcb.ByteSize = static_cast<BYTE>(cfg->device.dataBits);
    info.dcb.StopBits = (cfg->device.stopBits == 2) ? TWOSTOPBITS : ONESTOPBIT;

    const QString &parity = cfg->device.parity;
    if (parity == "Odd") {
        info.dcb.Parity = ODDPARITY;
        info.dcb.fParity = TRUE;
    } else if (parity == "Even") {
        info.dcb.Parity = EVENPARITY;
        info.dcb.fParity = TRUE;
    } else {
        info.dcb.Parity = NOPARITY;
        info.dcb.fParity = FALSE;
    }

    uart->setUartInfo(info);

    uart->setDataCallBackFunction([this](uint8_t *data, size_t len) {
        QByteArray bytes(reinterpret_cast<const char *>(data), static_cast<int>(len));
        QMetaObject::invokeMethod(this, [this, bytes]() {
            emit dataReceived(bytes);
        }, Qt::QueuedConnection);
    });

    if (uart->open() != Ok) {
        LOG_ERROR(QString("Failed to open serial port: %1").arg(cfg->device.port));
        delete uart;
        return false;
    }

    m_transfer = uart;
    m_controlTransfer = uart;

    LOG_INFO(QString("Serial port opened: %1 @ %2 baud")
                 .arg(cfg->device.port)
                 .arg(cfg->device.baudRate));

    emit connectionChanged(true);
    return true;
}

bool DeviceManager::connectUdp()
{
    Config *cfg = Config::instance();

    ScopeUdp *udp = new ScopeUdp();

    UdpInfo info;
    info.remoteIp   = cfg->device.udpRemoteIp.toStdString();
    info.remotePort = static_cast<uint16_t>(cfg->device.udpRemotePort);
    info.localPort  = static_cast<uint16_t>(cfg->device.udpLocalPort);

    udp->setUdpInfo(info);

    udp->setDataCallBackFunction([this](uint8_t *data, size_t len) {
        QByteArray bytes(reinterpret_cast<const char *>(data), static_cast<int>(len));
        QMetaObject::invokeMethod(this, [this, bytes]() {
            emit dataReceived(bytes);
        }, Qt::QueuedConnection);
    });

    if (udp->open() != Ok) {
        LOG_ERROR(QString("Failed to open UDP on local port: %1").arg(cfg->device.udpLocalPort));
        delete udp;
        return false;
    }

    m_transfer = udp;
    m_controlTransfer = udp;

    LOG_INFO(QString("UDP opened: local port %1, remote %2:%3")
                 .arg(cfg->device.udpLocalPort)
                 .arg(cfg->device.udpRemoteIp)
                 .arg(cfg->device.udpRemotePort));

    emit connectionChanged(true);
    return true;
}

void DeviceManager::disconnectDevice()
{
    if (!m_transfer) {
        return;
    }

    m_transfer->close();
    delete m_transfer;

    m_transfer = nullptr;
    m_controlTransfer = nullptr;

    emit connectionChanged(false);
}

bool DeviceManager::isConnected() const
{
    return m_transfer && m_transfer->isOpen();
}

QByteArray DeviceManager::sendCommand(const QByteArray &command)
{
    if (!isConnected()) {
        return QByteArray();
    }

    ScopeUart *uart = dynamic_cast<ScopeUart *>(m_transfer);
    if (uart) {
        uart->writeData(
            reinterpret_cast<uint8_t *>(const_cast<char *>(command.constData())),
            static_cast<uint32_t>(command.size()));
        return QByteArray();
    }

    ScopeUdp *udp = dynamic_cast<ScopeUdp *>(m_transfer);
    if (udp) {
        udp->writeData(
            reinterpret_cast<const uint8_t *>(command.constData()),
            static_cast<uint32_t>(command.size()));
        return QByteArray();
    }

    return QByteArray();
}

void DeviceManager::startPolling()
{
    // 异步模式（串口/UDP）均由各自线程/信号推送数据，无需额外轮询
}

void DeviceManager::stopPolling()
{
}

void DeviceManager::onTransferStatusChanged(ScopeTransferStatus status)
{
    Q_UNUSED(status)
}

void DeviceManager::onTransferDataReceived(const QByteArray &data)
{
    emit dataReceived(data);
}

void DeviceManager::onTransferError(const QString &error)
{
    emit errorOccurred(error);
}

void DeviceManager::onPollingDataReceived(const QByteArray &data)
{
    emit dataReceived(data);
}
