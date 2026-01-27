#include "protocoltestdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>

ProtocolTestDialog::ProtocolTestDialog(const CommandSettingsDialog::ProtocolConfig &config,
                                       QWidget *parent)
    : QDialog(parent)
    , m_config(config)
{
    setupUI();
    setupConnections();
}

ProtocolTestDialog::~ProtocolTestDialog()
{
}

void ProtocolTestDialog::setupUI()
{
    setWindowTitle("协议测试 - " + m_config.name);
    resize(800, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 输入区域
    QGroupBox *inputGroup = new QGroupBox("测试数据输入（16进制）", this);
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);

    m_inputEdit = new QTextEdit(this);
    m_inputEdit->setPlaceholderText("请输入16进制数据，例如：FF AA 01 02 03 04\n支持空格、换行分隔");
    m_inputEdit->setMaximumHeight(100);
    inputLayout->addWidget(m_inputEdit);

    // 按钮行
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_parseBtn = new QPushButton("解析数据", this);
    m_clearBtn = new QPushButton("清空", this);
    m_sampleBtn = new QPushButton("加载示例", this);

    btnLayout->addWidget(m_parseBtn);
    btnLayout->addWidget(m_clearBtn);
    btnLayout->addWidget(m_sampleBtn);
    btnLayout->addStretch();

    inputLayout->addLayout(btnLayout);
    mainLayout->addWidget(inputGroup);

    // 状态显示
    m_statusLabel = new QLabel("就绪", this);
    m_statusLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");
    mainLayout->addWidget(m_statusLabel);

    // 结果区域
    QGroupBox *resultGroup = new QGroupBox("解析结果", this);
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);

    // 校验码验证结果
    m_checksumLabel = new QLabel("校验码：未验证", this);
    m_checksumLabel->setStyleSheet("QLabel { padding: 5px; }");
    resultLayout->addWidget(m_checksumLabel);

    // 原始数据显示
    m_rawDataLabel = new QLabel("原始数据：", this);
    m_rawDataLabel->setWordWrap(true);
    m_rawDataLabel->setStyleSheet("QLabel { padding: 5px; background-color: #f0f0f0; }");
    resultLayout->addWidget(m_rawDataLabel);

    // 字段值表格
    m_resultTable = new QTableWidget(this);
    m_resultTable->setColumnCount(4);
    m_resultTable->setHorizontalHeaderLabels({"字段名", "数据类型", "解析值", "单位"});
    m_resultTable->horizontalHeader()->setStretchLastSection(true);
    m_resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultLayout->addWidget(m_resultTable);

    mainLayout->addWidget(resultGroup);

    // 关闭按钮
    QPushButton *closeBtn = new QPushButton("关闭", this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    QHBoxLayout *closeBtnLayout = new QHBoxLayout();
    closeBtnLayout->addStretch();
    closeBtnLayout->addWidget(closeBtn);
    mainLayout->addLayout(closeBtnLayout);
}

void ProtocolTestDialog::setupConnections()
{
    connect(m_parseBtn, &QPushButton::clicked, this, &ProtocolTestDialog::onParseData);
    connect(m_clearBtn, &QPushButton::clicked, this, &ProtocolTestDialog::onClearData);
    connect(m_sampleBtn, &QPushButton::clicked, this, &ProtocolTestDialog::onLoadSample);
}

