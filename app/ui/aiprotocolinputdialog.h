#ifndef AIPROTOCOLINPUTDIALOG_H
#define AIPROTOCOLINPUTDIALOG_H

#include <QDialog>
#include "commandsettingsdialog.h"

// 前向声明
class ProtocolAIGenerator;
class ProtocolCLIGenerator;
class QButtonGroup;

namespace Ui {
class AIProtocolInputDialog;
}

/**
 * @brief AI协议输入对话框 - 收集数据并调用AI生成协议
 *
 * 用户输入：
 * 1. 原始16进制数据样本
 * 2. 解析规则描述
 *
 * 点击"生成"后自动调用Claude API分析，生成协议配置
 */
class AIProtocolInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AIProtocolInputDialog(QWidget *parent = nullptr);
    ~AIProtocolInputDialog();

    // 获取生成的协议配置
    CommandSettingsDialog::ProtocolConfig getGeneratedConfig() const;
    bool isGenerationSuccess() const;

    // 设置API密钥
    void setApiKey(const QString &apiKey);

    // 设置API Base URL
    void setBaseUrl(const QString &baseUrl);

private slots:
    void onPasteExample();
    void onClearAll();
    void onGenerate();
    void onCancel();
    void onMethodChanged();  // 生成方式切换

    // AI生成器信号处理
    void onGenerationComplete(const CommandSettingsDialog::ProtocolConfig &config);
    void onGenerationFailed(const QString &errorMessage);
    void onProgressUpdate(const QString &message);

private:
    void setupUI();
    void setupConnections();
    void applyStyles();
    bool validateInputs(QString *errorMsg = nullptr);
    void setUIEnabled(bool enabled);

private:
    Ui::AIProtocolInputDialog *ui;

    // 方式选择组
    QButtonGroup *m_methodGroup;

    // AI生成器（两种方式）
    ProtocolAIGenerator *m_apiGenerator;
    ProtocolCLIGenerator *m_cliGenerator;

    // 数据
    CommandSettingsDialog::ProtocolConfig m_generatedConfig;
    bool m_generationSuccess;
    QString m_apiKey;
    QString m_baseUrl;

    // 常量
    static const int kMinDataLength = 10;
    static const int kMinRulesLength = 20;
};

#endif // AIPROTOCOLINPUTDIALOG_H
