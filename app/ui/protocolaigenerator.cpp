#include "protocolaigenerator.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QDebug>
#include <QUrlQuery>

// Claude API配置
const QString ProtocolAIGenerator::kDefaultBaseUrl = "https://api.anthropic.com";
const QString ProtocolAIGenerator::kModelName = "claude-sonnet-4-5-20250929";  // Claude Sonnet 4.5 最新版本
const int ProtocolAIGenerator::kMaxTokens = 32000;  // 增大token限制，支持复杂协议生成
const QString ProtocolAIGenerator::kSystemPrompt =
    "你是一个专业的协议解析专家，擅长分析二进制通信协议的帧结构、校验算法和数据字段。"
    "你的任务是根据用户提供的16进制数据样本和解析规则，生成完整准确的协议配置JSON。"
    "请严格按照指定的JSON格式输出，确保字段完整、类型正确、数值合理。";

ProtocolAIGenerator::ProtocolAIGenerator(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
{
}

ProtocolAIGenerator::~ProtocolAIGenerator()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

void ProtocolAIGenerator::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void ProtocolAIGenerator::setBaseUrl(const QString &baseUrl)
{
    m_baseUrl = baseUrl;
}

void ProtocolAIGenerator::generateProtocol(const QString &protocolName,
                                          const QString &rawDataSample,
                                          const QString &parseRules)
{
    if (m_apiKey.isEmpty()) {
        emit generationFailed("API密钥未设置。请在环境变量ANTHROPIC_API_KEY中配置，或在设置中输入。");
        return;
    }

    if (m_currentReply) {
        emit generationFailed("已有生成任务在进行中，请等待完成或取消后重试。");
        return;
    }

    m_currentProtocolName = protocolName;
    emit progressUpdate("正在构建请求...");

    // 构建提示词
    QString prompt = buildPrompt(protocolName, rawDataSample, parseRules);

    // 构建API请求
    QJsonObject requestJson = buildApiRequest(prompt);

    // 创建网络请求 - 使用动态 URL
    QString baseUrl = m_baseUrl.isEmpty() ? kDefaultBaseUrl : m_baseUrl;
    QString apiEndpoint = baseUrl + "/v1/messages";

    QNetworkRequest request(apiEndpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("x-api-key", m_apiKey.toUtf8());
    request.setRawHeader("anthropic-version", "2023-06-01");

    emit progressUpdate("正在调用Claude API分析协议...");

    // 发送POST请求
    QJsonDocument doc(requestJson);
    m_currentReply = m_networkManager->post(request, doc.toJson());

    // 连接信号
    connect(m_currentReply, &QNetworkReply::finished,
            this, &ProtocolAIGenerator::onNetworkReplyFinished);
    // Qt 5.14 兼容：使用 error 信号而不是 errorOccurred (Qt 5.15+)
    // 使用传统函数指针方式代替 QOverload，兼容性更好
    void (QNetworkReply::*errorSignal)(QNetworkReply::NetworkError) = &QNetworkReply::error;
    connect(m_currentReply, errorSignal, this, &ProtocolAIGenerator::onNetworkError);
}

void ProtocolAIGenerator::cancel()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        emit progressUpdate("已取消生成");
    }
}

CommandSettingsDialog::ProtocolConfig ProtocolAIGenerator::getLastGeneratedConfig() const
{
    return m_lastConfig;
}

QString ProtocolAIGenerator::buildPrompt(const QString &protocolName,
                                        const QString &rawDataSample,
                                        const QString &parseRules) const
{
    QString prompt = QString(R"(你是一个协议解析专家。请根据以下信息生成完整的协议配置。

# 协议名称
%1

# 原始16进制数据样本
%2

# 解析规则描述
%3

# 任务要求
请分析上述信息，生成完整的协议配置JSON。你需要：

1. **分析帧结构**：识别帧头、帧尾、长度字段、校验码位置
2. **推断校验方式**：根据数据特征推断可能的校验算法（Sum/XOR/CRC8/CRC16/CRC32）
3. **解析数据字段**：根据解析规则，确定每个字段的：
   - 起始位置（字节偏移）
   - 数据类型（int8/uint8/int16/uint16/int32/uint32/float/double）
   - 字节长度
   - 缩放因子（scale）
   - 偏移量（offset）
   - 单位
   - 合理的最大/最小值范围

4. **字节序判断**：分析数据是大端序还是小端序

# 输出格式
请严格按照以下JSON格式输出，不要添加任何额外的解释文字：

```json
{
  "name": "协议名称",
  "version": "1.0.0",
  "description": "协议描述",
  "frameHeader": "帧头16进制（如 AA 55）",
  "frameFooter": "帧尾16进制（如 0D 0A，无帧尾则为空字符串）",
  "lengthPosition": 长度字段位置（-1表示无长度字段）,
  "checksumType": "校验类型：None/Sum/XOR/CRC8/CRC16/CRC32",
  "checksumScope": "校验范围：FullFrame/AfterHeader/DataOnly/Custom",
  "checksumStart": 校验起始位置,
  "checksumLength": 校验字节数（-1表示到帧尾）,
  "checksumPosition": 校验码位置（-1表示帧尾前）,
  "byteOrder": "字节序：LittleEndian/BigEndian",
  "checksumByteOrder": "校验码字节序：LittleEndian/BigEndian",
  "frequency": 数据频率（Hz）,
  "separator": "分隔符（文本协议用）",
  "fields": [
    {
      "index": 1,
      "elementHead": 起始字节位置,
      "name": "字段名称",
      "type": "数据类型：Int8/UInt8/Int16/UInt16/Int32/UInt32/Float/Double/MByte/String",
      "byteLength": 字节长度,
      "scale": 缩放因子,
      "offset": 偏移量,
      "unit": "单位",
      "maximum": 最大值,
      "minimum": 最小值,
      "description": "字段描述",
      "tip": "提示信息"
    }
  ]
}
```

# 注意事项
1. 帧头、帧尾使用空格分隔的16进制字节，如 "AA 55"
2. 位置索引从0开始
3. 缩放因子和偏移量用于计算实际值：实际值 = (原始值 * scale) + offset
4. 字段index从1开始递增
5. 只输出JSON，不要有其他文字
)").arg(protocolName).arg(rawDataSample).arg(parseRules);

    return prompt;
}

