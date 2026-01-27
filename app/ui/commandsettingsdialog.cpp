#include "commandsettingsdialog.h"
#include "protocoltypeconverter.h"
#include "protocoltestdialog.h"
#include "protocol/protocolmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QInputDialog>
#include <QSettings>
#include <QDebug>

// 表格列索引常量定义
const int CommandSettingsDialog::kFieldTableIndexColumn;
const int CommandSettingsDialog::kFieldTableElementHeadColumn;
const int CommandSettingsDialog::kFieldTableNameColumn;
const int CommandSettingsDialog::kFieldTableTypeColumn;
const int CommandSettingsDialog::kFieldTableByteLengthColumn;
const int CommandSettingsDialog::kFieldTableScaleColumn;

CommandSettingsDialog::CommandSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_tabWidget(nullptr)
    , m_isModified(false)
{
    setupUI();
    setupConnections();
    loadProtocols();
    applyStyles();
}

CommandSettingsDialog::~CommandSettingsDialog()
{
}

void CommandSettingsDialog::setupUI()
{
    setWindowTitle("协议配置");
    resize(1200, 700);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 设置协议标签页
    setupProtocolTabs();
    mainLayout->addWidget(m_tabWidget);

    // 创建内容区域的分割器
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // 左侧：帧格式配置
    setupFrameFormatGroup();
    splitter->addWidget(m_frameFormatGroup);

    // 右侧：字段配置
    setupFieldConfigGroup();
    splitter->addWidget(m_fieldConfigGroup);

    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 3);

    mainLayout->addWidget(splitter);

    // 底部按钮
    setupButtons();
    mainLayout->addWidget(m_importProtocolBtn->parentWidget());
}

void CommandSettingsDialog::setupProtocolTabs()
{
    // 顶部协议标签页容器
    QWidget *tabContainer = new QWidget(this);
    QHBoxLayout *tabLayout = new QHBoxLayout(tabContainer);
    tabLayout->setContentsMargins(0, 0, 0, 0);
    tabLayout->setSpacing(5);

    // 标签页
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabsClosable(false);
    m_tabWidget->setMovable(true);

    // 新建/删除按钮
    m_newProtocolBtn = new QPushButton("新建协议", this);
    m_deleteProtocolBtn = new QPushButton("删除协议", this);

    tabLayout->addWidget(new QLabel("协议列表:", this));
    tabLayout->addWidget(m_tabWidget, 1);
    tabLayout->addWidget(m_newProtocolBtn);
    tabLayout->addWidget(m_deleteProtocolBtn);
}

void CommandSettingsDialog::setupFrameFormatGroup()
{
    m_frameFormatGroup = new QGroupBox("帧格式配置", this);
    QVBoxLayout *mainLayout = new QVBoxLayout(m_frameFormatGroup);
    mainLayout->setSpacing(8);

    // ========== 协议信息 ==========
    QGroupBox *infoGroup = new QGroupBox("协议信息", m_frameFormatGroup);
    QFormLayout *infoLayout = new QFormLayout(infoGroup);

    m_protocolNameEdit = new QLineEdit(this);
    m_protocolNameEdit->setPlaceholderText("例如: IMU_Protocol_V1");
    infoLayout->addRow("名称:", m_protocolNameEdit);

    m_protocolVersionEdit = new QLineEdit(this);
    m_protocolVersionEdit->setPlaceholderText("例如: 1.0.0");
    infoLayout->addRow("版本:", m_protocolVersionEdit);

    m_protocolDescEdit = new QTextEdit(this);
    m_protocolDescEdit->setPlaceholderText("协议描述...");
    m_protocolDescEdit->setMaximumHeight(60);
    infoLayout->addRow("描述:", m_protocolDescEdit);

    mainLayout->addWidget(infoGroup);

    // ========== 帧结构 ==========
    QGroupBox *frameGroup = new QGroupBox("帧结构", m_frameFormatGroup);
    QFormLayout *frameLayout = new QFormLayout(frameGroup);

    m_frameHeaderEdit = new QLineEdit(this);
    m_frameHeaderEdit->setPlaceholderText("例如: FF AA");
    frameLayout->addRow("帧头(HEX):", m_frameHeaderEdit);

    m_frameFooterEdit = new QLineEdit(this);
    m_frameFooterEdit->setPlaceholderText("可选，例如: 0D 0A");
    frameLayout->addRow("帧尾(HEX):", m_frameFooterEdit);

    m_lengthPositionSpin = new QSpinBox(this);
    m_lengthPositionSpin->setRange(-1, 255);
    m_lengthPositionSpin->setValue(-1);
    m_lengthPositionSpin->setSpecialValueText("无");
    frameLayout->addRow("长度位置:", m_lengthPositionSpin);

    mainLayout->addWidget(frameGroup);

    // ========== 校验配置 ==========
    QGroupBox *checksumGroup = new QGroupBox("校验配置", m_frameFormatGroup);
    QFormLayout *checksumLayout = new QFormLayout(checksumGroup);

    m_checksumTypeCombo = new QComboBox(this);
    m_checksumTypeCombo->addItems({"无校验", "Sum", "XOR", "CRC8", "CRC16", "CRC32"});
    checksumLayout->addRow("校验方式:", m_checksumTypeCombo);

    QHBoxLayout *checksumParamLayout = new QHBoxLayout();
    m_checksumStartSpin = new QSpinBox(this);
    m_checksumStartSpin->setRange(0, 255);
    checksumParamLayout->addWidget(new QLabel("起始:", this));
    checksumParamLayout->addWidget(m_checksumStartSpin);

    m_checksumLengthSpin = new QSpinBox(this);
    m_checksumLengthSpin->setRange(-1, 255);
    m_checksumLengthSpin->setValue(-1);
    m_checksumLengthSpin->setSpecialValueText("到帧尾");
    checksumParamLayout->addWidget(new QLabel("长度:", this));
    checksumParamLayout->addWidget(m_checksumLengthSpin);
    checksumLayout->addRow("", checksumParamLayout);

    m_checksumPositionSpin = new QSpinBox(this);
    m_checksumPositionSpin->setRange(-1, 255);
    m_checksumPositionSpin->setValue(-1);
    m_checksumPositionSpin->setSpecialValueText("帧尾前");
    checksumLayout->addRow("校验位置:", m_checksumPositionSpin);

    mainLayout->addWidget(checksumGroup);

    // ========== 其他配置 ==========
    QGroupBox *otherGroup = new QGroupBox("其他配置", m_frameFormatGroup);
    QFormLayout *otherLayout = new QFormLayout(otherGroup);

    m_byteOrderCombo = new QComboBox(this);
    m_byteOrderCombo->addItems({"LittleEndian", "BigEndian"});
    otherLayout->addRow("字节序:", m_byteOrderCombo);

    m_frequencySpin = new QSpinBox(this);
    m_frequencySpin->setRange(1, 10000);
    m_frequencySpin->setValue(1000);
    m_frequencySpin->setSuffix(" Hz");
    otherLayout->addRow("频率:", m_frequencySpin);

    m_separatorEdit = new QLineEdit(this);
    m_separatorEdit->setPlaceholderText("文本协议分隔符，如逗号");
    otherLayout->addRow("分隔符:", m_separatorEdit);

    mainLayout->addWidget(otherGroup);
    mainLayout->addStretch();
}

