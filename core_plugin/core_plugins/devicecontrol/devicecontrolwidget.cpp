#include "devicecontrolwidget.h"
#include "ui_devicecontrolwidget.h"
#include <QDateTime>
#include <QMessageBox>

DeviceControlWidget::DeviceControlWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DeviceControlWidget)
{
    ui->setupUi(this);

    // 连接预定义按钮信号
    connect(ui->calibrateButton, &QPushButton::clicked, this, &DeviceControlWidget::onCalibrateClicked);
    connect(ui->resetButton, &QPushButton::clicked, this, &DeviceControlWidget::onResetClicked);
    connect(ui->queryButton, &QPushButton::clicked, this, &DeviceControlWidget::onQueryClicked);
    connect(ui->startButton, &QPushButton::clicked, this, &DeviceControlWidget::onStartClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &DeviceControlWidget::onStopClicked);
    connect(ui->sendCustomButton, &QPushButton::clicked, this, &DeviceControlWidget::onSendCustomClicked);
}

DeviceControlWidget::~DeviceControlWidget()
{
    delete ui;
}

void DeviceControlWidget::setPredefinedCommands(const QMap<QString, QByteArray> &commands)
{
    m_commands = commands;
}

void DeviceControlWidget::addHistory(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
    ui->historyEdit->append(QString("[%1] %2").arg(timestamp).arg(message));

    // 限制历史记录行数（最多1000行）
    if (ui->historyEdit->document()->lineCount() > 1000) {
        QTextCursor cursor = ui->historyEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 100);
        cursor.removeSelectedText();
    }
}

void DeviceControlWidget::onCalibrateClicked()
{
    if (m_commands.contains("校准")) {
        emit sendCommand("校准", m_commands["校准"]);
    }
}

void DeviceControlWidget::onResetClicked()
{
    if (m_commands.contains("复位")) {
        emit sendCommand("复位", m_commands["复位"]);
    }
}

void DeviceControlWidget::onQueryClicked()
{
    if (m_commands.contains("查询状态")) {
        emit sendCommand("查询状态", m_commands["查询状态"]);
    }
}

void DeviceControlWidget::onStartClicked()
{
    if (m_commands.contains("开始采集")) {
        emit sendCommand("开始采集", m_commands["开始采集"]);
    }
}

void DeviceControlWidget::onStopClicked()
{
    if (m_commands.contains("停止采集")) {
        emit sendCommand("停止采集", m_commands["停止采集"]);
    }
}

void DeviceControlWidget::onSendCustomClicked()
{
    QString hexString = ui->customCommandEdit->text().trimmed();
    if (hexString.isEmpty()) {
        QMessageBox::warning(this, "错误", "请输入16进制指令");
        return;
    }

    // 移除空格和非法字符
    hexString.remove(QRegExp("[^0-9A-Fa-f]"));

    // 检查是否为有效的16进制
    if (hexString.length() % 2 != 0) {
        QMessageBox::warning(this, "错误", "16进制指令长度必须为偶数");
        return;
    }

    QByteArray commandData = QByteArray::fromHex(hexString.toLatin1());
    emit sendCommand("自定义指令", commandData);

    // 清空输入框
    ui->customCommandEdit->clear();
}
