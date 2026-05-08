#include "ovencontroldialog.h"
#include "config/config.h"
#include "device/ovenconnection.h"

#include <QComboBox>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <QStyle>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

namespace {
constexpr int kStableSampleCount = 3;
constexpr double kStableDeltaThreshold = 0.3;
}

OvenControlDialog::OvenControlDialog(QWidget *parent)
    : QDialog(parent)
    , m_connection(new OvenConnection(this))
    , m_pollTimer(new QTimer(this))
    , m_waitingForStable(false)
    , m_lastTemperature(0.0)
    , m_portCombo(nullptr)
    , m_deviceIdSpin(nullptr)
    , m_connectionStatusLabel(nullptr)
    , m_connectButton(nullptr)
    , m_saveConfigButton(nullptr)
    , m_targetTempSpin(nullptr)
    , m_currentTempEdit(nullptr)
    , m_currentStatusLabel(nullptr)
    , m_actionCombo(nullptr)
    , m_executeButton(nullptr)
    , m_refreshStatusButton(nullptr)
    , m_lastCommandEdit(nullptr)
    , m_historyEdit(nullptr)
{
    setupUi();
    refreshAvailablePorts();
    loadConfigToUi();
    updateConnectionUi(false);

    connect(m_connection, &OvenConnection::dataReceived,
            this, &OvenControlDialog::onReceivedData);
    connect(m_connection, &OvenConnection::connectionChanged,
            this, &OvenControlDialog::onConnectionChanged);
    connect(m_connection, &OvenConnection::errorOccurred,
            this, &OvenControlDialog::onConnectionError);
    connect(m_pollTimer, &QTimer::timeout,
            this, &OvenControlDialog::onPollTimeout);
}

