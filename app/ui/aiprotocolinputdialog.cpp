#include "aiprotocolinputdialog.h"
#include "ui_aiprotocolinputdialog.h"
#include "protocolaigenerator.h"
#include "protocolcligenerator.h"
#include <QMessageBox>
#include <QApplication>
#include <QButtonGroup>

AIProtocolInputDialog::AIProtocolInputDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AIProtocolInputDialog)
    , m_methodGroup(nullptr)
    , m_apiGenerator(nullptr)
    , m_cliGenerator(nullptr)
    , m_generationSuccess(false)
{
    ui->setupUi(this);

    // 注册自定义类型，用于信号槽传递
    qRegisterMetaType<CommandSettingsDialog::ProtocolConfig>("CommandSettingsDialog::ProtocolConfig");

    // 创建按钮组（.ui文件中的RadioButton需要手动分组）
    m_methodGroup = new QButtonGroup(this);
    m_methodGroup->addButton(ui->useAPIRadio, 0);
    m_methodGroup->addButton(ui->useCLIRadio, 1);

    // 创建生成器对象
    m_apiGenerator = new ProtocolAIGenerator(this);
    m_cliGenerator = new ProtocolCLIGenerator(this);

    setupUI();
    setupConnections();
    applyStyles();
}

AIProtocolInputDialog::~AIProtocolInputDialog()
{
    delete ui;
}

void AIProtocolInputDialog::setupUI()
{
    // UI已经通过.ui文件创建，这里只需要初始化动态内容
    onMethodChanged();  // 更新方式提示
}

void AIProtocolInputDialog::setupConnections()
{
    connect(ui->pasteExampleBtn, &QPushButton::clicked,
            this, &AIProtocolInputDialog::onPasteExample);
    connect(ui->clearBtn, &QPushButton::clicked,
            this, &AIProtocolInputDialog::onClearAll);
    connect(ui->generateBtn, &QPushButton::clicked,
            this, &AIProtocolInputDialog::onGenerate);
    connect(ui->cancelBtn, &QPushButton::clicked,
            this, &AIProtocolInputDialog::onCancel);

    // 连接方式切换信号
    connect(m_methodGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &AIProtocolInputDialog::onMethodChanged);

    // 连接API生成器信号
    connect(m_apiGenerator, &ProtocolAIGenerator::generationComplete,
            this, &AIProtocolInputDialog::onGenerationComplete);
    connect(m_apiGenerator, &ProtocolAIGenerator::generationFailed,
            this, &AIProtocolInputDialog::onGenerationFailed);
    connect(m_apiGenerator, &ProtocolAIGenerator::progressUpdate,
            this, &AIProtocolInputDialog::onProgressUpdate);

    // 连接CLI生成器信号
    connect(m_cliGenerator, &ProtocolCLIGenerator::generationComplete,
            this, &AIProtocolInputDialog::onGenerationComplete);
    connect(m_cliGenerator, &ProtocolCLIGenerator::generationFailed,
            this, &AIProtocolInputDialog::onGenerationFailed);
    connect(m_cliGenerator, &ProtocolCLIGenerator::progressUpdate,
            this, &AIProtocolInputDialog::onProgressUpdate);
}

void AIProtocolInputDialog::applyStyles()
{
    QString buttonStyle = R"(
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #1c5985;
        }
        QPushButton:disabled {
            background-color: #bdc3c7;
        }
    )";

    ui->generateBtn->setStyleSheet(buttonStyle);

    QString secondaryButtonStyle = R"(
        QPushButton {
            background-color: #95a5a6;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 13px;
        }
        QPushButton:hover {
            background-color: #7f8c8d;
        }
        QPushButton:pressed {
            background-color: #6c7a7b;
        }
    )";

    ui->pasteExampleBtn->setStyleSheet(secondaryButtonStyle);
    ui->clearBtn->setStyleSheet(secondaryButtonStyle);
    ui->cancelBtn->setStyleSheet(secondaryButtonStyle);

    QString groupBoxStyle = R"(
        QGroupBox {
            font-weight: bold;
            font-size: 13px;
            border: 2px solid #bdc3c7;
            border-radius: 6px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 15px;
            padding: 0 5px;
        }
    )";

    setStyleSheet(groupBoxStyle);
}

