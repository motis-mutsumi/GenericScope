#ifndef MONITORDATAMANAGER_H
#define MONITORDATAMANAGER_H

#include <QObject>
#include <QMap>
#include <QMutex>

/**
 * @brief 监控数据管理器（单例）
 *
 * 负责接收ProtocolParser解析的数据，并分发给所有订阅的MonitorChart。
 * 使用观察者模式，通过信号/槽实现解耦。
 */
class MonitorDataManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief 获取单例实例
     */
    static MonitorDataManager* instance();

    /**
     * @brief 接收协议解析数据
     * @param fieldValues 字段值映射 <字段名, 数值>
     *
     * 此方法会遍历所有字段，为每个字段生成时间戳并emit dataUpdated信号
     */
    void onProtocolDataParsed(const QMap<QString, double> &fieldValues);

signals:
    /**
     * @brief 数据更新信号
     * @param fieldName 字段名
     * @param value 数值
     * @param timestamp 时间戳（毫秒）
     *
     * 所有MonitorChart订阅此信号，根据字段名过滤自己需要的数据
     */
    void dataUpdated(const QString &fieldName, double value, qint64 timestamp);

private:
    explicit MonitorDataManager(QObject *parent = nullptr);
    ~MonitorDataManager();

    // 禁用拷贝和赋值
    MonitorDataManager(const MonitorDataManager&) = delete;
    MonitorDataManager& operator=(const MonitorDataManager&) = delete;

    static MonitorDataManager *s_instance;
    static QMutex s_mutex;  // 线程安全
};

#endif // MONITORDATAMANAGER_H
