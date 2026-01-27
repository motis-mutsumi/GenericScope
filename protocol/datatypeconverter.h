#ifndef DATATYPECONVERTER_H
#define DATATYPECONVERTER_H

#include "protocolconfig.h"
#include <QByteArray>
#include <QVariant>

/**
 * @brief 数据类型转换器
 *
 * 负责将二进制数据转换为各种数据类型：
 * - 基础类型：int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, float, double
 * - 特殊类型：mbyte_t（多字节整数+缩放因子）, string
 * - 支持大小端配置
 */
class DataTypeConverter
{
public:
    /**
     * @brief 构造函数
     * @param byteOrder 字节序
     */
    explicit DataTypeConverter(ByteOrder byteOrder = ByteOrder::LittleEndian);

    /**
     * @brief 设置字节序
     */
    void setByteOrder(ByteOrder byteOrder) { m_byteOrder = byteOrder; }

    /**
     * @brief 获取字节序
     */
    ByteOrder byteOrder() const { return m_byteOrder; }

    // ========== 基础类型转换 ==========

    QVariant convertInt8(const QByteArray &data, int offset) const;
    QVariant convertUInt8(const QByteArray &data, int offset) const;
    QVariant convertInt16(const QByteArray &data, int offset) const;
    QVariant convertUInt16(const QByteArray &data, int offset) const;
    QVariant convertInt32(const QByteArray &data, int offset) const;
    QVariant convertUInt32(const QByteArray &data, int offset) const;
    QVariant convertFloat(const QByteArray &data, int offset) const;
    QVariant convertDouble(const QByteArray &data, int offset) const;

    // ========== 特殊类型转换 ==========

    /**
     * @brief 转换多字节整数（mbyte_t）
     * @param data 数据
     * @param offset 偏移量
     * @param length 字节长度（1-8）
     * @param scale 缩放因子
     * @param offset_value 偏移值
     * @return 转换后的浮点数
     */
    QVariant convertMByte(const QByteArray &data, int offset, int length,
                          double scale, double offset_value) const;

    /**
     * @brief 转换字符串
     * @param data 数据
     * @param offset 偏移量
     * @param length 字符串长度
     * @return 字符串
     */
    QVariant convertString(const QByteArray &data, int offset, int length) const;

    /**
     * @brief 通用转换接口
     * @param data 数据
     * @param field 字段配置
     * @return 转换后的值
     */
    QVariant convert(const QByteArray &data, const FieldConfig &field) const;

private:
    ByteOrder m_byteOrder;
};

#endif // DATATYPECONVERTER_H