QJsonObject ProtocolAIGenerator::buildApiRequest(const QString &prompt) const
{
    QJsonObject request;
    request["model"] = kModelName;
    request["max_tokens"] = kMaxTokens;
    // 不设置 temperature，让 API 使用默认值

    // 添加系统提示（数组格式，与 ai.txt 成功案例一致）
    QJsonArray systemArray;
    QJsonObject systemPrompt;
    systemPrompt["type"] = "text";
    systemPrompt["text"] = kSystemPrompt;
    systemArray.append(systemPrompt);
    request["system"] = systemArray;

    // 构建用户消息数组
    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;
    messages.append(message);

    request["messages"] = messages;

    return request;
}

void ProtocolAIGenerator::onNetworkReplyFinished()
{
    if (!m_currentReply) {
        return;
    }

    emit progressUpdate("正在解析AI响应...");

    // 读取响应数据
    QByteArray responseData = m_currentReply->readAll();
    int statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    m_currentReply->deleteLater();
    m_currentReply = nullptr;

    // 检查HTTP状态码
    if (statusCode != 200) {
        QString errorMsg = QString("API请求失败 (HTTP %1)").arg(statusCode);
        QJsonDocument doc = QJsonDocument::fromJson(responseData);
        if (!doc.isNull()) {
            QJsonObject obj = doc.object();
            if (obj.contains("error")) {
                QString errType = obj["error"].toObject()["type"].toString();
                QString errMsg = obj["error"].toObject()["message"].toString();
                errorMsg = QString("%1: %2").arg(errType).arg(errMsg);
            }
        }
        emit generationFailed(errorMsg);
        return;
    }

    // 解析JSON响应
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    if (doc.isNull() || !doc.isObject()) {
        emit generationFailed("无效的API响应格式");
        return;
    }

    QJsonObject responseObj = doc.object();
    if (!responseObj.contains("content")) {
        emit generationFailed("API响应缺少content字段");
        return;
    }

    // 提取AI返回的文本
    QJsonArray contentArray = responseObj["content"].toArray();
    if (contentArray.isEmpty()) {
        emit generationFailed("API响应content为空");
        return;
    }

    QString aiResponse = contentArray[0].toObject()["text"].toString();
    qDebug() << "AI Response:" << aiResponse;

    // 解析AI返回的协议配置
    QString errorMsg;
    if (!parseAIResponse(aiResponse, m_lastConfig, &errorMsg)) {
        emit generationFailed(QString("解析AI响应失败: %1").arg(errorMsg));
        return;
    }

    // 设置协议名称
    m_lastConfig.name = m_currentProtocolName;

    emit progressUpdate("协议配置生成成功！");
    emit generationComplete(m_lastConfig);
}

void ProtocolAIGenerator::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    if (m_currentReply) {
        QString errorString = m_currentReply->errorString();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        emit generationFailed(QString("网络错误: %1").arg(errorString));
    }
}

