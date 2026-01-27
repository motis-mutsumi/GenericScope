#include "datatypeconverter.h"
#include <QtEndian>
#include <cstring>

DataTypeConverter::DataTypeConverter(ByteOrder byteOrder)
    : m_byteOrder(byteOrder)
{
}

// ============================================================================
// 基础类型转换 - 8位
// ============================================================================

QVariant DataTypeConverter::convertInt8(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 1 > data.size()) {
        return QVariant();
    }

    qint8 value = static_cast<qint8>(data[offset]);
    return QVariant(value);
}

QVariant DataTypeConverter::convertUInt8(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 1 > data.size()) {
        return QVariant();
    }

    quint8 value = static_cast<quint8>(data[offset]);
    return QVariant(value);
}

// ============================================================================
// 基础类型转换 - 16位
// ============================================================================

QVariant DataTypeConverter::convertInt16(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 2 > data.size()) {
        return QVariant();
    }

    qint16 value;

    if (m_byteOrder == ByteOrder::LittleEndian) {
        // 小端序：低字节在前
        value = qFromLittleEndian<qint16>(reinterpret_cast<const uchar*>(data.constData() + offset));
    } else {
        // 大端序：高字节在前
        value = qFromBigEndian<qint16>(reinterpret_cast<const uchar*>(data.constData() + offset));
    }

    return QVariant(value);
}

QVariant DataTypeConverter::convertUInt16(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 2 > data.size()) {
        return QVariant();
    }

    quint16 value;

    if (m_byteOrder == ByteOrder::LittleEndian) {
        value = qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.constData() + offset));
    } else {
        value = qFromBigEndian<quint16>(reinterpret_cast<const uchar*>(data.constData() + offset));
    }

    return QVariant(value);
}

// ============================================================================
// 基础类型转换 - 32位
// ============================================================================

QVariant DataTypeConverter::convertInt32(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 4 > data.size()) {
        return QVariant();
    }

    qint32 value;

    if (m_byteOrder == ByteOrder::LittleEndian) {
        value = qFromLittleEndian<qint32>(reinterpret_cast<const uchar*>(data.constData() + offset));
    } else {
        value = qFromBigEndian<qint32>(reinterpret_cast<const uchar*>(data.constData() + offset));
    }

    return QVariant(value);
}

QVariant DataTypeConverter::convertUInt32(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 4 > data.size()) {
        return QVariant();
    }

    quint32 value;

    if (m_byteOrder == ByteOrder::LittleEndian) {
        value = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.constData() + offset));
    } else {
        value = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(data.constData() + offset));
    }

    return QVariant(value);
}

// ============================================================================
// 浮点类型转换
// ============================================================================

QVariant DataTypeConverter::convertFloat(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 4 > data.size()) {
        return QVariant();
    }

    // 先读取为32位整数，再转换为float
    quint32 intValue;

    if (m_byteOrder == ByteOrder::LittleEndian) {
        intValue = qFromLittleEndian<quint32>(reinterpret_cast<const uchar*>(data.constData() + offset));
    } else {
        intValue = qFromBigEndian<quint32>(reinterpret_cast<const uchar*>(data.constData() + offset));
    }

    // 使用memcpy进行类型转换（避免strict aliasing问题）
    float floatValue;
    std::memcpy(&floatValue, &intValue, sizeof(float));

    return QVariant(floatValue);
}

QVariant DataTypeConverter::convertDouble(const QByteArray &data, int offset) const
{
    if (offset < 0 || offset + 8 > data.size()) {
        return QVariant();
    }

    // 先读取为64位整数，再转换为double
    quint64 intValue;

    if (m_byteOrder == ByteOrder::LittleEndian) {
        intValue = qFromLittleEndian<quint64>(reinterpret_cast<const uchar*>(data.constData() + offset));
    } else {
        intValue = qFromBigEndian<quint64>(reinterpret_cast<const uchar*>(data.constData() + offset));
    }

    // 使用memcpy进行类型转换
    double doubleValue;
    std::memcpy(&doubleValue, &intValue, sizeof(double));

    return QVariant(doubleValue);
}

// ============================================================================
// 特殊类型转换 - 多字节整数
// ============================================================================

QVariant DataTypeConverter::convertMByte(const QByteArray &data, int offset,
                                          int length, double scale, double offset_value) const
{
    if (offset < 0 || offset + length > data.size() || length < 1 || length > 8) {
        return QVariant();
    }

    // 读取多字节整数（支持1-8字节）
    qint64 rawValue = 0;

    if (m_byteOrder == ByteOrder::LittleEndian) {
        // 小端序：低字节在前
        for (int i = 0; i < length; ++i) {
            rawValue |= (static_cast<qint64>(static_cast<quint8>(data[offset + i])) << (i * 8));
        }
    } else {
        // 大端序：高字节在前
        for (int i = 0; i < length; ++i) {
            rawValue |= (static_cast<qint64>(static_cast<quint8>(data[offset + i])) << ((length - 1 - i) * 8));
        }
    }

    // 处理符号位（如果是有符号数）
    int bitCount = length * 8;
    if (rawValue & (1LL << (bitCount - 1))) {
        // 负数：符号扩展
        rawValue |= (~0LL << bitCount);
    }

    // 应用缩放因子和偏移量
    double result = rawValue * scale + offset_value;

    return QVariant(result);
}

// ============================================================================
// 特殊类型转换 - 字符串
// ============================================================================

QVariant DataTypeConverter::convertString(const QByteArray &data, int offset, int length) const
{
    if (offset < 0 || offset + length > data.size()) {
        return QVariant();
    }

    // 提取字符串（去除尾部的0）
    QByteArray strData = data.mid(offset, length);
    int nullPos = strData.indexOf('\0');
    if (nullPos >= 0) {
        strData = strData.left(nullPos);
    }

    return QVariant(QString::fromLatin1(strData));
}

// ============================================================================
// 通用转换接口
// ============================================================================

QVariant DataTypeConverter::convert(const QByteArray &data, const FieldConfig &field) const
{
    switch (field.type) {
        case DataType::Int8:
            return convertInt8(data, field.elementHead);

        case DataType::UInt8:
            return convertUInt8(data, field.elementHead);

        case DataType::Int16:
            return convertInt16(data, field.elementHead);

        case DataType::UInt16:
            return convertUInt16(data, field.elementHead);

        case DataType::Int32:
            return convertInt32(data, field.elementHead);

        case DataType::UInt32:
            return convertUInt32(data, field.elementHead);

        case DataType::Float:
            return convertFloat(data, field.elementHead);

        case DataType::Double:
            return convertDouble(data, field.elementHead);

        case DataType::MByte:
            return convertMByte(data, field.elementHead, field.byteLength,
                               field.scale, field.offset);

        case DataType::String:
            return convertString(data, field.elementHead, field.byteLength);

        default:
            return QVariant();
    }
}
