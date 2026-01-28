#ifndef CHECKSUMCALCULATOR_H
#define CHECKSUMCALCULATOR_H

#include <QByteArray>
#include <QtGlobal>

/**
 * @brief 校验码计算器
 *
 * 支持多种校验算法：
 * - Sum（累加和）
 * - XOR（异或）
 * - CRC8
 * - CRC16-MODBUS（多项式0xA001，初值0xFFFF）
 * - CRC16-CCITT（多项式0x1021，初值0xFFFF）
 * - CRC32（IEEE 802.3标准）
 */
class ChecksumCalculator
{
public:
    /**
     * @brief 计算累加和校验
     * @param data 数据
     * @param start 起始位置
     * @param length 长度
     * @return 校验码（8位）
     */
    static quint8 calculateSum(const QByteArray &data, int start, int length);

    /**
     * @brief 计算异或校验
     * @param data 数据
     * @param start 起始位置
     * @param length 长度
     * @return 校验码（8位）
     */
    static quint8 calculateXOR(const QByteArray &data, int start, int length);

    /**
     * @brief 计算CRC8校验
     * @param data 数据
     * @param start 起始位置
     * @param length 长度
     * @return 校验码（8位）
     */
    static quint8 calculateCRC8(const QByteArray &data, int start, int length);

    /**
     * @brief 计算CRC16-MODBUS校验
     * @param data 数据
     * @param start 起始位置
     * @param length 长度
     * @return 校验码（16位，多项式0xA001，初值0xFFFF）
     */
    static quint16 calculateCRC16_MODBUS(const QByteArray &data, int start, int length);

    /**
     * @brief 计算CRC16-CCITT校验
     * @param data 数据
     * @param start 起始位置
     * @param length 长度
     * @return 校验码（16位，多项式0x1021，初值0xFFFF）
     */
    static quint16 calculateCRC16_CCITT(const QByteArray &data, int start, int length);

    /**
     * @brief 计算CRC32校验（IEEE 802.3标准）
     * @param data 数据
     * @param start 起始位置
     * @param length 长度
     * @return 校验码（32位）
     */
    static quint32 calculateCRC32(const QByteArray &data, int start, int length);

private:
    ChecksumCalculator() = delete;  // 禁止实例化

    // CRC32查找表
    static const quint32 crc32Table[256];
};

#endif // CHECKSUMCALCULATOR_H