bool ProtocolAIGenerator::parseAIResponse(const QString &response,
                                         CommandSettingsDialog::ProtocolConfig &config,
                                         QString *errorMsg)
{
    // 提取JSON部分（AI可能返回```json...```格式）
    QString jsonStr = response.trimmed();

    // 移除markdown代码块标记
    if (jsonStr.startsWith("```json")) {
        int start = jsonStr.indexOf('{');
        int end = jsonStr.lastIndexOf('}');
        if (start >= 0 && end > start) {
            jsonStr = jsonStr.mid(start, end - start + 1);
        }
    } else if (jsonStr.startsWith("```")) {
        int start = jsonStr.indexOf('{');
        int end = jsonStr.lastIndexOf('}');
        if (start >= 0 && end > start) {
            jsonStr = jsonStr.mid(start, end - start + 1);
        }
    }

    // 解析JSON
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        if (errorMsg) *errorMsg = "无法解析JSON格式";
        return false;
    }

    QJsonObject obj = doc.object();

    // 解析基本信息
    config.name = obj["name"].toString();
    config.version = obj["version"].toString("1.0.0");
    config.description = obj["description"].toString();

    // 解析帧格式
    config.frameHeader = obj["frameHeader"].toString();
    config.frameFooter = obj["frameFooter"].toString();
    config.lengthPosition = obj["lengthPosition"].toInt(-1);

    // 解析校验类型
    QString checksumTypeStr = obj["checksumType"].toString("None");
    if (checksumTypeStr == "None") config.checksumType = CommandSettingsDialog::ChecksumType::None;
    else if (checksumTypeStr == "Sum") config.checksumType = CommandSettingsDialog::ChecksumType::Sum;
    else if (checksumTypeStr == "XOR") config.checksumType = CommandSettingsDialog::ChecksumType::XOR;
    else if (checksumTypeStr == "CRC8") config.checksumType = CommandSettingsDialog::ChecksumType::CRC8;
    else if (checksumTypeStr == "CRC16" || checksumTypeStr == "CRC16_XMODEM") config.checksumType = CommandSettingsDialog::ChecksumType::CRC16_XMODEM;
    else if (checksumTypeStr == "CRC32") config.checksumType = CommandSettingsDialog::ChecksumType::CRC32;

    // 解析校验范围
    QString checksumScopeStr = obj["checksumScope"].toString("AfterHeader");
    if (checksumScopeStr == "FullFrame") config.checksumScope = CommandSettingsDialog::ChecksumScope::FullFrame;
    else if (checksumScopeStr == "AfterHeader") config.checksumScope = CommandSettingsDialog::ChecksumScope::AfterHeader;
    else if (checksumScopeStr == "DataOnly") config.checksumScope = CommandSettingsDialog::ChecksumScope::DataOnly;
    else if (checksumScopeStr == "Custom") config.checksumScope = CommandSettingsDialog::ChecksumScope::Custom;

    config.checksumStart = obj["checksumStart"].toInt(0);
    config.checksumLength = obj["checksumLength"].toInt(-1);
    config.checksumPosition = obj["checksumPosition"].toInt(-1);

    // 解析字节序
    QString byteOrderStr = obj["byteOrder"].toString("LittleEndian");
    config.byteOrder = (byteOrderStr == "BigEndian") ?
        CommandSettingsDialog::ByteOrder::BigEndian :
        CommandSettingsDialog::ByteOrder::LittleEndian;

    QString checksumByteOrderStr = obj["checksumByteOrder"].toString(byteOrderStr);
    config.checksumByteOrder = (checksumByteOrderStr == "BigEndian") ?
        CommandSettingsDialog::ByteOrder::BigEndian :
        CommandSettingsDialog::ByteOrder::LittleEndian;

    config.frequency = obj["frequency"].toInt(1000);
    config.separator = obj["separator"].toString();

    // 解析字段配置
    config.fields.clear();
    QJsonArray fieldsArray = obj["fields"].toArray();
    for (const QJsonValue &fieldValue : fieldsArray) {
        QJsonObject fieldObj = fieldValue.toObject();

        CommandSettingsDialog::FieldConfig field;
        field.index = fieldObj["index"].toInt();
        field.elementHead = fieldObj["elementHead"].toInt();
        field.name = fieldObj["name"].toString();

        // 解析数据类型
        QString typeStr = fieldObj["type"].toString("Int32");
        if (typeStr == "Int8") field.type = CommandSettingsDialog::DataType::Int8;
        else if (typeStr == "UInt8") field.type = CommandSettingsDialog::DataType::UInt8;
        else if (typeStr == "Int16") field.type = CommandSettingsDialog::DataType::Int16;
        else if (typeStr == "UInt16") field.type = CommandSettingsDialog::DataType::UInt16;
        else if (typeStr == "Int32") field.type = CommandSettingsDialog::DataType::Int32;
        else if (typeStr == "UInt32") field.type = CommandSettingsDialog::DataType::UInt32;
        else if (typeStr == "Float") field.type = CommandSettingsDialog::DataType::Float;
        else if (typeStr == "Double") field.type = CommandSettingsDialog::DataType::Double;
        else if (typeStr == "MByte") field.type = CommandSettingsDialog::DataType::MByte;
        else if (typeStr == "String") field.type = CommandSettingsDialog::DataType::String;

        field.byteLength = fieldObj["byteLength"].toInt();
        field.scale = fieldObj["scale"].toDouble(1.0);
        field.offset = fieldObj["offset"].toDouble(0.0);
        field.unit = fieldObj["unit"].toString();
        field.maximum = fieldObj["maximum"].toDouble();
        field.minimum = fieldObj["minimum"].toDouble();
        field.description = fieldObj["description"].toString();
        field.tip = fieldObj["tip"].toString();

        config.fields.append(field);
    }

    return true;
}
