#include "turntablecontroldialog.h"
#include "config/config.h"
#include "device/turntableconnection.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFontDatabase>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QHostAddress>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStyle>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

#include <cstring>

namespace {
constexpr quint8 kControlHold = 0x00;
constexpr quint8 kControlStop = 0x01;
constexpr quint8 kControlPowerOn = 0x02;
constexpr quint8 kControlPowerOff = 0x03;
constexpr quint8 kControlEnable = 0x04;
constexpr quint8 kControlDisable = 0x05;
constexpr quint8 kControlHome = 0x06;

constexpr int kCommandAxisBytes = 13;
constexpr int kStatusAxisBytes = 10;
constexpr int kFrameEnvelopeBytes = 5;
constexpr int kFeedbackFixedBytes = 7;
constexpr int kMaxRxBufferBytes = 4096;
}

TurntableControlDialog::TurntableControlDialog(QWidget *parent)
    : QDialog(parent)
    , m_connection(new TurntableConnection(this))
    , m_waitingStatusRefresh(false)
    , m_remoteIpEdit(nullptr)
    , m_remotePortSpin(nullptr)
    , m_localPortSpin(nullptr)
    , m_connectionStatusLabel(nullptr)
    , m_connectButton(nullptr)
    , m_saveConfigButton(nullptr)
    , m_axisCountSpin(nullptr)
    , m_axisSpin(nullptr)
    , m_allAxesCheck(nullptr)
    , m_modeCombo(nullptr)
    , m_param1Label(nullptr)
    , m_param2Label(nullptr)
    , m_param3Label(nullptr)
    , m_param1Spin(nullptr)
    , m_param2Spin(nullptr)
    , m_param3Spin(nullptr)
    , m_framePreviewEdit(nullptr)
    , m_historyEdit(nullptr)
    , m_statusTable(nullptr)
{
    setupUi();
    loadConfigToUi();
    setupCommands();
    updateParamLabels();
    updateStatusRows();
    updateConnectionUi(false);

    connect(m_connection, &TurntableConnection::dataReceived,
            this, &TurntableControlDialog::onReceivedData);
    connect(m_connection, &TurntableConnection::connectionChanged,
            this, &TurntableControlDialog::onConnectionChanged);
    connect(m_connection, &TurntableConnection::errorOccurred,
            this, &TurntableControlDialog::onConnectionError);
}

