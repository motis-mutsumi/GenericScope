#ifndef HISTOGRAMPLOT_H
#define HISTOGRAMPLOT_H

#include "basicplot.h"

/**
 * @brief 直方图部件
 *
 * 用于显示数据分布和统计信息
 */
class HistogramPlot : public BasicPlot
{
    Q_OBJECT

public:
    explicit HistogramPlot(QWidget *parent = nullptr);
    ~HistogramPlot();

    /**
     * @brief 设置柱状图颜色
     * @param color 颜色
     */
    void setBarColor(const QColor &color);

    /**
     * @brief 设置分组数量
     * @param bins 分组数
     */
    void setBinCount(int bins);

    /**
     * @brief 设置数据（自动计算直方图）
     * @param data 原始数据
     */
    void setRawData(const QVector<double> &data);

protected:
    void drawPlot(QPainter &painter) override;

private:
    void calculateHistogram();

private:
    QColor m_barColor;
    int m_binCount;
    QVector<double> m_rawData;
    QVector<int> m_histogram;
    double m_dataMin;
    double m_dataMax;
};

#endif // HISTOGRAMPLOT_H
