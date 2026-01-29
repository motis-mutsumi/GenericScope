#include "newprotocoldialog.h"
#include "aiprotocolinputdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

NewProtocolDialog::NewProtocolDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setupConnections();
}

NewProtocolDialog::~NewProtocolDialog()
{
}

void NewProtocolDialog::setupUI()
{
    setWindowTitle("新建协议向导");
    resize(600, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // ========== 基本信息组 ==========
    QGroupBox *infoGroup = new QGroupBox("协议基本信息", this);
    QFormLayout *infoLayout = new QFormLayout(infoGroup);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("例如: IMU_Protocol_V1");
    infoLayout->addRow("协议名称*:", m_nameEdit);

    m_versionEdit = new QLineEdit(this);
    m_versionEdit->setText("1.0.0");
    m_versionEdit->setPlaceholderText("例如: 1.0.0");
    infoLayout->addRow("协议版本:", m_versionEdit);

    m_descriptionEdit = new QTextEdit(this);
    m_descriptionEdit->setPlaceholderText("输入协议描述...");
    m_descriptionEdit->setMaximumHeight(80);
    infoLayout->addRow("协议描述:", m_descriptionEdit);

    mainLayout->addWidget(infoGroup);

    // ========== 协议模板组 ==========
    QGroupBox *templateGroup = new QGroupBox("协议模板", this);
    QVBoxLayout *templateLayout = new QVBoxLayout(templateGroup);

    QHBoxLayout *templateSelectLayout = new QHBoxLayout();
    QLabel *templateLabel = new QLabel("选择模板:", this);
    m_templateCombo = new QComboBox(this);
    m_templateCombo->addItem("空协议");
    m_templateCombo->addItem("基础串口协议");
    m_templateCombo->addItem("MODBUS RTU协议");
    m_templateCombo->addItem("自定义IMU协议");
    m_templateCombo->addItem("文本CSV协议");
    m_templateCombo->addItem("🤖 AI智能生成");  // 新增AI生成选项
    templateSelectLayout->addWidget(templateLabel);
    templateSelectLayout->addWidget(m_templateCombo, 1);
    templateLayout->addLayout(templateSelectLayout);

    // 模板描述
    m_templateDescEdit = new QTextEdit(this);
    m_templateDescEdit->setReadOnly(true);
    m_templateDescEdit->setMaximumHeight(120);
    templateLayout->addWidget(m_templateDescEdit);

    // 添加默认字段选项
    m_addDefaultFieldsCheck = new QCheckBox("添加默认数据字段（适用于选定模板）", this);
    m_addDefaultFieldsCheck->setChecked(true);
    templateLayout->addWidget(m_addDefaultFieldsCheck);

    mainLayout->addWidget(templateGroup);

    // ========== 提示信息 ==========
    QLabel *noteLabel = new QLabel("提示: 创建后可以在协议配置界面中修改所有设置", this);
    noteLabel->setStyleSheet("color: #666; font-style: italic;");
    mainLayout->addWidget(noteLabel);

    mainLayout->addStretch();

    // ========== 底部按钮 ==========
    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    QPushButton *okBtn = m_buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelBtn = m_buttonBox->button(QDialogButtonBox::Cancel);
    okBtn->setText("创建");
    cancelBtn->setText("取消");

    mainLayout->addWidget(m_buttonBox);

    // 初始化模板描述
    updateTemplateDescription();
}

void NewProtocolDialog::setupConnections()
{
    connect(m_buttonBox, &QDialogButtonBox::accepted,
            this, &NewProtocolDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
    connect(m_templateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NewProtocolDialog::onTemplateChanged);
    connect(m_nameEdit, &QLineEdit::textChanged,
            this, &NewProtocolDialog::onNameChanged);
}

void NewProtocolDialog::onTemplateChanged(int index)
{
    Q_UNUSED(index);
    updateTemplateDescription();
}

void NewProtocolDialog::onNameChanged(const QString &text)
{
    // 实时验证名称
    QPushButton *okBtn = m_buttonBox->button(QDialogButtonBox::Ok);
    okBtn->setEnabled(!text.trimmed().isEmpty());
}

void NewProtocolDialog::onAccepted()
{
    if (!validateInput()) {
        return;
    }

    // 根据选择的模板创建协议配置
    ProtocolTemplate templateType = static_cast<ProtocolTemplate>(
        m_templateCombo->currentIndex());

    switch (templateType) {
    case ProtocolTemplate::Empty:
        m_config = createEmptyProtocol();
        break;
    case ProtocolTemplate::SerialBasic:
        m_config = createSerialBasicProtocol();
        break;
    case ProtocolTemplate::ModbusRTU:
        m_config = createModbusRTUProtocol();
        break;
    case ProtocolTemplate::CustomIMU:
        m_config = createCustomIMUProtocol();
        break;
    case ProtocolTemplate::TextCSV:
        m_config = createTextCSVProtocol();
        break;
    case ProtocolTemplate::AIGenerated:
        m_config = createAIGeneratedProtocol();
        if (!m_config.name.isEmpty()) {
            // AI生成成功
            m_protocolName = m_config.name;
            accept();
        }
        return;  // AI生成失败或用户取消，不关闭对话框
    }

    // 设置用户输入的基本信息
    m_config.name = m_nameEdit->text().trimmed();
    m_config.version = m_versionEdit->text().trimmed();
    m_config.description = m_descriptionEdit->toPlainText().trimmed();

    m_protocolName = m_config.name;

    accept();
}

bool NewProtocolDialog::validateInput()
{
    QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "协议名称不能为空！");
        m_nameEdit->setFocus();
        return false;
    }

    // 检查名称是否包含非法字符
    QRegExp nameRegex("^[a-zA-Z0-9_\\-]+$");
    if (!nameRegex.exactMatch(name)) {
        QMessageBox::warning(this, "输入错误",
            "协议名称只能包含字母、数字、下划线和连字符！");
        m_nameEdit->setFocus();
        return false;
    }

    return true;
}