void OvenControlDialog::setupUi()
{
    setObjectName(QStringLiteral("ovenControlDialog"));
    setWindowTitle(QStringLiteral("烘箱控制"));
    resize(1160, 760);
    setMinimumSize(1040, 680);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(16);

    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(18);

    auto *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(14);

    auto *connectionGroup = new QGroupBox(QStringLiteral("串口连接"), this);
    connectionGroup->setMinimumHeight(150);
    auto *connectionLayout = new QGridLayout(connectionGroup);
    connectionLayout->setContentsMargins(18, 24, 18, 16);
    connectionLayout->setHorizontalSpacing(14);
    connectionLayout->setVerticalSpacing(10);

    m_portCombo = new QComboBox(this);
    m_portCombo->setMinimumHeight(38);

    m_deviceIdSpin = new QSpinBox(this);
    m_deviceIdSpin->setRange(1, 255);
    m_deviceIdSpin->setMinimumHeight(38);

    m_connectionStatusLabel = new QLabel(this);
    m_connectionStatusLabel->setObjectName(QStringLiteral("connectionStatusLabel"));
    m_connectionStatusLabel->setMinimumHeight(42);
    m_connectionStatusLabel->setAlignment(Qt::AlignCenter);

    m_saveConfigButton = new QPushButton(QStringLiteral("保存参数"), this);
    m_saveConfigButton->setObjectName(QStringLiteral("secondaryButton"));
    m_saveConfigButton->setMinimumHeight(42);
    m_saveConfigButton->setMinimumWidth(130);

    m_connectButton = new QPushButton(QStringLiteral("连接串口"), this);
    m_connectButton->setMinimumHeight(42);
    m_connectButton->setMinimumWidth(146);

    auto *connectionActionLayout = new QHBoxLayout();
    connectionActionLayout->setSpacing(10);
    connectionActionLayout->addWidget(m_connectionStatusLabel, 1);
    connectionActionLayout->addWidget(m_saveConfigButton, 0, Qt::AlignRight);
    connectionActionLayout->addWidget(m_connectButton, 0, Qt::AlignRight);

    connectionLayout->addWidget(new QLabel(QStringLiteral("烘箱串口号"), this), 0, 0);
    connectionLayout->addWidget(m_portCombo, 0, 1);
    connectionLayout->addWidget(new QLabel(QStringLiteral("设备编号"), this), 0, 2);
    connectionLayout->addWidget(m_deviceIdSpin, 0, 3);
    connectionLayout->addLayout(connectionActionLayout, 1, 0, 1, 4);
    connectionLayout->setColumnStretch(1, 1);
    connectionLayout->setColumnStretch(3, 1);

    auto *paramGroup = new QGroupBox(QStringLiteral("温控状态"), this);
    paramGroup->setMinimumHeight(206);
    auto *paramLayout = new QGridLayout(paramGroup);
    paramLayout->setContentsMargins(18, 24, 18, 16);
    paramLayout->setHorizontalSpacing(14);
    paramLayout->setVerticalSpacing(12);

    m_targetTempSpin = new QDoubleSpinBox(this);
    m_targetTempSpin->setRange(-50.0, 300.0);
    m_targetTempSpin->setDecimals(1);
    m_targetTempSpin->setSingleStep(0.1);
    m_targetTempSpin->setSuffix(QStringLiteral(" °C"));
    m_targetTempSpin->setMinimumHeight(38);

    m_currentTempEdit = new QLineEdit(this);
    m_currentTempEdit->setReadOnly(true);
    m_currentTempEdit->setMinimumHeight(38);
    m_currentTempEdit->setText(QStringLiteral("0.0"));

    m_currentStatusLabel = new QLabel(this);
    m_currentStatusLabel->setObjectName(QStringLiteral("currentStatusLabel"));
    m_currentStatusLabel->setMinimumHeight(48);
    m_currentStatusLabel->setAlignment(Qt::AlignCenter);

    paramLayout->addWidget(new QLabel(QStringLiteral("目标温度(°C)"), this), 0, 0);
    paramLayout->addWidget(m_targetTempSpin, 0, 1);
    paramLayout->addWidget(new QLabel(QStringLiteral("当前温度(°C)"), this), 1, 0);
    paramLayout->addWidget(m_currentTempEdit, 1, 1);
    paramLayout->addWidget(new QLabel(QStringLiteral("当前状态"), this), 2, 0);
    paramLayout->addWidget(m_currentStatusLabel, 2, 1);
    paramLayout->setColumnStretch(1, 1);

    auto *actionGroup = new QGroupBox(QStringLiteral("动作执行"), this);
    actionGroup->setMinimumHeight(154);
    auto *actionLayout = new QGridLayout(actionGroup);
    actionLayout->setContentsMargins(18, 24, 18, 16);
    actionLayout->setHorizontalSpacing(14);
    actionLayout->setVerticalSpacing(12);

    m_actionCombo = new QComboBox(this);
    m_actionCombo->setMinimumHeight(38);
    m_actionCombo->addItem(QStringLiteral("设置目标温度"));
    m_actionCombo->addItem(QStringLiteral("读取当前温度"));
    m_actionCombo->addItem(QStringLiteral("等待稳定"));
    m_actionCombo->addItem(QStringLiteral("关闭烘箱"));

    m_executeButton = new QPushButton(QStringLiteral("执行动作"), this);
    m_executeButton->setMinimumHeight(44);

    m_refreshStatusButton = new QPushButton(QStringLiteral("刷新状态"), this);
    m_refreshStatusButton->setObjectName(QStringLiteral("secondaryButton"));
    m_refreshStatusButton->setMinimumHeight(44);

    actionLayout->addWidget(new QLabel(QStringLiteral("执行"), this), 0, 0);
    actionLayout->addWidget(m_actionCombo, 0, 1, 1, 2);
    actionLayout->addWidget(m_refreshStatusButton, 1, 1);
    actionLayout->addWidget(m_executeButton, 1, 2);
    actionLayout->setColumnStretch(1, 1);
    actionLayout->setColumnStretch(2, 1);

    leftLayout->addWidget(connectionGroup);
    leftLayout->addWidget(paramGroup);
    leftLayout->addWidget(actionGroup);
    leftLayout->addStretch(1);

    auto *rightGroup = new QGroupBox(QStringLiteral("通讯监视"), this);
    rightGroup->setObjectName(QStringLiteral("monitorPanel"));
    auto *rightLayout = new QVBoxLayout(rightGroup);
    rightLayout->setContentsMargins(18, 24, 18, 16);
    rightLayout->setSpacing(12);

    auto *lastCommandLayout = new QHBoxLayout();
    lastCommandLayout->setSpacing(10);
    lastCommandLayout->addWidget(new QLabel(QStringLiteral("最近发送命令"), this));

    m_lastCommandEdit = new QLineEdit(this);
    m_lastCommandEdit->setReadOnly(true);
    m_lastCommandEdit->setMinimumHeight(36);
    lastCommandLayout->addWidget(m_lastCommandEdit, 1);

    m_historyEdit = new QTextEdit(this);
    m_historyEdit->setReadOnly(true);
    m_historyEdit->setPlaceholderText(QStringLiteral("等待串口连接与烘箱反馈..."));

    rightLayout->addLayout(lastCommandLayout);
    rightLayout->addWidget(m_historyEdit, 1);

    topLayout->addLayout(leftLayout, 0);
    topLayout->addWidget(rightGroup, 1);
    topLayout->setStretch(0, 5);
    topLayout->setStretch(1, 6);

    mainLayout->addLayout(topLayout, 1);

    connect(m_connectButton, &QPushButton::clicked,
            this, &OvenControlDialog::onConnectClicked);
    connect(m_saveConfigButton, &QPushButton::clicked,
            this, &OvenControlDialog::onSaveConfigClicked);
    connect(m_executeButton, &QPushButton::clicked,
            this, &OvenControlDialog::onExecuteClicked);
    connect(m_refreshStatusButton, &QPushButton::clicked,
            this, &OvenControlDialog::onRefreshStatusClicked);
}

