#include "protocolparser.h"
#include <QDebug>

ProtocolParser::ProtocolParser(const ProtocolConfig &config)
    : m_config(config)
    , m_converter(config.byteOrder)
{
}

void ProtocolParser::setConfig(const ProtocolConfig &config)
{
    m_config = config;
    m_converter.setByteOrder(config.byteOrder);
}

ParseResult ProtocolParser::parse(const QByteArray &data)
{
    ParseResult result;

    // 1. 查找帧头
    int headerPos = findFrameHeader(data);
    if (headerPos < 0) {
        result.errorMsg = "Frame header not found";
        return result;
    }

    // 2. 提取完整帧
    QByteArray frame = extractFrame(data, headerPos);
    if (frame.isEmpty()) {
        result.errorMsg = "Incomplete frame";
        return result;
    }

    // 3. 验证校验码
    if (!verifyChecksum(frame)) {
        result.errorMsg = "Checksum verification failed";
        return result;
    }

    // 4. 解析所有字段
    for (const FieldConfig &field : m_config.fields) {
        QVariant value = parseField(frame, field);
        if (value.isValid()) {
            result.fieldValues[field.name] = value;
        }
    }

    result.success = true;
    result.rawData = frame;
    result.timestamp = QDateTime::currentMSecsSinceEpoch();

    return result;
}

int ProtocolParser::findFrameHeader(const QByteArray &data, int startPos) const
{
    if (m_config.frameHeader.isEmpty()) {
        return 0;  // 无帧头，从头开始
    }

    return data.indexOf(m_config.frameHeader, startPos);
}

QByteArray ProtocolParser::extractFrame(const QByteArray &data, int headerPos) const
{
    int frameStart = headerPos;
    int frameLength = 0;

    // 方法1：根据长度字段确定帧长度
    if (m_config.lengthPosition >= 0) {
        int lengthPos = frameStart + m_config.lengthPosition;
        if (lengthPos + 1 < data.size()) {
            quint8 length = static_cast<quint8>(data[lengthPos]);
            frameLength = m_config.frameHeader.size() + length;

            // 如果有帧尾，加上帧尾长度
            if (!m_config.frameFooter.isEmpty()) {
                frameLength += m_config.frameFooter.size();
            }
        }
    }
    // 方法2：根据帧尾确定帧长度
    else if (!m_config.frameFooter.isEmpty()) {
        int footerPos = data.indexOf(m_config.frameFooter, frameStart + m_config.frameHeader.size());
        if (footerPos >= 0) {
            frameLength = footerPos - frameStart + m_config.frameFooter.size();
        }
    }
    // 方法3：根据字段配置计算帧长度
    else {
        int maxOffset = 0;
        for (const FieldConfig &field : m_config.fields) {
            int endPos = field.elementHead + field.byteLength;
            if (endPos > maxOffset) {
                maxOffset = endPos;
            }
        }

        frameLength = m_config.frameHeader.size() + maxOffset;

        // 加上校验码长度
        if (m_config.checksumType != ChecksumType::None) {
            if (m_config.checksumType == ChecksumType::CRC32) {
                frameLength += 4;
            } else if (m_config.checksumType == ChecksumType::CRC16) {
                frameLength += 2;
            } else {
                frameLength += 1;
            }
        }

        // 加上帧尾长度
        if (!m_config.frameFooter.isEmpty()) {
            frameLength += m_config.frameFooter.size();
        }
    }

    // 检查数据是否足够
    if (frameStart + frameLength > data.size()) {
        return QByteArray();  // 数据不完整
    }

    return data.mid(frameStart, frameLength);
}

bool ProtocolParser::verifyChecksum(const QByteArray &frame) const
{
    if (m_config.checksumType == ChecksumType::None) {
        return true;  // 无校验
    }

    // 确定校验范围
    int checksumStart = m_config.checksumStart;
    int checksumLength = m_config.checksumLength;

    if (checksumLength < 0) {
        // 计算到校验码位置
        int checksumSize = 1;
        if (m_config.checksumType == ChecksumType::CRC32) {
            checksumSize = 4;
        } else if (m_config.checksumType == ChecksumType::CRC16) {
            checksumSize = 2;
        }

        checksumLength = frame.size() - checksumStart - checksumSize;
        if (!m_config.frameFooter.isEmpty()) {
            checksumLength -= m_config.frameFooter.size();
        }
    }

    // 计算校验码
    quint32 calculated = 0;
    switch (m_config.checksumType) {
        case ChecksumType::Sum:
            calculated = ChecksumCalculator::calculateSum(frame, checksumStart, checksumLength);
            break;
        case ChecksumType::XOR:
            calculated = ChecksumCalculator::calculateXOR(frame, checksumStart, checksumLength);
            break;
        case ChecksumType::CRC8:
            calculated = ChecksumCalculator::calculateCRC8(frame, checksumStart, checksumLength);
            break;
        case ChecksumType::CRC16:
            calculated = ChecksumCalculator::calculateCRC16(frame, checksumStart, checksumLength);
            break;
        case ChecksumType::CRC32:
            calculated = ChecksumCalculator::calculateCRC32(frame, checksumStart, checksumLength);
            break;
        default:
            return false;
    }

    // 提取帧中的校验码
    int checksumPos = m_config.checksumPosition;
    if (checksumPos < 0) {
        // 校验码在帧尾前
        int checksumSize = 1;
        if (m_config.checksumType == ChecksumType::CRC32) {
            checksumSize = 4;
        } else if (m_config.checksumType == ChecksumType::CRC16) {
            checksumSize = 2;
        }

        checksumPos = frame.size() - checksumSize;
        if (!m_config.frameFooter.isEmpty()) {
            checksumPos -= m_config.frameFooter.size();
        }
    }

    // 读取校验码
    quint32 received = 0;
    if (m_config.checksumType == ChecksumType::CRC32) {
        if (checksumPos + 4 <= frame.size()) {
            received = m_converter.convertUInt32(frame, checksumPos).toUInt();
        }
    } else if (m_config.checksumType == ChecksumType::CRC16) {
        if (checksumPos + 2 <= frame.size()) {
            received = m_converter.convertUInt16(frame, checksumPos).toUInt();
        }
    } else {
        if (checksumPos + 1 <= frame.size()) {
            received = m_converter.convertUInt8(frame, checksumPos).toUInt();
        }
    }

    return calculated == received;
}

QVariant ProtocolParser::parseField(const QByteArray &frame, const FieldConfig &field) const
{
    // 调整偏移量（跳过帧头）
    int actualOffset = m_config.frameHeader.size() + field.elementHead;

    // 创建临时字段配置
    FieldConfig tempField = field;
    tempField.elementHead = actualOffset;

    // 使用转换器解析
    return m_converter.convert(frame, tempField);
}