void TurntableControlDialog::setupUi()
{
    setObjectName(QStringLiteral("turntableControlDialog"));
    setWindowTitle(QStringLiteral("转台控制"));
    resize(1400, 900);
    setMinimumSize(1280, 820);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);

    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(20);

    auto *leftPanelWidget = new QWidget(this);
    auto *leftPanel = new QVBoxLayout(leftPanelWidget);
    leftPanel->setContentsMargins(0, 0, 0, 0);
    leftPanel->setSpacing(14);

    auto *connectionGroup = new QGroupBox(QStringLiteral("网络连接"), this);
    connectionGroup->setMinimumHeight(248);
    connectionGroup->setMaximumHeight(268);
    auto *connectionLayout = new QGridLayout(connectionGroup);
    connectionLayout->setContentsMargins(16, 22, 16, 14);
    connectionLayout->setHorizontalSpacing(14);
    connectionLayout->setVerticalSpacing(10);

    m_remoteIpEdit = new QLineEdit(this);
    m_remoteIpEdit->setMinimumWidth(220);
    m_remoteIpEdit->setPlaceholderText(QStringLiteral("192.168.1.101"));
    m_remoteIpEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    m_remotePortSpin = new QSpinBox(this);
    m_remotePortSpin->setRange(1, 65535);
    m_remotePortSpin->setFixedWidth(130);
    m_remotePortSpin->setFixedHeight(36);

    m_localPortSpin = new QSpinBox(this);
    m_localPortSpin->setRange(1, 65535);
    m_localPortSpin->setFixedWidth(130);
    m_localPortSpin->setFixedHeight(36);

    m_connectionStatusLabel = new QLabel(this);
    m_connectionStatusLabel->setObjectName(QStringLiteral("connectionStatusLabel"));
    m_connectionStatusLabel->setMinimumHeight(38);

    m_connectButton = new QPushButton(QStringLiteral("连接网口"), this);
    m_connectButton->setObjectName(QStringLiteral("primaryButton"));
    m_connectButton->setMinimumHeight(44);
    m_connectButton->setMinimumWidth(150);
    m_connectButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_saveConfigButton = new QPushButton(QStringLiteral("保存参数"), this);
    m_saveConfigButton->setObjectName(QStringLiteral("secondaryButton"));
    m_saveConfigButton->setMinimumHeight(44);
    m_saveConfigButton->setMinimumWidth(126);
    m_saveConfigButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *connectionActionLayout = new QHBoxLayout();
    connectionActionLayout->setSpacing(10);
    connectionActionLayout->addStretch(1);
    connectionActionLayout->addWidget(m_saveConfigButton, 0, Qt::AlignRight);
    connectionActionLayout->addWidget(m_connectButton, 0, Qt::AlignRight);

    connectionLayout->addWidget(new QLabel(QStringLiteral("目标 IP"), this), 0, 0);
    connectionLayout->addWidget(m_remoteIpEdit, 0, 1, 1, 3);
    connectionLayout->addWidget(new QLabel(QStringLiteral("目标端口"), this), 1, 0);
    connectionLayout->addWidget(m_remotePortSpin, 1, 1);
    connectionLayout->addWidget(new QLabel(QStringLiteral("本地端口"), this), 1, 2);
    connectionLayout->addWidget(m_localPortSpin, 1, 3);
    connectionLayout->addWidget(m_connectionStatusLabel, 2, 0, 1, 4);
    connectionLayout->addLayout(connectionActionLayout, 3, 0, 1, 4);
    connectionLayout->setColumnStretch(1, 1);
    connectionLayout->setColumnStretch(3, 1);

    auto *axisGroup = new QGroupBox(QStringLiteral("轴选择"), this);
    axisGroup->setMinimumHeight(142);
    axisGroup->setMaximumHeight(156);
    auto *axisLayout = new QGridLayout(axisGroup);
    axisLayout->setContentsMargins(16, 24, 16, 16);
    axisLayout->setHorizontalSpacing(16);
    axisLayout->setVerticalSpacing(12);

    m_axisCountSpin = new QSpinBox(this);
    m_axisCountSpin->setRange(1, 16);
    m_axisCountSpin->setValue(Config::instance()->turntable.axisCount);
    m_axisCountSpin->setFixedWidth(120);
    m_axisCountSpin->setFixedHeight(36);

    m_axisSpin = new QSpinBox(this);
    m_axisSpin->setRange(1, 1);
    m_axisSpin->setValue(1);
    m_axisSpin->setFixedWidth(120);
    m_axisSpin->setFixedHeight(36);

    m_allAxesCheck = new QCheckBox(QStringLiteral("全部轴同步下发"), this);
    m_allAxesCheck->setMinimumHeight(32);

    axisLayout->addWidget(new QLabel(QStringLiteral("轴数"), this), 0, 0);
    axisLayout->addWidget(m_axisCountSpin, 0, 1);
    axisLayout->addWidget(new QLabel(QStringLiteral("目标轴"), this), 0, 2);
    axisLayout->addWidget(m_axisSpin, 0, 3);
    axisLayout->addWidget(m_allAxesCheck, 1, 0, 1, 4);
    axisLayout->setColumnStretch(4, 1);

    auto *motionGroup = new QGroupBox(QStringLiteral("运动参数"), this);
    motionGroup->setMinimumHeight(236);
    motionGroup->setMaximumHeight(258);
    auto *motionLayout = new QGridLayout(motionGroup);
    motionLayout->setContentsMargins(16, 24, 16, 16);
    motionLayout->setHorizontalSpacing(16);
    motionLayout->setVerticalSpacing(12);

    m_modeCombo = new QComboBox(this);
    m_modeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_modeCombo->setMinimumHeight(36);
    m_modeCombo->addItem(QStringLiteral("绝对位置"), 0x07);
    m_modeCombo->addItem(QStringLiteral("绝对速率"), 0x08);
    m_modeCombo->addItem(QStringLiteral("位置正弦"), 0x09);
    m_modeCombo->addItem(QStringLiteral("速率正弦"), 0x0A);
    m_modeCombo->addItem(QStringLiteral("相对位置"), 0x0B);
    m_modeCombo->addItem(QStringLiteral("相对速率"), 0x0C);
    m_modeCombo->addItem(QStringLiteral("梯形波运动"), 0x0D);
    m_modeCombo->addItem(QStringLiteral("三角波"), 0x0E);
    m_modeCombo->addItem(QStringLiteral("方波运动"), 0x10);

    m_param1Label = new QLabel(this);
    m_param2Label = new QLabel(this);
    m_param3Label = new QLabel(this);
    for (QLabel *label : {m_param1Label, m_param2Label, m_param3Label}) {
        label->setMinimumWidth(100);
    }

    m_param1Spin = new QDoubleSpinBox(this);
    m_param2Spin = new QDoubleSpinBox(this);
    m_param3Spin = new QDoubleSpinBox(this);
    for (QDoubleSpinBox *spin : {m_param1Spin, m_param2Spin, m_param3Spin}) {
        spin->setRange(-1000000.0, 1000000.0);
        spin->setDecimals(4);
        spin->setSingleStep(1.0);
        spin->setFixedWidth(200);
        spin->setFixedHeight(36);
        spin->setAlignment(Qt::AlignRight);
    }

    auto *modeLabel = new QLabel(QStringLiteral("控制模式"), this);
    modeLabel->setMinimumWidth(100);
    motionLayout->addWidget(modeLabel, 0, 0);
    motionLayout->addWidget(m_modeCombo, 0, 1, 1, 3);
    motionLayout->addWidget(m_param1Label, 1, 0);
    motionLayout->addWidget(m_param1Spin, 1, 1);
    motionLayout->addWidget(m_param2Label, 2, 0);
    motionLayout->addWidget(m_param2Spin, 2, 1);
    motionLayout->addWidget(m_param3Label, 3, 0);
    motionLayout->addWidget(m_param3Spin, 3, 1);
    motionLayout->setColumnStretch(3, 1);

    auto *buttonGroup = new QGroupBox(QStringLiteral("动作执行"), this);
    buttonGroup->setMinimumHeight(156);
    buttonGroup->setMaximumHeight(174);
    auto *buttonLayout = new QGridLayout(buttonGroup);
    buttonLayout->setContentsMargins(16, 24, 16, 16);
    buttonLayout->setHorizontalSpacing(12);
    buttonLayout->setVerticalSpacing(12);

    auto *quickCommandCombo = new QComboBox(this);
    quickCommandCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    quickCommandCombo->setMinimumHeight(36);
    quickCommandCombo->addItem(QStringLiteral("上电"), kControlPowerOn);
    quickCommandCombo->addItem(QStringLiteral("断电"), kControlPowerOff);
    quickCommandCombo->addItem(QStringLiteral("使能"), kControlEnable);
    quickCommandCombo->addItem(QStringLiteral("断使能"), kControlDisable);
    quickCommandCombo->addItem(QStringLiteral("回零"), kControlHome);
    quickCommandCombo->addItem(QStringLiteral("保持"), kControlHold);
    quickCommandCombo->addItem(QStringLiteral("停止"), kControlStop);

    auto *executeQuickButton = new QPushButton(QStringLiteral("执行命令"), this);
    executeQuickButton->setObjectName(QStringLiteral("secondaryButton"));
    auto *sendMotionButton = new QPushButton(QStringLiteral("发送运动"), this);
    sendMotionButton->setObjectName(QStringLiteral("primaryButton"));
    for (QPushButton *button : {executeQuickButton, sendMotionButton}) {
        button->setMinimumHeight(42);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    sendMotionButton->setDefault(true);

    buttonLayout->addWidget(new QLabel(QStringLiteral("快捷命令"), this), 0, 0);
    buttonLayout->addWidget(quickCommandCombo, 0, 1, 1, 2);
    buttonLayout->addWidget(executeQuickButton, 1, 0, 1, 2);
    buttonLayout->addWidget(sendMotionButton, 1, 2);

    leftPanel->addWidget(connectionGroup);
    leftPanel->addWidget(axisGroup);
    leftPanel->addWidget(motionGroup);
    leftPanel->addWidget(buttonGroup);
    leftPanel->setStretch(0, 0);
    leftPanel->setStretch(1, 0);
    leftPanel->setStretch(2, 0);
    leftPanel->setStretch(3, 0);
    leftPanel->addStretch(1);

    auto *leftScrollArea = new QScrollArea(this);
    leftScrollArea->setWidgetResizable(true);
    leftScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    leftScrollArea->setWidget(leftPanelWidget);
    leftScrollArea->setStyleSheet(QStringLiteral("QScrollArea { border: none; background: transparent; }"));

    auto *statusGroup = new QGroupBox(QStringLiteral("转台状态"), this);
    statusGroup->setObjectName(QStringLiteral("statusPanel"));
    auto *statusLayout = new QVBoxLayout(statusGroup);
    statusLayout->setContentsMargins(16, 24, 16, 16);
    statusLayout->setSpacing(12);

    auto *statusCommandLayout = new QHBoxLayout();
    statusCommandLayout->setSpacing(12);
    statusCommandLayout->addStretch(1);

    auto *refreshStatusButton = new QPushButton(QStringLiteral("获取状态"), this);
    refreshStatusButton->setObjectName(QStringLiteral("secondaryButton"));
    refreshStatusButton->setMinimumHeight(42);
    refreshStatusButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    statusCommandLayout->addWidget(refreshStatusButton);

    m_statusTable = new QTableWidget(this);
    m_statusTable->setColumnCount(5);
    m_statusTable->setHorizontalHeaderLabels({
        QStringLiteral("轴"), QStringLiteral("状态"), QStringLiteral("错误码"),
        QStringLiteral("位置"), QStringLiteral("速率")
    });
    m_statusTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_statusTable->horizontalHeader()->setMinimumHeight(42);
    m_statusTable->horizontalHeader()->setHighlightSections(false);
    m_statusTable->verticalHeader()->setVisible(false);
    m_statusTable->verticalHeader()->setDefaultSectionSize(42);
    m_statusTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_statusTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_statusTable->setFocusPolicy(Qt::NoFocus);
    m_statusTable->setAlternatingRowColors(true);
    m_statusTable->setMinimumWidth(720);
    m_statusTable->setShowGrid(false);

    statusLayout->addLayout(statusCommandLayout);
    statusLayout->addWidget(m_statusTable);

    topLayout->addWidget(leftScrollArea, 0);
    topLayout->addWidget(statusGroup, 1);
    topLayout->setStretch(0, 5);
    topLayout->setStretch(1, 8);

    auto *commGroup = new QGroupBox(QStringLiteral("通讯监视"), this);
    commGroup->setMaximumHeight(196);
    auto *commLayout = new QVBoxLayout(commGroup);
    commLayout->setContentsMargins(16, 24, 16, 16);
    commLayout->setSpacing(12);

    auto *frameLayout = new QHBoxLayout();
    frameLayout->setSpacing(12);
    frameLayout->addWidget(new QLabel(QStringLiteral("最近发送帧"), this));
    m_framePreviewEdit = new QLineEdit(this);
    m_framePreviewEdit->setReadOnly(true);
    m_framePreviewEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_framePreviewEdit->setMinimumHeight(36);
    m_framePreviewEdit->setPlaceholderText(QStringLiteral("等待发送控制帧..."));
    frameLayout->addWidget(m_framePreviewEdit, 1);

    m_historyEdit = new QTextEdit(this);
    m_historyEdit->setReadOnly(true);
    m_historyEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    m_historyEdit->setMinimumHeight(82);
    m_historyEdit->setMaximumHeight(96);
    m_historyEdit->setPlaceholderText(QStringLiteral("等待网络连接与转台反馈..."));

    commLayout->addLayout(frameLayout);
    commLayout->addWidget(m_historyEdit);

    mainLayout->addLayout(topLayout, 1);
    mainLayout->addWidget(commGroup, 0);

    connect(m_connectButton, &QPushButton::clicked,
            this, &TurntableControlDialog::onConnectClicked);
    connect(m_saveConfigButton, &QPushButton::clicked,
            this, &TurntableControlDialog::onSaveConfigClicked);
    connect(m_axisCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &TurntableControlDialog::onAxisCountChanged);
    connect(m_axisSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this]() { setupCommands(); });
    connect(m_allAxesCheck, &QCheckBox::toggled,
            this, [this]() { setupCommands(); });
    connect(m_modeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TurntableControlDialog::onModeChanged);
    connect(m_param1Spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this]() { setupCommands(); });
    connect(m_param2Spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this]() { setupCommands(); });
    connect(m_param3Spin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this]() { setupCommands(); });
    connect(sendMotionButton, &QPushButton::clicked,
            this, &TurntableControlDialog::onSendMotionClicked);
    connect(refreshStatusButton, &QPushButton::clicked,
            this, &TurntableControlDialog::onRefreshStatusClicked);
    connect(executeQuickButton, &QPushButton::clicked, this, [this, quickCommandCombo]() {
        const quint8 control = static_cast<quint8>(quickCommandCombo->currentData().toInt());
        sendQuickCommand(control, quickCommandCombo->currentText(), control == kControlStop);
    });
}

