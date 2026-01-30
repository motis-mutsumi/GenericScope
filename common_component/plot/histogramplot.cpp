#include "histogramplot.h"
#include <QtMath>
#include <algorithm>

HistogramPlot::HistogramPlot(QWidget *parent)
    : BasicPlot(parent)
    , m_barColor(Qt::darkGreen)
    , m_binCount(20)
    , m_dataMin(0.0)
    , m_dataMax(100.0)
{
}

HistogramPlot::~HistogramPlot()
{
}

void HistogramPlot::setBarColor(const QColor &color)
{
    m_barColor = color;
    update();
}

void HistogramPlot::setBinCount(int bins)
{
    m_binCount = bins;
    calculateHistogram();
    update();
}

void HistogramPlot::setRawData(const QVector<double> &data)
{
    m_rawData = data;
    calculateHistogram();
    update();
}

void HistogramPlot::calculateHistogram()
{
    if (m_rawData.isEmpty()) {
        return;
    }

    // 计算数据范围
    m_dataMin = *std::min_element(m_rawData.begin(), m_rawData.end());
    m_dataMax = *std::max_element(m_rawData.begin(), m_rawData.end());

    double range = m_dataMax - m_dataMin;
    if (range < 0.001) range = 1.0;

    // 初始化直方图
    m_histogram.clear();
    m_histogram.resize(m_binCount);
    m_histogram.fill(0);

    // 计算直方图
    for (double value : m_rawData) {
        int bin = qFloor((value - m_dataMin) / range * m_binCount);
        if (bin >= m_binCount) bin = m_binCount - 1;
        if (bin < 0) bin = 0;
        m_histogram[bin]++;
    }

    // 设置 X 和 Y 数据用于基类绘制
    m_xData.clear();
    m_yData.clear();
    for (int i = 0; i < m_binCount; ++i) {
        double binCenter = m_dataMin + (i + 0.5) * range / m_binCount;
        m_xData.append(binCenter);
        m_yData.append(m_histogram[i]);
    }
}

void HistogramPlot::drawPlot(QPainter &painter)
{
    if (m_histogram.isEmpty()) {
        return;
    }

    int margin = 50;
    int plotWidth = width() - 2 * margin;
    int plotHeight = height() - 2 * margin;

    // 计算最大频数
    int maxCount = *std::max_element(m_histogram.begin(), m_histogram.end());
    if (maxCount == 0) maxCount = 1;

    double range = m_dataMax - m_dataMin;
    if (range < 0.001) range = 1.0;

    double barWidth = (double)plotWidth / m_binCount;

    // 绘制柱状图
    painter.setPen(getTextColor());
    painter.setBrush(m_barColor);

    for (int i = 0; i < m_binCount; ++i) {
        double x = margin + i * barWidth;
        double barHeight = (double)m_histogram[i] / maxCount * plotHeight;
        double y = height() - margin - barHeight;

        painter.drawRect(QRectF(x, y, barWidth - 1, barHeight));
    }

    // 绘制刻度值
    painter.setPen(getTextColor());
    painter.setFont(QFont("Arial", 8));

    // Y 轴刻度（频数）
    for (int i = 0; i <= 5; ++i) {
        int value = i * maxCount / 5;
        int y = height() - margin - i * plotHeight / 5;
        painter.drawText(5, y + 5, QString::number(value));
    }

    // X 轴刻度（数据值）
    for (int i = 0; i <= 5; ++i) {
        double value = m_dataMin + i * range / 5;
        int x = margin + i * plotWidth / 5;
        painter.drawText(x - 20, height() - margin + 20, QString::number(value, 'f', 2));
    }
}
