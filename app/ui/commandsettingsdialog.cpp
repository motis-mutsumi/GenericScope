#include "commandsettingsdialog.h"
#include "ui_commandsettingsdialog.h"
#include "protocoltypeconverter.h"
#include "protocoltestdialog.h"
#include "newprotocoldialog.h"
#include "protocol/protocolmanager.h"
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
#include <QComboBox>

// 表格列索引常量定义
const int CommandSettingsDialog::kFieldTableIndexColumn;
const int CommandSettingsDialog::kFieldTableElementHeadColumn;
const int CommandSettingsDialog::kFieldTableNameColumn;
const int CommandSettingsDialog::kFieldTableTypeColumn;
const int CommandSettingsDialog::kFieldTableByteLengthColumn;
const int CommandSettingsDialog::kFieldTableScaleColumn;
const int CommandSettingsDialog::kFieldTableOffsetColumn;
const int CommandSettingsDialog::kFieldTableUnitColumn;
const int CommandSettingsDialog::kFieldTableMaxColumn;
const int CommandSettingsDialog::kFieldTableMinColumn;
const int CommandSettingsDialog::kFieldTableDescColumn;
const int CommandSettingsDialog::kFieldTableTipColumn;

CommandSettingsDialog::CommandSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CommandSettingsDialog)
    , m_isModified(false)
{
    ui->setupUi(this);

    // 移除标题栏的帮助按钮（？），改用底部的"帮助"按钮
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setupUI();
    setupConnections();
    loadProtocols();
    applyStyles();
}

CommandSettingsDialog::~CommandSettingsDialog()
{
    delete ui;
}

void CommandSettingsDialog::setupUI()
{
    // UI已经通过.ui文件创建，这里只需要设置表格列宽和按钮文本
    setupTableColumns();

    // 设置Splitter的拉伸比例
    ui->splitter->setStretchFactor(0, 1);  // 左侧帧格式配置
    ui->splitter->setStretchFactor(1, 3);  // 右侧字段配置

    // 设置对话框按钮文本（中文化）
    QPushButton *okBtn = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelBtn = ui->buttonBox->button(QDialogButtonBox::Cancel);
    QPushButton *applyBtn = ui->buttonBox->button(QDialogButtonBox::Apply);
    QPushButton *helpBtn = ui->buttonBox->button(QDialogButtonBox::Help);

    if (okBtn) okBtn->setText("确定");
    if (cancelBtn) cancelBtn->setText("取消");
    if (applyBtn) applyBtn->setText("应用");
    if (helpBtn) helpBtn->setText("帮助");
}

void CommandSettingsDialog::setupTableColumns()
{
    // 设置表格列宽 - 根据内容重要性调整
    ui->fieldTable->setColumnWidth(kFieldTableIndexColumn, 50);       // 序号
    ui->fieldTable->setColumnWidth(kFieldTableElementHeadColumn, 70); // 起始
    ui->fieldTable->setColumnWidth(kFieldTableNameColumn, 120);       // 名称（加宽）
    ui->fieldTable->setColumnWidth(kFieldTableTypeColumn, 90);        // 类型
    ui->fieldTable->setColumnWidth(kFieldTableByteLengthColumn, 60);  // 长度
    ui->fieldTable->setColumnWidth(kFieldTableScaleColumn, 90);       // 缩放因子
    ui->fieldTable->setColumnWidth(kFieldTableOffsetColumn, 80);      // 偏移量
    ui->fieldTable->setColumnWidth(kFieldTableUnitColumn, 70);        // 单位
    ui->fieldTable->setColumnWidth(kFieldTableMaxColumn, 80);         // 最大值
    ui->fieldTable->setColumnWidth(kFieldTableMinColumn, 80);         // 最小值
    ui->fieldTable->setColumnWidth(kFieldTableDescColumn, 150);       // 描述（加宽）
}