void TurntableControlDialog::loadConfigToUi()
{
    Config *cfg = Config::instance();
    m_remoteIpEdit->setText(cfg->turntable.udpRemoteIp);
    m_remotePortSpin->setValue(cfg->turntable.udpRemotePort);
    m_localPortSpin->setValue(cfg->turntable.udpLocalPort);
    m_axisCountSpin->setValue(cfg->turntable.axisCount);
    m_axisSpin->setMaximum(cfg->turntable.axisCount);
}

void TurntableControlDialog::saveUiToConfig()
{
    Config *cfg = Config::instance();
    cfg->turntable.udpRemoteIp = m_remoteIpEdit->text().trimmed();
    cfg->turntable.udpRemotePort = m_remotePortSpin->value();
    cfg->turntable.udpLocalPort = m_localPortSpin->value();
    cfg->turntable.axisCount = m_axisCountSpin->value();
    cfg->save();
}

bool TurntableControlDialog::validateConnectionParams(QString *error) const
{
    QHostAddress address;
    if (!address.setAddress(m_remoteIpEdit->text().trimmed())) {
        if (error) {
            *error = QStringLiteral("请输入有效的远端 IP 地址。");
        }
        return false;
    }

    const Config *cfg = Config::instance();
    if (cfg->device.type.compare(QStringLiteral("UDP"), Qt::CaseInsensitive) == 0
        && cfg->device.udpLocalPort == m_localPortSpin->value()) {
        if (error) {
            *error = QStringLiteral("转台本地端口不能与主设备 UDP 本地端口相同。");
        }
        return false;
    }

    return true;
}