void AIProtocolInputDialog::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void AIProtocolInputDialog::setBaseUrl(const QString &baseUrl)
{
    m_baseUrl = baseUrl;
}

CommandSettingsDialog::ProtocolConfig AIProtocolInputDialog::getGeneratedConfig() const
{
    return m_generatedConfig;
}

bool AIProtocolInputDialog::isGenerationSuccess() const
{
    return m_generationSuccess;
}

void AIProtocolInputDialog::onPasteExample()
{
    // 粘贴示例数据
    ui->rawDataEdit->setPlainText(
        "AA55 0101 2E00 C7F6 2901 601C 0000 0266 F7FF 38DA FFFF 959B 0100 "
        "C9C9 0E00 6104 EDFF BF46 1810 D4FF 0000 E1FF 0000 40FF 0000 0000 DC7E"
    );

    ui->rulesEdit->setPlainText(
        "原始数据从第7字节开始，取44字节，按照小端int32模式解析为11个数据\n"
        "数据排列顺序：时间戳、温度、角速度(3个)、加速度(3个)、欧拉角(3个)\n\n"
        "转换公式：\n"
        "- 温度/256 = 实际温度\n"
        "- 角速度/65536/1000000*量程 = 实际角速度\n"
        "- 加速度/65536/32768*量程 = 实际加速度\n"
        "- 欧拉角转成I16后*180/32768 = 实际欧拉角"
    );

    ui->statusLabel->setText("已粘贴示例数据");
}

void AIProtocolInputDialog::onClearAll()
{
    ui->rawDataEdit->clear();
    ui->rulesEdit->clear();
    ui->statusLabel->clear();
}

void AIProtocolInputDialog::onGenerate()
{
    QString errorMsg;
    if (!validateInputs(&errorMsg)) {
        QMessageBox::warning(this, "输入错误", errorMsg);
        return;
    }

    // 获取输入数据
    QString protocolName = "Generated_Protocol";  // 临时名称
    QString rawData = ui->rawDataEdit->toPlainText().trimmed();
    QString rules = ui->rulesEdit->toPlainText().trimmed();

    // 根据选择的方式调用不同的生成器
    if (ui->useAPIRadio->isChecked()) {
        // 使用API方式
        if (m_apiKey.isEmpty()) {
            QMessageBox::warning(this, "配置错误",
                "未配置Claude API密钥！\n\n"
                "请设置环境变量 ANTHROPIC_API_KEY，或在设置中配置。\n\n"
                "获取API密钥：https://console.anthropic.com/");
            return;
        }

        // 禁用UI
        setUIEnabled(false);
        ui->progressBar->setVisible(true);
        ui->statusLabel->setText("正在调用Claude API分析协议...");

        // 设置API配置
        m_apiGenerator->setApiKey(m_apiKey);
        m_apiGenerator->setBaseUrl(m_baseUrl);

        // 调用API生成器
        m_apiGenerator->generateProtocol(protocolName, rawData, rules);
    } else {
        // 使用CLI方式
        // 禁用UI
        setUIEnabled(false);
        ui->progressBar->setVisible(true);
        ui->statusLabel->setText("正在调用Claude CLI分析协议...");

        // 调用CLI生成器
        m_cliGenerator->generateProtocol(protocolName, rawData, rules);
    }
}

void AIProtocolInputDialog::onCancel()
{
    // 如果正在生成，取消生成
    if (ui->progressBar->isVisible()) {
        m_apiGenerator->cancel();
        m_cliGenerator->cancel();
        setUIEnabled(true);
        ui->progressBar->setVisible(false);
        ui->statusLabel->setText("已取消生成");
    } else {
        reject();
    }
}

