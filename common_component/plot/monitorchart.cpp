#include "monitorchart.h"
#include "monitordatamanager.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>
#include <QtMath>

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

    connect(MonitorDataManager::instance(), &MonitorDataManager::dataUpdated,
            this, &MonitorChart::onDataReceived,
            Qt::QueuedConnection);
}

MonitorChart::~MonitorChart()
{
}

void MonitorChart::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    const QString title = m_unit.isEmpty()
        ? m_fieldName
        : QString("%1 (%2)").arg(m_fieldName, m_unit);
    QLabel *titleLabel = new QLabel(title, this);
    titleLabel->setObjectName("titleLabel");
    QFont font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);
    mainLayout->addWidget(titleLabel);

    m_plot = new QCustomPlot(this);
    m_plot->setFixedHeight(200);
    m_plot->setContextMenuPolicy(Qt::NoContextMenu);
    m_plot->installEventFilter(this);
    mainLayout->addWidget(m_plot);

    setupPlot();

    setObjectName("monitorChart");
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setContextMenuPolicy(Qt::DefaultContextMenu);
}

void MonitorChart::setupPlot()
{
    m_plot->addGraph();

    QPen pen(QColor(0, 120, 215));
    pen.setWidth(2);
    m_plot->graph(0)->setPen(pen);
    m_plot->graph(0)->setAdaptiveSampling(true);

    m_plot->xAxis->setLabel(QStringLiteral("Time (s)"));
    m_plot->yAxis->setLabel(m_unit.isEmpty() ? QStringLiteral("Value") : m_unit);
    m_plot->xAxis->setRange(0, m_xRangeSeconds);
    m_plot->xAxis->setPadding(6);
    m_plot->yAxis->setPadding(6);

    m_plot->xAxis->grid()->setVisible(true);
    m_plot->yAxis->grid()->setVisible(true);
    m_plot->xAxis->grid()->setSubGridVisible(false);
    m_plot->yAxis->grid()->setSubGridVisible(false);
    m_plot->xAxis->grid()->setPen(QPen(QColor(220, 225, 232), 1, Qt::DotLine));
    m_plot->yAxis->grid()->setPen(QPen(QColor(220, 225, 232), 1, Qt::DotLine));

    updateXAxisTicker(m_xRangeSeconds);

    m_plot->setAntialiasedElements(QCP::aeAxes | QCP::aeGrid | QCP::aePlottables);
    m_plot->setBackground(QBrush(Qt::white));
    m_plot->axisRect()->setBackground(QColor(252, 253, 255));
    m_plot->setInteractions(QCP::iNone);
}

void MonitorChart::createActions()
{
    m_editAction = new QAction(QStringLiteral("Edit"), this);
    connect(m_editAction, &QAction::triggered, this, &MonitorChart::onEditAction);

    m_pauseAction = new QAction(QStringLiteral("Pause"), this);
    connect(m_pauseAction, &QAction::triggered, this, &MonitorChart::onPauseAction);

    m_clearAction = new QAction(QStringLiteral("Clear Data"), this);
    connect(m_clearAction, &QAction::triggered, this, &MonitorChart::onClearAction);

    m_deleteAction = new QAction(QStringLiteral("Delete"), this);
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

    m_pauseAction->setText(m_paused ? QStringLiteral("Resume") : QStringLiteral("Pause"));
    menu.exec(event->globalPos());
}

bool MonitorChart::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_plot && event->type() == QEvent::ContextMenu) {
        QContextMenuEvent *contextEvent = static_cast<QContextMenuEvent *>(event);
        contextMenuEvent(contextEvent);
        return true;
    }

    return QFrame::eventFilter(watched, event);
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
    qDebug() << QString("[%1] %2")
                    .arg(m_fieldName)
                    .arg(m_paused ? QStringLiteral("paused") : QStringLiteral("resumed"));
}

void MonitorChart::onClearAction()
{
    clearData();
}

void MonitorChart::setFieldName(const QString &fieldName)
{
    if (m_fieldName == fieldName) {
        return;
    }

    m_fieldName = fieldName;
    updateTitle();
    clearData();

    qDebug() << QString("[MonitorChart] field changed to: %1").arg(m_fieldName);
}

void MonitorChart::setUnit(const QString &unit)
{
    if (m_unit == unit) {
        return;
    }

    m_unit = unit;
    updateTitle();

    if (m_plot) {
        m_plot->yAxis->setLabel(m_unit.isEmpty() ? QStringLiteral("Value") : m_unit);
        m_plot->replot(QCustomPlot::rpQueuedReplot);
    }

    qDebug() << QString("[%1] unit changed to: %2").arg(m_fieldName, m_unit);
}

void MonitorChart::setXRange(int seconds)
{
    if (m_xRangeSeconds == seconds) {
        return;
    }

    m_xRangeSeconds = seconds;

    if (!m_timestamps.isEmpty()) {
        const qint64 latestTime = m_timestamps.last();
        const qint64 cutoffTime = latestTime - static_cast<qint64>(m_xRangeSeconds) * 1000;

        while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
            m_timestamps.removeFirst();
            m_values.removeFirst();
        }
    }

    updatePlot();
    qDebug() << QString("[%1] x range changed to: %2 s").arg(m_fieldName).arg(m_xRangeSeconds);
}