void TurntableControlDialog::updateConnectionUi(bool connected)
{
    m_remoteIpEdit->setEnabled(!connected);
    m_remotePortSpin->setEnabled(!connected);
    m_localPortSpin->setEnabled(!connected);
    m_saveConfigButton->setEnabled(!connected);
    m_connectButton->setText(connected ? QStringLiteral("断开网口") : QStringLiteral("连接网口"));
    m_connectionStatusLabel->setText(connected
        ? QStringLiteral("网口已连接，可直接下发转台命令")
        : QStringLiteral("网口未连接，请先配置地址并连接"));
    m_connectionStatusLabel->setProperty("state", connected ? QStringLiteral("connected") : QStringLiteral("error"));
    m_connectionStatusLabel->style()->unpolish(m_connectionStatusLabel);
    m_connectionStatusLabel->style()->polish(m_connectionStatusLabel);
}

void TurntableControlDialog::setupCommands()
{
    if (!m_framePreviewEdit || !m_modeCombo) {
        return;
    }

    const quint8 control = static_cast<quint8>(m_modeCombo->currentData().toInt());
    const QByteArray frame = buildCommandFrame(buildSelectedAxisCommands(
        control,
        static_cast<float>(m_param1Spin->value()),
        static_cast<float>(m_param2Spin->value()),
        static_cast<float>(m_param3Spin->value())));
    m_framePreviewEdit->setText(hexText(frame));
}

