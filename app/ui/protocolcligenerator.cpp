#include "protocolcligenerator.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QStandardPaths>
#include <QCoreApplication>

// Claude CLI配置
const QString ProtocolCLIGenerator::kDefaultCommand = "claude";  // 默认使用PATH中的claude命令
const int ProtocolCLIGenerator::kTimeoutMs = 120000;  // 2分钟超时
const QString ProtocolCLIGenerator::kSystemPrompt =
    "你是一个专业的协议解析专家，擅长分析二进制通信协议的帧结构、校验算法和数据字段。"
    "你的任务是根据用户提供的16进制数据样本和解析规则，生成完整准确的协议配置JSON。"
    "请严格按照指定的JSON格式输出，确保字段完整、类型正确、数值合理。";

ProtocolCLIGenerator::ProtocolCLIGenerator(QObject *parent)
    : QObject(parent)
    , m_process(nullptr)
{
}

ProtocolCLIGenerator::~ProtocolCLIGenerator()
{
    if (m_process) {
        m_process->kill();
        m_process->waitForFinished(1000);
        m_process->deleteLater();
    }
    cleanupTempFiles();
}

void ProtocolCLIGenerator::setCommandPath(const QString &commandPath)
{
    m_commandPath = commandPath;
}

void ProtocolCLIGenerator::generateProtocol(const QString &protocolName,
                                           const QString &rawDataSample,
                                           const QString &parseRules)
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        emit generationFailed("已有生成任务在进行中，请等待完成或取消后重试。");
        return;
    }

    m_currentProtocolName = protocolName;
    m_stdoutBuffer.clear();
    m_stderrBuffer.clear();

    emit progressUpdate("正在构建提示词...");

    // 构建提示词
    QString prompt = buildPrompt(protocolName, rawDataSample, parseRules);

    // 保存到临时文件
    QString promptFile;
    if (!saveTempPromptFile(prompt, promptFile)) {
        emit generationFailed("创建临时文件失败");
        return;
    }
    m_tempPromptFile = promptFile;

    emit progressUpdate("正在调用Claude CLI...");

    // 检测Claude命令
    QString command = m_commandPath.isEmpty() ? detectClaudeCommand() : m_commandPath;
    if (command.isEmpty()) {
        cleanupTempFiles();
        emit generationFailed("未找到Claude CLI命令。\n\n"
                            "请确保：\n"
                            "1. 已安装Claude Code CLI\n"
                            "2. claude命令在系统PATH中\n"
                            "3. 或在设置中配置Claude命令路径");
        return;
    }

    // 创建进程
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &ProtocolCLIGenerator::onProcessFinished);
        connect(m_process, &QProcess::errorOccurred,
                this, &ProtocolCLIGenerator::onProcessError);
        connect(m_process, &QProcess::readyReadStandardOutput,
                this, &ProtocolCLIGenerator::onProcessReadyReadStdOut);
        connect(m_process, &QProcess::readyReadStandardError,
                this, &ProtocolCLIGenerator::onProcessReadyReadStdErr);
    }

    // 构建命令参数
    // 使用: claude --dangerously-skip-permissions < prompt.txt
    QStringList arguments;
    arguments << "--dangerously-skip-permissions";

    // 设置标准输入重定向
    m_process->setStandardInputFile(promptFile);

    qDebug() << "========== 启动Claude进程 ==========";
    qDebug() << "命令:" << command;
    qDebug() << "参数:" << arguments.join(" ");
    qDebug() << "提示词文件:" << promptFile;
    qDebug() << "工作目录:" << QDir::currentPath();
    qDebug() << "======================================";

    // 启动进程
    m_process->start(command, arguments);

    if (!m_process->waitForStarted(5000)) {
        cleanupTempFiles();
        emit generationFailed("启动Claude CLI失败：超时\n\n"
                             "请检查：\n"
                             "1. claude命令是否在PATH中\n"
                             "2. 是否有执行权限\n"
                             "3. 查看调试日志获取详细信息");
        return;
    }

    qDebug() << "进程已启动，PID:" << m_process->processId();
    emit progressUpdate("Claude正在分析协议（进程已启动）...");
}

void ProtocolCLIGenerator::cancel()
{
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        emit progressUpdate("已取消生成");
    }
}

CommandSettingsDialog::ProtocolConfig ProtocolCLIGenerator::getLastGeneratedConfig() const
{
    return m_lastConfig;
}