void OvenControlDialog::refreshAvailablePorts()
{
    const QString current = m_portCombo->currentText();
    m_portCombo->clear();

    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        m_portCombo->addItem(port.portName());
    }

    if (m_portCombo->count() == 0) {
        for (int i = 1; i <= 10; ++i) {
            m_portCombo->addItem(QStringLiteral("COM%1").arg(i));
        }
    }

    const int index = m_portCombo->findText(current);
    if (index >= 0) {
        m_portCombo->setCurrentIndex(index);
    }
}

void OvenControlDialog::loadConfigToUi()
{
    Config *cfg = Config::instance();
    const int portIndex = m_portCombo->findText(cfg->oven.port);
    if (portIndex >= 0) {
        m_portCombo->setCurrentIndex(portIndex);
    } else if (!cfg->oven.port.isEmpty()) {
        m_portCombo->addItem(cfg->oven.port);
        m_portCombo->setCurrentText(cfg->oven.port);
    }
    m_deviceIdSpin->setValue(cfg->oven.deviceId);
    m_targetTempSpin->setValue(cfg->oven.targetTemperature);
    updateStatusDisplay(0.0, QStringLiteral("Waiting"));
    m_currentStatusLabel->setProperty("state", QStringLiteral("off"));
    m_currentStatusLabel->style()->unpolish(m_currentStatusLabel);
    m_currentStatusLabel->style()->polish(m_currentStatusLabel);
    appendHistory(QStringLiteral("[初始化] 烘箱控制模块已就绪"));
}

void OvenControlDialog::saveUiToConfig()
{
    Config *cfg = Config::instance();
    cfg->oven.port = m_portCombo->currentText();
    cfg->oven.baudRate = 19200;
    cfg->oven.dataBits = 8;
    cfg->oven.stopBits = 1;
    cfg->oven.parity = QStringLiteral("None");
    cfg->oven.timeout = 1000;
    cfg->oven.deviceId = m_deviceIdSpin->value();
    cfg->oven.targetTemperature = m_targetTempSpin->value();
    cfg->save();
}

