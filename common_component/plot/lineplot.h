#ifndef LINEPLOT_H
#define LINEPLOT_H

#include "basicplot.h"

/**
 * @brief 线图部件
 *
 * 用于显示传感器数据的实时曲线
 */
class LinePlot : public BasicPlot
{
    Q_OBJECT

public:
    explicit LinePlot(QWidget *parent = nullptr);
    ~LinePlot();

    /**
     * @brief 设置线条颜色
     * @param color 颜色
     */
    void setLineColor(const QColor &color);

    /**
     * @brief 设置线条宽度
     * @param width 宽度
     */
    void setLineWidth(int width);

    /**
     * @brief 设置是否显示数据点
     * @param show 是否显示
     */
    void setShowDataPoints(bool show);

protected:
    void drawPlot(QPainter &painter) override;

private:
    QColor m_lineColor;
    int m_lineWidth;
    bool m_showDataPoints;
};

#endif // LINEPLOT_H
