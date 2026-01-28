#ifndef PROTOCOLAIGENERATOR_H
#define PROTOCOLAIGENERATOR_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include "commandsettingsdialog.h"

/**
 * @brief AI协议生成器 - 调用Claude API分析协议
 *
 * 功能：
 * 1. 接收原始16进制数据样本
 * 2. 接收解析规则描述
 * 3. 调用Claude API进行智能分析
 * 4. 解析返回的JSON，生成ProtocolConfig
 *
 * 使用示例：
 * @code
 * ProtocolAIGenerator *generator = new ProtocolAIGenerator(this);
 * generator->setApiKey("sk-ant-...");
 *
 * connect(generator, &ProtocolAIGenerator::generationComplete,
 *         this, &MyClass::onProtocolGenerated);
 * connect(generator, &ProtocolAIGenerator::generationFailed,
 *         this, &MyClass::onGenerationFailed);
 *
 * generator->generateProtocol("协议名称", rawData, parseRules);
 * @endcode
 */
class ProtocolAIGenerator : public QObject
{
    Q_OBJECT

public:
    explicit ProtocolAIGenerator(QObject *parent = nullptr);
    ~ProtocolAIGenerator();

    /**
     * @brief 设置Claude API密钥
     * @param apiKey API密钥（从环境变量或配置文件读取）
     */
    void setApiKey(const QString &apiKey);

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
    void onNetworkReplyFinished();
    void onNetworkError(QNetworkReply::NetworkError error);

private:
    QString buildPrompt(const QString &protocolName,
                       const QString &rawDataSample,
                       const QString &parseRules) const;

    bool parseAIResponse(const QString &response,
                        CommandSettingsDialog::ProtocolConfig &config,
                        QString *errorMsg = nullptr);

    QJsonObject buildApiRequest(const QString &prompt) const;

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QString m_apiKey;
    CommandSettingsDialog::ProtocolConfig m_lastConfig;
    QString m_currentProtocolName;

    // Claude API配置
    static const QString kApiUrl;
    static const QString kModelName;
    static const int kMaxTokens;
    static const double kTemperature;
};

#endif // PROTOCOLAIGENERATOR_H
