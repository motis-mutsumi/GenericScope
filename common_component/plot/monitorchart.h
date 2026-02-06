#ifndef MONITORCHART_H
#define MONITORCHART_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QMenu>
#include <QAction>
#include <QFrame>
#include "qcustomplot.h"

class MonitorChart : public QFrame {
    Q_OBJECT
public:
    explicit MonitorChart(const QString &fieldName,
                          const QString &unit,
                          int xRangeSeconds,
                          int xTickCount,
                          QWidget *parent = nullptr);
    ~MonitorChart();

    // 配置访问器
    QString fieldName() const { return m_fieldName; }
    QString unit() const { return m_unit; }
    int xRangeSeconds() const { return m_xRangeSeconds; }
    int xTickCount() const { return m_xTickCount; }
    bool isPaused() const { return m_paused; }

    /**
     * @brief 设置监控字段
     * @param fieldName 新字段名
     */
    void setFieldName(const QString &fieldName);

    /**
     * @brief 设置单位
     * @param unit 新单位
     */
    void setUnit(const QString &unit);

    /**
     * @brief 设置X轴时间范围
     * @param seconds 时间范围（秒）
     */
    void setXRange(int seconds);

    /**
     * @brief 设置X轴刻度数量
     * @param count 刻度数量
     */
    void setXTickCount(int count);

    /**
     * @brief 添加数据点
     * @param value 数值
     * @param timestamp 时间戳（毫秒）
     */
    void appendData(double value, qint64 timestamp);

    /**
     * @brief 清空数据缓存
     */
    void clearData();

    /**
     * @brief 设置暂停状态
     * @param paused true=暂停，false=恢复
     */
    void setPaused(bool paused);

signals:
    void deleteRequested(MonitorChart *chart);
    void editRequested(MonitorChart *chart);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onDataReceived(const QString &fieldName, double value, qint64 timestamp);
    void onDeleteAction();
    void onEditAction();
    void onPauseAction();
    void onClearAction();
    void updatePlot();

private:
    void setupUI();
    void setupPlot();  // 配置QCustomPlot样式
    void createActions();
    void updateTitle();  // 更新标题

    QString m_fieldName;               // 字段名
    QString m_unit;                    // 单位
    int m_xRangeSeconds;               // X轴时间范围（秒）
    int m_xTickCount;                  // X轴刻度数量
    bool m_paused;                     // 暂停状态

    QCustomPlot *m_plot;               // 绘图组件
    QVector<qint64> m_timestamps;      // 时间戳缓存
    QVector<double> m_values;          // 数值缓存

    QAction *m_deleteAction;
    QAction *m_editAction;
    QAction *m_pauseAction;
    QAction *m_clearAction;
};

#endif // MONITORCHART_H
