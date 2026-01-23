#ifndef CORE_PLUGINSBASE_H
#define CORE_PLUGINSBASE_H

#include <QObject>
#include <QWidget>
#include "core_pluginmetadata.h"

/**
 * @brief 插件基类
 *
 * 所有插件必须继承此类并实现其虚函数
 */
class CorePluginsBase : public QObject
{
    Q_OBJECT

public:
    virtual ~CorePluginsBase() {}

    /**
     * @brief 获取插件名称
     * @return 插件名称
     */
    virtual QString pluginName() const = 0;

    /**
     * @brief 获取插件版本
     * @return 插件版本
     */
    virtual QString pluginVersion() const = 0;

    /**
     * @brief 获取插件描述
     * @return 插件描述
     */
    virtual QString pluginDescription() const = 0;

    /**
     * @brief 获取插件 UI 部件
     * @return UI 部件指针（如果没有则返回 nullptr）
     */
    virtual QWidget* pluginWidget() = 0;

    /**
     * @brief 初始化插件
     * @return 是否成功
     */
    virtual bool initialize() = 0;

    /**
     * @brief 反初始化插件
     */
    virtual void uninitialize() = 0;

    /**
     * @brief 处理来自管理器的消息
     * @param data 消息数据
     */
    virtual void handleManagerMessage(const CorePluginMetaData &data) = 0;

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
};

#define CorePluginsBase_iid "com.genericscope.CorePluginsBase"
Q_DECLARE_INTERFACE(CorePluginsBase, CorePluginsBase_iid)

#endif // CORE_PLUGINSBASE_H