bool OvenControlDialog::validateConnectionParams(QString *error) const
{
    const Config *cfg = Config::instance();
    if (m_portCombo->currentText().trimmed().isEmpty()) {
        if (error) {
            *error = QStringLiteral("请选择烘箱串口号。");
        }
        return false;
    }

    if (cfg->device.type.compare(QStringLiteral("UART"), Qt::CaseInsensitive) == 0
        && cfg->device.port == m_portCombo->currentText()) {
        if (error) {
            *error = QStringLiteral("烘箱串口不能与主设备串口相同。");
        }
        return false;
    }

    return true;
}

void OvenControlDialog::updateConnectionUi(bool connected)
{
    m_portCombo->setEnabled(!connected);
    m_deviceIdSpin->setEnabled(!connected);
    m_saveConfigButton->setEnabled(!connected);
    m_connectButton->setText(connected ? QStringLiteral("断开串口") : QStringLiteral("连接串口"));
    m_connectionStatusLabel->setText(connected
        ? QStringLiteral("串口已连接，可执行烘箱动作")
        : QStringLiteral("串口未连接，请先选择串口并连接"));
    m_connectionStatusLabel->setProperty("state", connected ? QStringLiteral("connected") : QStringLiteral("error"));
    m_connectionStatusLabel->style()->unpolish(m_connectionStatusLabel);
    m_connectionStatusLabel->style()->polish(m_connectionStatusLabel);

    if (!connected) {
        m_currentStatusLabel->setProperty("state", QStringLiteral("off"));
        m_currentStatusLabel->style()->unpolish(m_currentStatusLabel);
        m_currentStatusLabel->style()->polish(m_currentStatusLabel);
    }
}

void OvenControlDialog::appendHistory(const QString &message)
{
    const QString time = QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss.zzz"));
    m_historyEdit->append(QStringLiteral("[%1] %2").arg(time, message));
}

void OvenControlDialog::sendCommand(const QString &name, const QByteArray &command)
{
    if (!m_connection->isConnected()) {
        QMessageBox::warning(this, QStringLiteral("串口未连接"), QStringLiteral("请先连接烘箱串口。"));
        return;
    }

    m_lastCommandEdit->setText(QString::fromLatin1(command));
    if (!m_connection->sendCommand(command)) {
        const QString message = m_connection->lastError().isEmpty()
            ? QStringLiteral("烘箱串口发送失败")
            : m_connection->lastError();
        appendHistory(QStringLiteral("[发送失败] %1: %2").arg(name, message));
        QMessageBox::warning(this, QStringLiteral("发送失败"), message);
        return;
    }

    appendHistory(QStringLiteral("[发送] %1: %2").arg(name, QString::fromLatin1(command)));
}

QByteArray OvenControlDialog::buildReadTemperatureCommand(int deviceId) const
{
    return QStringLiteral("%1.TEMP?\r\n").arg(deviceId, 2, 10, QLatin1Char('0')).toLatin1();
}

QByteArray OvenControlDialog::buildSetConstantModeCommand(int deviceId) const
{
    return QStringLiteral("%1.MODE.CONSTANT\r\n").arg(deviceId, 2, 10, QLatin1Char('0')).toLatin1();
}

QByteArray OvenControlDialog::buildSetTargetTemperatureCommand(int deviceId, double targetTemp) const
{
    return QStringLiteral("%1.TEMP%2\r\n")
        .arg(deviceId, 2, 10, QLatin1Char('0'))
        .arg(targetTemp, 0, 'f', 1)
        .toLatin1();
}

QByteArray OvenControlDialog::buildPowerOffCommand(int deviceId) const
{
    return QStringLiteral("%1.POWER_OFF\r\n").arg(deviceId, 2, 10, QLatin1Char('0')).toLatin1();
}

