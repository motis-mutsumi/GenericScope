#include "lineplot.h"
#include <QtMath>

LinePlot::LinePlot(QWidget *parent)
    : BasicPlot(parent)
    , m_lineColor(Qt::blue)
    , m_lineWidth(2)
    , m_showDataPoints(false)
{
}

LinePlot::~LinePlot()
{
}

void LinePlot::setLineColor(const QColor &color)
{
    m_lineColor = color;
    update();
}

void LinePlot::setLineWidth(int width)
{
    m_lineWidth = width;
    update();
}

void LinePlot::setShowDataPoints(bool show)
{
    m_showDataPoints = show;
    update();
}

void LinePlot::drawPlot(QPainter &painter)
{
    if (m_xData.isEmpty() || m_yData.isEmpty()) {
        return;
    }

    int margin = 50;
    int plotWidth = width() - 2 * margin;
    int plotHeight = height() - 2 * margin;

    // 计算数据范围
    double xMin = m_xData.first();
    double xMax = m_xData.last();
    double yMin = m_yMin;
    double yMax = m_yMax;

    if (m_autoScale) {
        yMin = *std::min_element(m_yData.begin(), m_yData.end());
        yMax = *std::max_element(m_yData.begin(), m_yData.end());

        // 添加一些边距
        double yRange = yMax - yMin;
        if (yRange < 0.001) yRange = 1.0;
        yMin -= yRange * 0.1;
        yMax += yRange * 0.1;
    }

    double xRange = xMax - xMin;
    if (xRange < 0.001) xRange = 1.0;
    double yRange = yMax - yMin;
    if (yRange < 0.001) yRange = 1.0;

    // 绘制线条
    painter.setPen(QPen(m_lineColor, m_lineWidth));

    for (int i = 0; i < m_xData.size() - 1; ++i) {
        double x1 = margin + (m_xData[i] - xMin) / xRange * plotWidth;
        double y1 = height() - margin - (m_yData[i] - yMin) / yRange * plotHeight;
        double x2 = margin + (m_xData[i + 1] - xMin) / xRange * plotWidth;
        double y2 = height() - margin - (m_yData[i + 1] - yMin) / yRange * plotHeight;

        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }

    // 绘制数据点
    if (m_showDataPoints) {
        painter.setBrush(m_lineColor);
        for (int i = 0; i < m_xData.size(); ++i) {
            double x = margin + (m_xData[i] - xMin) / xRange * plotWidth;
            double y = height() - margin - (m_yData[i] - yMin) / yRange * plotHeight;
            painter.drawEllipse(QPointF(x, y), 3, 3);
        }
    }

    // 绘制刻度值
    painter.setPen(getTextColor());
    painter.setFont(QFont("Arial", 8));

    // Y 轴刻度
    for (int i = 0; i <= 5; ++i) {
        double value = yMin + i * (yMax - yMin) / 5;
        int y = height() - margin - i * plotHeight / 5;
        painter.drawText(5, y + 5, QString::number(value, 'f', 2));
    }

    // X 轴刻度
    for (int i = 0; i <= 5; ++i) {
        double value = xMin + i * (xMax - xMin) / 5;
        int x = margin + i * plotWidth / 5;
        painter.drawText(x - 20, height() - margin + 20, QString::number(value, 'f', 1));
    }
}
