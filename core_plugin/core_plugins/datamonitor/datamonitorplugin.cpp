#include "datamonitorplugin.h"
#include "datamonitorwidget.h"
#include <QDebug>

DataMonitorPlugin::DataMonitorPlugin(QObject *parent)
    : QObject(parent)
    , m_widget(nullptr)
{
    // 设置默认阈值
    m_thresholds["Roll"] = 45.0;          // 横滚角超过45度报警
    m_thresholds["Pitch"] = 45.0;         // 俯仰角超过45度报警
    m_thresholds["Temperature"] = 60.0;   // 温度超过60度报警
}

DataMonitorPlugin::~DataMonitorPlugin()
{
    if (m_widget) {
        delete m_widget;
        m_widget = nullptr;
    }
}

QString DataMonitorPlugin::pluginName() const
{
    return "数据监控插件";
}

QString DataMonitorPlugin::pluginVersion() const
{
    return "1.0.0";
}

QString DataMonitorPlugin::pluginDescription() const
{
    return "实时监控传感器数据，超过阈值时发出警报";
}

QWidget* DataMonitorPlugin::pluginWidget()
{
    if (!m_widget) {
        m_widget = new DataMonitorWidget();
        m_widget->setThresholds(m_thresholds);

        QObject::connect(m_widget, &DataMonitorWidget::thresholdExceeded,
                         this, &DataMonitorPlugin::onThresholdExceeded);
    }
    return m_widget;
}

bool DataMonitorPlugin::initialize()
{
    qDebug() << "DataMonitorPlugin initialized";
    return true;
}

void DataMonitorPlugin::uninitialize()
{
    qDebug() << "DataMonitorPlugin uninitialized";
}

void DataMonitorPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    if (data.type == CorePluginMetaData::Data) {
        // 接收来自主窗口的数据
        QVariantMap fieldData = data.data.toMap();

        // 更新UI显示
        if (m_widget) {
            m_widget->updateData(fieldData);
        }

        // 检查阈值
        for (auto it = fieldData.begin(); it != fieldData.end(); ++it) {
            QString fieldName = it.key();
            double value = it.value().toDouble();

            if (m_thresholds.contains(fieldName)) {
                if (qAbs(value) > m_thresholds[fieldName]) {
                    onThresholdExceeded(fieldName, value);
                }
            }
        }
    }
}

void DataMonitorPlugin::onThresholdExceeded(const QString &fieldName, double value)
{
    qWarning() << "DataMonitor: Threshold exceeded!" << fieldName << "=" << value;

    // 发送警报消息到主窗口
    CorePluginMetaData alertData;
    alertData.type = CorePluginMetaData::Alert;
    alertData.source = pluginName();
    alertData.data = QVariantMap{
        {"field", fieldName},
        {"value", value},
        {"threshold", m_thresholds[fieldName]},
        {"message", QString("%1超过阈值: %2 > %3")
                        .arg(fieldName)
                        .arg(value)
                        .arg(m_thresholds[fieldName])}
    };

    emit sendMessageToMain(alertData);
}
