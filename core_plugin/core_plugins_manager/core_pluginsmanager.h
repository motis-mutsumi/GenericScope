#ifndef CORE_PLUGINSMANAGER_H
#define CORE_PLUGINSMANAGER_H

#include <QObject>
#include <QMap>
#include <QPluginLoader>
#include "core_pluginsbase.h"

/**
 * @brief 插件管理器（单例）
 *
 * 负责加载、管理和卸载插件
 */
class Core_PluginsManager : public QObject
{
    Q_OBJECT

public:
    static Core_PluginsManager* instance();

    /**
     * @brief 加载所有插件
     * @param pluginDir 插件目录
     * @return 成功加载的插件数量
     */
    int loadAllPlugins(const QString &pluginDir);

    /**
     * @brief 加载单个插件
     * @param pluginPath 插件路径
     * @return 是否成功
     */
    bool loadPlugin(const QString &pluginPath);

    /**
     * @brief 卸载所有插件
     */
    void unloadAllPlugins();

    /**
     * @brief 卸载单个插件
     * @param pluginName 插件名称
     */
    void unloadPlugin(const QString &pluginName);

    /**
     * @brief 获取插件
     * @param pluginName 插件名称
     * @return 插件指针
     */
    CorePluginsBase* getPlugin(const QString &pluginName);

    /**
     * @brief 获取所有插件名称
     * @return 插件名称列表
     */
    QStringList getPluginNames() const;

    /**
     * @brief 向插件发送消息
     * @param pluginName 插件名称
     * @param data 消息数据
     */
    void sendMessageToPlugin(const QString &pluginName, const CorePluginMetaData &data);

signals:
    /**
     * @brief 插件加载信号
     * @param pluginName 插件名称
     */
    void pluginLoaded(const QString &pluginName);

    /**
     * @brief 插件卸载信号
     * @param pluginName 插件名称
     */
    void pluginUnloaded(const QString &pluginName);

    /**
     * @brief 插件消息信号
     * @param data 消息数据
     */
    void pluginMessage(const CorePluginMetaData &data);

private slots:
    void onPluginMessage(const CorePluginMetaData &data);

private:
    explicit Core_PluginsManager(QObject *parent = nullptr);
    ~Core_PluginsManager();
    Core_PluginsManager(const Core_PluginsManager&) = delete;
    Core_PluginsManager& operator=(const Core_PluginsManager&) = delete;

private:
    static Core_PluginsManager *s_instance;
    QMap<QString, CorePluginsBase*> m_plugins;
    QMap<QString, QPluginLoader*> m_loaders;
};

#endif // CORE_PLUGINSMANAGER_H