void CommandSettingsDialog::setupConnections()
{
    // 协议标签页
    connect(ui->tabWidget, &QTabWidget::currentChanged,
            this, &CommandSettingsDialog::onTabChanged);
    connect(ui->newProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onNewProtocol);
    connect(ui->deleteProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onDeleteProtocol);

    // 帧格式配置
    connect(ui->frameHeaderEdit, &QLineEdit::textChanged,
            this, &CommandSettingsDialog::onFrameHeaderChanged);
    connect(ui->frameFooterEdit, &QLineEdit::textChanged,
            this, &CommandSettingsDialog::onFrameFooterChanged);
    connect(ui->checksumTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandSettingsDialog::onChecksumTypeChanged);
    connect(ui->byteOrderCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandSettingsDialog::onByteOrderChanged);

    // 字段配置
    connect(ui->addFieldBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onAddField);
    connect(ui->deleteFieldBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onDeleteField);
    connect(ui->moveUpBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onMoveFieldUp);
    connect(ui->moveDownBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onMoveFieldDown);
    connect(ui->importFieldBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onImportFields);
    connect(ui->fieldTable, &QTableWidget::cellChanged,
            this, &CommandSettingsDialog::onFieldCellChanged);

    // 导入导出
    connect(ui->importProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onImportProtocol);
    connect(ui->exportProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onExportProtocol);

    // 按钮
    connect(ui->generateProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onGenerateProtocol);
    connect(ui->testProtocolBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onTestProtocol);
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &CommandSettingsDialog::onOk);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &CommandSettingsDialog::onCancel);

    QPushButton *applyBtn = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::onApply);

    QPushButton *helpBtn = ui->buttonBox->button(QDialogButtonBox::Help);
    connect(helpBtn, &QPushButton::clicked,
            this, &CommandSettingsDialog::showHelp);
}

// ========== 协议管理槽函数 ==========

void CommandSettingsDialog::onNewProtocol()
{
    // 使用新建协议向导对话框
    NewProtocolDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString name = dialog.getProtocolName();

        // 检查协议名称是否已存在
        if (m_protocols.contains(name)) {
            QMessageBox::warning(this, "警告",
                QString("协议名称 \"%1\" 已存在！\n请使用其他名称。").arg(name));
            return;
        }

        // 获取新建的协议配置
        ProtocolConfig config = dialog.getProtocolConfig();

        // 保存到协议列表
        m_protocols[name] = config;
        m_currentProtocolName = name;

        // 更新标签页并显示
        updateProtocolTabs();

        // 显示成功提示
        int fieldCount = config.fields.size();
        QString message = QString("协议 \"%1\" 创建成功！").arg(name);
        if (fieldCount > 0) {
            message += QString("\n已自动添加 %1 个数据字段。").arg(fieldCount);
        }
        QMessageBox::information(this, "成功", message);

        m_isModified = true;
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
    if (index < 0 || index >= ui->tabWidget->count()) {
        return;
    }

    QString name = ui->tabWidget->tabText(index);
    if (m_protocols.contains(name)) {
        m_currentProtocolName = name;
        setCurrentConfig(m_protocols[name]);
    }
}

// ========== 帧格式配置槽函数 ==========

void CommandSettingsDialog::onFrameHeaderChanged()
{
    QString header = ui->frameHeaderEdit->text().trimmed();
    if (!validateHexString(header) && !header.isEmpty()) {
        QMessageBox::warning(this, "警告", "帧头格式错误！请输入16进制字符串，如: FF AA");
        return;
    }
    m_isModified = true;
}