QString ProtocolCLIGenerator::buildPrompt(const QString &protocolName,
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

bool ProtocolCLIGenerator::parseAIResponse(const QString &response,
                                          CommandSettingsDialog::ProtocolConfig &config,
                                          QString *errorMsg)
{
    qDebug() << "========== 开始解析AI响应 ==========";
    qDebug() << "原始响应长度:" << response.length() << "字节";

    // 提取JSON部分（AI可能返回```json...```格式）
    QString jsonStr = response.trimmed();
    qDebug() << "去除空白后长度:" << jsonStr.length() << "字节";
    qDebug() << "前20个字符:" << jsonStr.left(20);
    qDebug() << "后20个字符:" << jsonStr.right(20);

    // 处理双重包装：如果整个输出被双引号包裹并且换行符被转义
    if (jsonStr.startsWith("\"") && jsonStr.endsWith("\"")) {
        qDebug() << "检测到双重包装，开始反转义...";
        // 去除最外层双引号
        jsonStr = jsonStr.mid(1, jsonStr.length() - 2);
        // 替换转义的换行符
        jsonStr.replace("\\n", "\n");
        jsonStr.replace("\\\"", "\"");
        jsonStr.replace("\\\\", "\\");
        qDebug() << "反转义后长度:" << jsonStr.length() << "字节";
        qDebug() << "反转义后前20个字符:" << jsonStr.left(20);
    }

    // 通用JSON提取：查找第一个{和最后一个}
    qDebug() << "查找JSON边界...";
    int jsonStart = jsonStr.indexOf('{');
    int jsonEnd = jsonStr.lastIndexOf('}');
    qDebug() << "JSON起始位置:" << jsonStart << "结束位置:" << jsonEnd;

    if (jsonStart >= 0 && jsonEnd > jsonStart) {
        jsonStr = jsonStr.mid(jsonStart, jsonEnd - jsonStart + 1);
        qDebug() << "提取后长度:" << jsonStr.length() << "字节";
    } else {
        qDebug() << "未找到有效的JSON边界！";
        if (errorMsg) *errorMsg = "未找到有效的JSON内容";
        return false;
    }

    qDebug() << "最终JSON前100个字符:" << jsonStr.left(100);

    // 解析JSON
    qDebug() << "开始解析JSON...";
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8());
    qDebug() << "JSON解析完成，isNull:" << doc.isNull() << "isObject:" << doc.isObject();

    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "JSON解析失败！";
        if (errorMsg) *errorMsg = "无法解析JSON格式";
        return false;
    }

    qDebug() << "JSON解析成功，开始提取字段...";

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

    qDebug() << "基本信息提取完成，开始提取字段...";

    // 解析字段配置
    config.fields.clear();
    QJsonArray fieldsArray = obj["fields"].toArray();
    qDebug() << "字段数组大小:" << fieldsArray.size();

    int fieldIndex = 0;
    for (const QJsonValue &fieldValue : fieldsArray) {
        qDebug() << "========== 处理字段" << fieldIndex << "==========";
        QJsonObject fieldObj = fieldValue.toObject();

        CommandSettingsDialog::FieldConfig field;
        field.index = fieldObj["index"].toInt();
        field.elementHead = fieldObj["elementHead"].toInt();
        field.name = fieldObj["name"].toString();
        qDebug() << "  名称:" << field.name << "索引:" << field.index;

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

        qDebug() << "  字段提取完成，添加到列表...";
        config.fields.append(field);
        qDebug() << "  字段已添加，当前字段总数:" << config.fields.size();

        fieldIndex++;
    }

    qDebug() << "========== 所有字段提取完成 ==========";
    qDebug() << "总字段数:" << config.fields.size();
    qDebug() << "解析成功，返回true";

    return true;
}

QString ProtocolCLIGenerator::detectClaudeCommand() const
{
    // 尝试几个常见的Claude CLI命令
    QStringList candidates;
    candidates << "claude" << "claude-code" << "npx claude";

    // Windows下可能需要.cmd或.exe后缀
#ifdef Q_OS_WIN
    candidates << "claude.cmd" << "claude.exe";
#endif

    for (const QString &cmd : candidates) {
        QProcess testProcess;
        testProcess.start(cmd, QStringList() << "--version");
        if (testProcess.waitForFinished(3000)) {
            if (testProcess.exitCode() == 0) {
                qDebug() << "检测到Claude命令:" << cmd;
                return cmd;
            }
        }
    }

    return kDefaultCommand;  // 返回默认值，让用户看到错误信息
}

bool ProtocolCLIGenerator::saveTempPromptFile(const QString &prompt, QString &filePath)
{
    // 创建临时文件
    QTemporaryFile tempFile(QDir::temp().filePath("claude_prompt_XXXXXX.txt"));
    tempFile.setAutoRemove(false);  // 手动管理删除

    if (!tempFile.open()) {
        qWarning() << "无法创建临时文件:" << tempFile.errorString();
        return false;
    }

    // 写入提示词
    QTextStream out(&tempFile);
    out.setCodec("UTF-8");
    out << prompt;
    tempFile.close();

    filePath = tempFile.fileName();
    qDebug() << "临时提示词文件已创建:" << filePath;
    return true;
}

void ProtocolCLIGenerator::cleanupTempFiles()
{
    if (!m_tempPromptFile.isEmpty()) {
        QFile::remove(m_tempPromptFile);
        qDebug() << "已删除临时文件:" << m_tempPromptFile;
        m_tempPromptFile.clear();
    }
}

