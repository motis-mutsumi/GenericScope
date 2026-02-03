#include "newprotocoldialog.h"
#include "ui_newprotocoldialog.h"
#include "aiprotocolinputdialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>

NewProtocolDialog::NewProtocolDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewProtocolDialog)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
}

NewProtocolDialog::~NewProtocolDialog()
{
    delete ui;
}

void NewProtocolDialog::setupUI()
{
    // UI已经通过.ui文件创建，这里只需要初始化按钮文本
    QPushButton *okBtn = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelBtn = ui->buttonBox->button(QDialogButtonBox::Cancel);
    okBtn->setText("创建");
    cancelBtn->setText("取消");

    // 初始化模板描述
    updateTemplateDescription();
}

void NewProtocolDialog::setupConnections()
{
    connect(ui->buttonBox, &QDialogButtonBox::accepted,
            this, &NewProtocolDialog::onAccepted);
    connect(ui->buttonBox, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
    connect(ui->templateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NewProtocolDialog::onTemplateChanged);
    connect(ui->nameEdit, &QLineEdit::textChanged,
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
    QPushButton *okBtn = ui->buttonBox->button(QDialogButtonBox::Ok);
    okBtn->setEnabled(!text.trimmed().isEmpty());
}

void NewProtocolDialog::onAccepted()
{
    if (!validateInput()) {
        return;
    }

    // 根据选择的模板创建协议配置
    ProtocolTemplate templateType = static_cast<ProtocolTemplate>(
        ui->templateCombo->currentIndex());

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
    m_config.name = ui->nameEdit->text().trimmed();
    m_config.version = ui->versionEdit->text().trimmed();
    m_config.description = ui->descriptionEdit->toPlainText().trimmed();

    m_protocolName = m_config.name;

    accept();
}

bool NewProtocolDialog::validateInput()
{
    QString name = ui->nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "输入错误", "协议名称不能为空！");
        ui->nameEdit->setFocus();
        return false;
    }

    // 检查名称是否包含非法字符
    QRegExp nameRegex("^[a-zA-Z0-9_\\-]+$");
    if (!nameRegex.exactMatch(name)) {
        QMessageBox::warning(this, "输入错误",
            "协议名称只能包含字母、数字、下划线和连字符！");
        ui->nameEdit->setFocus();
        return false;
    }

    return true;
}

void NewProtocolDialog::updateTemplateDescription()
{
    ProtocolTemplate templateType = static_cast<ProtocolTemplate>(
        ui->templateCombo->currentIndex());

    QString description;

    switch (templateType) {
    case ProtocolTemplate::Empty:
        description = "创建一个空白协议配置。\n\n"
                     "适用于: 完全自定义的协议\n"
                     "默认配置:\n"
                     "- 无帧头帧尾\n"
                     "- 无校验\n"
                     "- 无默认字段";
        ui->addDefaultFieldsCheck->setEnabled(false);
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
        ui->addDefaultFieldsCheck->setEnabled(true);
        break;

    case ProtocolTemplate::ModbusRTU:
        description = "创建MODBUS RTU协议配置。\n\n"
                     "适用于: MODBUS设备通信\n"
                     "默认配置:\n"
                     "- 无帧头帧尾\n"
                     "- 校验: CRC16-MODBUS (小端序)\n"
                     "- 数据字节序: 大端序\n"
                     "默认字段: DeviceAddr, FunctionCode, DataAddr, DataValue";
        ui->addDefaultFieldsCheck->setEnabled(true);
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
        ui->addDefaultFieldsCheck->setEnabled(true);
        break;

    case ProtocolTemplate::TextCSV:
        description = "创建文本CSV协议配置。\n\n"
                     "适用于: 文本格式数据解析\n"
                     "默认配置:\n"
                     "- 无帧头帧尾\n"
                     "- 无校验\n"
                     "- 分隔符: 逗号\n"
                     "默认字段: Value1~Value4 (string)";
        ui->addDefaultFieldsCheck->setEnabled(true);
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
        ui->addDefaultFieldsCheck->setEnabled(false);
        break;
    }

    ui->templateDescEdit->setText(description);
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

    if (ui->addDefaultFieldsCheck->isChecked()) {
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

    if (ui->addDefaultFieldsCheck->isChecked()) {
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

    if (ui->addDefaultFieldsCheck->isChecked()) {
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

    if (ui->addDefaultFieldsCheck->isChecked()) {
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