void CommandSettingsDialog::onFrameFooterChanged()
{
    QString footer = ui->frameFooterEdit->text().trimmed();
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
    ui->checksumStartSpin->setEnabled(enabled);
    ui->checksumLengthSpin->setEnabled(enabled);
    ui->checksumPositionSpin->setEnabled(enabled);
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
    int row = ui->fieldTable->rowCount();
    ui->fieldTable->insertRow(row);

    // 设置默认值 - 所有12列
    ui->fieldTable->setItem(row, kFieldTableIndexColumn,
                          new QTableWidgetItem(QString::number(row + 1)));
    ui->fieldTable->setItem(row, kFieldTableElementHeadColumn,
                          new QTableWidgetItem("0"));
    ui->fieldTable->setItem(row, kFieldTableNameColumn,
                          new QTableWidgetItem(QString("field_%1").arg(row + 1)));

    // 类型下拉框
    QComboBox *typeCombo = new QComboBox(this);
    typeCombo->addItems({"int8_t", "uint8_t", "int16_t", "uint16_t",
                         "int32_t", "uint32_t", "float", "double",
                         "mbyte_t", "string"});
    typeCombo->setCurrentText("int16_t");
    ui->fieldTable->setCellWidget(row, kFieldTableTypeColumn, typeCombo);

    ui->fieldTable->setItem(row, kFieldTableByteLengthColumn,
                          new QTableWidgetItem("2"));
    ui->fieldTable->setItem(row, kFieldTableScaleColumn,
                          new QTableWidgetItem("1.0"));
    ui->fieldTable->setItem(row, kFieldTableOffsetColumn,
                          new QTableWidgetItem("0.0"));
    ui->fieldTable->setItem(row, kFieldTableUnitColumn,
                          new QTableWidgetItem(""));
    ui->fieldTable->setItem(row, kFieldTableMaxColumn,
                          new QTableWidgetItem("0.0"));
    ui->fieldTable->setItem(row, kFieldTableMinColumn,
                          new QTableWidgetItem("0.0"));
    ui->fieldTable->setItem(row, kFieldTableDescColumn,
                          new QTableWidgetItem(""));
    ui->fieldTable->setItem(row, kFieldTableTipColumn,
                          new QTableWidgetItem(""));

    m_isModified = true;
}

void CommandSettingsDialog::onDeleteField()
{
    int currentRow = ui->fieldTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要删除的字段！");
        return;
    }

    ui->fieldTable->removeRow(currentRow);

    // 更新序号
    for (int i = 0; i < ui->fieldTable->rowCount(); ++i) {
        ui->fieldTable->item(i, kFieldTableIndexColumn)->setText(QString::number(i + 1));
    }

    m_isModified = true;
}

void CommandSettingsDialog::onMoveFieldUp()
{
    int currentRow = ui->fieldTable->currentRow();
    if (currentRow <= 0) {
        return;
    }

    // 交换两行
    for (int col = 0; col < ui->fieldTable->columnCount(); ++col) {
        QTableWidgetItem *item1 = ui->fieldTable->takeItem(currentRow, col);
        QTableWidgetItem *item2 = ui->fieldTable->takeItem(currentRow - 1, col);
        ui->fieldTable->setItem(currentRow, col, item2);
        ui->fieldTable->setItem(currentRow - 1, col, item1);
    }

    // 更新序号
    ui->fieldTable->item(currentRow, kFieldTableIndexColumn)->setText(QString::number(currentRow + 1));
    ui->fieldTable->item(currentRow - 1, kFieldTableIndexColumn)->setText(QString::number(currentRow));

    ui->fieldTable->setCurrentCell(currentRow - 1, 0);
    m_isModified = true;
}

void CommandSettingsDialog::onMoveFieldDown()
{
    int currentRow = ui->fieldTable->currentRow();
    if (currentRow < 0 || currentRow >= ui->fieldTable->rowCount() - 1) {
        return;
    }

    // 交换两行
    for (int col = 0; col < ui->fieldTable->columnCount(); ++col) {
        QTableWidgetItem *item1 = ui->fieldTable->takeItem(currentRow, col);
        QTableWidgetItem *item2 = ui->fieldTable->takeItem(currentRow + 1, col);
        ui->fieldTable->setItem(currentRow, col, item2);
        ui->fieldTable->setItem(currentRow + 1, col, item1);
    }

    // 更新序号
    ui->fieldTable->item(currentRow, kFieldTableIndexColumn)->setText(QString::number(currentRow + 1));
    ui->fieldTable->item(currentRow + 1, kFieldTableIndexColumn)->setText(QString::number(currentRow + 2));

    ui->fieldTable->setCurrentCell(currentRow + 1, 0);
    m_isModified = true;
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

    // 同步到ProtocolManager，这会触发currentProtocolChanged信号
    syncToProtocolManager();

    m_isModified = false;
    QMessageBox::information(this, "成功", "设置已应用！");
}