void MonitorChart::setXTickCount(int count)
{
    if (m_xTickCount == count) {
        return;
    }

    m_xTickCount = count;
    updatePlot();

    qDebug() << QString("[%1] x tick count changed to: %2").arg(m_fieldName).arg(m_xTickCount);
}

void MonitorChart::updateTitle()
{
    const QString title = m_unit.isEmpty()
        ? m_fieldName
        : QString("%1 (%2)").arg(m_fieldName, m_unit);

    QLabel *titleLabel = findChild<QLabel *>("titleLabel");
    if (titleLabel) {
        titleLabel->setText(title);
    }
}

void MonitorChart::appendData(double value, qint64 timestamp)
{
    if (!m_timestamps.isEmpty() && timestamp <= m_timestamps.last()) {
        timestamp = m_timestamps.last() + 1;
    }

    m_timestamps.append(timestamp);
    m_values.append(value);

    const qint64 cutoffTime = timestamp - static_cast<qint64>(m_xRangeSeconds) * 1000;
    while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
        m_timestamps.removeFirst();
        m_values.removeFirst();
    }

    const int maxPoints = 10000;
    if (m_timestamps.size() > maxPoints) {
        const int removeCount = m_timestamps.size() - maxPoints;
        m_timestamps.remove(0, removeCount);
        m_values.remove(0, removeCount);
    }

    updatePlot();
}

void MonitorChart::clearData()
{
    m_timestamps.clear();
    m_values.clear();

    if (m_plot && m_plot->graph(0)) {
        m_plot->graph(0)->data()->clear();
        m_plot->replot(QCustomPlot::rpQueuedReplot);
    }

    qDebug() << QString("[%1] data cleared").arg(m_fieldName);
}

void MonitorChart::setPaused(bool paused)
{
    m_paused = paused;
}

void MonitorChart::onDataReceived(const QString &fieldName, double value, qint64 timestamp)
{
    if (fieldName != m_fieldName || m_paused) {
        return;
    }

    appendData(value, timestamp);

#ifdef QT_DEBUG
    static int debugCounter = 0;
    if (++debugCounter % 10 == 0) {
        qDebug() << QString("[%1] received data: %2 @ %3, buffered: %4 points")
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
        m_plot->replot(QCustomPlot::rpQueuedReplot);
        return;
    }

    QVector<double> xData;
    QVector<double> yData;
    xData.reserve(m_timestamps.size());
    yData.reserve(m_values.size());

    const qint64 firstTime = m_timestamps.first();
    const qint64 latestTime = m_timestamps.last();
    const double elapsedSeconds = qMax(0.0, (latestTime - firstTime) / 1000.0);
    const double visibleRangeSeconds = qMax(1.0, qMin(static_cast<double>(m_xRangeSeconds), elapsedSeconds));
    const bool shouldScroll = elapsedSeconds >= m_xRangeSeconds;
    const double windowStartSeconds = shouldScroll ? (elapsedSeconds - m_xRangeSeconds) : 0.0;

    for (int i = 0; i < m_timestamps.size(); ++i) {
        const double elapsed = (m_timestamps[i] - firstTime) / 1000.0;
        xData.append(elapsed - windowStartSeconds);
        yData.append(m_values[i]);
    }

    m_plot->graph(0)->setData(xData, yData);
    m_plot->xAxis->setRange(0, shouldScroll ? m_xRangeSeconds : visibleRangeSeconds);
    updateXAxisTicker(shouldScroll ? m_xRangeSeconds : visibleRangeSeconds);
    updateYAxisRange();
    m_plot->replot(QCustomPlot::rpQueuedReplot);
}

void MonitorChart::updateXAxisTicker(double visibleRangeSeconds)
{
    if (!m_plot) {
        return;
    }

    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    const double range = qMax(1.0, visibleRangeSeconds);
    const double tickStep = (m_xTickCount > 1)
        ? range / (m_xTickCount - 1)
        : range;
    fixedTicker->setTickStep(tickStep);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    m_plot->xAxis->setTicker(fixedTicker);
}

void MonitorChart::updateYAxisRange()
{
    if (!m_plot || m_values.isEmpty()) {
        return;
    }

    double minValue = m_values.first();
    double maxValue = m_values.first();
    for (double value : m_values) {
        minValue = qMin(minValue, value);
        maxValue = qMax(maxValue, value);
    }

    const double center = (minValue + maxValue) * 0.5;
    const double valueRange = maxValue - minValue;
    double padding = valueRange * 0.2;
    padding = qMax(padding, 0.05);
    padding = qMax(padding, qAbs(center) * 0.002);

    if (valueRange < 1e-6) {
        minValue = center - padding;
        maxValue = center + padding;
    } else {
        minValue -= padding;
        maxValue += padding;
    }

    m_plot->yAxis->setRange(minValue, maxValue);
}
