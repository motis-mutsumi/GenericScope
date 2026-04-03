#include "protocolconfig.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
// ============================================================================
// FieldConfig 实现
// ============================================================================
FieldConfig::FieldConfig()
    : index(0)
    , elementHead(0)
    , type(DataType::Int16)
    , byteLength(2)
    , scale(1.0)
    , offset(0.0)
    , maximum(0.0)
    , minimum(0.0)
{
}
QJsonObject FieldConfig::toJson() const
{
    QJsonObject json;
    json["index"] = index;
    json["elementHead"] = elementHead;
    json["name"] = name;
    // 数据类型转换为字符串
    QString typeStr;
    switch (type) {
        case DataType::Int8: typeStr = "int8_t"; break;
        case DataType::UInt8: typeStr = "uint8_t"; break;
        case DataType::Int16: typeStr = "int16_t"; break;
        case DataType::UInt16: typeStr = "uint16_t"; break;
        case DataType::Int32: typeStr = "int32_t"; break;
        case DataType::UInt32: typeStr = "uint32_t"; break;
        case DataType::Float: typeStr = "float"; break;
        case DataType::Double: typeStr = "double"; break;
        case DataType::MByte: typeStr = "mbyte_t"; break;
        case DataType::String: typeStr = "string"; break;
        default: typeStr = "int16_t"; break;
    }
    json["type"] = typeStr;
    json["byteLength"] = byteLength;
    json["scale"] = scale;
    json["offset"] = offset;
    json["unit"] = unit;
    json["maximum"] = maximum;
    json["minimum"] = minimum;
    json["description"] = description;
    json["tip"] = tip;
    return json;
}
FieldConfig FieldConfig::fromJson(const QJsonObject &json)
{
    FieldConfig field;
    field.index = json["index"].toInt();
    field.elementHead = json["elementHead"].toInt();
    field.name = json["name"].toString();
    // 字符串转换为数据类型
    QString typeStr = json["type"].toString();
    if (typeStr == "int8_t") field.type = DataType::Int8;
    else if (typeStr == "uint8_t") field.type = DataType::UInt8;
    else if (typeStr == "int16_t") field.type = DataType::Int16;
    else if (typeStr == "uint16_t") field.type = DataType::UInt16;
    else if (typeStr == "int32_t") field.type = DataType::Int32;
    else if (typeStr == "uint32_t") field.type = DataType::UInt32;
    else if (typeStr == "float") field.type = DataType::Float;
    else if (typeStr == "double") field.type = DataType::Double;
    else if (typeStr == "mbyte_t") field.type = DataType::MByte;
    else if (typeStr == "string") field.type = DataType::String;
    else field.type = DataType::Int16;
    field.byteLength = json["byteLength"].toInt();
    field.scale = json["scale"].toDouble(1.0);
    field.offset = json["offset"].toDouble(0.0);
    field.unit = json["unit"].toString();
    field.maximum = json["maximum"].toDouble(0.0);
    field.minimum = json["minimum"].toDouble(0.0);
    field.description = json["description"].toString();
    field.tip = json["tip"].toString();
    return field;
}
// ============================================================================
// ProtocolConfig 实现
// ============================================================================
ProtocolConfig::ProtocolConfig()
    : lengthPosition(-1)
    , checksumType(ChecksumType::None)
    , checksumScope(ChecksumScope::AfterHeader)  // 默认从帧头后开始（常见场景）
    , checksumStart(0)
    , checksumLength(-1)
    , checksumPosition(-1)
    , byteOrder(ByteOrder::LittleEndian)
    , checksumByteOrder(ByteOrder::LittleEndian)  // 默认与数据字节序相同
    , frequency(1000)
{
}
QJsonObject ProtocolConfig::toJson() const
{
    QJsonObject json;
    // 协议基本信息
    json["name"] = name;
    json["version"] = version;
    json["description"] = description;
    // 帧格式配置
    QJsonObject frameFormat;
    frameFormat["header"] = QString::fromLatin1(frameHeader.toHex(' ').toUpper());
    frameFormat["footer"] = QString::fromLatin1(frameFooter.toHex(' ').toUpper());
    frameFormat["lengthPosition"] = lengthPosition;
    // 校验方式转换为字符串
    QString checksumStr;
    switch (checksumType) {
        case ChecksumType::None: checksumStr = "None"; break;
        case ChecksumType::Sum: checksumStr = "Sum"; break;
        case ChecksumType::XOR: checksumStr = "XOR"; break;
        case ChecksumType::CRC8: checksumStr = "CRC8"; break;
        case ChecksumType::CRC16_XMODEM: checksumStr = "CRC16"; break;
        case ChecksumType::CRC32: checksumStr = "CRC32"; break;
        default: checksumStr = "None"; break;
    }
    frameFormat["checksumType"] = checksumStr;
    // 校验范围转换为字符串
    QString checksumScopeStr;
    switch (checksumScope) {
        case ChecksumScope::FullFrame: checksumScopeStr = "FullFrame"; break;
        case ChecksumScope::AfterHeader: checksumScopeStr = "AfterHeader"; break;
        case ChecksumScope::DataOnly: checksumScopeStr = "DataOnly"; break;
        case ChecksumScope::Custom: checksumScopeStr = "Custom"; break;
        default: checksumScopeStr = "AfterHeader"; break;
    }
    frameFormat["checksumScope"] = checksumScopeStr;
    frameFormat["checksumStart"] = checksumStart;
    frameFormat["checksumLength"] = checksumLength;
    frameFormat["checksumPosition"] = checksumPosition;
    frameFormat["byteOrder"] = (byteOrder == ByteOrder::LittleEndian) ? "LittleEndian" : "BigEndian";
    frameFormat["checksumByteOrder"] = (checksumByteOrder == ByteOrder::LittleEndian) ? "LittleEndian" : "BigEndian";
    frameFormat["frequency"] = frequency;
    frameFormat["separator"] = separator;
    json["frameFormat"] = frameFormat;
    // 数据字段配置
    QJsonArray fieldsArray;
    for (const FieldConfig &field : fields) {
        fieldsArray.append(field.toJson());
    }
    json["fields"] = fieldsArray;
    return json;
}
ProtocolConfig ProtocolConfig::fromJson(const QJsonObject &json)
{
    ProtocolConfig config;
    // 协议基本信息
    config.name = json["name"].toString();
    config.version = json["version"].toString();
    config.description = json["description"].toString();
    // 帧格式配置
    QJsonObject frameFormat = json["frameFormat"].toObject();
    // 解析帧头（16进制字符串转二进制）
    QString headerStr = frameFormat["header"].toString();
    config.frameHeader = QByteArray::fromHex(headerStr.remove(' ').toLatin1());
    // 解析帧尾
    QString footerStr = frameFormat["footer"].toString();
    config.frameFooter = QByteArray::fromHex(footerStr.remove(' ').toLatin1());
    config.lengthPosition = frameFormat["lengthPosition"].toInt(-1);
    // 解析校验方式
    QString checksumStr = frameFormat["checksumType"].toString();
    if (checksumStr == "Sum") config.checksumType = ChecksumType::Sum;
    else if (checksumStr == "XOR") config.checksumType = ChecksumType::XOR;
    else if (checksumStr == "CRC8") config.checksumType = ChecksumType::CRC8;
    else if (checksumStr == "CRC16") config.checksumType = ChecksumType::CRC16_XMODEM;
    else if (checksumStr == "CRC32") config.checksumType = ChecksumType::CRC32;
    else config.checksumType = ChecksumType::None;
    // 解析校验范围
    QString checksumScopeStr = frameFormat["checksumScope"].toString();
    if (checksumScopeStr == "FullFrame") config.checksumScope = ChecksumScope::FullFrame;
    else if (checksumScopeStr == "AfterHeader") config.checksumScope = ChecksumScope::AfterHeader;
    else if (checksumScopeStr == "DataOnly") config.checksumScope = ChecksumScope::DataOnly;
    else if (checksumScopeStr == "Custom") config.checksumScope = ChecksumScope::Custom;
    else if (checksumScopeStr.isEmpty()) config.checksumScope = ChecksumScope::Custom;  // 向后兼容：旧配置没有此字段
    else config.checksumScope = ChecksumScope::AfterHeader;
    config.checksumStart = frameFormat["checksumStart"].toInt(0);
    config.checksumLength = frameFormat["checksumLength"].toInt(-1);
    config.checksumPosition = frameFormat["checksumPosition"].toInt(-1);
    QString byteOrderStr = frameFormat["byteOrder"].toString();
    config.byteOrder = (byteOrderStr == "LittleEndian") ? ByteOrder::LittleEndian : ByteOrder::BigEndian;
    // 读取校验码字节序，如果不存在则使用数据字节序
    QString checksumByteOrderStr = frameFormat["checksumByteOrder"].toString();
    if (!checksumByteOrderStr.isEmpty()) {
        config.checksumByteOrder = (checksumByteOrderStr == "LittleEndian") ? ByteOrder::LittleEndian : ByteOrder::BigEndian;
    } else {
        config.checksumByteOrder = config.byteOrder;  // 向后兼容：默认与数据字节序相同
    }
    config.frequency = frameFormat["frequency"].toInt(1000);
    config.separator = frameFormat["separator"].toString();
    // 数据字段配置
    QJsonArray fieldsArray = json["fields"].toArray();
    for (const QJsonValue &value : fieldsArray) {
        config.fields.append(FieldConfig::fromJson(value.toObject()));
    }
    return config;
}
bool ProtocolConfig::saveToFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }
    QJsonDocument doc(toJson());
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
ProtocolConfig ProtocolConfig::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for reading:" << filePath;
        return ProtocolConfig();
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON format:" << filePath;
        return ProtocolConfig();
    }
    return fromJson(doc.object());
}
bool ProtocolConfig::validate(QString *errorMsg) const
{
    // 验证协议名称
    if (name.isEmpty()) {
        if (errorMsg) *errorMsg = "Protocol name cannot be empty";
        return false;
    }
    // 验证帧头
    if (frameHeader.isEmpty()) {
        if (errorMsg) *errorMsg = "Frame header cannot be empty";
        return false;
    }
    // 验证字段
    if (fields.isEmpty()) {
        if (errorMsg) *errorMsg = "At least one field is required";
        return false;
    }
    return true;
}