void CommandSettingsDialog::setupFieldConfigGroup()
{
    m_fieldConfigGroup = new QGroupBox("字段配置", this);
    QVBoxLayout *mainLayout = new QVBoxLayout(m_fieldConfigGroup);
    mainLayout->setSpacing(8);

    // 字段表格工具栏
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    m_addFieldBtn = new QPushButton("添加字段", this);
    m_deleteFieldBtn = new QPushButton("删除字段", this);
    m_moveUpBtn = new QPushButton("↑", this);
    m_moveDownBtn = new QPushButton("↓", this);
    m_importFieldBtn = new QPushButton("导入字段...", this);

    m_moveUpBtn->setMaximumWidth(40);
    m_moveDownBtn->setMaximumWidth(40);

    toolbarLayout->addWidget(m_addFieldBtn);
    toolbarLayout->addWidget(m_deleteFieldBtn);
    toolbarLayout->addWidget(m_moveUpBtn);
    toolbarLayout->addWidget(m_moveDownBtn);
    toolbarLayout->addWidget(m_importFieldBtn);
    toolbarLayout->addStretch();

    mainLayout->addLayout(toolbarLayout);

    // 字段表格
    m_fieldTable = new QTableWidget(this);
    m_fieldTable->setColumnCount(6);
    m_fieldTable->setHorizontalHeaderLabels({
        "序号", "起始", "名称", "类型", "长度", "缩放因子"
    });

    m_fieldTable->horizontalHeader()->setStretchLastSection(true);
    m_fieldTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_fieldTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_fieldTable->setAlternatingRowColors(true);
    m_fieldTable->verticalHeader()->setVisible(false);

    // 设置列宽
    m_fieldTable->setColumnWidth(kFieldTableIndexColumn, 50);
    m_fieldTable->setColumnWidth(kFieldTableElementHeadColumn, 60);
    m_fieldTable->setColumnWidth(kFieldTableNameColumn, 100);
    m_fieldTable->setColumnWidth(kFieldTableTypeColumn, 100);
    m_fieldTable->setColumnWidth(kFieldTableByteLengthColumn, 60);

    mainLayout->addWidget(m_fieldTable);

    // 字段详情
    m_fieldDetailGroup = new QGroupBox("字段详情", this);
    QFormLayout *detailLayout = new QFormLayout(m_fieldDetailGroup);

    QHBoxLayout *rangeLayout = new QHBoxLayout();
    m_fieldOffsetSpin = new QDoubleSpinBox(this);
    m_fieldOffsetSpin->setRange(-1e6, 1e6);
    m_fieldOffsetSpin->setDecimals(6);
    rangeLayout->addWidget(new QLabel("偏移:", this));
    rangeLayout->addWidget(m_fieldOffsetSpin);

    m_fieldUnitEdit = new QLineEdit(this);
    m_fieldUnitEdit->setPlaceholderText("例如: °/s");
    rangeLayout->addWidget(new QLabel("单位:", this));
    rangeLayout->addWidget(m_fieldUnitEdit);
    detailLayout->addRow("", rangeLayout);

    QHBoxLayout *limitLayout = new QHBoxLayout();
    m_fieldMaxSpin = new QDoubleSpinBox(this);
    m_fieldMaxSpin->setRange(-1e6, 1e6);
    m_fieldMaxSpin->setDecimals(3);
    limitLayout->addWidget(new QLabel("最大:", this));
    limitLayout->addWidget(m_fieldMaxSpin);

    m_fieldMinSpin = new QDoubleSpinBox(this);
    m_fieldMinSpin->setRange(-1e6, 1e6);
    m_fieldMinSpin->setDecimals(3);
    limitLayout->addWidget(new QLabel("最小:", this));
    limitLayout->addWidget(m_fieldMinSpin);
    detailLayout->addRow("", limitLayout);

    m_fieldDescEdit = new QLineEdit(this);
    m_fieldDescEdit->setPlaceholderText("字段描述");
    detailLayout->addRow("描述:", m_fieldDescEdit);

    m_fieldTipEdit = new QLineEdit(this);
    m_fieldTipEdit->setPlaceholderText("提示信息");
    detailLayout->addRow("提示:", m_fieldTipEdit);

    mainLayout->addWidget(m_fieldDetailGroup);
}

void CommandSettingsDialog::setupButtons()
{
    QWidget *buttonWidget = new QWidget(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(0, 0, 0, 0);

    m_importProtocolBtn = new QPushButton("导入协议...", this);
    m_exportProtocolBtn = new QPushButton("导出协议...", this);
    m_generateProtocolBtn = new QPushButton("生成协议", this);
    m_testProtocolBtn = new QPushButton("测试协议", this);

    buttonLayout->addWidget(m_importProtocolBtn);
    buttonLayout->addWidget(m_exportProtocolBtn);
    buttonLayout->addWidget(m_generateProtocolBtn);
    buttonLayout->addWidget(m_testProtocolBtn);
    buttonLayout->addStretch();

    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                     QDialogButtonBox::Cancel |
                                     QDialogButtonBox::Apply);

    QPushButton *okBtn = m_buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelBtn = m_buttonBox->button(QDialogButtonBox::Cancel);
    QPushButton *applyBtn = m_buttonBox->button(QDialogButtonBox::Apply);

    okBtn->setText("确定");
    cancelBtn->setText("取消");
    applyBtn->setText("应用");

    buttonLayout->addWidget(m_buttonBox);
}

