#ifndef MONITORCONFIGDIALOG_H
#define MONITORCONFIGDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMap>

class MonitorConfigDialog : public QDialog {
    Q_OBJECT
public:
    explicit MonitorConfigDialog(QWidget *parent = nullptr);

    /**
     * @brief 设置可用字段列表
     * @param fields 字段映射 <字段名, 单位>
     */
    void setAvailableFields(const QMap<QString, QString> &fields);

    /**
     * @brief 获取选中的字段名
     */
    QString selectedField() const;

    /**
     * @brief 获取选中字段的单位
     */
    QString selectedUnit() const;

    /**
     * @brief 获取X轴时间范围
     */
    int xRangeSeconds() const;

    /**
     * @brief 获取X轴刻度数量
     */
    int xTickCount() const;

    /**
     * @brief 设置编辑模式（填充当前配置）
     * @param fieldName 字段名
     * @param unit 单位
     * @param xRangeSeconds X轴范围
     * @param xTickCount X轴刻度数
     */
    void setEditMode(const QString &fieldName,
                     const QString &unit,
                     int xRangeSeconds,
                     int xTickCount);

private slots:
    void onFieldChanged(int index);

private:
    void setupUI();

    QComboBox *m_fieldComboBox;        // 字段选择
    QSpinBox *m_xRangeSpinBox;         // X轴范围
    QSpinBox *m_xTickSpinBox;          // X轴刻度数
    QDialogButtonBox *m_buttonBox;     // 按钮
    QMap<QString, QString> m_fieldUnitMap;  // 字段-单位映射

    bool m_editMode;                   // 编辑模式标志
};

#endif // MONITORCONFIGDIALOG_H