void TurntableControlDialog::onConnectClicked()
{
    if (m_connection->isConnected()) {
        m_connection->close();
        appendHistory(QStringLiteral("[连接] 转台网口已断开"));
        return;
    }

    QString error;
    if (!validateConnectionParams(&error)) {
        QMessageBox::warning(this, QStringLiteral("参数错误"), error);
        return;
    }

    if (!m_connection->open(m_remoteIpEdit->text().trimmed(),
                            m_remotePortSpin->value(),
                            m_localPortSpin->value())) {
        const QString message = m_connection->lastError().isEmpty()
            ? QStringLiteral("转台 UDP 连接失败")
            : m_connection->lastError();
        appendHistory(QStringLiteral("[连接失败] %1").arg(message));
        QMessageBox::warning(this, QStringLiteral("连接失败"), message);
        return;
    }

    appendHistory(QStringLiteral("[连接] 网口已连接 %1:%2，本地端口=%3")
                      .arg(m_remoteIpEdit->text().trimmed())
                      .arg(m_remotePortSpin->value())
                      .arg(m_localPortSpin->value()));
}

void TurntableControlDialog::onSaveConfigClicked()
{
    QString error;
    if (!validateConnectionParams(&error)) {
        QMessageBox::warning(this, QStringLiteral("参数错误"), error);
        return;
    }

    saveUiToConfig();
    appendHistory(QStringLiteral("[配置] 转台 UDP 配置已保存"));
}

void TurntableControlDialog::onAxisCountChanged(int axisCount)
{
    m_axisSpin->setMaximum(axisCount);
    Config::instance()->turntable.axisCount = axisCount;
    updateStatusRows();
    setupCommands();
}

void TurntableControlDialog::onModeChanged()
{
    updateParamLabels();
    setupCommands();
}

void TurntableControlDialog::onSendMotionClicked()
{
    const quint8 control = static_cast<quint8>(m_modeCombo->currentData().toInt());
    const QVector<AxisCommand> commands = buildSelectedAxisCommands(
        control,
        static_cast<float>(m_param1Spin->value()),
        static_cast<float>(m_param2Spin->value()),
        static_cast<float>(m_param3Spin->value()));

    sendFrame(m_modeCombo->currentText(), buildCommandFrame(commands));
}

void TurntableControlDialog::onRefreshStatusClicked()
{
    if (!m_connection->isConnected()) {
        appendHistory(QStringLiteral("[状态] 转台 UDP 未连接，无法获取当前状态"));
        QMessageBox::warning(this, QStringLiteral("设备未连接"),
                             QStringLiteral("请先连接转台 UDP。"));
        return;
    }

    m_waitingStatusRefresh = true;
    appendHistory(QStringLiteral("[状态] 等待下一帧状态广播 (ZTD)..."));

    QTimer::singleShot(1500, this, [this]() {
        if (!m_waitingStatusRefresh) {
            return;
        }

        m_waitingStatusRefresh = false;
        appendHistory(QStringLiteral("[状态] 超时：未收到状态广播帧"));
    });
}

void TurntableControlDialog::sendQuickCommand(quint8 control, const QString &name, bool useStopAcceleration)
{
    const float stopAcceleration = useStopAcceleration ? static_cast<float>(m_param2Spin->value()) : 0.0f;
    sendFrame(name, buildCommandFrame(buildSelectedAxisCommands(control, 0.0f, stopAcceleration, 0.0f)));
}

