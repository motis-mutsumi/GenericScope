#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include "logmanager.h"

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

private slots:
    void onLevelFilterChanged(int index);

private:
    void setupUI();
    QString getColorForLevel(LogManager::LogLevel level);

private:
    QTextEdit *m_logTextEdit;
    QComboBox *m_levelFilter;
    QPushButton *m_clearButton;
    QPushButton *m_saveButton;
    LogManager::LogLevel m_currentFilter;
};

#endif // LOGWIDGET_H
