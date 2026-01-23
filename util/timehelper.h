#ifndef TIMEHELPER_H
#define TIMEHELPER_H

#include <QObject>
#include <QElapsedTimer>
#include <QString>

/**
 * @brief 时间辅助工具类
 *
 * 提供时间测量和格式化功能
 */
class TimeHelper : public QObject
{
    Q_OBJECT

public:
    explicit TimeHelper(QObject *parent = nullptr);
    ~TimeHelper();

    /**
     * @brief 开始计时
     */
    void start();

    /**
     * @brief 获取经过的时间（毫秒）
     * @return 经过的毫秒数
     */
    qint64 elapsed() const;

    /**
     * @brief 重启计时器
     * @return 上次启动到现在经过的毫秒数
     */
    qint64 restart();

    /**
     * @brief 格式化时间戳
     * @param timestamp 时间戳（毫秒）
     * @param format 格式字符串
     * @return 格式化后的字符串
     */
    static QString formatTimestamp(qint64 timestamp, const QString &format = "yyyy-MM-dd hh:mm:ss.zzz");

    /**
     * @brief 获取当前时间戳（毫秒）
     * @return 当前时间戳
     */
    static qint64 currentTimestamp();

    /**
     * @brief 格式化持续时间
     * @param milliseconds 毫秒数
     * @return 格式化后的字符串（如 "1h 23m 45s"）
     */
    static QString formatDuration(qint64 milliseconds);

private:
    QElapsedTimer m_timer;
};

#endif // TIMEHELPER_H