void NewProtocolDialog::updateTemplateDescription()
{
    ProtocolTemplate templateType = static_cast<ProtocolTemplate>(
        m_templateCombo->currentIndex());

    QString description;

    switch (templateType) {
    case ProtocolTemplate::Empty:
        description = "创建一个空白协议配置。\n\n"
                     "适用于: 完全自定义的协议\n"
                     "默认配置:\n"
                     "- 无帧头帧尾\n"
                     "- 无校验\n"
                     "- 无默认字段";
        m_addDefaultFieldsCheck->setEnabled(false);
        break;

    case ProtocolTemplate::SerialBasic:
        description = "创建基础串口协议配置。\n\n"
                     "适用于: 通用串口通信\n"
                     "默认配置:\n"
                     "- 帧头: FF AA\n"
                     "- 帧尾: 0D 0A\n"
                     "- 校验: CRC16-MODBUS\n"
                     "- 字节序: 小端序\n"
                     "默认字段: Data1~Data4 (int16)";
        m_addDefaultFieldsCheck->setEnabled(true);
        break;

    case ProtocolTemplate::ModbusRTU:
        description = "创建MODBUS RTU协议配置。\n\n"
                     "适用于: MODBUS设备通信\n"
                     "默认配置:\n"
                     "- 无帧头帧尾\n"
                     "- 校验: CRC16-MODBUS (小端序)\n"
                     "- 数据字节序: 大端序\n"
                     "默认字段: DeviceAddr, FunctionCode, DataAddr, DataValue";
        m_addDefaultFieldsCheck->setEnabled(true);
        break;

    case ProtocolTemplate::CustomIMU:
        description = "创建自定义IMU协议配置。\n\n"
                     "适用于: IMU传感器数据采集\n"
                     "默认配置:\n"
                     "- 帧头: FF AA\n"
                     "- 帧尾: 0D 0A\n"
                     "- 校验: CRC16-CCITT\n"
                     "- 字节序: 小端序\n"
                     "默认字段: Roll, Pitch, Yaw, AccX~Z, GyroX~Z, MagX~Z, Temp";
        m_addDefaultFieldsCheck->setEnabled(true);
        break;

    case ProtocolTemplate::TextCSV:
        description = "创建文本CSV协议配置。\n\n"
                     "适用于: 文本格式数据解析\n"
                     "默认配置:\n"
                     "- 无帧头帧尾\n"
                     "- 无校验\n"
                     "- 分隔符: 逗号\n"
                     "默认字段: Value1~Value4 (string)";
        m_addDefaultFieldsCheck->setEnabled(true);
        break;

    case ProtocolTemplate::AIGenerated:
        description = "🤖 AI智能生成协议配置\n\n"
                     "通过Claude AI分析原始数据和解析规则，\n"
                     "自动生成完整的协议配置。\n\n"
                     "您需要提供：\n"
                     "1. 一条完整的16进制数据样本\n"
                     "2. 详细的解析规则描述\n\n"
                     "AI将自动分析：\n"
                     "- 帧头、帧尾、校验方式\n"
                     "- 数据字段类型和位置\n"
                     "- 缩放因子和字节序\n\n"
                     "⚠️ 需要配置Claude API密钥";
        m_addDefaultFieldsCheck->setEnabled(false);
        break;
    }

    m_templateDescEdit->setText(description);
}