void CommandSettingsDialog::onOk()
{
    if (!m_currentProtocolName.isEmpty()) {
        m_protocols[m_currentProtocolName] = getCurrentConfig();
    }
    saveProtocols();

    // 同步到ProtocolManager，这会触发currentProtocolChanged信号
    syncToProtocolManager();

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
    ui->tabWidget->clear();

    for (auto it = m_protocols.begin(); it != m_protocols.end(); ++it) {
        QWidget *tabPage = new QWidget(this);
        ui->tabWidget->addTab(tabPage, it.key());
    }

    // 选择当前协议
    if (!m_currentProtocolName.isEmpty()) {
        for (int i = 0; i < ui->tabWidget->count(); ++i) {
            if (ui->tabWidget->tabText(i) == m_currentProtocolName) {
                ui->tabWidget->setCurrentIndex(i);
                break;
            }
        }
    }

    // 如果有协议，显示第一个
    if (ui->tabWidget->count() > 0 && m_currentProtocolName.isEmpty()) {
        m_currentProtocolName = ui->tabWidget->tabText(0);
        setCurrentConfig(m_protocols[m_currentProtocolName]);
    }
}

void CommandSettingsDialog::updateFrameFormatUI()
{
    if (!m_protocols.contains(m_currentProtocolName)) {
        return;
    }

    const ProtocolConfig &config = m_protocols[m_currentProtocolName];

    ui->protocolNameEdit->setText(config.name);
    ui->protocolVersionEdit->setText(config.version);
    ui->protocolDescEdit->setPlainText(config.description);

    ui->frameHeaderEdit->setText(config.frameHeader);
    ui->frameFooterEdit->setText(config.frameFooter);
    ui->lengthPositionSpin->setValue(config.lengthPosition);

    ui->checksumTypeCombo->setCurrentText(checksumTypeToString(config.checksumType));
    ui->checksumStartSpin->setValue(config.checksumStart);
    ui->checksumLengthSpin->setValue(config.checksumLength);
    ui->checksumPositionSpin->setValue(config.checksumPosition);

    ui->byteOrderCombo->setCurrentText(byteOrderToString(config.byteOrder));
    ui->frequencySpin->setValue(config.frequency);
    ui->separatorEdit->setText(config.separator);
}

