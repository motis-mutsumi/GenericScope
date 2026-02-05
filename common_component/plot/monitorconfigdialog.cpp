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