void OvenControlDialog::executeSetTargetTemperature()
{
    const int deviceId = m_deviceIdSpin->value();
    sendCommand(QStringLiteral("切换恒温模式"), buildSetConstantModeCommand(deviceId));
    sendCommand(QStringLiteral("设置目标温度"), buildSetTargetTemperatureCommand(deviceId, m_targetTempSpin->value()));
    updateStatusDisplay(m_lastTemperature, QStringLiteral("加热中"));
    m_currentStatusLabel->setProperty("state", QStringLiteral("running"));
    m_currentStatusLabel->style()->unpolish(m_currentStatusLabel);
    m_currentStatusLabel->style()->polish(m_currentStatusLabel);
}

void OvenControlDialog::executeReadCurrentTemperature()
{
    sendCommand(QStringLiteral("读取当前温度"), buildReadTemperatureCommand(m_deviceIdSpin->value()));
}

void OvenControlDialog::executeWaitForStable()
{
    m_waitingForStable = true;
    m_recentTemperatures.clear();
    updateStatusDisplay(m_lastTemperature, QStringLiteral("等待稳定"));
    m_currentStatusLabel->setProperty("state", QStringLiteral("warning"));
    m_currentStatusLabel->style()->unpolish(m_currentStatusLabel);
    m_currentStatusLabel->style()->polish(m_currentStatusLabel);
    if (!m_pollTimer->isActive()) {
        m_pollTimer->start(Config::instance()->oven.pollingIntervalMs);
    }
    executeReadCurrentTemperature();
}

void OvenControlDialog::executePowerOff()
{
    m_waitingForStable = false;
    sendCommand(QStringLiteral("关闭烘箱"), buildPowerOffCommand(m_deviceIdSpin->value()));
    updateStatusDisplay(m_lastTemperature, QStringLiteral("已关闭"));
    m_currentStatusLabel->setProperty("state", QStringLiteral("off"));
    m_currentStatusLabel->style()->unpolish(m_currentStatusLabel);
    m_currentStatusLabel->style()->polish(m_currentStatusLabel);
}

void OvenControlDialog::processReceiveBuffer()
{
    while (tryParseLine()) {
    }
}

bool OvenControlDialog::tryParseLine()
{
    int end = m_rxBuffer.indexOf('\n');
    if (end < 0) {
        end = m_rxBuffer.indexOf('\r');
    }
    if (end < 0) {
        return false;
    }

    QByteArray line = m_rxBuffer.left(end).trimmed();
    m_rxBuffer.remove(0, end + 1);
    if (line.isEmpty()) {
        return true;
    }

    double temperature = 0.0;
    if (parseTemperatureResponse(line, &temperature)) {
        m_lastTemperature = temperature;
        QString statusText = m_waitingForStable ? QStringLiteral("等待稳定") : QStringLiteral("运行中");
        m_recentTemperatures.append(temperature);
        while (m_recentTemperatures.size() > kStableSampleCount) {
            m_recentTemperatures.remove(0);
        }

        if (m_waitingForStable && m_recentTemperatures.size() == kStableSampleCount) {
            double minTemp = m_recentTemperatures.first();
            double maxTemp = m_recentTemperatures.first();
            for (double value : m_recentTemperatures) {
                minTemp = qMin(minTemp, value);
                maxTemp = qMax(maxTemp, value);
            }
            if ((maxTemp - minTemp) <= kStableDeltaThreshold) {
                m_waitingForStable = false;
                statusText = QStringLiteral("已稳定");
            }
        }

        updateStatusDisplay(temperature, statusText);
        if (statusText == QStringLiteral("已稳定")) {
            m_currentStatusLabel->setProperty("state", QStringLiteral("success"));
        } else if (statusText == QStringLiteral("等待稳定")) {
            m_currentStatusLabel->setProperty("state", QStringLiteral("warning"));
        } else {
            m_currentStatusLabel->setProperty("state", QStringLiteral("running"));
        }
        m_currentStatusLabel->style()->unpolish(m_currentStatusLabel);
        m_currentStatusLabel->style()->polish(m_currentStatusLabel);
        appendHistory(QStringLiteral("[接收] %1").arg(QString::fromLatin1(line)));
        return true;
    }

    appendHistory(QStringLiteral("[接收] %1").arg(QString::fromLatin1(line)));
    return true;
}

