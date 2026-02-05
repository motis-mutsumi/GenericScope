#ifndef MONITORPANEL_H
#define MONITORPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>

class MonitorChart;

class MonitorPanel : public QWidget {
    Q_OBJECT
public:
    explicit MonitorPanel(QWidget *parent = nullptr);
    ~MonitorPanel();

    /**
     * @brief 添加监控图表
     * @param fieldName 字段名称
     * @param unit 单位
     * @param xRangeSeconds X轴时间范围（秒）
     * @param xTickCount X轴刻度数量
     */
    void addChart(const QString &fieldName,
                  const QString &unit,
                  int xRangeSeconds,
                  int xTickCount);

    /**
     * @brief 移除指定图表
     * @param chart 要移除的图表指针
     */
    void removeChart(MonitorChart *chart);

    /**
     * @brief 清空所有图表
     */
    void clearAllCharts();

private slots:
    void onAddButtonClicked();
    void onChartDeleteRequested(MonitorChart *chart);
    void onChartEditRequested(MonitorChart *chart);

private:
    void setupUI();

    QPushButton *m_addButton;          // 添加按钮
    QScrollArea *m_scrollArea;         // 滚动区域
    QWidget *m_contentWidget;          // 内容容器
    QVBoxLayout *m_contentLayout;      // 纵向布局
    QList<MonitorChart*> m_charts;     // 图表列表
};

#endif // MONITORPANEL_H
