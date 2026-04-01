#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

#include "protocolconfig.h"
#include "protocolparser.h"
#include <QObject>
#include <QMap>
#include <QSharedPointer>
#include <QMutex>

/**
 * @brief 协议管理器（单例）
 *
 * 负责：
 * - 加载/保存协议配置
 * - 管理协议列表
 * - 创建协议解析器
 * - 管理当前使用的协议
 */
class ProtocolManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取单例实例
     */
    static ProtocolManager* instance();

    /**
     * @brief 加载协议
     * @param filePath 协议文件路径
     * @return 是否成功
     */
    bool loadProtocol(const QString &filePath);

    /**
     * @brief 批量加载协议
     * @param dirPath 协议目录路径
     * @return 成功加载的协议数量
     */
    int loadProtocols(const QString &dirPath);
    int loadProtocolsFromSettings(const QString &organization, const QString &application);

    /**
     * @brief 保存协议
     * @param name 协议名称
     * @param filePath 保存路径
     * @return 是否成功
     */
    bool saveProtocol(const QString &name, const QString &filePath);

    /**
     * @brief 添加协议
     */
    void addProtocol(const ProtocolConfig &config);

    /**
     * @brief 删除协议
     */
    void removeProtocol(const QString &name);

    /**
     * @brief 是否存在协议
     */
    bool hasProtocol(const QString &name) const;

    /**
     * @brief 获取协议配置
     */
    ProtocolConfig getProtocol(const QString &name) const;

    /**
     * @brief 获取所有协议名称
     */
    QStringList getProtocolNames() const;

    /**
     * @brief 创建解析器
     * @param name 协议名称
     * @return 解析器（智能指针）
     */
    QSharedPointer<ProtocolParser> createParser(const QString &name);

    /**
     * @brief 设置当前协议
     */
    void setCurrentProtocol(const QString &name);

    /**
     * @brief 获取当前协议名称
     */
    QString getCurrentProtocol() const { return m_currentProtocol; }

    /**
     * @brief 获取当前解析器
     */
    QSharedPointer<ProtocolParser> getCurrentParser();

signals:
    /**
     * @brief 协议添加信号
     */
    void protocolAdded(const QString &name);

    /**
     * @brief 协议删除信号
     */
    void protocolRemoved(const QString &name);

    /**
     * @brief 当前协议变化信号
     */
    void currentProtocolChanged(const QString &name);

private:
    ProtocolManager(QObject *parent = nullptr);
    ~ProtocolManager();
    ProtocolManager(const ProtocolManager&) = delete;
    ProtocolManager& operator=(const ProtocolManager&) = delete;

    QMap<QString, ProtocolConfig> m_protocols;
    QString m_currentProtocol;
    QSharedPointer<ProtocolParser> m_currentParser;
    mutable QMutex m_mutex;
};

#endif // PROTOCOLMANAGER_H
