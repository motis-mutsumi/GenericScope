#include "checksumcalculator.h"

quint8 ChecksumCalculator::calculateSum(const QByteArray &data, int start, int length)
{
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint8 sum = 0;
    for (int i = 0; i < length; ++i) {
        sum += static_cast<quint8>(data[start + i]);
    }
    return sum;
}

quint8 ChecksumCalculator::calculateXOR(const QByteArray &data, int start, int length)
{
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint8 xorValue = 0;
    for (int i = 0; i < length; ++i) {
        xorValue ^= static_cast<quint8>(data[start + i]);
    }
    return xorValue;
}

quint8 ChecksumCalculator::calculateCRC8(const QByteArray &data, int start, int length)
{
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint8 crc = 0x00;
    for (int i = 0; i < length; ++i) {
        crc ^= static_cast<quint8>(data[start + i]);
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// 项目中“CRC16”统一使用 XMODEM 规则：poly=0x1021, init=0x0000, refin=false, refout=false, xorout=0x0000
quint16 ChecksumCalculator::calculateCRC16_XMODEM(const QByteArray &data, int start, int length)
{
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint16 crc = 0x0000;
    for (int i = 0; i < length; ++i) {
        crc ^= static_cast<quint16>(static_cast<quint8>(data[start + i])) << 8;
        for (int j = 0; j < 8; ++j) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

quint32 ChecksumCalculator::calculateCRC32(const QByteArray &data, int start, int length)
{
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint32 crc = 0xFFFFFFFF;
    for (int i = 0; i < length; ++i) {
        quint8 byte = static_cast<quint8>(data[start + i]);
        crc ^= byte;
        for (int j = 0; j < 8; ++j) {
            if (crc & 1U) {
                crc = (crc >> 1) ^ 0xEDB88320U;
            } else {
                crc >>= 1;
            }
        }
    }
    return ~crc;
}
