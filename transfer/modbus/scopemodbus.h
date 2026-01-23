#ifndef SCOPEMODBUS_H
#define SCOPEMODBUS_H

#include "scopecontroltransfer.h"
#include <QModbusTcpClient>
#include <shared_mutex>

#pragma pack(push, 1)
struct ModBusInfo
{
    std::string ip = "192.168.6.10";
    uint16_t port = 502;
};
#pragma pack(pop)

class TRANSFER_EXPORT ScopeModBus : public QObject, public ScopeControlTransfer
{
    Q_OBJECT
public:
    ScopeModBus();
    ~ScopeModBus();

    // override ScopeTransferBasic
    virtual ScopeTransferStatus open() override;
    virtual ScopeTransferStatus close() override;

    // override ScopeControlTransfer
    void setModBusInfo(const ModBusInfo &info) noexcept
    {
        std::unique_lock<std::shared_mutex> lock(m_dataMutex);
        m_infoData = info;
    }
    ModBusInfo getModBusInfo() const noexcept
    {
        std::shared_lock<std::shared_mutex> lock(m_dataMutex);
        return m_infoData;
    }

    void setErrorHandler(std::function<void(QModbusDevice::Error)> handler)
    {
        m_errorHandler = handler;
    }
    void setStateChangedHandler(std::function<void(QModbusDevice::State)> handler)
    {
        m_stateChangedHandler = handler;
    }

    QModbusDataUnit readModbusData(QModbusDataUnit::RegisterType, int, quint16 numbers);
    ScopeTransferStatus writeModbusData(QModbusDataUnit::RegisterType, const QVector<quint16> &, int);

private:
    mutable std::shared_mutex m_dataMutex;
    ModBusInfo m_infoData;
    QModbusTcpClient *m_modbusClient = nullptr;

    std::function<void(QModbusDevice::Error)> m_errorHandler;
    std::function<void(QModbusDevice::State)> m_stateChangedHandler;
};

#endif // SCOPEMODBUS_H
