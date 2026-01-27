#include "protocoltypeconverter.h"

// ========== DataType转换 ==========

::DataType ProtocolTypeConverter::uiToProtocolDataType(CommandSettingsDialog::DataType uiType)
{
    switch (uiType) {
        case CommandSettingsDialog::DataType::Int8:    return ::DataType::Int8;
        case CommandSettingsDialog::DataType::UInt8:   return ::DataType::UInt8;
        case CommandSettingsDialog::DataType::Int16:   return ::DataType::Int16;
        case CommandSettingsDialog::DataType::UInt16:  return ::DataType::UInt16;
        case CommandSettingsDialog::DataType::Int32:   return ::DataType::Int32;
        case CommandSettingsDialog::DataType::UInt32:  return ::DataType::UInt32;
        case CommandSettingsDialog::DataType::Float:   return ::DataType::Float;
        case CommandSettingsDialog::DataType::Double:  return ::DataType::Double;
        case CommandSettingsDialog::DataType::MByte:   return ::DataType::MByte;
        case CommandSettingsDialog::DataType::String:  return ::DataType::String;
        default: return ::DataType::Int16;
    }
}

CommandSettingsDialog::DataType ProtocolTypeConverter::protocolToUiDataType(::DataType protocolType)
{
    switch (protocolType) {
        case ::DataType::Int8:    return CommandSettingsDialog::DataType::Int8;
        case ::DataType::UInt8:   return CommandSettingsDialog::DataType::UInt8;
        case ::DataType::Int16:   return CommandSettingsDialog::DataType::Int16;
        case ::DataType::UInt16:  return CommandSettingsDialog::DataType::UInt16;
        case ::DataType::Int32:   return CommandSettingsDialog::DataType::Int32;
        case ::DataType::UInt32:  return CommandSettingsDialog::DataType::UInt32;
        case ::DataType::Float:   return CommandSettingsDialog::DataType::Float;
        case ::DataType::Double:  return CommandSettingsDialog::DataType::Double;
        case ::DataType::MByte:   return CommandSettingsDialog::DataType::MByte;
        case ::DataType::String:  return CommandSettingsDialog::DataType::String;
        default: return CommandSettingsDialog::DataType::Int16;
    }
}

// ========== ByteOrder转换 ==========

::ByteOrder ProtocolTypeConverter::uiToProtocolByteOrder(CommandSettingsDialog::ByteOrder uiOrder)
{
    switch (uiOrder) {
        case CommandSettingsDialog::ByteOrder::LittleEndian: return ::ByteOrder::LittleEndian;
        case CommandSettingsDialog::ByteOrder::BigEndian:    return ::ByteOrder::BigEndian;
        default: return ::ByteOrder::LittleEndian;
    }
}

CommandSettingsDialog::ByteOrder ProtocolTypeConverter::protocolToUiByteOrder(::ByteOrder protocolOrder)
{
    switch (protocolOrder) {
        case ::ByteOrder::LittleEndian: return CommandSettingsDialog::ByteOrder::LittleEndian;
        case ::ByteOrder::BigEndian:    return CommandSettingsDialog::ByteOrder::BigEndian;
        default: return CommandSettingsDialog::ByteOrder::LittleEndian;
    }
}

// ========== ChecksumType转换 ==========

::ChecksumType ProtocolTypeConverter::uiToProtocolChecksumType(CommandSettingsDialog::ChecksumType uiType)
{
    switch (uiType) {
        case CommandSettingsDialog::ChecksumType::None:   return ::ChecksumType::None;
        case CommandSettingsDialog::ChecksumType::Sum:    return ::ChecksumType::Sum;
        case CommandSettingsDialog::ChecksumType::XOR:    return ::ChecksumType::XOR;
        case CommandSettingsDialog::ChecksumType::CRC8:   return ::ChecksumType::CRC8;
        case CommandSettingsDialog::ChecksumType::CRC16:  return ::ChecksumType::CRC16;
        case CommandSettingsDialog::ChecksumType::CRC32:  return ::ChecksumType::CRC32;
        default: return ::ChecksumType::None;
    }
}

CommandSettingsDialog::ChecksumType ProtocolTypeConverter::protocolToUiChecksumType(::ChecksumType protocolType)
{
    switch (protocolType) {
        case ::ChecksumType::None:   return CommandSettingsDialog::ChecksumType::None;
        case ::ChecksumType::Sum:    return CommandSettingsDialog::ChecksumType::Sum;
        case ::ChecksumType::XOR:    return CommandSettingsDialog::ChecksumType::XOR;
        case ::ChecksumType::CRC8:   return CommandSettingsDialog::ChecksumType::CRC8;
        case ::ChecksumType::CRC16:  return CommandSettingsDialog::ChecksumType::CRC16;
        case ::ChecksumType::CRC32:  return CommandSettingsDialog::ChecksumType::CRC32;
        default: return CommandSettingsDialog::ChecksumType::None;
    }
}