void AIProtocolInputDialog::onMethodChanged()
{
    if (ui->useAPIRadio->isChecked()) {
        ui->methodHintLabel->setText(
            "💡 API方式：需要配置API密钥，适合生产环境和批量生成。\n"
            "获取密钥：https://console.anthropic.com/");
    } else {
        ui->methodHintLabel->setText(
            "💡 CLI方式：需要安装claude命令并登录，适合个人开发和测试。\n"
            "安装方法：详见文档 docs/markdown_all/ai-protocol-cli-guide.md");
    }
}

void AIProtocolInputDialog::onGenerationComplete(
    const CommandSettingsDialog::ProtocolConfig &config)
{
    qDebug() << "========== onGenerationComplete被调用 ==========";
    qDebug() << "接收到的配置字段数:" << config.fields.size();
    qDebug() << "协议名称:" << config.name;

    m_generatedConfig = config;
    m_generationSuccess = true;

    qDebug() << "恢复UI...";
    // 恢复UI
    setUIEnabled(true);
    ui->progressBar->setVisible(false);
    ui->statusLabel->setText("✓ 协议配置生成成功！");
    ui->statusLabel->setStyleSheet("color: #27ae60; font-weight: bold; font-size: 14px;");

    qDebug() << "显示成功对话框...";
    // 显示成功提示
    QMessageBox::information(this, "生成成功",
        QString("协议配置已成功生成！\n\n"
                "协议名称：%1\n"
                "字段数量：%2\n\n"
                "点击确定返回并应用配置。")
            .arg(config.name)
            .arg(config.fields.size()));

    qDebug() << "调用accept()关闭对话框...";
    accept();
    qDebug() << "========== onGenerationComplete执行完毕 ==========";
}

void AIProtocolInputDialog::onGenerationFailed(const QString &errorMessage)
{
    // 恢复UI
    setUIEnabled(true);
    ui->progressBar->setVisible(false);
    ui->statusLabel->setText("✗ 生成失败");
    ui->statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold; font-size: 14px;");

    // 显示错误信息
    QMessageBox::critical(this, "生成失败",
        QString("AI协议生成失败：\n\n%1\n\n"
                "请检查：\n"
                "1. API密钥是否正确\n"
                "2. 网络连接是否正常\n"
                "3. 输入数据是否完整")
            .arg(errorMessage));
}

void AIProtocolInputDialog::onProgressUpdate(const QString &message)
{
    ui->statusLabel->setText(message);
    ui->statusLabel->setStyleSheet("color: #3498db; font-weight: bold;");
    QApplication::processEvents();  // 更新UI
}

bool AIProtocolInputDialog::validateInputs(QString *errorMsg)
{
    QString rawData = ui->rawDataEdit->toPlainText().trimmed();
    QString rules = ui->rulesEdit->toPlainText().trimmed();

    if (rawData.length() < kMinDataLength) {
        if (errorMsg) *errorMsg = "原始数据样本太短，请输入完整的16进制数据！";
        return false;
    }

    if (rules.length() < kMinRulesLength) {
        if (errorMsg) *errorMsg = "解析规则描述太简单，请详细描述解析规则！";
        return false;
    }

    // 验证16进制数据格式
    QString cleanData = rawData;
    cleanData.remove(' ').remove('\n').remove('\r');
    QRegExp hexRegex("^[0-9A-Fa-f]+$");
    if (!hexRegex.exactMatch(cleanData)) {
        if (errorMsg) *errorMsg = "原始数据包含非16进制字符，请检查输入！";
        return false;
    }

    return true;
}

void AIProtocolInputDialog::setUIEnabled(bool enabled)
{
    ui->rawDataEdit->setEnabled(enabled);
    ui->rulesEdit->setEnabled(enabled);
    ui->pasteExampleBtn->setEnabled(enabled);
    ui->clearBtn->setEnabled(enabled);
    ui->generateBtn->setEnabled(enabled);
    ui->cancelBtn->setText(enabled ? "取消" : "取消生成");
}