bool OvenControlDialog::parseTemperatureResponse(const QByteArray &line, double *temperature) const
{
    static const QRegularExpression re(QStringLiteral("(-?\\d+(?:\\.\\d+)?)"));
    const QString text = QString::fromLatin1(line);
    const QRegularExpressionMatch match = re.match(text);
    if (!match.hasMatch()) {
        return false;
    }

    bool ok = false;
    const double value = match.captured(1).toDouble(&ok);
    if (!ok) {
        return false;
    }

    if (temperature) {
        *temperature = value;
    }
    return true;
}

void OvenControlDialog::updateStatusDisplay(double currentTemp, const QString &statusText)
{
    m_currentTempEdit->setText(QString::number(currentTemp, 'f', 1));
    m_currentStatusLabel->setText(statusText);
}

void OvenControlDialog::onConnectClicked()
{
    if (m_connection->isConnected()) {
        m_pollTimer->stop();
        m_waitingForStable = false;
        m_connection->close();
        appendHistory(QStringLiteral("[连接] 烘箱串口已断开"));
        return;
    }

    QString error;
    if (!validateConnectionParams(&error)) {
        QMessageBox::warning(this, QStringLiteral("参数错误"), error);
        return;
    }

    saveUiToConfig();
    Config *cfg = Config::instance();
    if (!m_connection->open(cfg->oven.port,
                            cfg->oven.baudRate,
                            cfg->oven.dataBits,
                            cfg->oven.stopBits,
                            cfg->oven.parity,
                            cfg->oven.timeout)) {
        const QString message = m_connection->lastError().isEmpty()
            ? QStringLiteral("无法打开烘箱串口")
            : m_connection->lastError();
        appendHistory(QStringLiteral("[连接失败] %1").arg(message));
        QMessageBox::warning(this, QStringLiteral("连接失败"), message);
        return;
    }

    appendHistory(QStringLiteral("[连接] 串口已连接: %1").arg(cfg->oven.port));
    if (cfg->oven.autoQueryStatus) {
        m_pollTimer->start(cfg->oven.pollingIntervalMs);
        executeReadCurrentTemperature();
    }
}

void OvenControlDialog::onSaveConfigClicked()
{
    QString error;
    if (!validateConnectionParams(&error)) {
        QMessageBox::warning(this, QStringLiteral("参数错误"), error);
        return;
    }

    saveUiToConfig();
    appendHistory(QStringLiteral("[配置] 烘箱参数已保存"));
}

void OvenControlDialog::onExecuteClicked()
{
    switch (m_actionCombo->currentIndex()) {
    case 0:
        executeSetTargetTemperature();
        break;
    case 1:
        executeReadCurrentTemperature();
        break;
    case 2:
        executeWaitForStable();
        break;
    case 3:
        executePowerOff();
        break;
    default:
        break;
    }
}

void OvenControlDialog::onRefreshStatusClicked()
{
    executeReadCurrentTemperature();
}

void OvenControlDialog::onPollTimeout()
{
    if (!m_connection->isConnected()) {
        m_pollTimer->stop();
        return;
    }
    executeReadCurrentTemperature();
}

void OvenControlDialog::onReceivedData(const QByteArray &data)
{
    if (data.isEmpty()) {
        return;
    }

    m_rxBuffer.append(data);
    processReceiveBuffer();
}

void OvenControlDialog::onConnectionChanged(bool connected)
{
    if (!connected) {
        m_pollTimer->stop();
        m_waitingForStable = false;
    }
    updateConnectionUi(connected);
}

void OvenControlDialog::onConnectionError(const QString &error)
{
    if (!error.isEmpty()) {
        appendHistory(QStringLiteral("[异常] %1").arg(error));
    }
}
