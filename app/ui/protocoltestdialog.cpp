#include "protocoltestdialog.h"
#include "ui_protocoltestdialog.h"
#include "protocoltypeconverter.h"
#include "protocol/checksumcalculator.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QHash>

namespace {
QHash<const ProtocolTestDialog*, QByteArray> g_lastParsedFrameByDialog;
}

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
    g_lastParsedFrameByDialog.remove(this);
    delete ui;
}

void ProtocolTestDialog::setupUI()
{
    // UI已经通过.ui文件创建，这里只需要设置窗口标题
    setWindowTitle("协议测试 - " + m_config.name);

    // 增加“原始数据(HEX)”列，方便核对每个字段的字节切片
    ui->resultTable->setColumnCount(5);
    ui->resultTable->setHorizontalHeaderLabels({"字段名", "数据类型", "解析值", "单位", "原始数据(HEX)"});
    ui->resultTable->horizontalHeader()->setStretchLastSection(true);
    ui->resultTable->setColumnWidth(0, 130);
    ui->resultTable->setColumnWidth(1, 120);
    ui->resultTable->setColumnWidth(2, 220);
    ui->resultTable->setColumnWidth(3, 80);
    ui->resultTable->setColumnWidth(4, 240);
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
    g_lastParsedFrameByDialog[this] = result.rawData.isEmpty() ? data : result.rawData;

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
    g_lastParsedFrameByDialog.remove(this);
}

