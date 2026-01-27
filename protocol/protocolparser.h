#ifndef PROTOCOLPARSER_H
#define PROTOCOLPARSER_H

#include "protocolconfig.h"
#include "checksumcalculator.h"
#include "datatypeconverter.h"
#include <QByteArray>
#include <QMap>
#include <QVariant>
#include <QDateTime>

/**
 * @brief 解析结果结构
 */
struct ParseResult {
    bool success;                        // 是否解析成功
    QString errorMsg;                    // 错误信息
    QMap<QString, QVariant> fieldValues; // 字段名 -> 值
    QByteArray rawData;                  // 原始数据
    quint64 timestamp;                   // 时间戳（毫秒）

    ParseResult() : success(false), timestamp(0) {}
};

/**
 * @brief 协议解析器
 *
 * 根据协议配置解析二进制数据帧：
 * 1. 查找帧头
 * 2. 提取完整帧
 * 3. 验证校验码
 * 4. 解析所有字段
 */
class ProtocolParser
{
public:
    /**
     * @brief 构造函数
     * @param config 协议配置
     */
    explicit ProtocolParser(const ProtocolConfig &config);

    /**
     * @brief 解析数据帧
     * @param data 原始数据
     * @return 解析结果
     */
    ParseResult parse(const QByteArray &data);

    /**
     * @brief 查找帧头
     * @param data 数据
     * @param startPos 起始位置
     * @return 帧头位置（-1表示未找到）
     */
    int findFrameHeader(const QByteArray &data, int startPos = 0) const;

    /**
     * @brief 提取完整帧
     * @param data 数据
     * @param headerPos 帧头位置
     * @return 完整帧数据（空表示不完整）
     */
    QByteArray extractFrame(const QByteArray &data, int headerPos) const;

    /**
     * @brief 验证校验码
     * @param frame 帧数据
     * @return 是否通过校验
     */
    bool verifyChecksum(const QByteArray &frame) const;

    /**
     * @brief 解析单个字段
     * @param frame 帧数据
     * @param field 字段配置
     * @return 字段值
     */
    QVariant parseField(const QByteArray &frame, const FieldConfig &field) const;

    /**
     * @brief 获取配置
     */
    const ProtocolConfig& config() const { return m_config; }

    /**
     * @brief 更新配置
     */
    void setConfig(const ProtocolConfig &config);

private:
    ProtocolConfig m_config;
    DataTypeConverter m_converter;
};

#endif // PROTOCOLPARSER_H
