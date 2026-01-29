#ifndef DEVICECONTROLPLUGIN_H
#define DEVICECONTROLPLUGIN_H

#include "core_pluginsbase.h"
#include <QObject>

class DeviceControlWidget;

/**
 * @brief 设备控制插件
 *
 * 向设备发送控制指令，例如校准、复位、查询状态等。
 */
class DeviceControlPlugin : public QObject, public CorePluginsBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.genericscope.plugin.devicecontrol" FILE "metadata.json")
    Q_INTERFACES(CorePluginsBase)

public:
    explicit DeviceControlPlugin(QObject *parent = nullptr);
    ~DeviceControlPlugin() override;

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
    void onSendCommand(const QString &commandName, const QByteArray &commandData);

private:
    DeviceControlWidget *m_widget;

    // 预定义指令映射
    QMap<QString, QByteArray> m_predefinedCommands;

    void initPredefinedCommands();
};

#endif // DEVICECONTROLPLUGIN_H
