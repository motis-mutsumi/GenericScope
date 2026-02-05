#include "monitorpanel.h"
#include "monitorchart.h"
#include "monitorconfigdialog.h"
#include "protocol/protocolmanager.h"
#include <QVBoxLayout>
#include <QMessageBox>

MonitorPanel::MonitorPanel(QWidget *parent)
    : QWidget(parent)
    , m_addButton(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_contentLayout(nullptr)
{
    setupUI();
}

MonitorPanel::~MonitorPanel()
{
    clearAllCharts();
}

void MonitorPanel::setupUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 添加按钮
    m_addButton = new QPushButton("+ 添加监控", this);
    mainLayout->addWidget(m_addButton);

    // 滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainLayout->addWidget(m_scrollArea);

    // 内容容器
    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(10);
    m_contentLayout->addStretch();  // 底部弹簧，保持图表紧凑

    m_scrollArea->setWidget(m_contentWidget);

    // 连接信号
    connect(m_addButton, &QPushButton::clicked,
            this, &MonitorPanel::onAddButtonClicked);
}

void MonitorPanel::addChart(const QString &fieldName,
                            const QString &unit,
                            int xRangeSeconds,
                            int xTickCount)
{
    auto *chart = new MonitorChart(fieldName, unit, xRangeSeconds, xTickCount, this);

    // 连接信号
    connect(chart, &MonitorChart::deleteRequested,
            this, &MonitorPanel::onChartDeleteRequested);
    connect(chart, &MonitorChart::editRequested,
            this, &MonitorPanel::onChartEditRequested);

    // 添加到布局（在弹簧之前）
    m_contentLayout->insertWidget(m_contentLayout->count() - 1, chart);
    m_charts.append(chart);
}

void MonitorPanel::removeChart(MonitorChart *chart)
{
    if (!chart) {
        return;
    }

    // 从布局移除
    m_contentLayout->removeWidget(chart);

    // 从列表移除
    m_charts.removeOne(chart);

    // 安全删除（事件循环结束后）
    chart->deleteLater();
}

void MonitorPanel::clearAllCharts()
{
    // 复制列表，避免遍历时修改
    QList<MonitorChart*> charts = m_charts;
    for (auto *chart : charts) {
        removeChart(chart);
    }
}

void MonitorPanel::onAddButtonClicked()
{
    // 获取当前激活的协议配置
    auto *protocolManager = ProtocolManager::instance();
    if (!protocolManager) {
        QMessageBox::warning(this, "提示", "协议管理器未初始化");
        return;
    }

    // 获取当前协议名称
    QString currentProtocolName = protocolManager->getCurrentProtocol();
    if (currentProtocolName.isEmpty()) {
        QMessageBox::warning(this, "提示", "请先配置并激活协议");
        return;
    }

    // 获取协议配置
    ProtocolConfig currentProtocol = protocolManager->getProtocol(currentProtocolName);
    if (currentProtocol.name.isEmpty()) {
        QMessageBox::warning(this, "提示", "协议配置无效");
        return;
    }

    // 构建字段列表
    QMap<QString, QString> fields;
    for (const auto &field : currentProtocol.fields) {
        fields[field.name] = field.unit;
    }

    if (fields.isEmpty()) {
        QMessageBox::warning(this, "提示", "当前协议没有可监控的字段");
        return;
    }

    // 创建并显示配置对话框
    MonitorConfigDialog dialog(this);
    dialog.setAvailableFields(fields);

    if (dialog.exec() == QDialog::Accepted) {
        // 添加图表
        addChart(dialog.selectedField(),
                 dialog.selectedUnit(),
                 dialog.xRangeSeconds(),
                 dialog.xTickCount());
    }
}

void MonitorPanel::onChartDeleteRequested(MonitorChart *chart)
{
    removeChart(chart);
}

void MonitorPanel::onChartEditRequested(MonitorChart *chart)
{
    // TODO: TASK-008中实现
    // 弹出编辑对话框
}
