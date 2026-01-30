#include "logwidget.h"
#include "ui_logwidget.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

LogWidget::LogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogWidget)
    , m_currentFilter(LogManager::Trace)
    , m_isDarkMode(false)
    , m_caseSensitive(false)
{
    ui->setupUi(this);

    // 取消布局边距，避免显示父窗口的背景色
    if (layout()) {
        layout()->setContentsMargins(0, 0, 0, 0);
    }

    // 设置ComboBox的数据
    ui->levelFilter->setItemData(0, LogManager::Trace);
    ui->levelFilter->setItemData(1, LogManager::Debug);
    ui->levelFilter->setItemData(2, LogManager::Info);
    ui->levelFilter->setItemData(3, LogManager::Warning);
    ui->levelFilter->setItemData(4, LogManager::Error);
    ui->levelFilter->setItemData(5, LogManager::Critical);

    // 连接信号
    connect(ui->levelFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LogWidget::onLevelFilterChanged);
    connect(ui->searchEdit, &QLineEdit::textChanged,
            this, &LogWidget::onSearchTextChanged);
    connect(ui->caseSensitiveCheck, &QCheckBox::stateChanged,
            this, &LogWidget::onCaseSensitiveChanged);
    connect(ui->clearButton, &QPushButton::clicked, this, &LogWidget::clearLog);
    connect(ui->saveButton, &QPushButton::clicked, this, &LogWidget::saveLog);

    // 连接日志管理器
    connect(LogManager::instance(), &LogManager::logMessage,
            this, &LogWidget::addLog);
}

LogWidget::~LogWidget()
{
    delete ui;
}

void LogWidget::addLog(LogManager::LogLevel level, const QString &message)
{
    // 保存所有日志到缓存
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.plainMessage = message;  // 纯文本版本
    m_allLogs.append(entry);

    // 检查是否通过筛选
    if (matchesFilter(level, message)) {
        // 设置颜色
        QString color = getColorForLevel(level);
        QString html = QString("<span style='color:%1'>%2</span>").arg(color, message);

        ui->logTextEdit->append(html);

        // 自动滚动到底部
        ui->logTextEdit->moveCursor(QTextCursor::End);
    }
}

void LogWidget::clearLog()
{
    ui->logTextEdit->clear();
    m_allLogs.clear();
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
        stream << ui->logTextEdit->toPlainText();
        file.close();
    }
}

void LogWidget::onLevelFilterChanged(int index)
{
    m_currentFilter = static_cast<LogManager::LogLevel>(ui->levelFilter->itemData(index).toInt());
    applyFilters();
}

void LogWidget::onSearchTextChanged(const QString &text)
{
    m_searchText = text;
    applyFilters();
}

void LogWidget::onCaseSensitiveChanged(int state)
{
    m_caseSensitive = (state == Qt::Checked);
    applyFilters();
}

void LogWidget::applyFilters()
{
    // 清空显示区域
    ui->logTextEdit->clear();

    // 重新应用筛选并显示
    for (const LogEntry &entry : m_allLogs) {
        if (matchesFilter(entry.level, entry.plainMessage)) {
            QString color = getColorForLevel(entry.level);
            QString html = QString("<span style='color:%1'>%2</span>").arg(color, entry.message);
            ui->logTextEdit->append(html);
        }
    }

    // 滚动到底部
    ui->logTextEdit->moveCursor(QTextCursor::End);
}

bool LogWidget::matchesFilter(LogManager::LogLevel level, const QString &message)
{
    // 检查日志级别
    if (level < m_currentFilter) {
        return false;
    }

    // 检查文本筛选
    if (!m_searchText.isEmpty()) {
        Qt::CaseSensitivity cs = m_caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;
        if (!message.contains(m_searchText, cs)) {
            return false;
        }
    }

    return true;
}

void LogWidget::setTheme(bool isDarkMode)
{
    m_isDarkMode = isDarkMode;

    // 不再设置局部样式表，改为依赖全局 QSS
    // 只需要清除可能存在的局部样式
    this->setStyleSheet("");
    ui->logTextEdit->setStyleSheet("");
}

QString LogWidget::getColorForLevel(LogManager::LogLevel level)
{
    if (m_isDarkMode) {
        // 暗色模式下的颜色
        switch (level) {
        case LogManager::Trace:    return "#888888";  // 亮灰色
        case LogManager::Debug:    return "#4FC3F7";  // 亮蓝色
        case LogManager::Info:     return "#E0E0E0";  // 浅白色
        case LogManager::Warning:  return "#FFB74D";  // 亮橙色
        case LogManager::Error:    return "#EF5350";  // 亮红色
        case LogManager::Critical: return "#F44336";  // 鲜红色
        default:                   return "#E0E0E0";
        }
    } else {
        // 亮色模式下的颜色
        switch (level) {
        case LogManager::Trace:    return "#757575";  // 灰色
        case LogManager::Debug:    return "#1976D2";  // 蓝色
        case LogManager::Info:     return "#212121";  // 黑色
        case LogManager::Warning:  return "#F57C00";  // 橙色
        case LogManager::Error:    return "#D32F2F";  // 红色
        case LogManager::Critical: return "#B71C1C";  // 深红色
        default:                   return "#212121";
        }
    }
}