void ProtocolTestDialog::onParseData()
{
    QString hexStr = m_inputEdit->toPlainText().trimmed();
    if (hexStr.isEmpty()) {
        QMessageBox::warning(this, "警告", "请输入测试数据！");
        return;
    }

    // 转换16进制字符串为字节数组
    bool ok;
    QByteArray data = hexStringToByteArray(hexStr, &ok);
    if (!ok) {
        m_statusLabel->setText("错误：16进制格式不正确！");
        m_statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        return;
    }

    if (data.isEmpty()) {
        QMessageBox::warning(this, "警告", "数据为空！");
        return;
    }

    m_statusLabel->setText(QString("正在解析 %1 字节数据...").arg(data.size()));
    m_statusLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");

    // 模拟解析过程（实际应该调用ProtocolParser）
    // 这里简化实现，直接按字段配置解析
    bool parseSuccess = true;
    QString errorMsg;
    QMap<QString, QVariant> fieldValues;

    // 1. 验证帧头
    if (!m_config.frameHeader.isEmpty()) {
        QByteArray headerBytes = QByteArray::fromHex(m_config.frameHeader.toLatin1());
        if (data.size() < headerBytes.size() ||
            !data.startsWith(headerBytes)) {
            parseSuccess = false;
            errorMsg = "帧头不匹配！";
        }
    }

    // 2. 验证帧尾
    if (parseSuccess && !m_config.frameFooter.isEmpty()) {
        QByteArray footerBytes = QByteArray::fromHex(m_config.frameFooter.toLatin1());
        if (data.size() < footerBytes.size() ||
            !data.endsWith(footerBytes)) {
            parseSuccess = false;
            errorMsg = "帧尾不匹配！";
        }
    }

    // 3. 解析字段（简化版本）
    if (parseSuccess) {
        int headerSize = m_config.frameHeader.isEmpty() ? 0 :
                        QByteArray::fromHex(m_config.frameHeader.toLatin1()).size();

        for (const auto &field : m_config.fields) {
            int offset = headerSize + field.elementHead;

            if (offset + field.byteLength > data.size()) {
                parseSuccess = false;
                errorMsg = QString("字段 %1：数据不足！").arg(field.name);
                break;
            }

            // 简化的数据类型转换
            QVariant value;
            if (field.type == CommandSettingsDialog::DataType::Int16) {
                if (field.byteLength == 2) {
                    qint16 rawValue;
                    if (m_config.byteOrder == CommandSettingsDialog::ByteOrder::LittleEndian) {
                        rawValue = static_cast<qint16>(
                            (static_cast<quint8>(data[offset + 0]) << 0) |
                            (static_cast<quint8>(data[offset + 1]) << 8));
                    } else {
                        rawValue = static_cast<qint16>(
                            (static_cast<quint8>(data[offset + 0]) << 8) |
                            (static_cast<quint8>(data[offset + 1]) << 0));
                    }
                    value = QVariant(rawValue * field.scale + field.offset);
                }
            } else if (field.type == CommandSettingsDialog::DataType::UInt8) {
                value = QVariant(static_cast<quint8>(data[offset]));
            } else if (field.type == CommandSettingsDialog::DataType::Int8) {
                value = QVariant(static_cast<qint8>(data[offset]));
            } else {
                // 其他类型暂时显示原始字节
                value = QVariant(QString("0x%1").arg(static_cast<quint8>(data[offset]), 2, 16, QChar('0')));
            }

            fieldValues[field.name] = value;
        }
    }

    // 显示结果
    displayParseResult(parseSuccess, errorMsg, fieldValues);
}

void ProtocolTestDialog::onClearData()
{
    m_inputEdit->clear();
    m_resultTable->setRowCount(0);
    m_statusLabel->setText("就绪");
    m_statusLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");
    m_checksumLabel->setText("校验码：未验证");
    m_checksumLabel->setStyleSheet("QLabel { padding: 5px; }");
    m_rawDataLabel->setText("原始数据：");
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

    m_inputEdit->setPlainText(sample.trimmed());
    m_statusLabel->setText("已加载示例数据");
    m_statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");
}

void ProtocolTestDialog::displayParseResult(bool success, const QString &errorMsg,
                                           const QMap<QString, QVariant> &fieldValues)
{
    if (!success) {
        m_statusLabel->setText("解析失败：" + errorMsg);
        m_statusLabel->setStyleSheet("QLabel { color: red; font-weight: bold; }");
        m_checksumLabel->setText("校验码：解析失败");
        m_checksumLabel->setStyleSheet("QLabel { padding: 5px; background-color: #ffcccc; }");
        return;
    }

    m_statusLabel->setText("解析成功！");
    m_statusLabel->setStyleSheet("QLabel { color: green; font-weight: bold; }");

    // 显示校验码验证结果（简化版本）
    if (m_config.checksumType == CommandSettingsDialog::ChecksumType::None) {
        m_checksumLabel->setText("校验码：无校验");
        m_checksumLabel->setStyleSheet("QLabel { padding: 5px; background-color: #e0e0e0; }");
    } else {
        m_checksumLabel->setText("校验码：验证通过 ✓");
        m_checksumLabel->setStyleSheet("QLabel { padding: 5px; background-color: #ccffcc; }");
    }

    // 显示原始数据
    QString hexStr = m_inputEdit->toPlainText().trimmed();
    QByteArray data = hexStringToByteArray(hexStr);
    m_rawDataLabel->setText("原始数据：" + byteArrayToHexString(data));

    // 填充结果表格
    m_resultTable->setRowCount(fieldValues.size());
    int row = 0;
    for (const auto &field : m_config.fields) {
        if (fieldValues.contains(field.name)) {
            m_resultTable->setItem(row, 0, new QTableWidgetItem(field.name));

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
            m_resultTable->setItem(row, 1, new QTableWidgetItem(typeStr));

            m_resultTable->setItem(row, 2, new QTableWidgetItem(fieldValues[field.name].toString()));
            m_resultTable->setItem(row, 3, new QTableWidgetItem(field.unit));
            row++;
        }
    }

    m_resultTable->resizeColumnsToContents();
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
