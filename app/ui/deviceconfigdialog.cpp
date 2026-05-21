#include "deviceconfigdialog.h"
#include "ui_deviceconfigdialog.h"
#include "device/devicemanager.h"
#include "config/config.h"
#include "common_component/log/logmanager.h"
#include <QMessageBox>
#include <QStyle>

DeviceConfigDialog::DeviceConfigDialog(DeviceManager *deviceManager, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DeviceConfigDialog)
    , m_deviceManager(deviceManager)
    , m_isConnected(false)
{
    ui->setupUi(this);
    setObjectName(QStringLiteral("deviceConfigDialog"));

    // 移除标题栏的帮助按钮（问号）
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    // 设置默认值
    ui->baudRateCombo->setCurrentText("115200");
    ui->frequencyCombo->setCurrentText("100");
    ui->gyroRangeCombo->setCurrentText("2000");
    ui->gyroFilterCombo->setCurrentText("20");
    ui->accelRangeCombo->setCurrentText("8");
    ui->accelFilterCombo->setCurrentText("20");

    setupConnections();

    ui->refreshBtn->setObjectName(QStringLiteral("secondaryButton"));
    ui->calibrateBtn->setObjectName(QStringLiteral("successButton"));
    ui->resetBtn->setObjectName(QStringLiteral("dangerButton"));
    ui->restartBtn->setObjectName(QStringLiteral("dangerButton"));
    ui->writeOutputBtn->setObjectName(QStringLiteral("secondaryButton"));
    ui->writeGyroBtn->setObjectName(QStringLiteral("secondaryButton"));
    ui->writeAccelBtn->setObjectName(QStringLiteral("secondaryButton"));

    updateConnectionStatus(false);
}

DeviceConfigDialog::~DeviceConfigDialog()
{
    delete ui;
}

void DeviceConfigDialog::setupConnections()
{
    // 工具栏按钮
    connect(ui->refreshBtn, &QPushButton::clicked, this, &DeviceConfigDialog::onRefresh);
    connect(ui->calibrateBtn, &QPushButton::clicked, this, &DeviceConfigDialog::onHorizontalCalibration);
    connect(ui->resetBtn, &QPushButton::clicked, this, &DeviceConfigDialog::onReset);
    connect(ui->restartBtn, &QPushButton::clicked, this, &DeviceConfigDialog::onRestart);

    // 协议切换
    connect(ui->binaryProtocolRadio, &QRadioButton::toggled, this, &DeviceConfigDialog::onProtocolChanged);
    connect(ui->quaternionProtocolRadio, &QRadioButton::toggled, this, &DeviceConfigDialog::onProtocolChanged);

    // 参数写入
    connect(ui->writeOutputBtn, &QPushButton::clicked, this, &DeviceConfigDialog::onWriteOutputParams);
    connect(ui->writeGyroBtn, &QPushButton::clicked, this, &DeviceConfigDialog::onWriteGyroParams);
    connect(ui->writeAccelBtn, &QPushButton::clicked, this, &DeviceConfigDialog::onWriteAccelParams);

    // 设备管理器信号
    if (m_deviceManager) {
        connect(m_deviceManager, &DeviceManager::connectionChanged,
                this, &DeviceConfigDialog::onDeviceConnectionChanged);
    }
}

void DeviceConfigDialog::onRefresh()
{
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "设备未连接！");
        return;
    }

    LOG_INFO("Refreshing device info");
    loadDeviceInfo();
    QMessageBox::information(this, QStringLiteral("提示"),
                             QStringLiteral("设备信息读取指令尚未接入，当前显示占位信息。"));
}

void DeviceConfigDialog::onHorizontalCalibration()
{
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "设备未连接！");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认", "确定要进行水平校准吗？\n请确保设备处于水平静止状态。",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        LOG_INFO("Starting horizontal calibration");
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("水平校准指令尚未接入，未向设备发送数据。"));
    }
}

void DeviceConfigDialog::onReset()
{
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "设备未连接！");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认", "确定要重置设备参数吗？\n所有配置将恢复为出厂设置。",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        LOG_INFO("Resetting device");
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("设备重置指令尚未接入，未向设备发送数据。"));
    }
}

