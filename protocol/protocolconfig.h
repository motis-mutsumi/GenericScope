#ifndef PROTOCOLCONFIG_H
#define PROTOCOLCONFIG_H

#include <QString>
#include <QVector>
#include <QVariant>
#include <QJsonObject>
#include <QByteArray>

/**
 * @brief 协议配置数据结构
 *
 * 定义了完整的协议配置，包括：
 * - 协议基本信息（名称、版本、描述）
 * - 帧格式配置（帧头、帧尾、校验方式、字节序等）
 * - 数据字段配置（字段列表）
 *
 * 支持JSON序列化和反序列化
 */

// 数据类型枚举
enum class DataType {
    Int8,      // int8_t - 有符号8位整数
    UInt8,     // uint8_t - 无符号8位整数
    Int16,     // int16_t - 有符号16位整数
    UInt16,    // uint16_t - 无符号16位整数
    Int32,     // int32_t - 有符号32位整数
    UInt32,    // uint32_t - 无符号32位整数
    Float,     // float - 32位浮点数
    Double,    // double - 64位浮点数
    MByte,     // 多字节整数（需要缩放因子）
    String     // 字符串
};

// 字节序枚举
enum class ByteOrder {
    LittleEndian,  // 小端序（低字节在前）
    BigEndian      // 大端序（高字节在前）
};

// 校验方式枚举
enum class ChecksumType {
    None,    // 无校验
    Sum,     // 累加和
    XOR,     // 异或
    CRC8,    // CRC8
    CRC16,   // CRC16（MODBUS）
    CRC32    // CRC32（IEEE 802.3）
};

/**
 * @brief 数据字段配置结构
 */
struct FieldConfig {
    int index;               // 字段索引
    int elementHead;         // 起始位置（字节偏移）
    QString name;            // 字段名称
    DataType type;           // 数据类型
    int byteLength;          // 字节长度
    double scale;            // 缩放因子
    double offset;           // 偏移量
    QString unit;            // 单位
    double maximum;          // 最大值
    double minimum;          // 最小值
    QString description;     // 描述
    QString tip;             // 提示信息

    // 运行时数据
    QVariant value;          // 当前值

    // 构造函数
    FieldConfig();

    // JSON序列化
    QJsonObject toJson() const;
    static FieldConfig fromJson(const QJsonObject &json);
};

/**
 * @brief 协议配置类
 */
class ProtocolConfig {
public:
    ProtocolConfig();

    // 协议基本信息
    QString name;               // 协议名称
    QString version;            // 协议版本
    QString description;        // 协议描述

    // 帧格式配置
    QByteArray frameHeader;     // 帧头（二进制）
    QByteArray frameFooter;     // 帧尾（二进制，可选）
    int lengthPosition;         // 长度字段位置（-1表示无长度字段）
    ChecksumType checksumType;  // 校验方式
    int checksumStart;          // 校验起始位置
    int checksumLength;         // 校验字节数（-1表示到帧尾）
    int checksumPosition;       // 校验码位置（-1表示帧尾前）
    ByteOrder byteOrder;        // 字节序
    int frequency;              // 数据频率（Hz）
    QString separator;          // 分隔符（文本协议）

    // 数据字段配置
    QVector<FieldConfig> fields;

    // JSON序列化
    QJsonObject toJson() const;
    static ProtocolConfig fromJson(const QJsonObject &json);

    // 保存/加载
    bool saveToFile(const QString &filePath) const;
    static ProtocolConfig loadFromFile(const QString &filePath);

    // 验证配置有效性
    bool validate(QString *errorMsg = nullptr) const;
};

#endif // PROTOCOLCONFIG_H
