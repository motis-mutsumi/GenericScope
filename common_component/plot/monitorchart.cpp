#include "monitorchart.h"
#include "monitordatamanager.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>
#include <QDebug>

MonitorChart::MonitorChart(const QString &fieldName,
                           const QString &unit,
                           int xRangeSeconds,
                           int xTickCount,
                           QWidget *parent)
    : QFrame(parent)
    , m_fieldName(fieldName)
    , m_unit(unit)
    , m_xRangeSeconds(xRangeSeconds)
    , m_xTickCount(xTickCount)
    , m_paused(false)
    , m_plot(nullptr)
{
    setupUI();
    createActions();

    // ========== 订阅数据更新信号 ==========
    // 使用Qt::QueuedConnection确保线程安全
    connect(MonitorDataManager::instance(), &MonitorDataManager::dataUpdated,
            this, &MonitorChart::onDataReceived,
            Qt::QueuedConnection);
    // ======================================
}

MonitorChart::~MonitorChart()
{
}

void MonitorChart::setupUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 标题栏
    QString title = m_unit.isEmpty() ? m_fieldName : QString("%1 (%2)").arg(m_fieldName, m_unit);
    QLabel *titleLabel = new QLabel(title, this);
    QFont font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);
    mainLayout->addWidget(titleLabel);

    // 创建QCustomPlot
    m_plot = new QCustomPlot(this);
    m_plot->setFixedHeight(200);
    mainLayout->addWidget(m_plot);

    // 配置图表
    setupPlot();

    // 设置边框
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
}

void MonitorChart::setupPlot()
{
    // 添加图形
    m_plot->addGraph();

    // 设置曲线样式
    QPen pen;
    pen.setColor(QColor(0, 120, 215));  // 蓝色
    pen.setWidth(2);
    m_plot->graph(0)->setPen(pen);

    // 配置坐标轴
    m_plot->xAxis->setLabel("时间 (秒)");
    m_plot->yAxis->setLabel(m_unit.isEmpty() ? "数值" : m_unit);

    // X轴范围（负数表示过去时间，0表示当前）
    m_plot->xAxis->setRange(-m_xRangeSeconds, 0);

    // 启用抗锯齿
    m_plot->setAntialiasedElements(QCP::aeAll);

    // 设置背景色
    m_plot->setBackground(QBrush(Qt::white));

    // 允许用户交互
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void MonitorChart::createActions()
{
    m_editAction = new QAction("编辑", this);
    connect(m_editAction, &QAction::triggered, this, &MonitorChart::onEditAction);

    m_pauseAction = new QAction("暂停", this);
    connect(m_pauseAction, &QAction::triggered, this, &MonitorChart::onPauseAction);

    m_clearAction = new QAction("Clear数据", this);
    connect(m_clearAction, &QAction::triggered, this, &MonitorChart::onClearAction);

    m_deleteAction = new QAction("删除", this);
    connect(m_deleteAction, &QAction::triggered, this, &MonitorChart::onDeleteAction);
}

void MonitorChart::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_editAction);
    menu.addAction(m_pauseAction);
    menu.addAction(m_clearAction);
    menu.addSeparator();
    menu.addAction(m_deleteAction);

    // 更新暂停按钮文本
    m_pauseAction->setText(m_paused ? "恢复" : "暂停");

    menu.exec(event->globalPos());
}

void MonitorChart::onDeleteAction()
{
    emit deleteRequested(this);
}

void MonitorChart::onEditAction()
{
    emit editRequested(this);
}

void MonitorChart::onPauseAction()
{
    setPaused(!m_paused);
    qDebug() << QString("[%1] %2").arg(m_fieldName).arg(m_paused ? "已暂停" : "已恢复");
}

void MonitorChart::onClearAction()
{
    clearData();
}

// 以下方法在后续任务中实现

void MonitorChart::setFieldName(const QString &fieldName)
{
    m_fieldName = fieldName;
    // TODO: 更新标题
}

void MonitorChart::setXRange(int seconds)
{
    m_xRangeSeconds = seconds;
    // TODO: 更新图表
}

void MonitorChart::setXTickCount(int count)
{
    m_xTickCount = count;
    // TODO: 更新图表
}

void MonitorChart::appendData(double value, qint64 timestamp)
{
    // 添加新数据点
    m_timestamps.append(timestamp);
    m_values.append(value);

    // 移除超出X轴范围的旧数据
    qint64 cutoffTime = timestamp - m_xRangeSeconds * 1000;  // 转换为毫秒
    while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
        m_timestamps.removeFirst();
        m_values.removeFirst();
    }

    // 限制最大缓存点数（防止极端情况）
    const int MAX_POINTS = 10000;
    if (m_timestamps.size() > MAX_POINTS) {
        int removeCount = m_timestamps.size() - MAX_POINTS;
        m_timestamps.remove(0, removeCount);
        m_values.remove(0, removeCount);
    }

    // 更新图表
    updatePlot();
}

void MonitorChart::clearData()
{
    m_timestamps.clear();
    m_values.clear();

    // 清空图表显示
    if (m_plot && m_plot->graph(0)) {
        m_plot->graph(0)->data()->clear();
        m_plot->replot();
    }

    qDebug() << QString("[%1] 数据已清空").arg(m_fieldName);
}

void MonitorChart::setPaused(bool paused)
{
    m_paused = paused;
}

void MonitorChart::onDataReceived(const QString &fieldName, double value, qint64 timestamp)
{
    // 字段过滤：只处理匹配的字段
    if (fieldName != m_fieldName) {
        return;
    }

    // 暂停检查
    if (m_paused) {
        return;
    }

    // 添加数据并绘制
    appendData(value, timestamp);

#ifdef QT_DEBUG
    // 调试输出（可选）
    static int debugCounter = 0;
    if (++debugCounter % 10 == 0) {  // 每10个点输出一次，避免刷屏
        qDebug() << QString("[%1] 收到数据: %2 @ %3, 缓存: %4个点")
                        .arg(m_fieldName)
                        .arg(value, 0, 'f', 2)
                        .arg(timestamp)
                        .arg(m_timestamps.size());
    }
#endif
}

void MonitorChart::updatePlot()
{
    if (!m_plot) {
        return;
    }

    if (m_timestamps.isEmpty()) {
        m_plot->graph(0)->data()->clear();
        m_plot->replot();
        return;
    }

    // 转换时间戳为相对时间（秒）
    QVector<double> xData, yData;
    qint64 latestTime = m_timestamps.last();

    for (int i = 0; i < m_timestamps.size(); ++i) {
        // 相对时间（秒），负数表示过去的时间
        double relativeTime = (m_timestamps[i] - latestTime) / 1000.0;
        xData.append(relativeTime);
        yData.append(m_values[i]);
    }

    // 更新QCustomPlot数据
    m_plot->graph(0)->setData(xData, yData);

    // 设置X轴范围
    m_plot->xAxis->setRange(-m_xRangeSeconds, 0);

    // Y轴自动缩放
    m_plot->graph(0)->rescaleValueAxis();

    // 设置X轴刻度数量
    QSharedPointer<QCPAxisTicker> ticker = m_plot->xAxis->ticker();
    ticker->setTickCount(m_xTickCount);

    // 重绘
    m_plot->replot();
}
