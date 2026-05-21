#include "commandsenddialog.h"

#include "device/devicemanager.h"

#include <QCheckBox>
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
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegExp>
#include <QSettings>
#include <QSignalBlocker>
#include <QDateTime>
#include <QVBoxLayout>

CommandSendDialog::CommandSendDialog(DeviceManager *deviceManager, QWidget *parent)
    : QDialog(parent)
    , m_deviceManager(deviceManager)
    , m_protocolCombo(nullptr)
    , m_commandCombo(nullptr)
    , m_onlyFavoriteCheck(nullptr)
    , m_hexEdit(nullptr)
    , m_descLabel(nullptr)
    , m_statusLabel(nullptr)
    , m_historyCombo(nullptr)
    , m_refreshBtn(nullptr)
    , m_toggleFavoriteBtn(nullptr)
    , m_resendBtn(nullptr)
    , m_sendBtn(nullptr)
    , m_rxLogEdit(nullptr)
    , m_clearRxLogBtn(nullptr)
{
    setupUI();
    setupConnections();
    loadFavoriteKeys();
    loadSendHistory();
    loadCommandsFromSettings();
}

void CommandSendDialog::setupUI()
{
    setObjectName(QStringLiteral("commandSendDialog"));
    setWindowTitle(QStringLiteral("指令发送"));
    setModal(true);
    resize(700, 380);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);

    auto *selectGroup = new QGroupBox(QStringLiteral("选择指令"), this);
    auto *formLayout = new QFormLayout(selectGroup);
    formLayout->setContentsMargins(10, 12, 10, 10);
    formLayout->setSpacing(8);

    m_protocolCombo = new QComboBox(selectGroup);
    m_commandCombo = new QComboBox(selectGroup);
    m_onlyFavoriteCheck = new QCheckBox(QStringLiteral("仅显示常用"), selectGroup);
    m_hexEdit = new QLineEdit(selectGroup);
    m_descLabel = new QLabel(selectGroup);
    m_descLabel->setWordWrap(true);
    m_descLabel->setMinimumHeight(48);
    m_historyCombo = new QComboBox(selectGroup);
    m_historyCombo->setMinimumWidth(480);

    m_hexEdit->setPlaceholderText(QStringLiteral("HEX，如 FF AA 01 00"));

    auto *commandRow = new QWidget(selectGroup);
    auto *commandRowLayout = new QHBoxLayout(commandRow);
    commandRowLayout->setContentsMargins(0, 0, 0, 0);
    commandRowLayout->setSpacing(8);
    commandRowLayout->addWidget(m_commandCombo, 1);
    commandRowLayout->addWidget(m_onlyFavoriteCheck);

    formLayout->addRow(QStringLiteral("协议："), m_protocolCombo);
    formLayout->addRow(QStringLiteral("指令："), commandRow);
    formLayout->addRow(QStringLiteral("HEX："), m_hexEdit);
    formLayout->addRow(QStringLiteral("说明："), m_descLabel);
    formLayout->addRow(QStringLiteral("最近发送："), m_historyCombo);

    m_statusLabel = new QLabel(QStringLiteral("请先选择指令。"), this);
    m_statusLabel->setObjectName(QStringLiteral("secondaryLabel"));

    auto *buttonRow = new QHBoxLayout();
    m_refreshBtn = new QPushButton(QStringLiteral("刷新列表"), this);
    m_refreshBtn->setObjectName(QStringLiteral("secondaryButton"));
    m_toggleFavoriteBtn = new QPushButton(QStringLiteral("加入常用"), this);
    m_toggleFavoriteBtn->setObjectName(QStringLiteral("secondaryButton"));
    m_resendBtn = new QPushButton(QStringLiteral("重发历史"), this);
    m_resendBtn->setObjectName(QStringLiteral("secondaryButton"));
    m_sendBtn = new QPushButton(QStringLiteral("发送"), this);
    auto *closeBtn = new QPushButton(QStringLiteral("关闭"), this);
    closeBtn->setObjectName(QStringLiteral("secondaryButton"));

    m_sendBtn->setDefault(true);

    buttonRow->addWidget(m_statusLabel, 1);
    buttonRow->addStretch();
    buttonRow->addWidget(m_refreshBtn);
    buttonRow->addWidget(m_toggleFavoriteBtn);
    buttonRow->addWidget(m_resendBtn);
    buttonRow->addWidget(m_sendBtn);
    buttonRow->addWidget(closeBtn);

    auto *rxGroup = new QGroupBox(QStringLiteral("设备返回值"), this);
    auto *rxLayout = new QVBoxLayout(rxGroup);
    rxLayout->setContentsMargins(10, 10, 10, 10);
    rxLayout->setSpacing(6);

    m_rxLogEdit = new QPlainTextEdit(rxGroup);
    m_rxLogEdit->setReadOnly(true);
    m_rxLogEdit->setMaximumBlockCount(500);
    m_rxLogEdit->setPlaceholderText(QStringLiteral("设备返回数据将显示在这里（时间 + HEX）"));

    m_clearRxLogBtn = new QPushButton(QStringLiteral("清空返回"), rxGroup);
    m_clearRxLogBtn->setObjectName(QStringLiteral("secondaryButton"));
    auto *rxButtonRow = new QHBoxLayout();
    rxButtonRow->addStretch();
    rxButtonRow->addWidget(m_clearRxLogBtn);

    rxLayout->addWidget(m_rxLogEdit, 1);
    rxLayout->addLayout(rxButtonRow);

    mainLayout->addWidget(selectGroup);
    mainLayout->addWidget(rxGroup, 1);
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
    connect(m_resendBtn, &QPushButton::clicked,
            this, &CommandSendDialog::onResendClicked);
    connect(m_toggleFavoriteBtn, &QPushButton::clicked,
            this, &CommandSendDialog::onToggleFavoriteClicked);
    connect(m_onlyFavoriteCheck, &QCheckBox::toggled,
            this, &CommandSendDialog::onOnlyFavoriteToggled);
    connect(m_clearRxLogBtn, &QPushButton::clicked,
            this, &CommandSendDialog::onClearRxLogClicked);
    if (m_deviceManager) {
        connect(m_deviceManager, &DeviceManager::dataReceived,
                this, &CommandSendDialog::onDeviceDataReceived);
    }
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
                payloadHex = commandObj.value("payload").toString().trimmed();
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
        m_protocolCombo->addItem(QStringLiteral("无可用协议"));
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
        m_commandCombo->addItem(QStringLiteral("无可用指令"));
        m_commandCombo->setEnabled(false);
        m_sendBtn->setEnabled(false);
        m_toggleFavoriteBtn->setEnabled(false);
        return;
    }

    const bool onlyFavorite = m_onlyFavoriteCheck && m_onlyFavoriteCheck->isChecked();
    for (int i = 0; i < m_commands.size(); ++i) {
        const CommandItem &item = m_commands.at(i);
        if (item.protocolName != protocolName) {
            continue;
        }
        const QString key = commandKey(item.protocolName, item.commandName);
        if (onlyFavorite && !m_favoriteKeys.contains(key)) {
            continue;
        }
        m_commandCombo->addItem(item.commandName, i);
    }

    if (m_commandCombo->count() == 0) {
        m_commandCombo->addItem(onlyFavorite ? QStringLiteral("无常用指令")
                                             : QStringLiteral("无可用指令"));
        m_commandCombo->setEnabled(false);
        m_sendBtn->setEnabled(false);
        m_toggleFavoriteBtn->setEnabled(false);
        return;
    }

    m_commandCombo->setEnabled(true);
    m_sendBtn->setEnabled(true);
    m_toggleFavoriteBtn->setEnabled(true);

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
        m_descLabel->setText(QStringLiteral("暂无指令，请先在“指令设置”中添加。"));
        m_statusLabel->setText(QStringLiteral("未找到可发送的指令。"));
        m_sendBtn->setEnabled(false);
        m_toggleFavoriteBtn->setEnabled(false);
        return;
    }

    const int commandIndex = m_commandCombo->currentData().toInt();
    if (commandIndex < 0 || commandIndex >= m_commands.size()) {
        m_hexEdit->clear();
        m_descLabel->clear();
        m_statusLabel->setText(QStringLiteral("指令索引无效。"));
        m_sendBtn->setEnabled(false);
        m_toggleFavoriteBtn->setEnabled(false);
        return;
    }

    const CommandItem &item = m_commands.at(commandIndex);
    m_hexEdit->setText(item.payloadHex);
    m_descLabel->setText(item.description.isEmpty() ? QStringLiteral("无说明") : item.description);

    const QString key = commandKey(item.protocolName, item.commandName);
    const bool isFavorite = m_favoriteKeys.contains(key);
    m_toggleFavoriteBtn->setText(isFavorite ? QStringLiteral("取消常用")
                                            : QStringLiteral("加入常用"));
    m_toggleFavoriteBtn->setEnabled(true);

    const int byteCount = item.payloadHex.simplified().remove(' ').size() / 2;
    m_statusLabel->setText(QStringLiteral("当前指令长度：%1 字节").arg(byteCount));
    m_sendBtn->setEnabled(true);
}

