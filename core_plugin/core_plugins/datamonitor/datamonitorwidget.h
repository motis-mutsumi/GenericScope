#ifndef DATAMONITORWIDGET_H
#define DATAMONITORWIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class DataMonitorWidget;
}

/**
 * @brief 数据监控插件UI组件
 *
 * 显示传感器数据和阈值，提供警报日志。
 */
class DataMonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataMonitorWidget(QWidget *parent = nullptr);
    ~DataMonitorWidget();

    /**
     * @brief 设置阈值配置
     * @param thresholds 字段名->阈值映射
     */
    void setThresholds(const QMap<QString, double> &thresholds);

    /**
     * @brief 更新数据显示
     * @param fieldData 字段名->值映射
     */
    void updateData(const QVariantMap &fieldData);

signals:
    /**
     * @brief 阈值超限信号
     * @param fieldName 字段名
     * @param value 当前值
     */
    void thresholdExceeded(const QString &fieldName, double value);

private:
    void initTable();
    void addAlertLog(const QString &message);

private:
    Ui::DataMonitorWidget *ui;
    QMap<QString, double> m_thresholds;
    QMap<QString, int> m_fieldRowMap;  // 字段名->表格行号映射
};

#endif // DATAMONITORWIDGET_H
