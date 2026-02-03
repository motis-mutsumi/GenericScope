#include "protocoltestdialog.h"
#include "ui_protocoltestdialog.h"
#include "protocoltypeconverter.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>

ProtocolTestDialog::ProtocolTestDialog(const CommandSettingsDialog::ProtocolConfig &config,
                                       QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProtocolTestDialog)
    , m_config(config)
{
    ui->setupUi(this);

    // 将 UI 配置转换为 protocol 模块配置
    ::ProtocolConfig protocolConfig = ProtocolTypeConverter::uiToProtocolConfig(config);

    // 创建协议解析器
    m_parser = QSharedPointer<ProtocolParser>::create(protocolConfig);

    setupUI();
    setupConnections();
}

ProtocolTestDialog::~ProtocolTestDialog()
{
    delete ui;
}

void ProtocolTestDialog::setupUI()
{
    // UI已经通过.ui文件创建，这里只需要设置窗口标题
    setWindowTitle("协议测试 - " + m_config.name);
}

void ProtocolTestDialog::setupConnections()
{
    connect(ui->parseBtn, &QPushButton::clicked, this, &ProtocolTestDialog::onParseData);
    connect(ui->clearBtn, &QPushButton::clicked, this, &ProtocolTestDialog::onClearData);
    connect(ui->sampleBtn, &QPushButton::clicked, this, &ProtocolTestDialog::onLoadSample);
    connect(ui->closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void ProtocolTestDialog::onParseData()
{
    QString hexStr = ui->inputEdit->toPlainText().trimmed();
    if (hexStr.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入测试数据！");
        return;
    }

    // 转换16进制字符串为字节数组
    bool ok;
    QByteArray data = hexStringToByteArray(hexStr, &ok);
    if (!ok) {
        ui->statusLabel->setText("错误：16进制格式不正确！");
        ui->statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        return;
    }

    if (data.isEmpty()) {
        QMessageBox::warning(this, "警告", "数据为空！");
        return;
    }

    ui->statusLabel->setText(QString("正在解析 %1 字节数据...").arg(data.size()));
    ui->statusLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");

    // 使用 ProtocolParser 解析数据
    ParseResult result = m_parser->parse(data);

    // 显示原始数据（包括校验信息）
    ui->rawDataLabel->setText("原始数据：" + byteArrayToHexString(data));

    // 显示校验码验证结果
    if (m_config.checksumType == CommandSettingsDialog::ChecksumType::None) {
        ui->checksumLabel->setText("校验码：无校验");
        ui->checksumLabel->setStyleSheet("QLabel { padding: 5px; background-color: #e0e0e0; }");
    } else if (result.success) {
        ui->checksumLabel->setText("校验码：验证通过 ✓");
        ui->checksumLabel->setStyleSheet("QLabel { padding: 5px; background-color: #ccffcc; }");
    } else if (result.errorMsg.contains("Checksum")) {
        ui->checksumLabel->setText("校验码：验证失败 ✗");
        ui->checksumLabel->setStyleSheet("QLabel { padding: 5px; background-color: #ffcccc; }");
    }

    // 显示解析结果
    displayParseResult(result.success, result.errorMsg, result.fieldValues);
}

void ProtocolTestDialog::onClearData()
{
    ui->inputEdit->clear();
    ui->resultTable->setRowCount(0);
    ui->statusLabel->setText("就绪");
    ui->statusLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");
    ui->checksumLabel->setText("校验码：未验证");
    ui->checksumLabel->setStyleSheet("QLabel { padding: 5px; }");
    ui->rawDataLabel->setText("原始数据：");
}

void ProtocolTestDialog::onLoadSample()
{
    // 生成示例数据
    QString sample;

    // 添加帧头
    if (!m_config.frameHeader.isEmpty()) {
        sample += m_config.frameHeader + " ";
    }

    // 添加示例字段数据
    for (int i = 0; i < m_config.fields.size(); ++i) {
        const auto &field = m_config.fields[i];
        for (int j = 0; j < field.byteLength; ++j) {
            sample += QString("%1 ").arg(i + j, 2, 16, QChar('0'));
        }
    }

    // 添加帧尾
    if (!m_config.frameFooter.isEmpty()) {
        sample += m_config.frameFooter;
    }

    ui->inputEdit->setPlainText(sample.trimmed());
    ui->statusLabel->setText("已加载示例数据");
    ui->statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
}

void ProtocolTestDialog::displayParseResult(bool success, const QString &errorMsg,
                                           const QMap<QString, QVariant> &fieldValues)
{
    if (!success) {
        ui->statusLabel->setText("解析失败：" + errorMsg);
        ui->statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        return;
    }

    ui->statusLabel->setText(QString("解析成功！共解析 %1 个字段").arg(fieldValues.size()));
    ui->statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");

    // 填充结果表格
    ui->resultTable->setRowCount(fieldValues.size());
    int row = 0;
    for (const auto &field : m_config.fields) {
        if (fieldValues.contains(field.name)) {
            ui->resultTable->setItem(row, 0, new QTableWidgetItem(field.name));

            QString typeStr;
            switch (field.type) {
                case CommandSettingsDialog::DataType::Int8: typeStr = "int8_t"; break;
                case CommandSettingsDialog::DataType::UInt8: typeStr = "uint8_t"; break;
                case CommandSettingsDialog::DataType::Int16: typeStr = "int16_t"; break;
                case CommandSettingsDialog::DataType::UInt16: typeStr = "uint16_t"; break;
                case CommandSettingsDialog::DataType::Int32: typeStr = "int32_t"; break;
                case CommandSettingsDialog::DataType::UInt32: typeStr = "uint32_t"; break;
                case CommandSettingsDialog::DataType::Float: typeStr = "float"; break;
                case CommandSettingsDialog::DataType::Double: typeStr = "double"; break;
                case CommandSettingsDialog::DataType::MByte: typeStr = "mbyte_t"; break;
                case CommandSettingsDialog::DataType::String: typeStr = "string"; break;
            }
            ui->resultTable->setItem(row, 1, new QTableWidgetItem(typeStr));

            // 格式化显示数值
            QString valueStr;
            QVariant value = fieldValues[field.name];
            if (field.type == CommandSettingsDialog::DataType::Float ||
                field.type == CommandSettingsDialog::DataType::Double ||
                field.type == CommandSettingsDialog::DataType::MByte) {
                // 浮点数类型，保留6位小数
                bool ok;
                double dValue = value.toDouble(&ok);
                if (ok) {
                    valueStr = QString::number(dValue, 'f', 6);
                } else {
                    valueStr = value.toString();
                }
            } else {
                valueStr = value.toString();
            }

            ui->resultTable->setItem(row, 2, new QTableWidgetItem(valueStr));
            ui->resultTable->setItem(row, 3, new QTableWidgetItem(field.unit));
            row++;
        }
    }

    ui->resultTable->resizeColumnsToContents();
}

QByteArray ProtocolTestDialog::hexStringToByteArray(const QString &hexStr, bool *ok)
{
    if (ok) *ok = true;

    QString cleaned = hexStr;
    cleaned.remove(' ');
    cleaned.remove('\n');
    cleaned.remove('\r');
    cleaned.remove('\t');

    if (cleaned.length() % 2 != 0) {
        if (ok) *ok = false;
        return QByteArray();
    }

    QByteArray result;
    for (int i = 0; i < cleaned.length(); i += 2) {
        QString byteStr = cleaned.mid(i, 2);
        bool convertOk;
        quint8 byte = byteStr.toUInt(&convertOk, 16);
        if (!convertOk) {
            if (ok) *ok = false;
            return QByteArray();
        }
        result.append(static_cast<char>(byte));
    }

    return result;
}

QString ProtocolTestDialog::byteArrayToHexString(const QByteArray &data)
{
    QString result;
    for (int i = 0; i < data.size(); ++i) {
        result += QString("%1 ").arg(static_cast<quint8>(data[i]), 2, 16, QChar('0')).toUpper();
    }
    return result.trimmed();
}
