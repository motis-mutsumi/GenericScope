#ifndef BASICPLOT_H
#define BASICPLOT_H

#include <QWidget>
#include <QVector>
#include <QPainter>

/**
 * @brief 基础绘图部件
 *
 * 提供基本的绘图功能，可以扩展为各种图表类型
 */
class BasicPlot : public QWidget
{
    Q_OBJECT

public:
    explicit BasicPlot(QWidget *parent = nullptr);
    ~BasicPlot();

    /**
     * @brief 设置数据
     * @param xData X 轴数据
     * @param yData Y 轴数据
     */
    void setData(const QVector<double> &xData, const QVector<double> &yData);

    /**
     * @brief 添加数据点
     * @param x X 值
     * @param y Y 值
     */
    void addDataPoint(double x, double y);

    /**
     * @brief 清空数据
     */
    void clearData();

    /**
     * @brief 设置标题
     * @param title 标题文本
     */
    void setTitle(const QString &title);

    /**
     * @brief 设置轴标签
     * @param xLabel X 轴标签
     * @param yLabel Y 轴标签
     */
    void setAxisLabels(const QString &xLabel, const QString &yLabel);

    /**
     * @brief 设置自动缩放
     * @param enabled 是否启用
     */
    void setAutoScale(bool enabled);

    /**
     * @brief 设置 Y 轴范围
     * @param min 最小值
     * @param max 最大值
     */
    void setYRange(double min, double max);

    /**
     * @brief 设置最大数据点数
     * @param maxPoints 最大点数（0表示无限制）
     */
    void setMaxDataPoints(int maxPoints);

public slots:
    /**
     * @brief 刷新绘图
     */
    void refresh();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    virtual void drawPlot(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawTitle(QPainter &painter);
    void drawGrid(QPainter &painter);

    // 主题相关颜色获取方法
    QColor getBackgroundColor() const;
    QColor getTextColor() const;
    QColor getGridColor() const;
    QColor getAxisColor() const;

protected:
    QVector<double> m_xData;
    QVector<double> m_yData;
    QString m_title;
    QString m_xLabel;
    QString m_yLabel;
    bool m_autoScale;
    double m_yMin;
    double m_yMax;
    int m_maxDataPoints;
};

#endif // BASICPLOT_H