// ========== 模板创建函数 ==========

CommandSettingsDialog::ProtocolConfig NewProtocolDialog::createEmptyProtocol() const
{
    CommandSettingsDialog::ProtocolConfig config;
    config.checksumType = CommandSettingsDialog::ChecksumType::None;
    config.byteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.checksumByteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.frequency = 1000;
    // 无默认字段
    return config;
}

CommandSettingsDialog::ProtocolConfig NewProtocolDialog::createSerialBasicProtocol() const
{
    CommandSettingsDialog::ProtocolConfig config;
    config.frameHeader = "FF AA";
    config.frameFooter = "0D 0A";
    config.checksumType = CommandSettingsDialog::ChecksumType::CRC16_MODBUS;
    config.checksumScope = CommandSettingsDialog::ChecksumScope::AfterHeader;
    config.byteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.checksumByteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.frequency = 1000;

    if (m_addDefaultFieldsCheck->isChecked()) {
        // 添加4个默认数据字段
        for (int i = 0; i < 4; ++i) {
            CommandSettingsDialog::FieldConfig field;
            field.index = i + 1;
            field.elementHead = i * 2;
            field.name = QString("Data%1").arg(i + 1);
            field.type = CommandSettingsDialog::DataType::Int16;
            field.byteLength = 2;
            field.scale = 1.0;
            field.offset = 0.0;
            field.unit = "";
            field.description = QString("数据字段%1").arg(i + 1);
            config.fields.append(field);
        }
    }

    return config;
}