// ========== FieldConfig转换 ==========

::FieldConfig ProtocolTypeConverter::uiToProtocolFieldConfig(const CommandSettingsDialog::FieldConfig &uiField)
{
    ::FieldConfig protocolField;
    protocolField.index = uiField.index;
    protocolField.elementHead = uiField.elementHead;
    protocolField.name = uiField.name;
    protocolField.type = uiToProtocolDataType(uiField.type);
    protocolField.byteLength = uiField.byteLength;
    protocolField.scale = uiField.scale;
    protocolField.offset = uiField.offset;
    protocolField.unit = uiField.unit;
    protocolField.maximum = uiField.maximum;
    protocolField.minimum = uiField.minimum;
    protocolField.description = uiField.description;
    protocolField.tip = uiField.tip;
    return protocolField;
}

CommandSettingsDialog::FieldConfig ProtocolTypeConverter::protocolToUiFieldConfig(const ::FieldConfig &protocolField)
{
    CommandSettingsDialog::FieldConfig uiField;
    uiField.index = protocolField.index;
    uiField.elementHead = protocolField.elementHead;
    uiField.name = protocolField.name;
    uiField.type = protocolToUiDataType(protocolField.type);
    uiField.byteLength = protocolField.byteLength;
    uiField.scale = protocolField.scale;
    uiField.offset = protocolField.offset;
    uiField.unit = protocolField.unit;
    uiField.maximum = protocolField.maximum;
    uiField.minimum = protocolField.minimum;
    uiField.description = protocolField.description;
    uiField.tip = protocolField.tip;
    return uiField;
}

// ========== ProtocolConfig转换 ==========

::ProtocolConfig ProtocolTypeConverter::uiToProtocolConfig(const CommandSettingsDialog::ProtocolConfig &uiConfig)
{
    ::ProtocolConfig protocolConfig;

    // 基本信息
    protocolConfig.name = uiConfig.name;
    protocolConfig.version = uiConfig.version;
    protocolConfig.description = uiConfig.description;

    // 帧格式配置
    // 将16进制字符串转换为QByteArray（移除空格后转换）
    QString headerHex = uiConfig.frameHeader;
    headerHex.remove(' ');
    protocolConfig.frameHeader = QByteArray::fromHex(headerHex.toLatin1());

    QString footerHex = uiConfig.frameFooter;
    footerHex.remove(' ');
    protocolConfig.frameFooter = QByteArray::fromHex(footerHex.toLatin1());
    protocolConfig.lengthPosition = uiConfig.lengthPosition;
    protocolConfig.checksumType = uiToProtocolChecksumType(uiConfig.checksumType);
    protocolConfig.checksumStart = uiConfig.checksumStart;
    protocolConfig.checksumLength = uiConfig.checksumLength;
    protocolConfig.checksumPosition = uiConfig.checksumPosition;
    protocolConfig.byteOrder = uiToProtocolByteOrder(uiConfig.byteOrder);
    protocolConfig.frequency = uiConfig.frequency;
    protocolConfig.separator = uiConfig.separator;

    // 数据字段配置
    for (const auto &uiField : uiConfig.fields) {
        protocolConfig.fields.append(uiToProtocolFieldConfig(uiField));
    }

    return protocolConfig;
}

CommandSettingsDialog::ProtocolConfig ProtocolTypeConverter::protocolToUiConfig(const ::ProtocolConfig &protocolConfig)
{
    CommandSettingsDialog::ProtocolConfig uiConfig;

    // 基本信息
    uiConfig.name = protocolConfig.name;
    uiConfig.version = protocolConfig.version;
    uiConfig.description = protocolConfig.description;

    // 帧格式配置
    // 将QByteArray转换为16进制字符串（每个字节用空格分隔）
    uiConfig.frameHeader = QString(protocolConfig.frameHeader.toHex(' ')).toUpper();
    uiConfig.frameFooter = QString(protocolConfig.frameFooter.toHex(' ')).toUpper();
    uiConfig.lengthPosition = protocolConfig.lengthPosition;
    uiConfig.checksumType = protocolToUiChecksumType(protocolConfig.checksumType);
    uiConfig.checksumStart = protocolConfig.checksumStart;
    uiConfig.checksumLength = protocolConfig.checksumLength;
    uiConfig.checksumPosition = protocolConfig.checksumPosition;
    uiConfig.byteOrder = protocolToUiByteOrder(protocolConfig.byteOrder);
    uiConfig.frequency = protocolConfig.frequency;
    uiConfig.separator = protocolConfig.separator;

    // 数据字段配置
    for (const auto &protocolField : protocolConfig.fields) {
        uiConfig.fields.append(protocolToUiFieldConfig(protocolField));
    }

    return uiConfig;
}