void CommandSettingsDialog::setupConnections()
{
    // 协议标签页
    connect(m_tabWidget, &QTabWidget::currentChanged,
            this, &CommandSettingsDialog::onTabChanged);
    connect(m_newProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onNewProtocol);
    connect(m_deleteProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onDeleteProtocol);

    // 帧格式配置
    connect(m_frameHeaderEdit, &QLineEdit::textChanged,
            this, &CommandSettingsDialog::onFrameHeaderChanged);
    connect(m_frameFooterEdit, &QLineEdit::textChanged,
            this, &CommandSettingsDialog::onFrameFooterChanged);
    connect(m_checksumTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandSettingsDialog::onChecksumTypeChanged);
    connect(m_byteOrderCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandSettingsDialog::onByteOrderChanged);

    // 字段配置
    connect(m_addFieldBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onAddField);
    connect(m_deleteFieldBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onDeleteField);
    connect(m_moveUpBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onMoveFieldUp);
    connect(m_moveDownBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onMoveFieldDown);
    connect(m_importFieldBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onImportFields);
    connect(m_fieldTable, &QTableWidget::itemSelectionChanged,
            this, &CommandSettingsDialog::onFieldSelectionChanged);
    connect(m_fieldTable, &QTableWidget::cellChanged,
            this, &CommandSettingsDialog::onFieldCellChanged);

    // 导入导出
    connect(m_importProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onImportProtocol);
    connect(m_exportProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onExportProtocol);

    // 按钮
    connect(m_generateProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onGenerateProtocol);
    connect(m_testProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onTestProtocol);
    connect(m_buttonBox, &QDialogButtonBox::accepted,
            this, &CommandSettingsDialog::onOk);
    connect(m_buttonBox, &QDialogButtonBox::rejected,
            this, &CommandSettingsDialog::onCancel);

    QPushButton *applyBtn = m_buttonBox->button(QDialogButtonBox::Apply);
    connect(applyBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onApply);
}

// ========== 协议管理槽函数 ==========

void CommandSettingsDialog::onNewProtocol()
{
    bool ok;
    QString name = QInputDialog::getText(this, "新建协议",
                                         "请输入协议名称:",
                                         QLineEdit::Normal,
                                         "New_Protocol", &ok);
    if (ok && !name.isEmpty()) {
        if (m_protocols.contains(name)) {
            QMessageBox::warning(this, "警告", "协议名称已存在！");
            return;
        }

        // 创建新协议
        ProtocolConfig config;
        config.name = name;
        config.version = "1.0.0";
        config.description = "新建协议";

        m_protocols[name] = config;
        m_currentProtocolName = name;

        // 添加标签页
        updateProtocolTabs();

        QMessageBox::information(this, "成功", "协议创建成功！");
    }
}

void CommandSettingsDialog::onDeleteProtocol()
{
    if (m_currentProtocolName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要删除的协议！");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除",
        QString("确定要删除协议 \"%1\" 吗？").arg(m_currentProtocolName),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_protocols.remove(m_currentProtocolName);
        m_currentProtocolName.clear();
        updateProtocolTabs();
    }
}

void CommandSettingsDialog::onTabChanged(int index)
{
    if (index < 0 || index >= m_tabWidget->count()) {
        return;
    }

    QString name = m_tabWidget->tabText(index);
    if (m_protocols.contains(name)) {
        m_currentProtocolName = name;
        setCurrentConfig(m_protocols[name]);
    }
}

// ========== 帧格式配置槽函数 ==========

void CommandSettingsDialog::onFrameHeaderChanged()
{
    QString header = m_frameHeaderEdit->text().trimmed();
    if (!validateHexString(header) && !header.isEmpty()) {
        QMessageBox::warning(this, "警告", "帧头格式错误！请输入16进制字符串，如: FF AA");
        return;
    }
    m_isModified = true;
}

void CommandSettingsDialog::onFrameFooterChanged()
{
    QString footer = m_frameFooterEdit->text().trimmed();
    if (!validateHexString(footer) && !footer.isEmpty()) {
        QMessageBox::warning(this, "警告", "帧尾格式错误！请输入16进制字符串，如: 0D 0A");
        return;
    }
    m_isModified = true;
}

void CommandSettingsDialog::onChecksumTypeChanged(int index)
{
    // 根据校验类型启用/禁用相关控件
    bool enabled = (index > 0); // 0是"无校验"
    m_checksumStartSpin->setEnabled(enabled);
    m_checksumLengthSpin->setEnabled(enabled);
    m_checksumPositionSpin->setEnabled(enabled);
    m_isModified = true;
}

void CommandSettingsDialog::onByteOrderChanged(int index)
{
    Q_UNUSED(index);
    m_isModified = true;
}

// ========== 字段配置槽函数 ==========

void CommandSettingsDialog::onAddField()
{
    int row = m_fieldTable->rowCount();
    m_fieldTable->insertRow(row);

    // 设置默认值
    m_fieldTable->setItem(row, kFieldTableIndexColumn,
                          new QTableWidgetItem(QString::number(row + 1)));
    m_fieldTable->setItem(row, kFieldTableElementHeadColumn,
                          new QTableWidgetItem("0"));
    m_fieldTable->setItem(row, kFieldTableNameColumn,
                          new QTableWidgetItem(QString("field_%1").arg(row + 1)));

    // 类型下拉框
    QComboBox *typeCombo = new QComboBox(this);
    typeCombo->addItems({"int8_t", "uint8_t", "int16_t", "uint16_t",
                         "int32_t", "uint32_t", "float", "double",
                         "mbyte_t", "string"});
    typeCombo->setCurrentText("int16_t");
    m_fieldTable->setCellWidget(row, kFieldTableTypeColumn, typeCombo);

    m_fieldTable->setItem(row, kFieldTableByteLengthColumn,
                          new QTableWidgetItem("2"));
    m_fieldTable->setItem(row, kFieldTableScaleColumn,
                          new QTableWidgetItem("1.0"));

    m_isModified = true;
}

void CommandSettingsDialog::onDeleteField()
{
    int currentRow = m_fieldTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要删除的字段！");
        return;
    }

    m_fieldTable->removeRow(currentRow);

    // 更新序号
    for (int i = 0; i < m_fieldTable->rowCount(); ++i) {
        m_fieldTable->item(i, kFieldTableIndexColumn)->setText(QString::number(i + 1));
    }

    m_isModified = true;
}

void CommandSettingsDialog::onMoveFieldUp()
{
    int currentRow = m_fieldTable->currentRow();
    if (currentRow <= 0) {
        return;
    }

    // 交换两行
    for (int col = 0; col < m_fieldTable->columnCount(); ++col) {
        QTableWidgetItem *item1 = m_fieldTable->takeItem(currentRow, col);
        QTableWidgetItem *item2 = m_fieldTable->takeItem(currentRow - 1, col);
        m_fieldTable->setItem(currentRow, col, item2);
        m_fieldTable->setItem(currentRow - 1, col, item1);
    }

    // 更新序号
    m_fieldTable->item(currentRow, kFieldTableIndexColumn)->setText(QString::number(currentRow + 1));
    m_fieldTable->item(currentRow - 1, kFieldTableIndexColumn)->setText(QString::number(currentRow));

    m_fieldTable->setCurrentCell(currentRow - 1, 0);
    m_isModified = true;
}