void TurntableControlDialog::sendFrame(const QString &name, const QByteArray &frame)
{
    m_framePreviewEdit->setText(hexText(frame));

    if (!m_connection->isConnected()) {
        appendHistory(QStringLiteral("[未发送] %1: 转台 UDP 未连接，帧=%2").arg(name, hexText(frame)));
        QMessageBox::warning(this, QStringLiteral("设备未连接"),
                             QStringLiteral("请先连接转台 UDP。"));
        return;
    }

    if (!m_connection->sendFrame(frame)) {
        const QString message = m_connection->lastError().isEmpty()
            ? QStringLiteral("转台 UDP 发送失败")
            : m_connection->lastError();
        appendHistory(QStringLiteral("[发送失败] %1: %2").arg(name, message));
        QMessageBox::warning(this, QStringLiteral("发送失败"), message);
        return;
    }

    appendHistory(QStringLiteral("[发送] %1: %2").arg(name, hexText(frame)));
}

QByteArray TurntableControlDialog::buildCommandFrame(const QVector<AxisCommand> &commands) const
{
    QByteArray frame;
    frame.reserve(commands.size() * kCommandAxisBytes + kFrameEnvelopeBytes);
    frame.append("ZT", 2);

    for (const AxisCommand &command : commands) {
        frame.append(static_cast<char>(command.control));
        appendFloatLE(&frame, command.param1);
        appendFloatLE(&frame, command.param2);
        appendFloatLE(&frame, command.param3);
    }

    frame.append(static_cast<char>(checksum(frame, 2, frame.size() - 2)));
    frame.append("ZW", 2);
    return frame;
}

QVector<TurntableControlDialog::AxisCommand> TurntableControlDialog::buildSelectedAxisCommands(
    quint8 control, float p1, float p2, float p3) const
{
    QVector<AxisCommand> commands(m_axisCountSpin->value());
    const int selectedAxis = m_axisSpin->value() - 1;

    for (int i = 0; i < commands.size(); ++i) {
        if (m_allAxesCheck->isChecked() || i == selectedAxis) {
            commands[i].control = control;
            commands[i].param1 = p1;
            commands[i].param2 = p2;
            commands[i].param3 = p3;
        }
    }

    return commands;
}

void TurntableControlDialog::appendFloatLE(QByteArray *frame, float value) const
{
    quint32 raw = 0;
    static_assert(sizeof(raw) == sizeof(value), "float must be 32-bit");
    std::memcpy(&raw, &value, sizeof(value));

    frame->append(static_cast<char>(raw & 0xFF));
    frame->append(static_cast<char>((raw >> 8) & 0xFF));
    frame->append(static_cast<char>((raw >> 16) & 0xFF));
    frame->append(static_cast<char>((raw >> 24) & 0xFF));
}

quint8 TurntableControlDialog::checksum(const QByteArray &frame, int start, int count) const
{
    quint8 sum = 0;
    for (int i = 0; i < count && start + i < frame.size(); ++i) {
        sum = static_cast<quint8>(sum + static_cast<quint8>(frame.at(start + i)));
    }
    return sum;
}

QString TurntableControlDialog::hexText(const QByteArray &frame) const
{
    return QString::fromLatin1(frame.toHex(' ').toUpper());
}

void TurntableControlDialog::appendHistory(const QString &message)
{
    const QString time = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz"));
    m_historyEdit->append(QStringLiteral("[%1] %2").arg(time, message));
}

void TurntableControlDialog::updateParamLabels()
{
    const int control = m_modeCombo->currentData().toInt();

    QString p1 = QStringLiteral("参数1");
    QString p2 = QStringLiteral("参数2");
    QString p3 = QStringLiteral("参数3");

    if (control == 0x07 || control == 0x0B) {
        p1 = QStringLiteral("目标位置");
        p2 = QStringLiteral("目标速率");
        p3 = QStringLiteral("目标加速度");
    } else if (control == 0x08 || control == 0x0C) {
        p1 = QStringLiteral("目标速率");
        p2 = QStringLiteral("目标加速度");
        p3 = QStringLiteral("保留");
    } else if (control == 0x09 || control == 0x0A) {
        p1 = QStringLiteral("幅值");
        p2 = QStringLiteral("频率");
        p3 = QStringLiteral("相位");
    } else if (control == 0x0D) {
        p1 = QStringLiteral("幅值");
        p2 = QStringLiteral("频率");
        p3 = QStringLiteral("占空比");
    } else if (control == 0x0E || control == 0x10) {
        p1 = QStringLiteral("幅值");
        p2 = QStringLiteral("频率");
        p3 = QStringLiteral("保留");
    }

    m_param1Label->setText(p1);
    m_param2Label->setText(p2);
    m_param3Label->setText(p3);
}

