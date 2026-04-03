#include "commandsenddialog.h"

#include "device/devicemanager.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegExp>
#include <QSettings>
#include <QSignalBlocker>
#include <QVBoxLayout>

#if 0
CommandSendDialog::CommandSendDialog(DeviceManager *deviceManager, QWidget *parent)
    : QDialog(parent)
    , m_deviceManager(deviceManager)
    , m_protocolCombo(nullptr)
    , m_commandCombo(nullptr)
    , m_hexEdit(nullptr)
    , m_descLabel(nullptr)
    , m_statusLabel(nullptr)
    , m_refreshBtn(nullptr)
    , m_sendBtn(nullptr)
{
    setupUI();
    setupConnections();
    loadCommandsFromSettings();
}

void CommandSendDialog::setupUI()
{
    setWindowTitle("指令发送");
    setModal(true);
    resize(640, 320);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    auto *selectGroup = new QGroupBox("选择指令", this);
    auto *formLayout = new QFormLayout(selectGroup);
    formLayout->setContentsMargins(10, 12, 10, 10);
    formLayout->setSpacing(8);

    m_protocolCombo = new QComboBox(selectGroup);
    m_commandCombo = new QComboBox(selectGroup);
    m_hexEdit = new QLineEdit(selectGroup);
    m_descLabel = new QLabel(selectGroup);
    m_descLabel->setWordWrap(true);
    m_descLabel->setMinimumHeight(48);

    m_hexEdit->setPlaceholderText("HEX，如 FF AA 01 00");

    formLayout->addRow("协议：", m_protocolCombo);
    formLayout->addRow("指令：", m_commandCombo);
    formLayout->addRow("HEX：", m_hexEdit);
    formLayout->addRow("说明：", m_descLabel);

    m_statusLabel = new QLabel("请先选择指令。", this);

    auto *buttonRow = new QHBoxLayout();
    m_refreshBtn = new QPushButton("刷新列表", this);
    m_sendBtn = new QPushButton("发送", this);
    auto *closeBtn = new QPushButton("关闭", this);

    m_sendBtn->setDefault(true);

    buttonRow->addWidget(m_statusLabel, 1);
    buttonRow->addStretch();
    buttonRow->addWidget(m_refreshBtn);
    buttonRow->addWidget(m_sendBtn);
    buttonRow->addWidget(closeBtn);

    mainLayout->addWidget(selectGroup);
    mainLayout->addLayout(buttonRow);

    connect(closeBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void CommandSendDialog::setupConnections()
{
    connect(m_protocolCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandSendDialog::onProtocolChanged);
    connect(m_commandCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CommandSendDialog::onCommandChanged);
    connect(m_refreshBtn, &QPushButton::clicked,
            this, &CommandSendDialog::onRefreshClicked);
    connect(m_sendBtn, &QPushButton::clicked,
            this, &CommandSendDialog::onSendClicked);
}

void CommandSendDialog::loadCommandsFromSettings()
{
    const QString currentProtocol = m_protocolCombo ? m_protocolCombo->currentText().trimmed() : QString();
    const QString currentCommand = m_commandCombo ? m_commandCombo->currentText().trimmed() : QString();

    m_commands.clear();

    QSettings settings("GenericScope", "ProtocolConfig");
    const int protocolCount = settings.beginReadArray("protocols");

    for (int i = 0; i < protocolCount; ++i) {
        settings.setArrayIndex(i);

        const QString fallbackName = settings.value("name").toString().trimmed();
        const QString jsonStr = settings.value("config").toString().trimmed();
        if (jsonStr.isEmpty()) {
            continue;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
        if (doc.isNull() || !doc.isObject()) {
            continue;
        }

        const QJsonObject configObj = doc.object();
        QString protocolName = configObj.value("name").toString().trimmed();
        if (protocolName.isEmpty()) {
            protocolName = fallbackName;
        }
        if (protocolName.isEmpty()) {
            continue;
        }

        const QJsonArray commandArray = configObj.value("commands").toArray();
        for (const QJsonValue &value : commandArray) {
            if (!value.isObject()) {
                continue;
            }

            const QJsonObject commandObj = value.toObject();
            const QString commandName = commandObj.value("name").toString().trimmed();
            const QString description = commandObj.value("description").toString().trimmed();

            QString payloadHex = commandObj.value("payloadHex").toString().trimmed();
            if (payloadHex.isEmpty()) {
                payloadHex = commandObj.value("payload").toString().trimmed(); // 兼容旧字段
            }
            payloadHex = normalizeHex(payloadHex);

            if (commandName.isEmpty() || payloadHex.isEmpty()) {
                continue;
            }

            CommandItem item;
            item.protocolName = protocolName;
            item.commandName = commandName;
            item.payloadHex = payloadHex;
            item.description = description;
            m_commands.append(item);
        }
    }

    settings.endArray();

    rebuildProtocolList(currentProtocol);
    rebuildCommandList(currentCommand);
    applyCommandToEditor();
}

void CommandSendDialog::rebuildProtocolList(const QString &preferredProtocol)
{
    if (!m_protocolCombo) {
        return;
    }

    QStringList protocols;
    for (const CommandItem &item : qAsConst(m_commands)) {
        if (!protocols.contains(item.protocolName)) {
            protocols.append(item.protocolName);
        }
    }
    protocols.sort(Qt::CaseInsensitive);

    const QSignalBlocker blocker(m_protocolCombo);
    m_protocolCombo->clear();

    if (protocols.isEmpty()) {
        m_protocolCombo->addItem("无可用协议");
        m_protocolCombo->setEnabled(false);
        return;
    }

    m_protocolCombo->setEnabled(true);
    for (const QString &protocol : protocols) {
        m_protocolCombo->addItem(protocol);
    }

    int index = preferredProtocol.isEmpty() ? -1 : m_protocolCombo->findText(preferredProtocol);
    if (index < 0) {
        index = 0;
    }
    m_protocolCombo->setCurrentIndex(index);
}

void CommandSendDialog::rebuildCommandList(const QString &preferredCommand)
{
    if (!m_commandCombo || !m_protocolCombo) {
        return;
    }

    const QString protocolName = m_protocolCombo->currentText().trimmed();
    const QSignalBlocker blocker(m_commandCombo);
    m_commandCombo->clear();

    if (protocolName.isEmpty() || !m_protocolCombo->isEnabled()) {
        m_commandCombo->addItem("无可用指令");
        m_commandCombo->setEnabled(false);
        m_sendBtn->setEnabled(false);
        return;
    }

    for (int i = 0; i < m_commands.size(); ++i) {
        const CommandItem &item = m_commands.at(i);
        if (item.protocolName == protocolName) {
            m_commandCombo->addItem(item.commandName, i);
        }
    }

    if (m_commandCombo->count() == 0) {
        m_commandCombo->addItem("无可用指令");
        m_commandCombo->setEnabled(false);
        m_sendBtn->setEnabled(false);
        return;
    }

    m_commandCombo->setEnabled(true);
    m_sendBtn->setEnabled(true);

    int index = preferredCommand.isEmpty() ? -1 : m_commandCombo->findText(preferredCommand);
    if (index < 0) {
        index = 0;
    }
    m_commandCombo->setCurrentIndex(index);
}

void CommandSendDialog::applyCommandToEditor()
{
    if (!m_commandCombo || !m_hexEdit || !m_descLabel || !m_statusLabel) {
        return;
    }

    const bool hasCommand = m_commandCombo->isEnabled() && m_commandCombo->currentData().isValid();
    if (!hasCommand) {
        m_hexEdit->clear();
        m_descLabel->setText("暂无指令，请先在“指令设置”中添加。");
        m_statusLabel->setText("未找到可发送的指令。");
        m_sendBtn->setEnabled(false);
        return;
    }

    const int commandIndex = m_commandCombo->currentData().toInt();
    if (commandIndex < 0 || commandIndex >= m_commands.size()) {
        m_hexEdit->clear();
        m_descLabel->clear();
        m_statusLabel->setText("指令索引无效。");
        m_sendBtn->setEnabled(false);
        return;
    }

    const CommandItem &item = m_commands.at(commandIndex);
    m_hexEdit->setText(item.payloadHex);
    m_descLabel->setText(item.description.isEmpty() ? "无说明" : item.description);

    const int byteCount = item.payloadHex.simplified().remove(' ').size() / 2;
    m_statusLabel->setText(QString("当前指令长度：%1 字节").arg(byteCount));
    m_sendBtn->setEnabled(true);
}

QString CommandSendDialog::normalizeHex(const QString &hex) const
{
    QString compact = hex.toUpper();
    compact.remove(QRegExp("[^0-9A-F]"));

    QString normalized;
    normalized.reserve(compact.size() + compact.size() / 2);
    for (int i = 0; i < compact.size(); ++i) {
        if (i > 0 && (i % 2) == 0) {
            normalized.append(' ');
        }
        normalized.append(compact.at(i));
    }
    return normalized.trimmed();
}

bool CommandSendDialog::isValidHex(const QString &hex) const
{
    const QString compact = hex.simplified().remove(' ');
    if (compact.isEmpty()) {
        return false;
    }

    QRegExp hexRegex("^[0-9A-Fa-f]+$");
    return hexRegex.exactMatch(compact) && ((compact.size() % 2) == 0);
}

void CommandSendDialog::onProtocolChanged(int index)
{
    Q_UNUSED(index);
    rebuildCommandList();
    applyCommandToEditor();
}

void CommandSendDialog::onCommandChanged(int index)
{
    Q_UNUSED(index);
    applyCommandToEditor();
}

void CommandSendDialog::onRefreshClicked()
{
    loadCommandsFromSettings();

    if (m_commands.isEmpty()) {
        m_statusLabel->setText("未读取到可用指令，请先在指令设置中添加并保存。");
    } else {
        m_statusLabel->setText(QString("已加载 %1 条指令。").arg(m_commands.size()));
    }
}

void CommandSendDialog::onSendClicked()
{
    if (!m_deviceManager) {
        QMessageBox::critical(this, "错误", "设备管理器不可用，无法发送指令。");
        return;
    }

    if (!m_deviceManager->isConnected()) {
        QMessageBox::warning(this, "未连接", "设备尚未连接，请先连接串口或UDP。");
        m_statusLabel->setText("发送失败：设备未连接。");
        return;
    }

    const QString normalizedHex = normalizeHex(m_hexEdit->text().trimmed());
    m_hexEdit->setText(normalizedHex);

    if (!isValidHex(normalizedHex)) {
        QMessageBox::warning(this, "HEX格式错误", "请输入合法的16进制字符串，例如：FF AA 01 00");
        m_statusLabel->setText("发送失败：HEX格式错误。");
        return;
    }

    const QByteArray commandBytes = QByteArray::fromHex(
        normalizedHex.simplified().remove(' ').toLatin1());

    if (commandBytes.isEmpty()) {
        QMessageBox::warning(this, "发送失败", "指令内容为空，无法发送。");
        m_statusLabel->setText("发送失败：指令为空。");
        return;
    }

    m_deviceManager->sendCommand(commandBytes);

    const QString commandName = m_commandCombo->currentText().trimmed();
    const QString nameForDisplay = commandName.isEmpty() ? "自定义指令" : commandName;
    m_statusLabel->setText(QString("已发送：%1（%2 字节）").arg(nameForDisplay).arg(commandBytes.size()));
}
#endif