void CommandSettingsDialog::onMoveFieldDown()
{
    int currentRow = m_fieldTable->currentRow();
    if (currentRow < 0 || currentRow >= m_fieldTable->rowCount() - 1) {
        return;
    }

    // 交换两行
    for (int col = 0; col < m_fieldTable->columnCount(); ++col) {
        QTableWidgetItem *item1 = m_fieldTable->takeItem(currentRow, col);
        QTableWidgetItem *item2 = m_fieldTable->takeItem(currentRow + 1, col);
        m_fieldTable->setItem(currentRow, col, item2);
        m_fieldTable->setItem(currentRow + 1, col, item1);
    }

    // 更新序号
    m_fieldTable->item(currentRow, kFieldTableIndexColumn)->setText(QString::number(currentRow + 1));
    m_fieldTable->item(currentRow + 1, kFieldTableIndexColumn)->setText(QString::number(currentRow + 2));

    m_fieldTable->setCurrentCell(currentRow + 1, 0);
    m_isModified = true;
}

void CommandSettingsDialog::onFieldSelectionChanged()
{
    updateFieldDetails();
}

void CommandSettingsDialog::onFieldCellChanged(int row, int column)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    m_isModified = true;
}

// ========== 导入导出槽函数 ==========

void CommandSettingsDialog::onImportProtocol()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "导入协议文件", "",
        "JSON文件 (*.json);;所有文件 (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件！");
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::critical(this, "错误", "JSON格式错误！");
        return;
    }

    ProtocolConfig config = jsonToConfig(doc.object());
    if (config.name.isEmpty()) {
        QMessageBox::critical(this, "错误", "协议配置无效！");
        return;
    }

    m_protocols[config.name] = config;
    m_currentProtocolName = config.name;
    updateProtocolTabs();

    QMessageBox::information(this, "成功", "协议导入成功！");
}

void CommandSettingsDialog::onExportProtocol()
{
    if (m_currentProtocolName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先选择要导出的协议！");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(
        this, "导出协议文件", m_currentProtocolName + ".json",
        "JSON文件 (*.json);;所有文件 (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    ProtocolConfig config = getCurrentConfig();
    QJsonObject json = configToJson(config);
    QJsonDocument doc(json);

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法保存文件！");
        return;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    QMessageBox::information(this, "成功", "协议导出成功！");
}

void CommandSettingsDialog::onImportFields()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "导入字段配置", "",
        "CSV文件 (*.csv);;JSON文件 (*.json);;所有文件 (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件：" + file.errorString());
        return;
    }

    QVector<FieldConfig> importedFields;
    QString errorMsg;

    // 根据文件扩展名判断格式
    if (fileName.endsWith(".csv", Qt::CaseInsensitive)) {
        // 解析CSV格式
        if (!parseFieldsFromCSV(&file, importedFields, &errorMsg)) {
            QMessageBox::warning(this, "导入失败", errorMsg);
            file.close();
            return;
        }
    } else if (fileName.endsWith(".json", Qt::CaseInsensitive)) {
        // 解析JSON格式
        if (!parseFieldsFromJSON(&file, importedFields, &errorMsg)) {
            QMessageBox::warning(this, "导入失败", errorMsg);
            file.close();
            return;
        }
    } else {
        QMessageBox::warning(this, "错误", "不支持的文件格式！\n请选择CSV或JSON文件。");
        file.close();
        return;
    }

    file.close();

    if (importedFields.isEmpty()) {
        QMessageBox::warning(this, "警告", "未找到有效的字段配置！");
        return;
    }

    // 询问是否清空现有字段
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认导入",
        QString("将导入 %1 个字段。\n\n是否清空现有字段？\n"
                "点击 Yes 清空现有字段后导入\n"
                "点击 No 追加到现有字段后\n"
                "点击 Cancel 取消导入").arg(importedFields.size()),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Cancel) {
        return;
    }

    // 获取当前配置
    ProtocolConfig config = getCurrentConfig();

    if (reply == QMessageBox::Yes) {
        // 清空现有字段
        config.fields.clear();
    }

    // 添加导入的字段
    for (const FieldConfig &field : importedFields) {
        config.fields.append(field);
    }

    // 重新分配索引
    for (int i = 0; i < config.fields.size(); ++i) {
        config.fields[i].index = i + 1;
    }

    // 保存修改后的配置到m_protocols
    m_protocols[m_currentProtocolName] = config;

    // 更新UI
    setCurrentConfig(config);
    updateFieldTable();

    m_isModified = true;

    QMessageBox::information(this, "成功",
        QString("成功导入 %1 个字段！").arg(importedFields.size()));
}

// ========== 按钮操作槽函数 ==========

void CommandSettingsDialog::onGenerateProtocol()
{
    if (m_currentProtocolName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先创建或选择协议！");
        return;
    }

    QString errorMsg;
    if (!validateConfig(&errorMsg)) {
        QMessageBox::warning(this, "配置错误", errorMsg);
        return;
    }

    // 保存当前配置
    m_protocols[m_currentProtocolName] = getCurrentConfig();
    saveProtocols();

    // 同步到ProtocolManager
    syncToProtocolManager();

    QMessageBox::information(this, "成功",
        QString("协议 \"%1\" 已生成并保存！").arg(m_currentProtocolName));
}

void CommandSettingsDialog::onTestProtocol()
{
    if (m_currentProtocolName.isEmpty()) {
        QMessageBox::warning(this, "警告", "请先创建或选择协议！");
        return;
    }

    QString errorMsg;
    if (!validateConfig(&errorMsg)) {
        QMessageBox::warning(this, "配置错误", "协议配置有误，无法测试！\n\n" + errorMsg);
        return;
    }

    // 获取当前配置
    ProtocolConfig config = getCurrentConfig();

    // 打开测试对话框
    ProtocolTestDialog testDialog(config, this);
    testDialog.exec();
}

void CommandSettingsDialog::onApply()
{
    if (!m_currentProtocolName.isEmpty()) {
        m_protocols[m_currentProtocolName] = getCurrentConfig();
    }
    saveProtocols();
    m_isModified = false;
    QMessageBox::information(this, "成功", "设置已应用！");
}

void CommandSettingsDialog::onOk()
{
    if (!m_currentProtocolName.isEmpty()) {
        m_protocols[m_currentProtocolName] = getCurrentConfig();
    }
    saveProtocols();
    accept();
}

void CommandSettingsDialog::onCancel()
{
    if (m_isModified) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this, "确认退出", "配置已修改，是否放弃更改？",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }
    reject();
}

// ========== UI更新函数 ==========

