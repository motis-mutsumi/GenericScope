#include "aiprotocolinputdialog.h"
#include "protocolaigenerator.h"
#include "protocolcligenerator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>

AIProtocolInputDialog::AIProtocolInputDialog(QWidget *parent)
    : QDialog(parent)
    , m_apiGenerator(nullptr)
    , m_cliGenerator(nullptr)
    , m_generationSuccess(false)
{
    // 注册自定义类型，用于信号槽传递
    qRegisterMetaType<CommandSettingsDialog::ProtocolConfig>("CommandSettingsDialog::ProtocolConfig");

    // ✅ 先创建生成器对象
    m_apiGenerator = new ProtocolAIGenerator(this);
    m_cliGenerator = new ProtocolCLIGenerator(this);

    // ✅ 再建立UI和信号槽连接
    setupUI();
    setupConnections();
    applyStyles();
}

AIProtocolInputDialog::~AIProtocolInputDialog()
{
}

void AIProtocolInputDialog::setupUI()
{
    setWindowTitle("AI智能协议生成");
    resize(700, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // ========== 说明标签 ==========
    QLabel *titleLabel = new QLabel("通过AI分析原始数据和解析规则，自动生成协议配置", this);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #2c3e50;");
    mainLayout->addWidget(titleLabel);

    // ========== 原始数据组 ==========
    QGroupBox *dataGroup = new QGroupBox("原始16进制数据样本", this);
    QVBoxLayout *dataLayout = new QVBoxLayout(dataGroup);

    QLabel *dataHint = new QLabel(
        "请输入一条完整的16进制数据样本（空格分隔，如：AA 55 01 02 03）", this);
    dataHint->setStyleSheet("color: #7f8c8d; font-size: 12px;");
    dataLayout->addWidget(dataHint);

    m_rawDataEdit = new QTextEdit(this);
    m_rawDataEdit->setPlaceholderText(
        "示例：\n"
        "AA55 0101 2E00 C7F6 2901 601C 0000 0266 F7FF 38DA FFFF 959B 0100 C9C9 0E00 6104 EDFF BF46 1810 D4FF 0000 E1FF 0000 40FF 0000 0000 DC7E"
    );
    m_rawDataEdit->setMinimumHeight(100);
    m_rawDataEdit->setMaximumHeight(150);
    dataLayout->addWidget(m_rawDataEdit);

    mainLayout->addWidget(dataGroup);

    // ========== 解析规则组 ==========
    QGroupBox *rulesGroup = new QGroupBox("解析规则描述", this);
    QVBoxLayout *rulesLayout = new QVBoxLayout(rulesGroup);

    QLabel *rulesHint = new QLabel(
        "请详细描述数据的解析规则（起始位置、数据类型、缩放因子等）", this);
    rulesHint->setStyleSheet("color: #7f8c8d; font-size: 12px;");
    rulesLayout->addWidget(rulesHint);

    m_rulesEdit = new QTextEdit(this);
    m_rulesEdit->setPlaceholderText(
        "示例：\n"
        "原始数据从第7字节开始，取44字节，按照小端int32模式解析为11个数据\n"
        "数据排列顺序：时间戳、温度、角速度(3个)、加速度(3个)、欧拉角(3个)\n\n"
        "转换公式：\n"
        "- 温度/256 = 实际温度\n"
        "- 角速度/65536/1000000*量程 = 实际角速度\n"
        "- 加速度/65536/32768*量程 = 实际加速度\n"
        "- 欧拉角转成I16后*180/32768 = 实际欧拉角"
    );
    m_rulesEdit->setMinimumHeight(150);
    rulesLayout->addWidget(m_rulesEdit);

    mainLayout->addWidget(rulesGroup);

    // ========== 生成方式选择 ==========
    QGroupBox *methodGroup = new QGroupBox("生成方式", this);
    QVBoxLayout *methodLayout = new QVBoxLayout(methodGroup);

    m_useAPIRadio = new QRadioButton("使用API调用（需要API密钥）", this);
    m_useCLIRadio = new QRadioButton("使用CLI命令行（需要安装claude命令）", this);

    // 默认选择CLI方式（更简单）
    m_useCLIRadio->setChecked(true);

    m_methodGroup = new QButtonGroup(this);
    m_methodGroup->addButton(m_useAPIRadio, 0);
    m_methodGroup->addButton(m_useCLIRadio, 1);

    methodLayout->addWidget(m_useAPIRadio);
    methodLayout->addWidget(m_useCLIRadio);

    // 添加方式提示
    m_methodHintLabel = new QLabel(this);
    m_methodHintLabel->setStyleSheet("color: #3498db; font-size: 12px; padding: 5px;");
    m_methodHintLabel->setWordWrap(true);
    methodLayout->addWidget(m_methodHintLabel);

    mainLayout->addWidget(methodGroup);

    // 更新提示信息
    onMethodChanged();

    // ========== 状态和进度 ==========
    m_statusLabel = new QLabel("", this);
    m_statusLabel->setStyleSheet("color: #27ae60; font-weight: bold;");
    m_statusLabel->setWordWrap(true);
    mainLayout->addWidget(m_statusLabel);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 0);  // 不确定进度
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);

    mainLayout->addStretch();

    // ========== 底部按钮 ==========
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_pasteExampleBtn = new QPushButton("粘贴示例", this);
    buttonLayout->addWidget(m_pasteExampleBtn);

    m_clearBtn = new QPushButton("清空", this);
    buttonLayout->addWidget(m_clearBtn);

    buttonLayout->addSpacing(20);

    m_generateBtn = new QPushButton("生成协议", this);
    m_generateBtn->setDefault(true);
    buttonLayout->addWidget(m_generateBtn);

    m_cancelBtn = new QPushButton("取消", this);
    buttonLayout->addWidget(m_cancelBtn);

    mainLayout->addLayout(buttonLayout);
}

