#ifndef PROTOCOLCLIGENERATOR_H
#define PROTOCOLCLIGENERATOR_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include "commandsettingsdialog.h"

/**
 * @brief AI协议生成器 - 通过CMD调用Claude CLI分析协议
 *
 * 功能：
 * 1. 接收原始16进制数据样本
 * 2. 接收解析规则描述
 * 3. 通过命令行调用Claude CLI进行智能分析
 * 4. 解析返回的JSON，生成ProtocolConfig
 *
 * 使用示例：
 * @code
 * ProtocolCLIGenerator *generator = new ProtocolCLIGenerator(this);
 *
 * connect(generator, &ProtocolCLIGenerator::generationComplete,
 *         this, &MyClass::onProtocolGenerated);
 * connect(generator, &ProtocolCLIGenerator::generationFailed,
 *         this, &MyClass::onGenerationFailed);
 *
 * generator->generateProtocol("协议名称", rawData, parseRules);
 * @endcode
 *
 * 前置要求：
 * - 安装Claude Code CLI工具（参考文档）
 * - 或使用Python脚本调用（推荐）
 */
class ProtocolCLIGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ProtocolCLIGenerator(QObject *parent = nullptr);
    ~ProtocolCLIGenerator();

    /**
     * @brief 设置Claude命令路径（可选）
     * @param commandPath Claude CLI命令路径，为空则使用默认
     */
    void setCommandPath(const QString &commandPath);

    /**
     * @brief 生成协议配置
     * @param protocolName 协议名称
     * @param rawDataSample 原始16进制数据样本
     * @param parseRules 解析规则描述
     */
    void generateProtocol(const QString &protocolName,
                         const QString &rawDataSample,
                         const QString &parseRules);

    /**
     * @brief 取消正在进行的生成
     */
    void cancel();

    /**
     * @brief 获取最后生成的协议配置
     */
    CommandSettingsDialog::ProtocolConfig getLastGeneratedConfig() const;

signals:
    /**
     * @brief 生成完成信号
     * @param config 生成的协议配置
     */
    void generationComplete(const CommandSettingsDialog::ProtocolConfig &config);

    /**
     * @brief 生成失败信号
     * @param errorMessage 错误信息
     */
    void generationFailed(const QString &errorMessage);

    /**
     * @brief 生成进度信号
     * @param message 进度消息
     */
    void progressUpdate(const QString &message);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onProcessReadyReadStdOut();
    void onProcessReadyReadStdErr();

private:
    QString buildPrompt(const QString &protocolName,
                       const QString &rawDataSample,
                       const QString &parseRules) const;

    bool parseAIResponse(const QString &response,
                        CommandSettingsDialog::ProtocolConfig &config,
                        QString *errorMsg = nullptr);

    QString detectClaudeCommand() const;
    bool saveTempPromptFile(const QString &prompt, QString &filePath);
    void cleanupTempFiles();

private:
    QProcess *m_process;
    QString m_commandPath;
    QString m_currentProtocolName;
    QString m_stdoutBuffer;
    QString m_stderrBuffer;
    QString m_tempPromptFile;
    CommandSettingsDialog::ProtocolConfig m_lastConfig;

    // Claude CLI配置
    static const QString kDefaultCommand;  // 默认Claude命令
    static const QString kSystemPrompt;    // 系统提示
    static const int kTimeoutMs;           // 超时时间（毫秒）
};

#endif // PROTOCOLCLIGENERATOR_H
