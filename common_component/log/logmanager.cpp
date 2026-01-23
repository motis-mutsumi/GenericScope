#include "logmanager.h"
#include <QDateTime>
#include <QDir>
#include <QDebug>

LogManager* LogManager::s_instance = nullptr;

LogManager::LogManager(QObject *parent)
    : QObject(parent)
    , m_logLevel(Info)
    , m_logToFile(false)
    , m_logToConsole(true)
    , m_logFile(nullptr)
    , m_logStream(nullptr)
{
}

LogManager::~LogManager()
{
    if (m_logStream) {
        delete m_logStream;
    }
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
    }
}

LogManager* LogManager::instance()
{
    if (!s_instance) {
        s_instance = new LogManager();
    }
    return s_instance;
}

void LogManager::initialize(const QString &logDir, bool logToFile, bool logToConsole)
{
    m_logToFile = logToFile;
    m_logToConsole = logToConsole;

    if (m_logToFile) {
        // 创建日志目录
        QDir dir;
        if (!dir.exists(logDir)) {
            dir.mkpath(logDir);
        }

        // 创建日志文件
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        QString logFileName = QString("%1/log_%2.txt").arg(logDir, timestamp);

        m_logFile = new QFile(logFileName);
        if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            m_logStream = new QTextStream(m_logFile);
        }
    }

    info("Log system initialized");
}

void LogManager::setLogLevel(LogLevel level)
{
    m_logLevel = level;
}

void LogManager::log(LogLevel level, const QString &message)
{
    if (level < m_logLevel) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    QString formattedMessage = formatMessage(level, message);

    // 输出到控制台
    if (m_logToConsole) {
        qDebug().noquote() << formattedMessage;
    }

    // 写入文件
    if (m_logToFile && m_logStream) {
        *m_logStream << formattedMessage << "\n";
        m_logStream->flush();
    }

    // 发送信号
    emit logMessage(level, formattedMessage);
}

QString LogManager::levelToString(LogLevel level)
{
    switch (level) {
    case Trace:    return "TRACE";
    case Debug:    return "DEBUG";
    case Info:     return "INFO";
    case Warning:  return "WARNING";
    case Error:    return "ERROR";
    case Critical: return "CRITICAL";
    default:       return "UNKNOWN";
    }
}

QString LogManager::formatMessage(LogLevel level, const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    return QString("[%1] [%2] %3").arg(timestamp, levelStr, message);
}