void CommandSettingsDialog::updateProtocolTabs()
{
    m_tabWidget->clear();

    for (auto it = m_protocols.begin(); it != m_protocols.end(); ++it) {
        QWidget *tabPage = new QWidget(this);
        m_tabWidget->addTab(tabPage, it.key());
    }

    // 选择当前协议
    if (!m_currentProtocolName.isEmpty()) {
        for (int i = 0; i < m_tabWidget->count(); ++i) {
            if (m_tabWidget->tabText(i) == m_currentProtocolName) {
                m_tabWidget->setCurrentIndex(i);
                break;
            }
        }
    }

    // 如果有协议，显示第一个
    if (m_tabWidget->count() > 0 && m_currentProtocolName.isEmpty()) {
        m_currentProtocolName = m_tabWidget->tabText(0);
        setCurrentConfig(m_protocols[m_currentProtocolName]);
    }
}

void CommandSettingsDialog::updateFrameFormatUI()
{
    if (!m_protocols.contains(m_currentProtocolName)) {
        return;
    }

    const ProtocolConfig &config = m_protocols[m_currentProtocolName];

    m_protocolNameEdit->setText(config.name);
    m_protocolVersionEdit->setText(config.version);
    m_protocolDescEdit->setPlainText(config.description);

    m_frameHeaderEdit->setText(config.frameHeader);
    m_frameFooterEdit->setText(config.frameFooter);
    m_lengthPositionSpin->setValue(config.lengthPosition);

    m_checksumTypeCombo->setCurrentText(checksumTypeToString(config.checksumType));
    m_checksumStartSpin->setValue(config.checksumStart);
    m_checksumLengthSpin->setValue(config.checksumLength);
    m_checksumPositionSpin->setValue(config.checksumPosition);

    m_byteOrderCombo->setCurrentText(byteOrderToString(config.byteOrder));
    m_frequencySpin->setValue(config.frequency);
    m_separatorEdit->setText(config.separator);
}

void CommandSettingsDialog::updateFieldTable()
{
    m_fieldTable->setRowCount(0);

    if (!m_protocols.contains(m_currentProtocolName)) {
        return;
    }

    const ProtocolConfig &config = m_protocols[m_currentProtocolName];

    for (int i = 0; i < config.fields.size(); ++i) {
        const FieldConfig &field = config.fields[i];

        int row = m_fieldTable->rowCount();
        m_fieldTable->insertRow(row);

        m_fieldTable->setItem(row, kFieldTableIndexColumn,
                              new QTableWidgetItem(QString::number(field.index)));
        m_fieldTable->setItem(row, kFieldTableElementHeadColumn,
                              new QTableWidgetItem(QString::number(field.elementHead)));
        m_fieldTable->setItem(row, kFieldTableNameColumn,
                              new QTableWidgetItem(field.name));

        QComboBox *typeCombo = new QComboBox(this);
        typeCombo->addItems({"int8_t", "uint8_t", "int16_t", "uint16_t",
                             "int32_t", "uint32_t", "float", "double",
                             "mbyte_t", "string"});
        typeCombo->setCurrentText(dataTypeToString(field.type));
        m_fieldTable->setCellWidget(row, kFieldTableTypeColumn, typeCombo);

        m_fieldTable->setItem(row, kFieldTableByteLengthColumn,
                              new QTableWidgetItem(QString::number(field.byteLength)));
        m_fieldTable->setItem(row, kFieldTableScaleColumn,
                              new QTableWidgetItem(QString::number(field.scale, 'g', 6)));
    }
}

void CommandSettingsDialog::updateFieldDetails()
{
    int currentRow = m_fieldTable->currentRow();
    if (currentRow < 0) {
        return;
    }

    if (!m_protocols.contains(m_currentProtocolName)) {
        return;
    }

    const ProtocolConfig &config = m_protocols[m_currentProtocolName];
    if (currentRow >= config.fields.size()) {
        return;
    }

    const FieldConfig &field = config.fields[currentRow];

    m_fieldOffsetSpin->setValue(field.offset);
    m_fieldUnitEdit->setText(field.unit);
    m_fieldMaxSpin->setValue(field.maximum);
    m_fieldMinSpin->setValue(field.minimum);
    m_fieldDescEdit->setText(field.description);
    m_fieldTipEdit->setText(field.tip);
}

// ========== 配置管理函数 ==========

CommandSettingsDialog::ProtocolConfig CommandSettingsDialog::getCurrentConfig() const
{
    ProtocolConfig config;

    config.name = m_protocolNameEdit->text().trimmed();
    config.version = m_protocolVersionEdit->text().trimmed();
    config.description = m_protocolDescEdit->toPlainText().trimmed();

    config.frameHeader = m_frameHeaderEdit->text().trimmed();
    config.frameFooter = m_frameFooterEdit->text().trimmed();
    config.lengthPosition = m_lengthPositionSpin->value();

    config.checksumType = stringToChecksumType(m_checksumTypeCombo->currentText());
    config.checksumStart = m_checksumStartSpin->value();
    config.checksumLength = m_checksumLengthSpin->value();
    config.checksumPosition = m_checksumPositionSpin->value();

    config.byteOrder = stringToByteOrder(m_byteOrderCombo->currentText());
    config.frequency = m_frequencySpin->value();
    config.separator = m_separatorEdit->text().trimmed();

    // 读取字段配置
    config.fields.clear();
    for (int row = 0; row < m_fieldTable->rowCount(); ++row) {
        FieldConfig field;
        field.index = m_fieldTable->item(row, kFieldTableIndexColumn)->text().toInt();
        field.elementHead = m_fieldTable->item(row, kFieldTableElementHeadColumn)->text().toInt();
        field.name = m_fieldTable->item(row, kFieldTableNameColumn)->text();

        QComboBox *typeCombo = qobject_cast<QComboBox*>(
            m_fieldTable->cellWidget(row, kFieldTableTypeColumn));
        if (typeCombo) {
            field.type = stringToDataType(typeCombo->currentText());
        }

        field.byteLength = m_fieldTable->item(row, kFieldTableByteLengthColumn)->text().toInt();
        field.scale = m_fieldTable->item(row, kFieldTableScaleColumn)->text().toDouble();

        // 如果是当前选中行，从详情区读取额外信息
        if (row == m_fieldTable->currentRow()) {
            field.offset = m_fieldOffsetSpin->value();
            field.unit = m_fieldUnitEdit->text();
            field.maximum = m_fieldMaxSpin->value();
            field.minimum = m_fieldMinSpin->value();
            field.description = m_fieldDescEdit->text();
            field.tip = m_fieldTipEdit->text();
        }

        config.fields.append(field);
    }

    return config;
}

void CommandSettingsDialog::setCurrentConfig(const ProtocolConfig &config)
{
    updateFrameFormatUI();
    updateFieldTable();
}

// ========== 验证函数 ==========

