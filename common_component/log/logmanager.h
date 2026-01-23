#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

/**
 * @brief 日志管理器（单例）
 *
 * 提供全局日志记录功能
 */
class LogManager : public QObject
{
    Q_OBJECT

public:
    enum LogLevel {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
        Critical
    };
    Q_ENUM(LogLevel)

    static LogManager* instance();

    /**
     * @brief 初始化日志系统
     * @param logDir 日志目录
     * @param logToFile 是否记录到文件
     * @param logToConsole 是否输出到控制台
     */
    void initialize(const QString &logDir, bool logToFile = true, bool logToConsole = true);

    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief 记录日志
     * @param level 日志级别
     * @param message 日志消息
     */
    void log(LogLevel level, const QString &message);

    /**
     * @brief 便捷日志函数
     */
    void trace(const QString &message) { log(Trace, message); }
    void debug(const QString &message) { log(Debug, message); }
    void info(const QString &message) { log(Info, message); }
    void warning(const QString &message) { log(Warning, message); }
    void error(const QString &message) { log(Error, message); }
    void critical(const QString &message) { log(Critical, message); }

signals:
    /**
     * @brief 日志消息信号
     * @param level 日志级别
     * @param message 日志消息
     */
    void logMessage(LogLevel level, const QString &message);

private:
    explicit LogManager(QObject *parent = nullptr);
    ~LogManager();
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    QString levelToString(LogLevel level);
    QString formatMessage(LogLevel level, const QString &message);

private:
    static LogManager *s_instance;
    LogLevel m_logLevel;
    bool m_logToFile;
    bool m_logToConsole;
    QFile *m_logFile;
    QTextStream *m_logStream;
    QMutex m_mutex;
};

// 全局日志宏
#define LOG_TRACE(msg) LogManager::instance()->trace(msg)
#define LOG_DEBUG(msg) LogManager::instance()->debug(msg)
#define LOG_INFO(msg) LogManager::instance()->info(msg)
#define LOG_WARNING(msg) LogManager::instance()->warning(msg)
#define LOG_ERROR(msg) LogManager::instance()->error(msg)
#define LOG_CRITICAL(msg) LogManager::instance()->critical(msg)

#endif // LOGMANAGER_H