QString CommandSendDialog::commandKey(const QString &protocolName, const QString &commandName) const
{
    return protocolName + "::" + commandName;
}

QString CommandSendDialog::toSpacedHex(const QByteArray &data) const
{
    const QString hex = QString::fromLatin1(data.toHex().toUpper());
    QString out;
    out.reserve(hex.size() + hex.size() / 2);
    for (int i = 0; i < hex.size(); ++i) {
        if (i > 0 && (i % 2) == 0) {
            out.append(' ');
        }
        out.append(hex.at(i));
    }
    return out;
}

void CommandSendDialog::loadFavoriteKeys()
{
    QSettings settings("GenericScope", "CommandSend");
    const QStringList keys = settings.value("favorites").toStringList();
    m_favoriteKeys = QSet<QString>(keys.begin(), keys.end());
}

void CommandSendDialog::saveFavoriteKeys() const
{
    QSettings settings("GenericScope", "CommandSend");
    QStringList keys = m_favoriteKeys.values();
    keys.sort(Qt::CaseInsensitive);
    settings.setValue("favorites", keys);
}

void CommandSendDialog::loadSendHistory()
{
    QSettings settings("GenericScope", "CommandSend");
    m_sendHistory = settings.value("history").toStringList();

    if (!m_historyCombo) {
        return;
    }

    m_historyCombo->clear();
    for (const QString &entry : qAsConst(m_sendHistory)) {
        const QStringList parts = entry.split('\t');
        if (parts.size() != 3) {
            continue;
        }
        const QString display = QString("[%1] %2 - %3").arg(parts.at(0), parts.at(1), parts.at(2));
        m_historyCombo->addItem(display, entry);
    }
}