void ProtocolCLIGenerator::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "========== 进程结束 ==========";
    qDebug() << "退出代码:" << exitCode;
    qDebug() << "退出状态:" << (exitStatus == QProcess::NormalExit ? "正常" : "崩溃");
    qDebug() << "标准输出长度:" << m_stdoutBuffer.length() << "字节";
    qDebug() << "标准错误长度:" << m_stderrBuffer.length() << "字节";
    qDebug() << "==============================";

    // 显示完整输出（调试用）
    if (!m_stdoutBuffer.isEmpty()) {
        qDebug() << "========== 完整标准输出 ==========";
        qDebug() << m_stdoutBuffer;
        qDebug() << "===================================";
    }

    if (!m_stderrBuffer.isEmpty()) {
        qDebug() << "========== 完整标准错误 ==========";
        qDebug() << m_stderrBuffer;
        qDebug() << "===================================";
    }

    cleanupTempFiles();

    if (exitStatus != QProcess::NormalExit) {
        emit generationFailed("Claude CLI异常退出（进程崩溃）");
        return;
    }

    if (exitCode != 0) {
        QString errorMsg = QString("Claude CLI返回错误代码 %1").arg(exitCode);
        if (!m_stderrBuffer.isEmpty()) {
            errorMsg += "\n\n错误信息:\n" + m_stderrBuffer;
        }
        if (m_stdoutBuffer.isEmpty()) {
            errorMsg += "\n\n提示：未收到任何标准输出，请检查：\n"
                       "1. claude命令是否正确安装\n"
                       "2. 是否已登录（claude auth status）\n"
                       "3. 查看调试日志获取详细信息";
        }
        emit generationFailed(errorMsg);
        return;
    }

    // 检查是否收到输出
    if (m_stdoutBuffer.isEmpty()) {
        emit generationFailed("未收到Claude CLI输出！\n\n"
                             "可能的原因：\n"
                             "1. claude命令未正确响应\n"
                             "2. 提示词文件读取失败\n"
                             "3. 网络连接问题\n\n"
                             "请查看调试日志获取详细信息");
        return;
    }

    emit progressUpdate("正在解析AI响应...");
    qDebug() << "========== 调用parseAIResponse ==========";

    // 解析AI返回的协议配置
    QString errorMsg;
    bool parseResult = parseAIResponse(m_stdoutBuffer, m_lastConfig, &errorMsg);
    qDebug() << "parseAIResponse返回结果:" << parseResult;

    if (!parseResult) {
        qDebug() << "解析失败，发送generationFailed信号";
        emit generationFailed(QString("解析AI响应失败: %1\n\n原始输出（前500字符）:\n%2")
                             .arg(errorMsg)
                             .arg(m_stdoutBuffer.left(500)));
        return;
    }

    qDebug() << "解析成功，设置协议名称...";
    // 设置协议名称
    m_lastConfig.name = m_currentProtocolName;
    qDebug() << "协议名称已设置为:" << m_lastConfig.name;

    qDebug() << "发送progressUpdate信号...";
    emit progressUpdate("协议配置生成成功！");
    qDebug() << "发送generationComplete信号...";
    emit generationComplete(m_lastConfig);
    qDebug() << "========== onProcessFinished执行完毕 ==========";
}

void ProtocolCLIGenerator::onProcessError(QProcess::ProcessError error)
{
    QString errorMsg;
    switch (error) {
    case QProcess::FailedToStart:
        errorMsg = "无法启动Claude CLI。请检查：\n"
                   "1. Claude命令是否正确安装\n"
                   "2. claude命令是否在PATH中\n"
                   "3. 是否有执行权限";
        break;
    case QProcess::Crashed:
        errorMsg = "Claude CLI进程崩溃";
        break;
    case QProcess::Timedout:
        errorMsg = "Claude CLI执行超时";
        break;
    default:
        errorMsg = QString("进程错误: %1").arg(m_process->errorString());
        break;
    }

    cleanupTempFiles();
    emit generationFailed(errorMsg);
}

void ProtocolCLIGenerator::onProcessReadyReadStdOut()
{
    if (m_process) {
        QString output = QString::fromUtf8(m_process->readAllStandardOutput());
        m_stdoutBuffer += output;

        // 详细调试信息
        qDebug() << "========== 收到标准输出 ==========";
        qDebug() << "长度:" << output.length() << "字节";
        qDebug() << "累计长度:" << m_stdoutBuffer.length() << "字节";
        qDebug() << "内容:" << output;
        qDebug() << "====================================";

        // 向界面发送进度更新，显示接收到的数据
        emit progressUpdate(QString("接收数据: %1 字节（累计 %2 字节）")
                           .arg(output.length())
                           .arg(m_stdoutBuffer.length()));
    }
}

void ProtocolCLIGenerator::onProcessReadyReadStdErr()
{
    if (m_process) {
        QString output = QString::fromUtf8(m_process->readAllStandardError());
        m_stderrBuffer += output;

        // 详细调试信息
        qDebug() << "========== 收到标准错误 ==========";
        qDebug() << "内容:" << output;
        qDebug() << "====================================";

        // 向界面发送错误信息
        emit progressUpdate(QString("错误信息: %1").arg(output.left(100)));
    }
}
