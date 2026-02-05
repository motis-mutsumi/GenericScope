# TASK-002: 实现配置对话框

## 任务信息

- **任务ID**: TASK-002
- **任务名称**: 实现配置对话框
- **优先级**: P0（必须完成）
- **预估工作量**: 3小时
- **依赖任务**: TASK-001
- **状态**: 待开始

## 任务描述

创建MonitorConfigDialog对话框，用于配置新图表的参数。用户通过此对话框选择要监控的字段、设置X轴范围和刻度数量。

## 具体目标

1. 创建MonitorConfigDialog类
2. 实现UI界面
   - 字段选择：QComboBox
   - X轴范围：QSpinBox (范围10-300秒，默认60)
   - X轴点数：QSpinBox (范围2-10，默认2)
   - 确定/取消按钮
3. 实现setAvailableFields()，填充字段下拉框
4. 实现获取配置的getter方法
5. 实现编辑模式setEditMode()

## 涉及文件

### 新增文件
- `common_component/plot/monitorconfigdialog.h`
- `common_component/plot/monitorconfigdialog.cpp`

### 修改文件
- `common_component/plot.pro`

## 实现细节

### 1. MonitorConfigDialog.h

```cpp
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
```

### 2. MonitorConfigDialog.cpp

```cpp
#include "monitorconfigdialog.h"
#include <QFormLayout>
#include <QVBoxLayout>

MonitorConfigDialog::MonitorConfigDialog(QWidget *parent)
    : QDialog(parent)
    , m_fieldComboBox(nullptr)
    , m_xRangeSpinBox(nullptr)
    , m_xTickSpinBox(nullptr)
    , m_buttonBox(nullptr)
    , m_editMode(false)
{
    setupUI();
}

void MonitorConfigDialog::setupUI()
{
    setWindowTitle("添加监控图表");
    setModal(true);
    resize(350, 200);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 表单布局
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setLabelAlignment(Qt::AlignRight);
    formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    // 字段选择
    m_fieldComboBox = new QComboBox(this);
    formLayout->addRow("监控字段:", m_fieldComboBox);

    // X轴范围
    m_xRangeSpinBox = new QSpinBox(this);
    m_xRangeSpinBox->setRange(10, 300);
    m_xRangeSpinBox->setValue(60);
    m_xRangeSpinBox->setSuffix(" 秒");
    formLayout->addRow("X轴范围:", m_xRangeSpinBox);

    // X轴点数
    m_xTickSpinBox = new QSpinBox(this);
    m_xTickSpinBox->setRange(2, 10);
    m_xTickSpinBox->setValue(2);
    formLayout->addRow("X轴点数:", m_xTickSpinBox);

    mainLayout->addLayout(formLayout);

    // 按钮
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        this);
    mainLayout->addWidget(m_buttonBox);

    // 连接信号
    connect(m_buttonBox, &QDialogButtonBox::accepted,
            this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
    connect(m_fieldComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MonitorConfigDialog::onFieldChanged);
}

void MonitorConfigDialog::setAvailableFields(const QMap<QString, QString> &fields)
{
    m_fieldUnitMap = fields;

    // 清空并填充下拉框
    m_fieldComboBox->clear();

    for (auto it = fields.begin(); it != fields.end(); ++it) {
        QString displayText = it.value().isEmpty()
                              ? it.key()  // 无单位
                              : QString("%1 (%2)").arg(it.key(), it.value());  // 有单位
        m_fieldComboBox->addItem(displayText, it.key());  // data存储字段名
    }

    // 如果有字段，默认选择第一个
    if (m_fieldComboBox->count() > 0) {
        m_fieldComboBox->setCurrentIndex(0);
    }
}

QString MonitorConfigDialog::selectedField() const
{
    return m_fieldComboBox->currentData().toString();
}

QString MonitorConfigDialog::selectedUnit() const
{
    QString fieldName = selectedField();
    return m_fieldUnitMap.value(fieldName, QString());
}

int MonitorConfigDialog::xRangeSeconds() const
{
    return m_xRangeSpinBox->value();
}

int MonitorConfigDialog::xTickCount() const
{
    return m_xTickSpinBox->value();
}

void MonitorConfigDialog::setEditMode(const QString &fieldName,
                                      const QString &unit,
                                      int xRangeSeconds,
                                      int xTickCount)
{
    m_editMode = true;
    setWindowTitle("编辑监控图表");

    // 查找并选择字段
    for (int i = 0; i < m_fieldComboBox->count(); ++i) {
        if (m_fieldComboBox->itemData(i).toString() == fieldName) {
            m_fieldComboBox->setCurrentIndex(i);
            break;
        }
    }

    // 设置参数
    m_xRangeSpinBox->setValue(xRangeSeconds);
    m_xTickSpinBox->setValue(xTickCount);
}

void MonitorConfigDialog::onFieldChanged(int index)
{
    // 字段切换时的处理（可选）
    Q_UNUSED(index);
}
```

