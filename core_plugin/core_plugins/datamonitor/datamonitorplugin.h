#ifndef DATAMONITORPLUGIN_H
#define DATAMONITORPLUGIN_H

#include "core_pluginsbase.h"
#include <QObject>

class DataMonitorWidget;

/**
 * @brief 数据监控插件
 *
 * 实时监控传感器数据，当数值超过阈值时发出警报。
 */
class DataMonitorPlugin : public QObject, public CorePluginsBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.genericscope.plugin.datamonitor" FILE "metadata.json")
    Q_INTERFACES(CorePluginsBase)

public:
    explicit DataMonitorPlugin(QObject *parent = nullptr);
    ~DataMonitorPlugin() override;

    // CorePluginsBase接口实现
    QString pluginName() const override;
    QString pluginVersion() const override;
    QString pluginDescription() const override;
    QWidget* pluginWidget() override;
    bool initialize() override;
    void uninitialize() override;

    void handleManagerMessage(const CorePluginMetaData &data) override;

signals:
    /**
     * @brief 向管理器发送消息
     * @param data 消息数据
     */
    void sendMessageToManager(const CorePluginMetaData &data);

    /**
     * @brief 向主窗口发送消息
     * @param data 消息数据
     */
    void sendMessageToMain(const CorePluginMetaData &data);

private slots:
    void onThresholdExceeded(const QString &fieldName, double value);

private:
    DataMonitorWidget *m_widget;

    // 阈值配置
    QMap<QString, double> m_thresholds;
};

#endif // DATAMONITORPLUGIN_H