void CommandSettingsDialog::updateFieldTable()
{
    ui->fieldTable->setRowCount(0);

    if (!m_protocols.contains(m_currentProtocolName)) {
        return;
    }

    const ProtocolConfig &config = m_protocols[m_currentProtocolName];

    for (int i = 0; i < config.fields.size(); ++i) {
        const FieldConfig &field = config.fields[i];

        int row = ui->fieldTable->rowCount();
        ui->fieldTable->insertRow(row);

        // 设置所有12列的值
        ui->fieldTable->setItem(row, kFieldTableIndexColumn,
                              new QTableWidgetItem(QString::number(field.index)));
        ui->fieldTable->setItem(row, kFieldTableElementHeadColumn,
                              new QTableWidgetItem(QString::number(field.elementHead)));
        ui->fieldTable->setItem(row, kFieldTableNameColumn,
                              new QTableWidgetItem(field.name));

        QComboBox *typeCombo = new QComboBox(this);
        typeCombo->addItems({"int8_t", "uint8_t", "int16_t", "uint16_t",
                             "int32_t", "uint32_t", "float", "double",
                             "mbyte_t", "string"});
        typeCombo->setCurrentText(dataTypeToString(field.type));
        ui->fieldTable->setCellWidget(row, kFieldTableTypeColumn, typeCombo);

        ui->fieldTable->setItem(row, kFieldTableByteLengthColumn,
                              new QTableWidgetItem(QString::number(field.byteLength)));
        ui->fieldTable->setItem(row, kFieldTableScaleColumn,
                              new QTableWidgetItem(QString::number(field.scale, 'g', 6)));
        ui->fieldTable->setItem(row, kFieldTableOffsetColumn,
                              new QTableWidgetItem(QString::number(field.offset, 'g', 6)));
        ui->fieldTable->setItem(row, kFieldTableUnitColumn,
                              new QTableWidgetItem(field.unit));
        ui->fieldTable->setItem(row, kFieldTableMaxColumn,
                              new QTableWidgetItem(QString::number(field.maximum, 'g', 6)));
        ui->fieldTable->setItem(row, kFieldTableMinColumn,
                              new QTableWidgetItem(QString::number(field.minimum, 'g', 6)));
        ui->fieldTable->setItem(row, kFieldTableDescColumn,
                              new QTableWidgetItem(field.description));
        ui->fieldTable->setItem(row, kFieldTableTipColumn,
                              new QTableWidgetItem(field.tip));
    }
}

// ========== 配置管理函数 ==========