void CommandSendDialog::appendSendHistory(const QString &protocolName, const QString &commandName, const QString &payloadHex)
{
    const QString entry = protocolName + '\t' + commandName + '\t' + payloadHex;
    m_sendHistory.removeAll(entry);
    m_sendHistory.prepend(entry);

    const int kMaxHistory = 20;
    while (m_sendHistory.size() > kMaxHistory) {
        m_sendHistory.removeLast();
    }

    QSettings settings("GenericScope", "CommandSend");
    settings.setValue("history", m_sendHistory);
    loadSendHistory();
}

bool CommandSendDialog::sendHexPayload(const QString &payloadHex, QString *errorMsg, int *byteCount)
{
    if (!m_deviceManager) {
        if (errorMsg) *errorMsg = QStringLiteral("设备管理器不可用。");
        return false;
    }
    if (!m_deviceManager->isConnected()) {
        if (errorMsg) *errorMsg = QStringLiteral("设备尚未连接。");
        return false;
    }

    const QString normalizedHex = normalizeHex(payloadHex.trimmed());
    if (!isValidHex(normalizedHex)) {
        if (errorMsg) *errorMsg = QStringLiteral("HEX格式错误。");
        return false;
    }

    const QByteArray commandBytes = QByteArray::fromHex(
        normalizedHex.simplified().remove(' ').toLatin1());
    if (commandBytes.isEmpty()) {
        if (errorMsg) *errorMsg = QStringLiteral("指令内容为空。");
        return false;
    }

    if (!m_deviceManager->sendCommand(commandBytes)) {
        if (errorMsg) *errorMsg = QStringLiteral("指令发送失败，请检查连接和通信配置。");
        return false;
    }
    if (byteCount) {
        *byteCount = commandBytes.size();
    }
    return true;
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
        m_statusLabel->setText(QStringLiteral("未读取到可用指令，请先在指令设置中添加并保存。"));
    } else {
        m_statusLabel->setText(QStringLiteral("已加载 %1 条指令。").arg(m_commands.size()));
    }
}