bool CommandSettingsDialog::validateConfig(QString *errorMsg)
{
    // 验证协议名称
    if (m_protocolNameEdit->text().trimmed().isEmpty()) {
        if (errorMsg) *errorMsg = "协议名称不能为空！";
        return false;
    }

    // 验证帧头
    QString header = m_frameHeaderEdit->text().trimmed();
    if (header.isEmpty()) {
        if (errorMsg) *errorMsg = "帧头不能为空！";
        return false;
    }
    if (!validateHexString(header)) {
        if (errorMsg) *errorMsg = "帧头格式错误！请输入16进制字符串。";
        return false;
    }

    // 验证帧尾（如果有）
    QString footer = m_frameFooterEdit->text().trimmed();
    if (!footer.isEmpty() && !validateHexString(footer)) {
        if (errorMsg) *errorMsg = "帧尾格式错误！请输入16进制字符串。";
        return false;
    }

    // 验证字段
    if (m_fieldTable->rowCount() == 0) {
        if (errorMsg) *errorMsg = "至少需要配置一个数据字段！";
        return false;
    }

    return true;
}

bool CommandSettingsDialog::validateHexString(const QString &hexStr) const
{
    if (hexStr.isEmpty()) {
        return true;
    }

    QString cleaned = hexStr.simplified().remove(' ');
    QRegExp hexRegex("^[0-9A-Fa-f]+$");
    return hexRegex.exactMatch(cleaned) && (cleaned.length() % 2 == 0);
}

// ========== 数据类型转换函数 ==========

QString CommandSettingsDialog::dataTypeToString(DataType type) const
{
    switch (type) {
        case DataType::Int8: return "int8_t";
        case DataType::UInt8: return "uint8_t";
        case DataType::Int16: return "int16_t";
        case DataType::UInt16: return "uint16_t";
        case DataType::Int32: return "int32_t";
        case DataType::UInt32: return "uint32_t";
        case DataType::Float: return "float";
        case DataType::Double: return "double";
        case DataType::MByte: return "mbyte_t";
        case DataType::String: return "string";
        default: return "int16_t";
    }
}

CommandSettingsDialog::DataType CommandSettingsDialog::stringToDataType(const QString &str) const
{
    QString normalized = str.toLower().trimmed();

    // 支持带_t和不带_t的格式
    if (normalized == "int8_t" || normalized == "int8") return DataType::Int8;
    if (normalized == "uint8_t" || normalized == "uint8") return DataType::UInt8;
    if (normalized == "int16_t" || normalized == "int16") return DataType::Int16;
    if (normalized == "uint16_t" || normalized == "uint16") return DataType::UInt16;
    if (normalized == "int32_t" || normalized == "int32") return DataType::Int32;
    if (normalized == "uint32_t" || normalized == "uint32") return DataType::UInt32;
    if (normalized == "float") return DataType::Float;
    if (normalized == "double") return DataType::Double;
    if (normalized == "mbyte_t" || normalized == "mbyte") return DataType::MByte;
    if (normalized == "string") return DataType::String;
    return DataType::Int16;
}

QString CommandSettingsDialog::byteOrderToString(ByteOrder order) const
{
    return (order == ByteOrder::LittleEndian) ? "LittleEndian" : "BigEndian";
}

CommandSettingsDialog::ByteOrder CommandSettingsDialog::stringToByteOrder(const QString &str) const
{
    return (str == "LittleEndian") ? ByteOrder::LittleEndian : ByteOrder::BigEndian;
}

QString CommandSettingsDialog::checksumTypeToString(ChecksumType type) const
{
    switch (type) {
        case ChecksumType::None: return "无校验";
        case ChecksumType::Sum: return "Sum";
        case ChecksumType::XOR: return "XOR";
        case ChecksumType::CRC8: return "CRC8";
        case ChecksumType::CRC16: return "CRC16";
        case ChecksumType::CRC32: return "CRC32";
        default: return "无校验";
    }
}

CommandSettingsDialog::ChecksumType CommandSettingsDialog::stringToChecksumType(const QString &str) const
{
    if (str == "Sum") return ChecksumType::Sum;
    if (str == "XOR") return ChecksumType::XOR;
    if (str == "CRC8") return ChecksumType::CRC8;
    if (str == "CRC16") return ChecksumType::CRC16;
    if (str == "CRC32") return ChecksumType::CRC32;
    return ChecksumType::None;
}

// ========== JSON序列化函数 ==========

QJsonObject CommandSettingsDialog::fieldToJson(const FieldConfig &field) const
{
    QJsonObject json;
    json["index"] = field.index;
    json["elementHead"] = field.elementHead;
    json["name"] = field.name;
    json["type"] = dataTypeToString(field.type);
    json["byteLength"] = field.byteLength;
    json["scale"] = field.scale;
    json["offset"] = field.offset;
    json["unit"] = field.unit;
    json["maximum"] = field.maximum;
    json["minimum"] = field.minimum;
    json["description"] = field.description;
    json["tip"] = field.tip;
    return json;
}

CommandSettingsDialog::FieldConfig CommandSettingsDialog::jsonToField(const QJsonObject &json) const
{
    FieldConfig field;
    field.index = json["index"].toInt();
    field.elementHead = json["elementHead"].toInt();
    field.name = json["name"].toString();
    field.type = stringToDataType(json["type"].toString());
    field.byteLength = json["byteLength"].toInt();
    field.scale = json["scale"].toDouble(1.0);
    field.offset = json["offset"].toDouble(0.0);
    field.unit = json["unit"].toString();
    field.maximum = json["maximum"].toDouble(0.0);
    field.minimum = json["minimum"].toDouble(0.0);
    field.description = json["description"].toString();
    field.tip = json["tip"].toString();
    return field;
}

QJsonObject CommandSettingsDialog::configToJson(const ProtocolConfig &config) const
{
    QJsonObject json;

    // 协议基本信息
    json["name"] = config.name;
    json["version"] = config.version;
    json["description"] = config.description;

    // 帧格式配置
    QJsonObject frameFormat;
    frameFormat["header"] = config.frameHeader;
    frameFormat["footer"] = config.frameFooter;
    frameFormat["lengthPosition"] = config.lengthPosition;
    frameFormat["checksumType"] = checksumTypeToString(config.checksumType);
    frameFormat["checksumStart"] = config.checksumStart;
    frameFormat["checksumLength"] = config.checksumLength;
    frameFormat["checksumPosition"] = config.checksumPosition;
    frameFormat["byteOrder"] = byteOrderToString(config.byteOrder);
    frameFormat["frequency"] = config.frequency;
    frameFormat["separator"] = config.separator;
    json["frameFormat"] = frameFormat;

    // 数据字段配置
    QJsonArray fields;
    for (const FieldConfig &field : config.fields) {
        fields.append(fieldToJson(field));
    }
    json["fields"] = fields;

    return json;
}

