#include "logwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

LogWidget::LogWidget(QWidget *parent)
    : QWidget(parent)
    , m_currentFilter(LogManager::Trace)
{
    setupUI();

    // 连接日志管理器
    connect(LogManager::instance(), &LogManager::logMessage,
            this, &LogWidget::addLog);
}

LogWidget::~LogWidget()
{
}

void LogWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 工具栏
    QHBoxLayout *toolLayout = new QHBoxLayout();

    m_levelFilter = new QComboBox(this);
    m_levelFilter->addItem("All", LogManager::Trace);
    m_levelFilter->addItem("Debug", LogManager::Debug);
    m_levelFilter->addItem("Info", LogManager::Info);
    m_levelFilter->addItem("Warning", LogManager::Warning);
    m_levelFilter->addItem("Error", LogManager::Error);
    m_levelFilter->addItem("Critical", LogManager::Critical);
    connect(m_levelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogWidget::onLevelFilterChanged);

    m_clearButton = new QPushButton("Clear", this);
    connect(m_clearButton, &QPushButton::clicked, this, &LogWidget::clearLog);

    m_saveButton = new QPushButton("Save", this);
    connect(m_saveButton, &QPushButton::clicked, this, &LogWidget::saveLog);

    toolLayout->addWidget(m_levelFilter);
    toolLayout->addStretch();
    toolLayout->addWidget(m_clearButton);
    toolLayout->addWidget(m_saveButton);

    // 日志文本框
    m_logTextEdit = new QTextEdit(this);
    m_logTextEdit->setReadOnly(true);
    m_logTextEdit->setFont(QFont("Consolas", 9));

    mainLayout->addLayout(toolLayout);
    mainLayout->addWidget(m_logTextEdit);
}

void LogWidget::addLog(LogManager::LogLevel level, const QString &message)
{
    // 过滤日志级别
    if (level < m_currentFilter) {
        return;
    }

    // 设置颜色
    QString color = getColorForLevel(level);
    QString html = QString("<span style='color:%1'>%2</span>").arg(color, message);

    m_logTextEdit->append(html);

    // 自动滚动到底部
    m_logTextEdit->moveCursor(QTextCursor::End);
}

void LogWidget::clearLog()
{
    m_logTextEdit->clear();
}

void LogWidget::saveLog()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save Log",
                                                    "",
                                                    "Text Files (*.txt);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << m_logTextEdit->toPlainText();
        file.close();
    }
}

void LogWidget::onLevelFilterChanged(int index)
{
    m_currentFilter = static_cast<LogManager::LogLevel>(m_levelFilter->itemData(index).toInt());
}

QString LogWidget::getColorForLevel(LogManager::LogLevel level)
{
    switch (level) {
    case LogManager::Trace:    return "gray";
    case LogManager::Debug:    return "blue";
    case LogManager::Info:     return "black";
    case LogManager::Warning:  return "orange";
    case LogManager::Error:    return "red";
    case LogManager::Critical: return "darkred";
    default:                   return "black";
    }
}