CommandSettingsDialog::ProtocolConfig NewProtocolDialog::createModbusRTUProtocol() const
{
    CommandSettingsDialog::ProtocolConfig config;
    config.checksumType = CommandSettingsDialog::ChecksumType::CRC16_MODBUS;
    config.checksumScope = CommandSettingsDialog::ChecksumScope::FullFrame;
    config.byteOrder = CommandSettingsDialog::ByteOrder::BigEndian;
    config.checksumByteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.frequency = 100;

    if (m_addDefaultFieldsCheck->isChecked()) {
        // 设备地址
        CommandSettingsDialog::FieldConfig devAddr;
        devAddr.index = 1;
        devAddr.elementHead = 0;
        devAddr.name = "DeviceAddr";
        devAddr.type = CommandSettingsDialog::DataType::UInt8;
        devAddr.byteLength = 1;
        devAddr.scale = 1.0;
        devAddr.description = "设备地址";
        config.fields.append(devAddr);

        // 功能码
        CommandSettingsDialog::FieldConfig funcCode;
        funcCode.index = 2;
        funcCode.elementHead = 1;
        funcCode.name = "FunctionCode";
        funcCode.type = CommandSettingsDialog::DataType::UInt8;
        funcCode.byteLength = 1;
        funcCode.scale = 1.0;
        funcCode.description = "功能码";
        config.fields.append(funcCode);

        // 数据地址
        CommandSettingsDialog::FieldConfig dataAddr;
        dataAddr.index = 3;
        dataAddr.elementHead = 2;
        dataAddr.name = "DataAddr";
        dataAddr.type = CommandSettingsDialog::DataType::UInt16;
        dataAddr.byteLength = 2;
        dataAddr.scale = 1.0;
        dataAddr.description = "数据地址";
        config.fields.append(dataAddr);

        // 数据值
        CommandSettingsDialog::FieldConfig dataValue;
        dataValue.index = 4;
        dataValue.elementHead = 4;
        dataValue.name = "DataValue";
        dataValue.type = CommandSettingsDialog::DataType::UInt16;
        dataValue.byteLength = 2;
        dataValue.scale = 1.0;
        dataValue.description = "数据值";
        config.fields.append(dataValue);
    }

    return config;
}

CommandSettingsDialog::ProtocolConfig NewProtocolDialog::createCustomIMUProtocol() const
{
    CommandSettingsDialog::ProtocolConfig config;
    config.frameHeader = "FF AA";
    config.frameFooter = "0D 0A";
    config.checksumType = CommandSettingsDialog::ChecksumType::CRC16_CCITT;
    config.checksumScope = CommandSettingsDialog::ChecksumScope::AfterHeader;
    config.byteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.checksumByteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.frequency = 100;

    if (m_addDefaultFieldsCheck->isChecked()) {
        int offset = 0;

        // 姿态角 (Roll, Pitch, Yaw)
        QStringList attitudes = {"Roll", "Pitch", "Yaw"};
        QStringList attitudeDesc = {"横滚角", "俯仰角", "偏航角"};
        for (int i = 0; i < 3; ++i) {
            CommandSettingsDialog::FieldConfig field;
            field.index = i + 1;
            field.elementHead = offset;
            field.name = attitudes[i];
            field.type = CommandSettingsDialog::DataType::Float;
            field.byteLength = 4;
            field.scale = 1.0;
            field.unit = "°";
            field.maximum = 180.0;
            field.minimum = -180.0;
            field.description = attitudeDesc[i];
            config.fields.append(field);
            offset += 4;
        }

        // 加速度 (AccX, AccY, AccZ)
        QStringList accAxes = {"AccX", "AccY", "AccZ"};
        for (int i = 0; i < 3; ++i) {
            CommandSettingsDialog::FieldConfig field;
            field.index = config.fields.size() + 1;
            field.elementHead = offset;
            field.name = accAxes[i];
            field.type = CommandSettingsDialog::DataType::Float;
            field.byteLength = 4;
            field.scale = 1.0;
            field.unit = "m/s²";
            field.description = QString("加速度%1轴").arg(QChar('X' + i));
            config.fields.append(field);
            offset += 4;
        }

        // 陀螺仪 (GyroX, GyroY, GyroZ)
        QStringList gyroAxes = {"GyroX", "GyroY", "GyroZ"};
        for (int i = 0; i < 3; ++i) {
            CommandSettingsDialog::FieldConfig field;
            field.index = config.fields.size() + 1;
            field.elementHead = offset;
            field.name = gyroAxes[i];
            field.type = CommandSettingsDialog::DataType::Float;
            field.byteLength = 4;
            field.scale = 1.0;
            field.unit = "°/s";
            field.description = QString("角速度%1轴").arg(QChar('X' + i));
            config.fields.append(field);
            offset += 4;
        }

        // 磁力计 (MagX, MagY, MagZ)
        QStringList magAxes = {"MagX", "MagY", "MagZ"};
        for (int i = 0; i < 3; ++i) {
            CommandSettingsDialog::FieldConfig field;
            field.index = config.fields.size() + 1;
            field.elementHead = offset;
            field.name = magAxes[i];
            field.type = CommandSettingsDialog::DataType::Float;
            field.byteLength = 4;
            field.scale = 1.0;
            field.unit = "μT";
            field.description = QString("磁场%1轴").arg(QChar('X' + i));
            config.fields.append(field);
            offset += 4;
        }

        // 温度
        CommandSettingsDialog::FieldConfig tempField;
        tempField.index = config.fields.size() + 1;
        tempField.elementHead = offset;
        tempField.name = "Temperature";
        tempField.type = CommandSettingsDialog::DataType::Float;
        tempField.byteLength = 4;
        tempField.scale = 1.0;
        tempField.unit = "°C";
        tempField.description = "温度";
        config.fields.append(tempField);
    }

    return config;
}