void CommandSendDialog::onSendClicked()
{
    const QString normalizedHex = normalizeHex(m_hexEdit->text().trimmed());
    m_hexEdit->setText(normalizedHex);

    QString errorMsg;
    int byteCount = 0;
    if (!sendHexPayload(normalizedHex, &errorMsg, &byteCount)) {
        QMessageBox::warning(this, QStringLiteral("发送失败"), errorMsg);
        m_statusLabel->setText(QStringLiteral("发送失败：%1").arg(errorMsg));
        return;
    }

    const QString protocolName = m_protocolCombo->currentText().trimmed();
    const QString commandName = m_commandCombo->currentText().trimmed();
    const QString nameForDisplay = commandName.isEmpty() ? QStringLiteral("自定义指令") : commandName;

    appendSendHistory(protocolName, nameForDisplay, normalizedHex);
    if (m_rxLogEdit) {
        const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
        m_rxLogEdit->appendPlainText(QString("[%1] TX (%2B): %3")
                                     .arg(timestamp)
                                     .arg(byteCount)
                                     .arg(normalizedHex));
    }
    m_statusLabel->setText(QStringLiteral("已发送：%1（%2 字节）").arg(nameForDisplay).arg(byteCount));
}

void CommandSendDialog::onResendClicked()
{
    if (!m_historyCombo || m_historyCombo->count() == 0) {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("暂无历史指令可重发。"));
        return;
    }

    const QString entry = m_historyCombo->currentData().toString();
    const QStringList parts = entry.split('\t');
    if (parts.size() != 3) {
        QMessageBox::warning(this, QStringLiteral("重发失败"), QStringLiteral("历史指令数据无效。"));
        return;
    }

    const QString protocolName = parts.at(0);
    const QString commandName = parts.at(1);
    const QString payloadHex = parts.at(2);

    QString errorMsg;
    int byteCount = 0;
    if (!sendHexPayload(payloadHex, &errorMsg, &byteCount)) {
        QMessageBox::warning(this, QStringLiteral("重发失败"), errorMsg);
        m_statusLabel->setText(QStringLiteral("重发失败：%1").arg(errorMsg));
        return;
    }

    appendSendHistory(protocolName, commandName, payloadHex);
    if (m_rxLogEdit) {
        const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
        m_rxLogEdit->appendPlainText(QString("[%1] TX (%2B): %3")
                                     .arg(timestamp)
                                     .arg(byteCount)
                                     .arg(payloadHex));
    }
    m_statusLabel->setText(QStringLiteral("已重发：%1（%2 字节）").arg(commandName).arg(byteCount));
}

void CommandSendDialog::onToggleFavoriteClicked()
{
    if (!m_commandCombo || !m_commandCombo->isEnabled() || !m_commandCombo->currentData().isValid()) {
        return;
    }

    const int commandIndex = m_commandCombo->currentData().toInt();
    if (commandIndex < 0 || commandIndex >= m_commands.size()) {
        return;
    }

    const CommandItem &item = m_commands.at(commandIndex);
    const QString key = commandKey(item.protocolName, item.commandName);

    if (m_favoriteKeys.contains(key)) {
        m_favoriteKeys.remove(key);
        m_statusLabel->setText(QStringLiteral("已取消常用：%1").arg(item.commandName));
    } else {
        m_favoriteKeys.insert(key);
        m_statusLabel->setText(QStringLiteral("已加入常用：%1").arg(item.commandName));
    }

    saveFavoriteKeys();

    const QString currentCommand = item.commandName;
    rebuildCommandList(currentCommand);
    applyCommandToEditor();
}

void CommandSendDialog::onDeviceDataReceived(const QByteArray &data)
{
    if (!m_rxLogEdit || data.isEmpty()) {
        return;
    }

    const QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    const QString hex = toSpacedHex(data);
    m_rxLogEdit->appendPlainText(QString("[%1] RX (%2B): %3")
                                 .arg(timestamp)
                                 .arg(data.size())
                                 .arg(hex));
}

void CommandSendDialog::onClearRxLogClicked()
{
    if (m_rxLogEdit) {
        m_rxLogEdit->clear();
    }
}

void CommandSendDialog::onOnlyFavoriteToggled(bool checked)
{
    Q_UNUSED(checked);
    rebuildCommandList();
    applyCommandToEditor();
}