### 3. 更新plot.pro

```pro
# 在HEADERS添加
HEADERS += \
    # ... existing headers ...
    $$PWD/monitorconfigdialog.h

# 在SOURCES添加
SOURCES += \
    # ... existing sources ...
    $$PWD/monitorconfigdialog.cpp
```

## 验收标准

- [ ] 对话框能弹出，显示所有配置项
- [ ] 字段下拉框能显示字段列表（格式："字段名 (单位)"）
- [ ] X轴范围：范围10-300，默认60，后缀" 秒"
- [ ] X轴点数：范围2-10，默认2
- [ ] 点击确定返回QDialog::Accepted
- [ ] 点击取消返回QDialog::Rejected
- [ ] selectedField()返回正确的字段名
- [ ] selectedUnit()返回正确的单位
- [ ] 编辑模式能正确填充现有配置
- [ ] 编译通过，无警告

## 测试方法

### 单元测试

```cpp
void TestMonitorConfigDialog::testFieldSelection()
{
    MonitorConfigDialog dialog;

    // 设置字段列表
    QMap<QString, QString> fields;
    fields["Roll"] = "度";
    fields["Pitch"] = "度";
    fields["Yaw"] = "度";
    dialog.setAvailableFields(fields);

    // 验证下拉框有3个选项
    QCOMPARE(dialog.fieldComboBox()->count(), 3);

    // 验证默认选中第一个
    QCOMPARE(dialog.selectedField(), "Roll");
    QCOMPARE(dialog.selectedUnit(), "度");
}

void TestMonitorConfigDialog::testEditMode()
{
    MonitorConfigDialog dialog;

    QMap<QString, QString> fields;
    fields["Roll"] = "度";
    fields["Pitch"] = "度";
    dialog.setAvailableFields(fields);

    // 设置编辑模式
    dialog.setEditMode("Pitch", "度", 30, 5);

    // 验证配置已填充
    QCOMPARE(dialog.selectedField(), "Pitch");
    QCOMPARE(dialog.xRangeSeconds(), 30);
    QCOMPARE(dialog.xTickCount(), 5);
    QCOMPARE(dialog.windowTitle(), "编辑监控图表");
}
```

### 手动测试

1. 创建测试代码：
```cpp
MonitorConfigDialog dialog;
QMap<QString, QString> fields;
fields["Roll"] = "度";
fields["Pitch"] = "度";
fields["AccX"] = "m/s²";
dialog.setAvailableFields(fields);

if (dialog.exec() == QDialog::Accepted) {
    qDebug() << "选中字段:" << dialog.selectedField();
    qDebug() << "单位:" << dialog.selectedUnit();
    qDebug() << "X轴范围:" << dialog.xRangeSeconds();
    qDebug() << "X轴点数:" << dialog.xTickCount();
}
```

2. 运行测试，验证：
   - 对话框显示3个字段选项
   - 修改参数，点击确定，输出正确
   - 点击取消，对话框关闭

## 注意事项

1. **下拉框数据存储**：
   - 使用QComboBox::addItem(text, data)
   - text显示"字段名 (单位)"
   - data存储字段名，便于获取

2. **单位处理**：
   - 单位可能为空，需要判断
   - 空单位时只显示字段名

3. **编辑模式**：
   - 窗口标题改为"编辑监控图表"
   - 字段选择需要匹配并设置

4. **参数验证**：
   - X轴范围和点数已通过QSpinBox限制
   - 不需要额外验证

## 后续任务

完成此任务后，继续执行：
- TASK-003: 集成到MainWindow（需要此对话框）

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-001: 搭建基础框架](./TASK-001-基础框架.md)
- [Qt QComboBox文档](https://doc.qt.io/qt-5.14/qcombobox.html)
- [Qt QSpinBox文档](https://doc.qt.io/qt-5.14/qspinbox.html)