CommandSettingsDialog::ProtocolConfig CommandSettingsDialog::getCurrentConfig() const
{
    ProtocolConfig config;

    config.name = ui->protocolNameEdit->text().trimmed();
    config.version = ui->protocolVersionEdit->text().trimmed();
    config.description = ui->protocolDescEdit->toPlainText().trimmed();

    config.frameHeader = ui->frameHeaderEdit->text().trimmed();
    config.frameFooter = ui->frameFooterEdit->text().trimmed();
    config.lengthPosition = ui->lengthPositionSpin->value();

    config.checksumType = stringToChecksumType(ui->checksumTypeCombo->currentText());
    config.checksumStart = ui->checksumStartSpin->value();
    config.checksumLength = ui->checksumLengthSpin->value();
    config.checksumPosition = ui->checksumPositionSpin->value();

    config.byteOrder = stringToByteOrder(ui->byteOrderCombo->currentText());
    config.frequency = ui->frequencySpin->value();
    config.separator = ui->separatorEdit->text().trimmed();

    // 读取字段配置 - 从所有12列读取
    config.fields.clear();
    for (int row = 0; row < ui->fieldTable->rowCount(); ++row) {
        FieldConfig field;
        field.index = ui->fieldTable->item(row, kFieldTableIndexColumn)->text().toInt();
        field.elementHead = ui->fieldTable->item(row, kFieldTableElementHeadColumn)->text().toInt();
        field.name = ui->fieldTable->item(row, kFieldTableNameColumn)->text();

        QComboBox *typeCombo = qobject_cast<QComboBox*>(
            ui->fieldTable->cellWidget(row, kFieldTableTypeColumn));
        if (typeCombo) {
            field.type = stringToDataType(typeCombo->currentText());
        }

        field.byteLength = ui->fieldTable->item(row, kFieldTableByteLengthColumn)->text().toInt();
        field.scale = ui->fieldTable->item(row, kFieldTableScaleColumn)->text().toDouble();
        field.offset = ui->fieldTable->item(row, kFieldTableOffsetColumn)->text().toDouble();
        field.unit = ui->fieldTable->item(row, kFieldTableUnitColumn)->text();
        field.maximum = ui->fieldTable->item(row, kFieldTableMaxColumn)->text().toDouble();
        field.minimum = ui->fieldTable->item(row, kFieldTableMinColumn)->text().toDouble();
        field.description = ui->fieldTable->item(row, kFieldTableDescColumn)->text();
        field.tip = ui->fieldTable->item(row, kFieldTableTipColumn)->text();

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
    if (ui->protocolNameEdit->text().trimmed().isEmpty()) {
        if (errorMsg) *errorMsg = "协议名称不能为空！";
        return false;
    }

    // 验证帧头
    QString header = ui->frameHeaderEdit->text().trimmed();
    if (header.isEmpty()) {
        if (errorMsg) *errorMsg = "帧头不能为空！";
        return false;
    }
    if (!validateHexString(header)) {
        if (errorMsg) *errorMsg = "帧头格式错误！请输入16进制字符串。";
        return false;
    }

    // 验证帧尾（如果有）
    QString footer = ui->frameFooterEdit->text().trimmed();
    if (!footer.isEmpty() && !validateHexString(footer)) {
        if (errorMsg) *errorMsg = "帧尾格式错误！请输入16进制字符串。";
        return false;
    }

    // 验证字段
    if (ui->fieldTable->rowCount() == 0) {
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
        case ChecksumType::CRC16_MODBUS: return "CRC16-MODBUS";
        case ChecksumType::CRC16_CCITT: return "CRC16-CCITT";
        case ChecksumType::CRC32: return "CRC32";
        default: return "无校验";
    }
}

CommandSettingsDialog::ChecksumType CommandSettingsDialog::stringToChecksumType(const QString &str) const
{
    if (str == "Sum") return ChecksumType::Sum;
    if (str == "XOR") return ChecksumType::XOR;
    if (str == "CRC8") return ChecksumType::CRC8;
    if (str == "CRC16-MODBUS") return ChecksumType::CRC16_MODBUS;
    if (str == "CRC16-CCITT") return ChecksumType::CRC16_CCITT;
    if (str == "CRC16") return ChecksumType::CRC16_MODBUS;  // 向后兼容：旧配置的"CRC16"映射为MODBUS
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
    // 移除硬编码的样式，让对话框继承全局主题（亮色/暗色）
    // 之前这里强制设置白色背景，导致暗色主题失效

    // 如果需要对话框特定样式，应该使用objectName配合全局QSS
    // 例如：setObjectName("protocolDialog");

    // 不再调用 setStyleSheet()，完全依赖全局主题
}

// ========== 与ProtocolManager交互 ==========

void CommandSettingsDialog::syncToProtocolManager()
{
    // 将UI层的所有协议配置同步到ProtocolManager
    ProtocolManager *manager = ProtocolManager::instance();
    const QStringList existingNames = manager->getProtocolNames();
    const QStringList targetNames = m_protocols.keys();

    int removedCount = 0;
    for (const QString &existingName : existingNames) {
        if (!m_protocols.contains(existingName)) {
            manager->removeProtocol(existingName);
            removedCount++;
        }
    }

    for (auto it = m_protocols.begin(); it != m_protocols.end(); ++it) {
        const QString &name = it.key();
        const ProtocolConfig &uiConfig = it.value();

        // 转换为protocol模块的类型
        ::ProtocolConfig protocolConfig = ProtocolTypeConverter::uiToProtocolConfig(uiConfig);

        // 添加或更新到ProtocolManager
        manager->addProtocol(protocolConfig);
    }

    // 设置当前协议（如果当前协议已被删除，回退到第一个可用协议）
    if (!m_currentProtocolName.isEmpty() && manager->hasProtocol(m_currentProtocolName)) {
        manager->setCurrentProtocol(m_currentProtocolName);
    } else if (!targetNames.isEmpty()) {
        m_currentProtocolName = targetNames.first();
        manager->setCurrentProtocol(m_currentProtocolName);
    } else {
        m_currentProtocolName.clear();
    }

    qDebug() << "已同步" << m_protocols.size() << "个协议到ProtocolManager，移除" << removedCount << "个过期协议";
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

// ========== 帮助功能 ==========

void CommandSettingsDialog::showHelp()
{
    QString helpText =
        "<h2>协议配置帮助</h2>"

        "<h3>一、协议信息</h3>"
        "<ul>"
        "<li><b>名称：</b>协议的唯一标识符，例如 IMU_Protocol_V1</li>"
        "<li><b>版本：</b>协议版本号，例如 1.0.0</li>"
        "<li><b>描述：</b>协议的详细说明</li>"
        "</ul>"

        "<h3>二、帧结构配置</h3>"
        "<ul>"
        "<li><b>帧头(HEX)：</b>帧起始标识，16进制字符串，如 FF AA</li>"
        "<li><b>帧尾(HEX)：</b>帧结束标识（可选），如 0D 0A</li>"
        "<li><b>长度位置：</b>数据长度字段在帧中的位置（-1表示无长度字段）</li>"
        "</ul>"

        "<h3>三、校验配置</h3>"
        "<ul>"
        "<li><b>无校验：</b>不进行数据校验</li>"
        "<li><b>Sum：</b>累加和校验</li>"
        "<li><b>XOR：</b>异或校验</li>"
        "<li><b>CRC8：</b>8位循环冗余校验</li>"
        "<li><b>CRC16：</b>16位CRC-MODBUS校验</li>"
        "<li><b>CRC32：</b>32位IEEE 802.3标准校验</li>"
        "</ul>"

        "<h3>四、字段配置</h3>"
        "<ul>"
        "<li><b>起始：</b>字段在数据区的字节偏移量</li>"
        "<li><b>名称：</b>字段名称（必填）</li>"
        "<li><b>类型：</b>支持10种数据类型<br/>"
        "　　• int8_t/uint8_t：有符号/无符号8位整数<br/>"
        "　　• int16_t/uint16_t：有符号/无符号16位整数<br/>"
        "　　• int32_t/uint32_t：有符号/无符号32位整数<br/>"
        "　　• float：32位浮点数<br/>"
        "　　• double：64位浮点数<br/>"
        "　　• mbyte_t：多字节整数（需要缩放因子）<br/>"
        "　　• string：字符串</li>"
        "<li><b>长度：</b>字段占用的字节数</li>"
        "<li><b>缩放因子：</b>原始值乘以缩放因子得到实际值</li>"
        "<li><b>偏移量：</b>缩放后加上的偏移值</li>"
        "<li><b>单位：</b>数据单位，如 °C、m/s</li>"
        "<li><b>最大值/最小值：</b>数据的有效范围</li>"
        "<li><b>描述：</b>字段的详细说明</li>"
        "<li><b>提示：</b>显示给用户的提示信息</li>"
        "</ul>"

        "<h3>五、快捷功能</h3>"
        "<ul>"
        "<li><b>新建协议：</b>创建新的协议配置</li>"
        "<li><b>删除协议：</b>删除当前协议</li>"
        "<li><b>导入字段：</b>从CSV或JSON文件批量导入字段</li>"
        "<li><b>导入/导出协议：</b>协议配置的备份与恢复</li>"
        "<li><b>生成协议：</b>保存并激活当前协议</li>"
        "<li><b>测试协议：</b>使用示例数据测试协议解析</li>"
        "</ul>"

        "<h3>六、数据转换公式</h3>"
        "<p>实际值 = (原始值 × 缩放因子) + 偏移量</p>"
        "<p>示例：温度传感器输出0-255，对应-40°C至85°C<br/>"
        "　　缩放因子 = 0.49<br/>"
        "　　偏移量 = -40<br/>"
        "　　实际温度 = (原始值 × 0.49) - 40</p>"

        "<hr/>"
        "<p><b>详细文档：</b>docs/markdown_all/protocol-config-system-spec.md</p>";

    QMessageBox helpBox(this);
    helpBox.setWindowTitle("协议配置帮助");
    helpBox.setTextFormat(Qt::RichText);
    helpBox.setText(helpText);
    helpBox.setIcon(QMessageBox::Information);
    helpBox.setStandardButtons(QMessageBox::Ok);

    // 设置对话框大小
    helpBox.setStyleSheet("QLabel{min-width: 600px; min-height: 500px;}");

    helpBox.exec();
}