CommandSettingsDialog::ProtocolConfig CommandSettingsDialog::jsonToConfig(const QJsonObject &json) const
{
    ProtocolConfig config;

    // 协议基本信息
    config.name = json["name"].toString();
    config.version = json["version"].toString();
    config.description = json["description"].toString();

    // 帧格式配置
    QJsonObject frameFormat = json["frameFormat"].toObject();
    config.frameHeader = frameFormat["header"].toString();
    config.frameFooter = frameFormat["footer"].toString();
    config.lengthPosition = frameFormat["lengthPosition"].toInt(-1);
    config.checksumType = stringToChecksumType(frameFormat["checksumType"].toString());
    config.checksumStart = frameFormat["checksumStart"].toInt(0);
    config.checksumLength = frameFormat["checksumLength"].toInt(-1);
    config.checksumPosition = frameFormat["checksumPosition"].toInt(-1);
    config.byteOrder = stringToByteOrder(frameFormat["byteOrder"].toString());
    config.frequency = frameFormat["frequency"].toInt(1000);
    config.separator = frameFormat["separator"].toString();

    // 数据字段配置
    QJsonArray fields = json["fields"].toArray();
    for (const QJsonValue &value : fields) {
        config.fields.append(jsonToField(value.toObject()));
    }

    return config;
}

// ========== 加载保存函数 ==========

void CommandSettingsDialog::loadProtocols()
{
    // 从配置文件加载协议列表
    QSettings settings("GenericScope", "ProtocolConfig");
    int count = settings.beginReadArray("protocols");

    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);
        QString name = settings.value("name").toString();
        QString jsonStr = settings.value("config").toString();

        QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (!doc.isNull() && doc.isObject()) {
            ProtocolConfig config = jsonToConfig(doc.object());
            m_protocols[name] = config;
        }
    }

    settings.endArray();

    // 如果没有协议，创建一个默认协议
    if (m_protocols.isEmpty()) {
        ProtocolConfig defaultConfig;
        defaultConfig.name = "Default_Protocol";
        defaultConfig.version = "1.0.0";
        defaultConfig.description = "默认协议";
        defaultConfig.frameHeader = "FF AA";
        defaultConfig.checksumType = ChecksumType::None;
        defaultConfig.byteOrder = ByteOrder::LittleEndian;
        defaultConfig.frequency = 1000;

        m_protocols[defaultConfig.name] = defaultConfig;
    }

    updateProtocolTabs();

    // 同步到ProtocolManager
    syncToProtocolManager();
}

void CommandSettingsDialog::saveProtocols()
{
    QSettings settings("GenericScope", "ProtocolConfig");
    settings.beginWriteArray("protocols");

    int index = 0;
    for (auto it = m_protocols.begin(); it != m_protocols.end(); ++it) {
        settings.setArrayIndex(index++);
        settings.setValue("name", it.key());

        QJsonObject json = configToJson(it.value());
        QJsonDocument doc(json);
        settings.setValue("config", QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
    }

    settings.endArray();
    qDebug() << "保存" << m_protocols.size() << "个协议配置";
}

// ========== 公共接口函数 ==========

QStringList CommandSettingsDialog::getProtocolNames() const
{
    return m_protocols.keys();
}

CommandSettingsDialog::ProtocolConfig CommandSettingsDialog::getProtocol(const QString &name) const
{
    return m_protocols.value(name);
}

void CommandSettingsDialog::setProtocols(const QMap<QString, ProtocolConfig> &protocols)
{
    m_protocols = protocols;
    updateProtocolTabs();
}

QMap<QString, CommandSettingsDialog::ProtocolConfig> CommandSettingsDialog::getProtocols() const
{
    return m_protocols;
}

// ========== 样式函数 ==========

void CommandSettingsDialog::applyStyles()
{
    QString styleSheet = R"(
        QGroupBox {
            border: 2px solid #ddd;
            border-radius: 6px;
            margin-top: 10px;
            font-weight: bold;
            padding-top: 10px;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 5px;
            background-color: white;
        }

        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
            min-width: 80px;
        }

        QPushButton:hover {
            background-color: #45a049;
        }

        QPushButton:pressed {
            background-color: #3d8b40;
        }

        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }

        QLineEdit, QTextEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }

        QLineEdit:focus, QTextEdit:focus {
            border: 1px solid #4CAF50;
        }

        QTableWidget {
            gridline-color: #ddd;
            background-color: white;
            alternate-background-color: #f9f9f9;
        }

        QTableWidget::item:selected {
            background-color: #4CAF50;
            color: white;
        }

        QHeaderView::section {
            background-color: #f0f0f0;
            padding: 6px;
            border: 1px solid #ddd;
            font-weight: bold;
        }

        QLabel {
            font-weight: bold;
            color: #333;
        }
    )";

    setStyleSheet(styleSheet);
}

// ========== 与ProtocolManager交互 ==========

void CommandSettingsDialog::syncToProtocolManager()
{
    // 将UI层的所有协议配置同步到ProtocolManager
    ProtocolManager *manager = ProtocolManager::instance();

    for (auto it = m_protocols.begin(); it != m_protocols.end(); ++it) {
        const QString &name = it.key();
        const ProtocolConfig &uiConfig = it.value();

        // 转换为protocol模块的类型
        ::ProtocolConfig protocolConfig = ProtocolTypeConverter::uiToProtocolConfig(uiConfig);

        // 添加或更新到ProtocolManager
        manager->addProtocol(protocolConfig);
    }

    // 设置当前协议
    if (!m_currentProtocolName.isEmpty()) {
        manager->setCurrentProtocol(m_currentProtocolName);
    }

    qDebug() << "已同步" << m_protocols.size() << "个协议到ProtocolManager";
}

void CommandSettingsDialog::syncFromProtocolManager()
{
    // 从ProtocolManager读取协议配置到UI层
    ProtocolManager *manager = ProtocolManager::instance();

    QStringList protocolNames = manager->getProtocolNames();
    m_protocols.clear();

    for (const QString &name : protocolNames) {
        ::ProtocolConfig protocolConfig = manager->getProtocol(name);

        // 转换为UI层的类型
        ProtocolConfig uiConfig = ProtocolTypeConverter::protocolToUiConfig(protocolConfig);

        m_protocols[name] = uiConfig;
    }

    // 设置当前协议
    QString currentName = manager->getCurrentProtocol();
    if (!currentName.isEmpty() && m_protocols.contains(currentName)) {
        m_currentProtocolName = currentName;
    }

    updateProtocolTabs();

    qDebug() << "已从ProtocolManager同步" << m_protocols.size() << "个协议";
}

// ========== 字段导入辅助函数 ==========

