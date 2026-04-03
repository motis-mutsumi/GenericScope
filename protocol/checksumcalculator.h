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
 * - CRC16/XMODEM（多项式0x1021，初值0x0000，不反转）
 * - CRC32（IEEE 802.3标准）
 */
class ChecksumCalculator
{
public:
    static quint8 calculateSum(const QByteArray &data, int start, int length);
    static quint8 calculateXOR(const QByteArray &data, int start, int length);
    static quint8 calculateCRC8(const QByteArray &data, int start, int length);

    /**
     * @brief 计算CRC16校验（当前项目中作为CRC16/XMODEM使用）
     */
    static quint16 calculateCRC16_XMODEM(const QByteArray &data, int start, int length);

    static quint32 calculateCRC32(const QByteArray &data, int start, int length);

private:
    ChecksumCalculator() = delete;
};

#endif // CHECKSUMCALCULATOR_H