void DeviceConfigDialog::onRestart()
{
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "设备未连接！");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认", "确定要重启设备吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        LOG_INFO("Restarting device");
        QMessageBox::information(this, QStringLiteral("提示"),
                                 QStringLiteral("设备重启指令尚未接入，未向设备发送数据。"));
    }
}

void DeviceConfigDialog::onProtocolChanged()
{
    QString protocol = ui->binaryProtocolRadio->isChecked() ? "二进制" : "二进制（四元数）";
    LOG_INFO(QString("Protocol changed to: %1").arg(protocol));
    // TODO: 切换协议处理
}

void DeviceConfigDialog::onWriteOutputParams()
{
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "设备未连接！");
        return;
    }

    QString baudRate = ui->baudRateCombo->currentText();
    QString frequency = ui->frequencyCombo->currentText();

    LOG_INFO(QString("Writing output params: BaudRate=%1, Frequency=%2").arg(baudRate).arg(frequency));
    QMessageBox::information(this, QStringLiteral("提示"),
        QStringLiteral("输出参数写入指令尚未接入，未向设备发送数据。\n波特率: %1 Bps\n频率: %2 Hz")
            .arg(baudRate).arg(frequency));
}

void DeviceConfigDialog::onWriteGyroParams()
{
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "设备未连接！");
        return;
    }

    QString range = ui->gyroRangeCombo->currentText();
    QString filter = ui->gyroFilterCombo->currentText();

    LOG_INFO(QString("Writing gyro params: Range=%1, Filter=%2").arg(range).arg(filter));
    QMessageBox::information(this, QStringLiteral("提示"),
        QStringLiteral("陀螺仪参数写入指令尚未接入，未向设备发送数据。\n量程: %1 deg/s\n滤波器: %2 Hz")
            .arg(range).arg(filter));
}

void DeviceConfigDialog::onWriteAccelParams()
{
    if (!m_isConnected) {
        QMessageBox::warning(this, "警告", "设备未连接！");
        return;
    }

    QString range = ui->accelRangeCombo->currentText();
    QString filter = ui->accelFilterCombo->currentText();

    LOG_INFO(QString("Writing accel params: Range=%1, Filter=%2").arg(range).arg(filter));
    QMessageBox::information(this, QStringLiteral("提示"),
        QStringLiteral("加速度计参数写入指令尚未接入，未向设备发送数据。\n量程: %1 g\n滤波器: %2 Hz")
            .arg(range).arg(filter));
}

void DeviceConfigDialog::onDeviceConnectionChanged(bool connected)
{
    m_isConnected = connected;
    updateConnectionStatus(connected);

    if (connected) {
        loadDeviceInfo();
    }
}

void DeviceConfigDialog::loadDeviceInfo()
{
    // TODO: 从设备读取实际信息
    // 目前使用占位符
    ui->snEdit->setText("NACK");
    ui->firmwareVersionEdit->setText("NACK");
    ui->algorithmVersionEdit->setText("NACK");
}

void DeviceConfigDialog::updateConnectionStatus(bool connected)
{
    Config *cfg = Config::instance();
    QString typeStr = (cfg->device.type.toUpper() == "UDP") ? "UDP" : "串口";

    ui->connectionStatusLabel->setObjectName(QStringLiteral("connectionStatusLabel"));
    ui->connectionStatusLabel->setProperty("state", connected ? "connected" : "error");

    if (connected) {
        ui->connectionStatusLabel->setText(typeStr + " 已连接");
    } else {
        ui->connectionStatusLabel->setText(typeStr + " 未连接");
    }

    ui->connectionStatusLabel->style()->unpolish(ui->connectionStatusLabel);
    ui->connectionStatusLabel->style()->polish(ui->connectionStatusLabel);
    ui->connectionStatusLabel->update();

    // 根据连接状态启用/禁用控件
    ui->refreshBtn->setEnabled(connected);
    ui->calibrateBtn->setEnabled(connected);
    ui->resetBtn->setEnabled(connected);
    ui->restartBtn->setEnabled(connected);
    ui->writeOutputBtn->setEnabled(connected);
    ui->writeGyroBtn->setEnabled(connected);
    ui->writeAccelBtn->setEnabled(connected);
}
