#ifndef PROTOCOLTYPECONVERTER_H
#define PROTOCOLTYPECONVERTER_H

#include "commandsettingsdialog.h"
#include "protocol/protocolconfig.h"

/**
 * @brief 协议类型转换工具类
 *
 * 用于在UI层类型（CommandSettingsDialog中定义）和
 * protocol模块类型之间进行转换
 */
class ProtocolTypeConverter
{
public:
    // ========== 枚举类型转换 ==========

    // DataType转换
    static ::DataType uiToProtocolDataType(CommandSettingsDialog::DataType uiType);
    static CommandSettingsDialog::DataType protocolToUiDataType(::DataType protocolType);

    // ByteOrder转换
    static ::ByteOrder uiToProtocolByteOrder(CommandSettingsDialog::ByteOrder uiOrder);
    static CommandSettingsDialog::ByteOrder protocolToUiByteOrder(::ByteOrder protocolOrder);

    // ChecksumType转换
    static ::ChecksumType uiToProtocolChecksumType(CommandSettingsDialog::ChecksumType uiType);
    static CommandSettingsDialog::ChecksumType protocolToUiChecksumType(::ChecksumType protocolType);

    // ChecksumScope转换
    static ::ChecksumScope uiToProtocolChecksumScope(CommandSettingsDialog::ChecksumScope uiScope);
    static CommandSettingsDialog::ChecksumScope protocolToUiChecksumScope(::ChecksumScope protocolScope);

    // ========== 结构体转换 ==========

    // FieldConfig转换
    static ::FieldConfig uiToProtocolFieldConfig(const CommandSettingsDialog::FieldConfig &uiField);
    static CommandSettingsDialog::FieldConfig protocolToUiFieldConfig(const ::FieldConfig &protocolField);

    // ProtocolConfig转换
    static ::ProtocolConfig uiToProtocolConfig(const CommandSettingsDialog::ProtocolConfig &uiConfig);
    static CommandSettingsDialog::ProtocolConfig protocolToUiConfig(const ::ProtocolConfig &protocolConfig);
};

#endif // PROTOCOLTYPECONVERTER_H