void ProtocolTestDialog::onLoadSample()
{
    // 生成完整示例帧：帧头 + 数据区(含长度位) + 校验 + 帧尾
    bool ok = false;
    const QByteArray header = hexStringToByteArray(m_config.frameHeader, &ok);
    if (!ok) {
        ui->statusLabel->setText("示例生成失败：帧头格式无效");
        ui->statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        return;
    }

    const QByteArray footer = hexStringToByteArray(m_config.frameFooter, &ok);
    if (!ok) {
        ui->statusLabel->setText("示例生成失败：帧尾格式无效");
        ui->statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        return;
    }

    auto checksumSize = [this]() -> int {
        switch (m_config.checksumType) {
            case CommandSettingsDialog::ChecksumType::CRC32: return 4;
            case CommandSettingsDialog::ChecksumType::CRC16_XMODEM: return 2;
            case CommandSettingsDialog::ChecksumType::Sum:
            case CommandSettingsDialog::ChecksumType::XOR:
            case CommandSettingsDialog::ChecksumType::CRC8: return 1;
            case CommandSettingsDialog::ChecksumType::None:
            default: return 0;
        }
    };

    int bodySize = 0;
    for (const auto &field : m_config.fields) {
        bodySize = qMax(bodySize, field.elementHead + field.byteLength);
    }
    if (m_config.lengthPosition >= 0) {
        bodySize = qMax(bodySize, m_config.lengthPosition + 1);
    }
    if (bodySize <= 0) {
        bodySize = 8; // 没有字段时也给一段可测数据
    }

    QByteArray body(bodySize, '\0');

    // 按字段偏移填充可读样例值（不会互相错位）
    for (int i = 0; i < m_config.fields.size(); ++i) {
        const auto &field = m_config.fields[i];
        for (int j = 0; j < field.byteLength; ++j) {
            const int pos = field.elementHead + j;
            if (pos >= 0 && pos < body.size()) {
                body[pos] = static_cast<char>((i * 17 + j + 1) & 0xFF);
            }
        }
    }

    const int cksSize = checksumSize();

    // 长度位语义：长度值=帧头之后到帧尾之前的字节数（数据区+校验）
    if (m_config.lengthPosition >= 0 && m_config.lengthPosition < body.size()) {
        const int lengthValue = body.size() + cksSize;
        body[m_config.lengthPosition] = static_cast<char>(lengthValue & 0xFF);
    }

    QByteArray frame = header + body;
    frame.append(QByteArray(cksSize, '\0'));
    frame.append(footer);

    if (m_config.checksumType != CommandSettingsDialog::ChecksumType::None && cksSize > 0) {
        int checksumStart = m_config.checksumStart;
        switch (m_config.checksumScope) {
            case CommandSettingsDialog::ChecksumScope::FullFrame:
                checksumStart = 0;
                break;
            case CommandSettingsDialog::ChecksumScope::AfterHeader:
                checksumStart = header.size();
                break;
            case CommandSettingsDialog::ChecksumScope::DataOnly:
                checksumStart = header.size();
                if (m_config.lengthPosition >= 0) {
                    checksumStart = m_config.lengthPosition + 1;
                }
                break;
            case CommandSettingsDialog::ChecksumScope::Custom:
                break;
        }

        int checksumPos = m_config.checksumPosition;
        if (checksumPos < 0) {
            checksumPos = frame.size() - cksSize - footer.size();
        }

        int checksumLength = m_config.checksumLength;
        if (checksumLength < 0) {
            checksumLength = checksumPos - checksumStart;
        }

        quint32 checksumValue = 0;
        if (checksumStart >= 0 && checksumLength > 0 && checksumStart + checksumLength <= frame.size()) {
            switch (m_config.checksumType) {
                case CommandSettingsDialog::ChecksumType::Sum:
                    checksumValue = ChecksumCalculator::calculateSum(frame, checksumStart, checksumLength);
                    break;
                case CommandSettingsDialog::ChecksumType::XOR:
                    checksumValue = ChecksumCalculator::calculateXOR(frame, checksumStart, checksumLength);
                    break;
                case CommandSettingsDialog::ChecksumType::CRC8:
                    checksumValue = ChecksumCalculator::calculateCRC8(frame, checksumStart, checksumLength);
                    break;
                case CommandSettingsDialog::ChecksumType::CRC16_XMODEM:
                    checksumValue = ChecksumCalculator::calculateCRC16_XMODEM(frame, checksumStart, checksumLength);
                    break;
                case CommandSettingsDialog::ChecksumType::CRC32:
                    checksumValue = ChecksumCalculator::calculateCRC32(frame, checksumStart, checksumLength);
                    break;
                case CommandSettingsDialog::ChecksumType::None:
                default:
                    break;
            }
        }

        if (checksumPos >= 0 && checksumPos + cksSize <= frame.size()) {
            const bool little = (m_config.checksumByteOrder == CommandSettingsDialog::ByteOrder::LittleEndian);
            if (cksSize == 1) {
                frame[checksumPos] = static_cast<char>(checksumValue & 0xFF);
            } else if (cksSize == 2) {
                const quint8 b0 = static_cast<quint8>(checksumValue & 0xFF);
                const quint8 b1 = static_cast<quint8>((checksumValue >> 8) & 0xFF);
                frame[checksumPos + (little ? 0 : 1)] = static_cast<char>(b0);
                frame[checksumPos + (little ? 1 : 0)] = static_cast<char>(b1);
            } else if (cksSize == 4) {
                const quint8 b0 = static_cast<quint8>(checksumValue & 0xFF);
                const quint8 b1 = static_cast<quint8>((checksumValue >> 8) & 0xFF);
                const quint8 b2 = static_cast<quint8>((checksumValue >> 16) & 0xFF);
                const quint8 b3 = static_cast<quint8>((checksumValue >> 24) & 0xFF);
                if (little) {
                    frame[checksumPos + 0] = static_cast<char>(b0);
                    frame[checksumPos + 1] = static_cast<char>(b1);
                    frame[checksumPos + 2] = static_cast<char>(b2);
                    frame[checksumPos + 3] = static_cast<char>(b3);
                } else {
                    frame[checksumPos + 0] = static_cast<char>(b3);
                    frame[checksumPos + 1] = static_cast<char>(b2);
                    frame[checksumPos + 2] = static_cast<char>(b1);
                    frame[checksumPos + 3] = static_cast<char>(b0);
                }
            }
        }
    }

    ui->inputEdit->setPlainText(byteArrayToHexString(frame));
    ui->statusLabel->setText("已加载示例数据（含长度与校验）");
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
    const QByteArray frame = g_lastParsedFrameByDialog.value(this);
    const int headerSize = m_config.frameHeader.isEmpty()
        ? 0
        : hexStringToByteArray(m_config.frameHeader, nullptr).size();
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

            // 字段原始字节（ProtocolParser 实际偏移：帧头长度 + elementHead）
            QString rawHex = "-";
            const int actualOffset = headerSize + field.elementHead;
            if (actualOffset >= 0 && field.byteLength > 0 &&
                actualOffset + field.byteLength <= frame.size()) {
                rawHex = byteArrayToHexString(frame.mid(actualOffset, field.byteLength));
            }
            ui->resultTable->setItem(row, 4, new QTableWidgetItem(rawHex));
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
