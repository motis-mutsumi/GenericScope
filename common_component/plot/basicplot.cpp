#include "basicplot.h"
#include <QPaintEvent>
#include <QtMath>
#include <QPalette>
#include <QApplication>

BasicPlot::BasicPlot(QWidget *parent)
    : QWidget(parent)
    , m_autoScale(true)
    , m_yMin(0.0)
    , m_yMax(100.0)
    , m_maxDataPoints(0)
{
    setMinimumSize(400, 300);
}

BasicPlot::~BasicPlot()
{
}

void BasicPlot::setData(const QVector<double> &xData, const QVector<double> &yData)
{
    m_xData = xData;
    m_yData = yData;
    update();
}

void BasicPlot::addDataPoint(double x, double y)
{
    m_xData.append(x);
    m_yData.append(y);

    // 限制数据点数量
    if (m_maxDataPoints > 0 && m_xData.size() > m_maxDataPoints) {
        m_xData.removeFirst();
        m_yData.removeFirst();
    }

    update();
}

void BasicPlot::clearData()
{
    m_xData.clear();
    m_yData.clear();
    update();
}

void BasicPlot::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void BasicPlot::setAxisLabels(const QString &xLabel, const QString &yLabel)
{
    m_xLabel = xLabel;
    m_yLabel = yLabel;
    update();
}

void BasicPlot::setAutoScale(bool enabled)
{
    m_autoScale = enabled;
    update();
}

void BasicPlot::setYRange(double min, double max)
{
    m_yMin = min;
    m_yMax = max;
    m_autoScale = false;
    update();
}

void BasicPlot::setMaxDataPoints(int maxPoints)
{
    m_maxDataPoints = maxPoints;
}

void BasicPlot::refresh()
{
    update();
}

void BasicPlot::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景 - 使用动态颜色
    painter.fillRect(rect(), getBackgroundColor());

    // 绘制标题
    drawTitle(painter);

    // 绘制网格
    drawGrid(painter);

    // 绘制坐标轴
    drawAxes(painter);

    // 绘制数据
    drawPlot(painter);
}

void BasicPlot::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    update();
}

void BasicPlot::drawPlot(QPainter &painter)
{
    // 子类实现具体绘图逻辑
    Q_UNUSED(painter);
}

void BasicPlot::drawAxes(QPainter &painter)
{
    int margin = 50;
    int plotWidth = width() - 2 * margin;
    int plotHeight = height() - 2 * margin;

    painter.setPen(QPen(getAxisColor(), 2));

    // X 轴
    painter.drawLine(margin, height() - margin, width() - margin, height() - margin);

    // Y 轴
    painter.drawLine(margin, margin, margin, height() - margin);

    // 轴标签
    painter.setFont(QFont("Arial", 10));
    painter.setPen(getTextColor());
    painter.drawText(width() / 2 - 50, height() - 10, m_xLabel);
    painter.save();
    painter.translate(10, height() / 2 + 50);
    painter.rotate(-90);
    painter.drawText(0, 0, m_yLabel);
    painter.restore();
}

void BasicPlot::drawTitle(QPainter &painter)
{
    if (m_title.isEmpty()) {
        return;
    }

    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.setPen(getTextColor());
    painter.drawText(rect(), Qt::AlignTop | Qt::AlignHCenter, m_title);
}

void BasicPlot::drawGrid(QPainter &painter)
{
    int margin = 50;
    int plotWidth = width() - 2 * margin;
    int plotHeight = height() - 2 * margin;

    painter.setPen(QPen(getGridColor(), 1, Qt::DotLine));

    // 水平网格线
    for (int i = 1; i < 5; ++i) {
        int y = margin + i * plotHeight / 5;
        painter.drawLine(margin, y, width() - margin, y);
    }

    // 垂直网格线
    for (int i = 1; i < 5; ++i) {
        int x = margin + i * plotWidth / 5;
        painter.drawLine(x, margin, x, height() - margin);
    }
}

QColor BasicPlot::getBackgroundColor() const
{
    // 检测当前主题 - 通过窗口背景色判断
    QPalette pal = QApplication::palette();
    QColor bgColor = pal.color(QPalette::Window);

    // 如果背景是深色，返回深色图表背景
    if (bgColor.lightness() < 128) {
        return QColor(0x1E, 0x1E, 0x1E); // 深色主题背景
    } else {
        return QColor(0xF5, 0xF5, 0xF5); // 浅色主题背景
    }
}

QColor BasicPlot::getTextColor() const
{
    QPalette pal = QApplication::palette();
    QColor bgColor = pal.color(QPalette::Window);

    if (bgColor.lightness() < 128) {
        return QColor(0xD4, 0xD4, 0xD4); // 深色主题文本
    } else {
        return QColor(0x21, 0x21, 0x21); // 浅色主题文本
    }
}

QColor BasicPlot::getGridColor() const
{
    QPalette pal = QApplication::palette();
    QColor bgColor = pal.color(QPalette::Window);

    if (bgColor.lightness() < 128) {
        return QColor(0x3A, 0x3A, 0x3A); // 深色主题网格线
    } else {
        return QColor(0xE0, 0xE0, 0xE0); // 浅色主题网格线
    }
}

QColor BasicPlot::getAxisColor() const
{
    QPalette pal = QApplication::palette();
    QColor bgColor = pal.color(QPalette::Window);

    if (bgColor.lightness() < 128) {
        return QColor(0xA0, 0xA0, 0xA0); // 深色主题坐标轴
    } else {
        return QColor(0x42, 0x42, 0x42); // 浅色主题坐标轴
    }
}
