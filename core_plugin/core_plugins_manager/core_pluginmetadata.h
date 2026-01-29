#ifndef CORE_PLUGINMETADATA_H
#define CORE_PLUGINMETADATA_H

#include <QVariant>
#include <QString>

/**
 * @brief 插件消息元数据
 *
 * 用于插件之间以及插件与主程序之间的消息传递
 */
struct CorePluginMetaData
{
    enum MessageType {
        Command,        // 命令消息
        Response,       // 响应消息
        Data,           // 数据消息
        Event,          // 事件消息
        Config,         // 配置消息
        Alert           // 警报消息
    };

    MessageType type;       // 消息类型
    QString source;         // 消息来源
    QString target;         // 消息目标
    QString command;        // 命令名称
    QVariant data;          // 消息数据
    qint64 timestamp;       // 时间戳

    CorePluginMetaData()
        : type(Data)
        , timestamp(0)
    {
    }
};

Q_DECLARE_METATYPE(CorePluginMetaData)

#endif // CORE_PLUGINMETADATA_H