void TurntableControlDialog::updateStatusRows()
{
    const int axisCount = m_axisCountSpin->value();
    m_statusTable->setRowCount(axisCount);

    for (int row = 0; row < axisCount; ++row) {
        if (!m_statusTable->item(row, 0)) {
            m_statusTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
            m_statusTable->setItem(row, 1, new QTableWidgetItem(QStringLiteral("-")));
            m_statusTable->setItem(row, 2, new QTableWidgetItem(QStringLiteral("-")));
            m_statusTable->setItem(row, 3, new QTableWidgetItem(QStringLiteral("-")));
            m_statusTable->setItem(row, 4, new QTableWidgetItem(QStringLiteral("-")));
        } else {
            m_statusTable->item(row, 0)->setText(QString::number(row + 1));
        }
    }
}

void TurntableControlDialog::onConnectionChanged(bool connected)
{
    if (!connected) {
        m_waitingStatusRefresh = false;
    }
    updateConnectionUi(connected);
}

void TurntableControlDialog::onConnectionError(const QString &error)
{
    if (!error.isEmpty()) {
        appendHistory(QStringLiteral("[连接异常] %1").arg(error));
    }
}

void TurntableControlDialog::onReceivedData(const QByteArray &data)
{
    if (data.isEmpty()) {
        return;
    }

    m_rxBuffer.append(data);
    if (m_rxBuffer.size() > kMaxRxBufferBytes) {
        m_rxBuffer = m_rxBuffer.right(kMaxRxBufferBytes / 2);
        appendHistory(QStringLiteral("[接收] RX 缓冲区过大，已裁剪"));
    }
    processReceiveBuffer();
}

void TurntableControlDialog::processReceiveBuffer()
{
    while (m_rxBuffer.size() >= 6) {
        const int header = m_rxBuffer.indexOf("ZT");
        if (header < 0) {
            m_rxBuffer.clear();
            return;
        }
        if (header > 0) {
            m_rxBuffer.remove(0, header);
        }

        if (m_rxBuffer.size() < 3) {
            return;
        }

        const char type = m_rxBuffer.at(2);
        if (type == 'D') {
            const int length = statusFrameLength();
            if (m_rxBuffer.size() < length) {
                return;
            }
            if (!tryParseFrame(length)) {
                m_rxBuffer.remove(0, 1);
            }
            continue;
        }

        if (type == 'B') {
            const int compactLength = compactAckFrameLength();
            if (m_rxBuffer.size() >= compactLength && tryParseFrame(compactLength)) {
                continue;
            }

            const int paddedLength = paddedAckFrameLength();
            if (m_rxBuffer.size() < paddedLength) {
                return;
            }
            if (!tryParseFrame(paddedLength)) {
                m_rxBuffer.remove(0, 1);
            }
            continue;
        }

        m_rxBuffer.remove(0, 1);
    }
}

bool TurntableControlDialog::tryParseFrame(int length)
{
    if (length <= 0 || m_rxBuffer.size() < length) {
        return false;
    }

    const QByteArray frame = m_rxBuffer.left(length);
    if (!verifyFrame(frame)) {
        return false;
    }

    const char type = frame.at(2);
    if (type == 'D') {
        handleStatusFrame(frame);
    } else if (type == 'B') {
        handleAckFrame(frame);
    } else {
        appendHistory(QStringLiteral("[接收] 未知反馈类型 0x%1: %2")
                          .arg(static_cast<int>(static_cast<quint8>(type)), 2, 16, QLatin1Char('0'))
                          .arg(hexText(frame)));
    }

    m_rxBuffer.remove(0, length);
    return true;
}

bool TurntableControlDialog::verifyFrame(const QByteArray &frame) const
{
    if (frame.size() < 6 || !frame.startsWith("ZT") || !frame.endsWith("ZW")) {
        return false;
    }

    const quint8 expected = checksum(frame, 2, frame.size() - 5);
    const quint8 actual = static_cast<quint8>(frame.at(frame.size() - 3));
    return expected == actual;
}

int TurntableControlDialog::statusFrameLength() const
{
    return kFeedbackFixedBytes + m_axisCountSpin->value() * kStatusAxisBytes;
}

int TurntableControlDialog::paddedAckFrameLength() const
{
    return statusFrameLength();
}

int TurntableControlDialog::compactAckFrameLength() const
{
    return kFeedbackFixedBytes + m_axisCountSpin->value();
}

