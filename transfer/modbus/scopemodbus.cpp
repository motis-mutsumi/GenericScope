#include "scopemodbus.h"
#include <QVariant>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>
#include "logmanager.h"

ScopeModBus::ScopeModBus() : m_modbusClient(new QModbusTcpClient())
{
    connect(m_modbusClient, &QModbusDevice::stateChanged, this, [this](QModbusDevice::State state)
            {
        m_open = (state == QModbusDevice::State::ConnectedState || state == QModbusDevice::State::ConnectingState);
        if(m_stateChangedHandler)
        {
            m_stateChangedHandler(state);
        } });
    connect(m_modbusClient, &QModbusClient::errorOccurred, [this](QModbusDevice::Error error)
            {
                if (m_errorHandler)
                {
                    m_errorHandler(error);
                }
            });

}

ScopeModBus::~ScopeModBus()
{
    delete m_modbusClient;
}

ScopeTransferStatus ScopeModBus::open()
{
    if (m_modbusClient && !m_open)
    {
        m_modbusClient->setConnectionParameter(QModbusDevice::NetworkAddressParameter, QString::fromStdString(m_infoData.ip));
        m_modbusClient->setConnectionParameter(QModbusDevice::NetworkPortParameter, m_infoData.port);
        m_modbusClient->setTimeout(500);
        m_modbusClient->setNumberOfRetries(3);
        return m_modbusClient->connectDevice() ? Ok : Error;
    }

    return Error;
}

ScopeTransferStatus ScopeModBus::close()
{
    if (m_modbusClient && m_open)
    {
        m_modbusClient->disconnectDevice();
        return Ok;
    }

    return Error;
}

QModbusDataUnit ScopeModBus::readModbusData(QModbusDataUnit::RegisterType register_type, int start_add, quint16 number)
{
    QModbusDataUnit reply_unit;
    if (m_modbusClient->state() != QModbusDevice::ConnectedState)
    {
        return reply_unit;
    }
    QModbusDataUnit read_unit(register_type, start_add, number);
    if (auto *reply = m_modbusClient->sendReadRequest(read_unit, 1))
    {
        QTimer timer;
        timer.setInterval(5000);
        timer.setSingleShot(true);
        if (!reply->isFinished())
        {
            QEventLoop loop;
            connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
            timer.start();
            loop.exec();
            if(!timer.isActive())
            {
                delete reply;
                return reply_unit;
            }
            timer.stop();
            if (reply->error() == QModbusDevice::NoError)
            {
                reply_unit = reply->result();
            }
        }
        else
        {
            if (reply->error() == QModbusDevice::NoError)
            {
                reply_unit = reply->result();
            }
            delete reply;
        }
    }
    return reply_unit;
}

ScopeTransferStatus ScopeModBus::writeModbusData(QModbusDataUnit::RegisterType register_type, const QVector<quint16> &array, int start_add)
{
    QModbusDataUnit write_unit(register_type, start_add, static_cast<quint16>(array.size()));

    for (auto i = 0; i < array.size(); i++)
    {
        write_unit.setValue(i, array.at(i));
    }

    if (auto *reply = m_modbusClient->sendWriteRequest(write_unit, 1))
    {
        QTimer timer;
        timer.setInterval(500);
        timer.setSingleShot(true);
        if (!reply->isFinished())
        {
            QEventLoop loop;
            connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
            connect(reply, &QModbusReply::finished, &loop, &QEventLoop::quit);
            timer.start();
            loop.exec();
            if(!timer.isActive())
            {
                delete reply;
                return Error;
            }
            timer.stop();
            if (reply->error() != QModbusDevice::NoError)
            {
                LOG_ERROR(QString("write response error: %1 (code: 0x%2)")
                    .arg(reply->errorString())
                    .arg(reply->error(), 0, 16));
            }
            delete reply;
            return reply->error() == QModbusDevice::NoError ? Ok : Error;
        }
        else
        {
            delete reply;
            return Error;
        }
    }
    else
    {
        LOG_ERROR(QString("write error: %1").arg(m_modbusClient->errorString()));
        return Error;
    }
}