void AIProtocolInputDialog::setupConnections()
{
    connect(m_pasteExampleBtn, &QPushButton::clicked,
            this, &AIProtocolInputDialog::onPasteExample);
    connect(m_clearBtn, &QPushButton::clicked,
            this, &AIProtocolInputDialog::onClearAll);
    connect(m_generateBtn, &QPushButton::clicked,
            this, &AIProtocolInputDialog::onGenerate);
    connect(m_cancelBtn, &QPushButton::clicked,
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

    m_generateBtn->setStyleSheet(buttonStyle);

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

    m_pasteExampleBtn->setStyleSheet(secondaryButtonStyle);
    m_clearBtn->setStyleSheet(secondaryButtonStyle);
    m_cancelBtn->setStyleSheet(secondaryButtonStyle);

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
    m_rawDataEdit->setPlainText(
        "AA55 0101 2E00 C7F6 2901 601C 0000 0266 F7FF 38DA FFFF 959B 0100 "
        "C9C9 0E00 6104 EDFF BF46 1810 D4FF 0000 E1FF 0000 40FF 0000 0000 DC7E"
    );

    m_rulesEdit->setPlainText(
        "原始数据从第7字节开始，取44字节，按照小端int32模式解析为11个数据\n"
        "数据排列顺序：时间戳、温度、角速度(3个)、加速度(3个)、欧拉角(3个)\n\n"
        "转换公式：\n"
        "- 温度/256 = 实际温度\n"
        "- 角速度/65536/1000000*量程 = 实际角速度\n"
        "- 加速度/65536/32768*量程 = 实际加速度\n"
        "- 欧拉角转成I16后*180/32768 = 实际欧拉角"
    );

    m_statusLabel->setText("已粘贴示例数据");
}

void AIProtocolInputDialog::onClearAll()
{
    m_rawDataEdit->clear();
    m_rulesEdit->clear();
    m_statusLabel->clear();
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
    QString rawData = m_rawDataEdit->toPlainText().trimmed();
    QString rules = m_rulesEdit->toPlainText().trimmed();

    // 根据选择的方式调用不同的生成器
    if (m_useAPIRadio->isChecked()) {
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
        m_progressBar->setVisible(true);
        m_statusLabel->setText("正在调用Claude API分析协议...");

        // 设置API配置
        m_apiGenerator->setApiKey(m_apiKey);
        m_apiGenerator->setBaseUrl(m_baseUrl);

        // 调用API生成器
        m_apiGenerator->generateProtocol(protocolName, rawData, rules);
    } else {
        // 使用CLI方式
        // 禁用UI
        setUIEnabled(false);
        m_progressBar->setVisible(true);
        m_statusLabel->setText("正在调用Claude CLI分析协议...");

        // 调用CLI生成器
        m_cliGenerator->generateProtocol(protocolName, rawData, rules);
    }
}

void AIProtocolInputDialog::onCancel()
{
    // 如果正在生成，取消生成
    if (m_progressBar->isVisible()) {
        m_apiGenerator->cancel();
        m_cliGenerator->cancel();
        setUIEnabled(true);
        m_progressBar->setVisible(false);
        m_statusLabel->setText("已取消生成");
    } else {
        reject();
    }
}

void AIProtocolInputDialog::onMethodChanged()
{
    if (m_useAPIRadio->isChecked()) {
        m_methodHintLabel->setText(
            "💡 API方式：需要配置API密钥，适合生产环境和批量生成。\n"
            "获取密钥：https://console.anthropic.com/");
    } else {
        m_methodHintLabel->setText(
            "💡 CLI方式：需要安装claude命令并登录，适合个人开发和测试。\n"
            "安装方法：详见文档 docs/ai-protocol-cli-guide.md");
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
    m_progressBar->setVisible(false);
    m_statusLabel->setText("✓ 协议配置生成成功！");
    m_statusLabel->setStyleSheet("color: #27ae60; font-weight: bold; font-size: 14px;");

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
    m_progressBar->setVisible(false);
    m_statusLabel->setText("✗ 生成失败");
    m_statusLabel->setStyleSheet("color: #e74c3c; font-weight: bold; font-size: 14px;");

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
    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet("color: #3498db; font-weight: bold;");
    QApplication::processEvents();  // 更新UI
}

bool AIProtocolInputDialog::validateInputs(QString *errorMsg)
{
    QString rawData = m_rawDataEdit->toPlainText().trimmed();
    QString rules = m_rulesEdit->toPlainText().trimmed();

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
    m_rawDataEdit->setEnabled(enabled);
    m_rulesEdit->setEnabled(enabled);
    m_pasteExampleBtn->setEnabled(enabled);
    m_clearBtn->setEnabled(enabled);
    m_generateBtn->setEnabled(enabled);
    m_cancelBtn->setText(enabled ? "取消" : "取消生成");
}