void TurntableControlDialog::handleStatusFrame(const QByteArray &frame)
{
    const quint8 deviceStatus = static_cast<quint8>(frame.at(3));
    appendHistory(QStringLiteral("[状态] 设备=%1，原始=%2")
                      .arg(describeDeviceStatus(deviceStatus), hexText(frame)));

    int offset = 4;
    for (int axis = 0; axis < m_axisCountSpin->value(); ++axis) {
        if (offset + kStatusAxisBytes > frame.size() - 3) {
            appendHistory(QStringLiteral("[状态] 第 %1 轴数据长度不足").arg(axis + 1));
            return;
        }

        const quint8 state = static_cast<quint8>(frame.at(offset));
        const quint8 error = static_cast<quint8>(frame.at(offset + 1));
        const float position = readFloatLE(frame, offset + 2);
        const float speed = readFloatLE(frame, offset + 6);

        m_statusTable->item(axis, 1)->setText(describeAxisState(state));
        m_statusTable->item(axis, 2)->setText(QStringLiteral("0x%1")
                                                  .arg(static_cast<int>(error), 2, 16, QLatin1Char('0'))
                                                  .toUpper());
        m_statusTable->item(axis, 3)->setText(QString::number(position, 'f', 4));
        m_statusTable->item(axis, 4)->setText(QString::number(speed, 'f', 4));
        offset += kStatusAxisBytes;
    }

    if (m_waitingStatusRefresh) {
        m_waitingStatusRefresh = false;
        appendHistory(QStringLiteral("[状态] 当前状态已刷新"));
    }
}

void TurntableControlDialog::handleAckFrame(const QByteArray &frame)
{
    const quint8 deviceAck = static_cast<quint8>(frame.at(3));
    QStringList axisMessages;
    for (int axis = 0; axis < m_axisCountSpin->value(); ++axis) {
        const int offset = 4 + axis;
        if (offset >= frame.size() - 3) {
            axisMessages << QStringLiteral("轴%1=缺失").arg(axis + 1);
            continue;
        }
        const quint8 status = static_cast<quint8>(frame.at(offset));
        axisMessages << QStringLiteral("轴%1=%2").arg(axis + 1).arg(describeAckStatus(status));
    }

    appendHistory(QStringLiteral("[反馈] 设备=%1，%2，原始=%3")
                      .arg(deviceAck == 0 ? QStringLiteral("命令正确") : QStringLiteral("命令异常"),
                           axisMessages.join(QStringLiteral("；")),
                           hexText(frame)));
}

QString TurntableControlDialog::describeDeviceStatus(quint8 status) const
{
    switch (status) {
    case 0x00: return QStringLiteral("初始未上电");
    case 0x01: return QStringLiteral("上电");
    case 0x02: return QStringLiteral("急停");
    case 0x03: return QStringLiteral("轴状态异常");
    default:
        return QStringLiteral("0x%1")
            .arg(static_cast<int>(status), 2, 16, QLatin1Char('0'))
            .toUpper();
    }
}

QString TurntableControlDialog::describeAxisState(quint8 state) const
{
    QStringList flags;
    flags << ((state & 0x01) ? QStringLiteral("上电") : QStringLiteral("断电"));
    flags << ((state & 0x02) ? QStringLiteral("使能") : QStringLiteral("断使能"));
    flags << ((state & 0x04) ? QStringLiteral("运行") : QStringLiteral("停止"));
    flags << ((state & 0x08) ? QStringLiteral("已回零") : QStringLiteral("未回零"));
    if (state & 0x10) {
        flags << QStringLiteral("到位");
    }
    if (state & 0x20) {
        flags << QStringLiteral("到速");
    }
    if (state & 0x40) {
        flags << QStringLiteral("振动停止");
    }
    return flags.join(QStringLiteral("/"));
}

QString TurntableControlDialog::describeAckStatus(quint8 status) const
{
    switch (status) {
    case 0x00: return QStringLiteral("命令正确");
    case 0x01: return QStringLiteral("未打开锁框");
    case 0x02: return QStringLiteral("已上电，请勿重复上电");
    case 0x03: return QStringLiteral("已使能，断电前请先停止运行");
    case 0x04: return QStringLiteral("运行中，关闭使能前请先停止运行");
    case 0x05: return QStringLiteral("未上电，操作前请先上电");
    case 0x06: return QStringLiteral("未使能，运行前请先上电并使能");
    case 0x07: return QStringLiteral("运行中，请先停止运行");
    case 0x08: return QStringLiteral("停止操作错误，该轴未运行");
    default:
        return QStringLiteral("0x%1")
            .arg(static_cast<int>(status), 2, 16, QLatin1Char('0'))
            .toUpper();
    }
}

float TurntableControlDialog::readFloatLE(const QByteArray &data, int offset) const
{
    quint32 raw = 0;
    raw |= static_cast<quint8>(data.at(offset));
    raw |= static_cast<quint32>(static_cast<quint8>(data.at(offset + 1))) << 8;
    raw |= static_cast<quint32>(static_cast<quint8>(data.at(offset + 2))) << 16;
    raw |= static_cast<quint32>(static_cast<quint8>(data.at(offset + 3))) << 24;

    float value = 0.0f;
    std::memcpy(&value, &raw, sizeof(value));
    return value;
}
