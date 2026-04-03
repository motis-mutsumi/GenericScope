#include "protocolparser.h"
#include <QDebug>

ProtocolParser::ProtocolParser(const ProtocolConfig &config)
    : m_config(config)
    , m_converter(config.byteOrder)
    , m_checksumConverter(config.checksumByteOrder)
{
}

void ProtocolParser::setConfig(const ProtocolConfig &config)
{
    m_config = config;
    m_converter.setByteOrder(config.byteOrder);
    m_checksumConverter.setByteOrder(config.checksumByteOrder);
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
    // 兼容部分协议的长度字段语义：长度不含末尾校验码（常见于二进制IMU协议）
    if (!verifyChecksum(frame)) {
        bool recoveredByLengthPlusChecksum = false;
        if (m_config.lengthPosition >= 0 && m_config.checksumType != ChecksumType::None) {
            const int checksumSize = getChecksumSize();
            const int recoveredLen = frame.size() + checksumSize;
            if (headerPos + recoveredLen <= data.size()) {
                QByteArray altFrame = data.mid(headerPos, recoveredLen);
                if (verifyChecksum(altFrame)) {
                    frame = altFrame;
                    recoveredByLengthPlusChecksum = true;
                    qDebug() << "Length-field compatibility path used: length excludes checksum, recovered with +"
                             << checksumSize << "bytes";
                }
            } else {
                // 当前缓冲区仅够“header + length”语义，但不够“header + length + checksum”语义。
                // 这通常是串口分包导致，必须等待后续字节，不能误判为CRC错误并丢弃数据。
                result.errorMsg = "Incomplete frame";
                return result;
            }
        }

        if (!recoveredByLengthPlusChecksum) {
            // 保留本次用于校验的原始帧，便于上层日志定位CRC不匹配原因
            result.rawData = frame;
            result.errorMsg = "Checksum verification failed";
            return result;
        }
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

    // 计算已消耗的字节数（用于缓冲区管理）
    // consumedBytes = 帧起始位置 + 帧长度
    result.consumedBytes = headerPos + frame.size();

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
    // 边界检查
    if (headerPos < 0 || headerPos >= data.size()) {
        qWarning() << "Invalid headerPos:" << headerPos << "data size:" << data.size();
        return QByteArray();
    }

    int frameStart = headerPos;
    int frameLength = 0;

    // 方法1：根据长度字段确定帧长度
    if (m_config.lengthPosition >= 0) {
        int lengthPos = frameStart + m_config.lengthPosition;
        if (lengthPos + 1 > data.size()) {
            qWarning() << "Length field position out of bounds:" << lengthPos;
            return QByteArray();
        }
        if (lengthPos + 1 <= data.size()) {
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
        frameLength += getChecksumSize();

        // 加上帧尾长度
        if (!m_config.frameFooter.isEmpty()) {
            frameLength += m_config.frameFooter.size();
        }
    }

    // 检查帧长度的合理性
    if (frameLength <= 0) {
        qWarning() << "Invalid frame length:" << frameLength;
        return QByteArray();
    }

    // 检查数据是否足够
    if (frameStart + frameLength > data.size()) {
        return QByteArray();  // 数据不完整（串口分包场景常见，静默等待下一包）
    }

    return data.mid(frameStart, frameLength);
}

bool ProtocolParser::verifyChecksum(const QByteArray &frame) const
{
    if (m_config.checksumType == ChecksumType::None) {
        return true;  // 无校验
    }

    // 根据checksumScope自动计算校验起始位置
    int checksumStart = m_config.checksumStart;
    switch (m_config.checksumScope) {
        case ChecksumScope::FullFrame:
            checksumStart = 0;  // 从帧起始（包含帧头）
            break;
        case ChecksumScope::AfterHeader:
            checksumStart = m_config.frameHeader.size();  // 从帧头后
            break;
        case ChecksumScope::DataOnly:
            // 从数据字段开始（排除帧头和长度字段）
            checksumStart = m_config.frameHeader.size();
            if (m_config.lengthPosition >= 0) {
                checksumStart = m_config.lengthPosition + 1;  // 长度字段后
            }
            break;
        case ChecksumScope::Custom:
            // 使用配置的checksumStart值
            break;
    }

    // 边界检查
    if (checksumStart < 0 || checksumStart >= frame.size()) {
        qWarning() << "Invalid checksumStart:" << checksumStart << "frame size:" << frame.size();
        return false;
    }

    // 确定校验长度
    int checksumLength = m_config.checksumLength;

    if (checksumLength < 0) {
        // 计算到校验码位置
        int checksumSize = getChecksumSize();

        checksumLength = frame.size() - checksumStart - checksumSize;
        if (!m_config.frameFooter.isEmpty()) {
            checksumLength -= m_config.frameFooter.size();
        }
    }

    // 边界检查：校验长度必须为正数
    if (checksumLength <= 0) {
        qWarning() << "Invalid checksum length:" << checksumLength
                   << "frame size:" << frame.size()
                   << "checksumStart:" << checksumStart;
        return false;
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
        case ChecksumType::CRC16_XMODEM:
            calculated = ChecksumCalculator::calculateCRC16_XMODEM(frame, checksumStart, checksumLength);
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
        int checksumSize = getChecksumSize();

        checksumPos = frame.size() - checksumSize;
        if (!m_config.frameFooter.isEmpty()) {
            checksumPos -= m_config.frameFooter.size();
        }
    }

    // 读取校验码（使用独立的校验码转换器）
    quint32 received = 0;
    if (m_config.checksumType == ChecksumType::CRC32) {
        if (checksumPos + 4 <= frame.size()) {
            received = m_checksumConverter.convertUInt32(frame, checksumPos).toUInt();
        }
    } else if (m_config.checksumType == ChecksumType::CRC16_XMODEM) {
        if (checksumPos + 2 <= frame.size()) {
            received = m_checksumConverter.convertUInt16(frame, checksumPos).toUInt();
        }
    } else {
        if (checksumPos + 1 <= frame.size()) {
            received = m_checksumConverter.convertUInt8(frame, checksumPos).toUInt();
        }
    }

    return calculated == received;
}

QVariant ProtocolParser::parseField(const QByteArray &frame, const FieldConfig &field) const
{
    // 调整偏移量（跳过帧头）
    int actualOffset = m_config.frameHeader.size() + field.elementHead;

    // 边界检查
    if (actualOffset < 0 || actualOffset + field.byteLength > frame.size()) {
        qWarning() << "Field" << field.name << "out of bounds: offset=" << actualOffset
                   << "length=" << field.byteLength << "frame size=" << frame.size();
        return QVariant();
    }

    // 创建临时字段配置
    FieldConfig tempField = field;
    tempField.elementHead = actualOffset;

    // 使用转换器解析
    return m_converter.convert(frame, tempField);
}

// ============================================================================
// 私有辅助函数
// ============================================================================

int ProtocolParser::getChecksumSize() const
{
    switch (m_config.checksumType) {
        case ChecksumType::CRC32:
            return 4;
        case ChecksumType::CRC16_XMODEM:
            return 2;
        case ChecksumType::Sum:
        case ChecksumType::XOR:
        case ChecksumType::CRC8:
            return 1;
        case ChecksumType::None:
        default:
            return 0;
    }
}