CommandSettingsDialog::ProtocolConfig NewProtocolDialog::createTextCSVProtocol() const
{
    CommandSettingsDialog::ProtocolConfig config;
    config.checksumType = CommandSettingsDialog::ChecksumType::None;
    config.byteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.checksumByteOrder = CommandSettingsDialog::ByteOrder::LittleEndian;
    config.separator = ",";
    config.frequency = 1000;

    if (m_addDefaultFieldsCheck->isChecked()) {
        // 添加4个字符串类型字段
        for (int i = 0; i < 4; ++i) {
            CommandSettingsDialog::FieldConfig field;
            field.index = i + 1;
            field.elementHead = i;
            field.name = QString("Value%1").arg(i + 1);
            field.type = CommandSettingsDialog::DataType::String;
            field.byteLength = 32;
            field.scale = 1.0;
            field.description = QString("字符串值%1").arg(i + 1);
            config.fields.append(field);
        }
    }

    return config;
}

CommandSettingsDialog::ProtocolConfig NewProtocolDialog::getProtocolConfig() const
{
    return m_config;
}

QString NewProtocolDialog::getProtocolName() const
{
    return m_protocolName;
}

CommandSettingsDialog::ProtocolConfig NewProtocolDialog::createAIGeneratedProtocol() const
{
    // 创建AI协议生成对话框
    AIProtocolInputDialog aiDialog(const_cast<NewProtocolDialog*>(this));

    // 读取API密钥（从环境变量或配置文件）
    // 优先使用标准环境变量 ANTHROPIC_AUTH_TOKEN，向后兼容 ANTHROPIC_API_KEY
    QString apiKey = qgetenv("ANTHROPIC_AUTH_TOKEN");
    if (apiKey.isEmpty()) {
        apiKey = qgetenv("ANTHROPIC_API_KEY");  // 向后兼容
    }
    if (apiKey.isEmpty()) {
        QSettings settings;
        apiKey = settings.value("AI/ApiKey", "").toString();
    }

    // 读取Base URL（从环境变量或配置文件）
    QString baseUrl = qgetenv("ANTHROPIC_BASE_URL");
    if (baseUrl.isEmpty()) {
        QSettings settings;
        baseUrl = settings.value("AI/BaseUrl", "").toString();
    }

    aiDialog.setApiKey(apiKey);
    aiDialog.setBaseUrl(baseUrl);

    // 执行对话框
    if (aiDialog.exec() == QDialog::Accepted && aiDialog.isGenerationSuccess()) {
        return aiDialog.getGeneratedConfig();
    }

    // 用户取消或生成失败，返回空配置
    return CommandSettingsDialog::ProtocolConfig();
}