bool CommandSettingsDialog::parseFieldsFromCSV(QFile *file, QVector<FieldConfig> &fields, QString *errorMsg)
{
    if (!file || !file->isOpen()) {
        if (errorMsg) *errorMsg = "文件未打开！";
        return false;
    }

    QTextStream in(file);
    in.setCodec("UTF-8");

    // 读取表头
    if (in.atEnd()) {
        if (errorMsg) *errorMsg = "CSV文件为空！";
        return false;
    }

    QString headerLine = in.readLine().trimmed();
    QStringList headers = headerLine.split(',');

    // 验证表头（至少需要name, type, elementHead, byteLength）
    QMap<QString, int> headerMap;
    for (int i = 0; i < headers.size(); ++i) {
        headerMap[headers[i].trimmed().toLower()] = i;
    }

    if (!headerMap.contains("name") || !headerMap.contains("type") ||
        !headerMap.contains("elementhead") || !headerMap.contains("bytelength")) {
        if (errorMsg) *errorMsg = "CSV文件缺少必需的列！\n必需列：name, type, elementHead, byteLength";
        return false;
    }

    // 读取数据行
    int lineNumber = 1;
    fields.clear();

    while (!in.atEnd()) {
        lineNumber++;
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        QStringList values = line.split(',');
        if (values.size() != headers.size()) {
            if (errorMsg) *errorMsg = QString("第%1行：列数不匹配！").arg(lineNumber);
            return false;
        }

        FieldConfig field;
        field.index = fields.size() + 1;

        // 解析必需字段
        field.name = values[headerMap["name"]].trimmed();
        if (field.name.isEmpty()) {
            if (errorMsg) *errorMsg = QString("第%1行：字段名称不能为空！").arg(lineNumber);
            return false;
        }

        QString typeStr = values[headerMap["type"]].trimmed().toLower();
        field.type = stringToDataType(typeStr);

        bool ok;
        field.elementHead = values[headerMap["elementhead"]].trimmed().toInt(&ok);
        if (!ok) {
            if (errorMsg) *errorMsg = QString("第%1行：elementHead格式错误！").arg(lineNumber);
            return false;
        }

        field.byteLength = values[headerMap["bytelength"]].trimmed().toInt(&ok);
        if (!ok || field.byteLength <= 0) {
            if (errorMsg) *errorMsg = QString("第%1行：byteLength格式错误或无效！").arg(lineNumber);
            return false;
        }

        // 解析可选字段
        if (headerMap.contains("scale")) {
            field.scale = values[headerMap["scale"]].trimmed().toDouble(&ok);
            if (!ok) field.scale = 1.0;
        } else {
            field.scale = 1.0;
        }

        if (headerMap.contains("offset")) {
            field.offset = values[headerMap["offset"]].trimmed().toDouble(&ok);
            if (!ok) field.offset = 0.0;
        } else {
            field.offset = 0.0;
        }

        if (headerMap.contains("unit")) {
            field.unit = values[headerMap["unit"]].trimmed();
        }

        if (headerMap.contains("maximum")) {
            field.maximum = values[headerMap["maximum"]].trimmed().toDouble(&ok);
            if (!ok) field.maximum = 0.0;
        } else {
            field.maximum = 0.0;
        }

        if (headerMap.contains("minimum")) {
            field.minimum = values[headerMap["minimum"]].trimmed().toDouble(&ok);
            if (!ok) field.minimum = 0.0;
        } else {
            field.minimum = 0.0;
        }

        if (headerMap.contains("description")) {
            field.description = values[headerMap["description"]].trimmed();
        }

        if (headerMap.contains("tip")) {
            field.tip = values[headerMap["tip"]].trimmed();
        }

        fields.append(field);
    }

    if (fields.isEmpty()) {
        if (errorMsg) *errorMsg = "CSV文件中没有有效的数据行！";
        return false;
    }

    return true;
}

bool CommandSettingsDialog::parseFieldsFromJSON(QFile *file, QVector<FieldConfig> &fields, QString *errorMsg)
{
    if (!file || !file->isOpen()) {
        if (errorMsg) *errorMsg = "文件未打开！";
        return false;
    }

    QByteArray jsonData = file->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (doc.isNull()) {
        if (errorMsg) *errorMsg = "JSON文件格式错误！";
        return false;
    }

    if (!doc.isObject()) {
        if (errorMsg) *errorMsg = "JSON文件根节点必须是对象！";
        return false;
    }

    QJsonObject root = doc.object();
    if (!root.contains("fields")) {
        if (errorMsg) *errorMsg = "JSON文件缺少'fields'数组！";
        return false;
    }

    QJsonValue fieldsValue = root["fields"];
    if (!fieldsValue.isArray()) {
        if (errorMsg) *errorMsg = "'fields'必须是数组！";
        return false;
    }

    QJsonArray fieldsArray = fieldsValue.toArray();
    fields.clear();

    for (int i = 0; i < fieldsArray.size(); ++i) {
        QJsonValue fieldValue = fieldsArray[i];
        if (!fieldValue.isObject()) {
            if (errorMsg) *errorMsg = QString("第%1个字段不是对象！").arg(i + 1);
            return false;
        }

        QJsonObject fieldObj = fieldValue.toObject();

        // 验证必需字段
        if (!fieldObj.contains("name") || !fieldObj.contains("type") ||
            !fieldObj.contains("elementHead") || !fieldObj.contains("byteLength")) {
            if (errorMsg) *errorMsg = QString("第%1个字段缺少必需属性！\n必需属性：name, type, elementHead, byteLength").arg(i + 1);
            return false;
        }

        FieldConfig field;

        // 解析字段
        field.index = fieldObj.value("index").toInt(i + 1);
        field.elementHead = fieldObj["elementHead"].toInt();
        field.name = fieldObj["name"].toString().trimmed();

        if (field.name.isEmpty()) {
            if (errorMsg) *errorMsg = QString("第%1个字段：name不能为空！").arg(i + 1);
            return false;
        }

        QString typeStr = fieldObj["type"].toString().trimmed().toLower();
        field.type = stringToDataType(typeStr);

        field.byteLength = fieldObj["byteLength"].toInt();
        if (field.byteLength <= 0) {
            if (errorMsg) *errorMsg = QString("第%1个字段：byteLength无效！").arg(i + 1);
            return false;
        }

        // 解析可选字段
        field.scale = fieldObj.value("scale").toDouble(1.0);
        field.offset = fieldObj.value("offset").toDouble(0.0);
        field.unit = fieldObj.value("unit").toString();
        field.maximum = fieldObj.value("maximum").toDouble(0.0);
        field.minimum = fieldObj.value("minimum").toDouble(0.0);
        field.description = fieldObj.value("description").toString();
        field.tip = fieldObj.value("tip").toString();

        fields.append(field);
    }

    if (fields.isEmpty()) {
        if (errorMsg) *errorMsg = "JSON文件中没有有效的字段！";
        return false;
    }

    return true;
}
