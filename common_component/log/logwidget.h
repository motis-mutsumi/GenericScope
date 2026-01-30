#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include "logmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LogWidget; }
QT_END_NAMESPACE

/**
 * @brief 日志显示部件
 *
 * 提供日志消息的可视化显示
 */
class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget();

public slots:
    /**
     * @brief 添加日志消息
     * @param level 日志级别
     * @param message 日志消息
     */
    void addLog(LogManager::LogLevel level, const QString &message);

    /**
     * @brief 清空日志
     */
    void clearLog();

    /**
     * @brief 保存日志到文件
     */
    void saveLog();

    /**
     * @brief 设置主题
     * @param isDarkMode 是否为暗色模式
     */
    void setTheme(bool isDarkMode);

private slots:
    void onLevelFilterChanged(int index);
    void onSearchTextChanged(const QString &text);
    void onCaseSensitiveChanged(int state);
    void applyFilters();

private:
    QString getColorForLevel(LogManager::LogLevel level);
    bool matchesFilter(LogManager::LogLevel level, const QString &message);

private:
    Ui::LogWidget *ui;
    LogManager::LogLevel m_currentFilter;
    bool m_isDarkMode;
    QString m_searchText;
    bool m_caseSensitive;

    struct LogEntry {
        LogManager::LogLevel level;
        QString message;
        QString plainMessage;  // 不带HTML标签的纯文本
    };
    QVector<LogEntry> m_allLogs;
};

#endif // LOGWIDGET_H
