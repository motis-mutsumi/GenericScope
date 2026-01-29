#include "devicecontrolplugin.h"
#include "devicecontrolwidget.h"
#include <QDebug>

DeviceControlPlugin::DeviceControlPlugin(QObject *parent)
    : QObject(parent)
    , m_widget(nullptr)
{
    initPredefinedCommands();
}

DeviceControlPlugin::~DeviceControlPlugin()
{
    if (m_widget) {
        delete m_widget;
        m_widget = nullptr;
    }
}

QString DeviceControlPlugin::pluginName() const
{
    return "设备控制插件";
}

QString DeviceControlPlugin::pluginVersion() const
{
    return "1.0.0";
}

QString DeviceControlPlugin::pluginDescription() const
{
    return "向设备发送控制指令";
}

QWidget* DeviceControlPlugin::pluginWidget()
{
    if (!m_widget) {
        m_widget = new DeviceControlWidget();
        m_widget->setPredefinedCommands(m_predefinedCommands);

        QObject::connect(m_widget, &DeviceControlWidget::sendCommand,
                         this, &DeviceControlPlugin::onSendCommand);
    }
    return m_widget;
}

bool DeviceControlPlugin::initialize()
{
    qDebug() << "DeviceControlPlugin initialized";
    return true;
}

void DeviceControlPlugin::uninitialize()
{
    qDebug() << "DeviceControlPlugin uninitialized";
}

void DeviceControlPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    if (data.type == CorePluginMetaData::Response) {
        // 接收来自主窗口的响应
        if (m_widget) {
            QString response = data.data.toString();
            m_widget->addHistory(QString("[响应] %1").arg(response));
        }
    }
}

void DeviceControlPlugin::onSendCommand(const QString &commandName, const QByteArray &commandData)
{
    qDebug() << "DeviceControl: Sending command:" << commandName << commandData.toHex();

    // 发送命令消息到主窗口
    CorePluginMetaData commandMsg;
    commandMsg.type = CorePluginMetaData::Command;
    commandMsg.source = pluginName();
    commandMsg.command = commandName;
    commandMsg.data = commandData;

    emit sendMessageToMain(commandMsg);

    // 更新历史记录
    if (m_widget) {
        m_widget->addHistory(QString("[发送] %1: %2")
                                 .arg(commandName)
                                 .arg(QString(commandData.toHex())));
    }
}

void DeviceControlPlugin::initPredefinedCommands()
{
    // 定义一些示例指令（16进制格式）
    m_predefinedCommands["校准"] = QByteArray::fromHex("FFAA0100");
    m_predefinedCommands["复位"] = QByteArray::fromHex("FFAA0200");
    m_predefinedCommands["查询状态"] = QByteArray::fromHex("FFAA0300");
    m_predefinedCommands["开始采集"] = QByteArray::fromHex("FFAA0400");
    m_predefinedCommands["停止采集"] = QByteArray::fromHex("FFAA0500");
}
