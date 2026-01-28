#ifndef AIPROTOCOLINPUTDIALOG_H
#define AIPROTOCOLINPUTDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QDialogButtonBox>
#include "commandsettingsdialog.h"

// 前向声明
class ProtocolAIGenerator;

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

private slots:
    void onPasteExample();
    void onClearAll();
    void onGenerate();
    void onCancel();

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
    // UI组件
    QTextEdit *m_rawDataEdit;           // 原始16进制数据
    QTextEdit *m_rulesEdit;             // 解析规则描述
    QPushButton *m_pasteExampleBtn;     // 粘贴示例按钮
    QPushButton *m_clearBtn;            // 清空按钮
    QPushButton *m_generateBtn;         // 生成按钮
    QPushButton *m_cancelBtn;           // 取消按钮
    QLabel *m_statusLabel;              // 状态提示标签
    QProgressBar *m_progressBar;        // 进度条

    // AI生成器
    ProtocolAIGenerator *m_aiGenerator;

    // 数据
    CommandSettingsDialog::ProtocolConfig m_generatedConfig;
    bool m_generationSuccess;
    QString m_apiKey;

    // 常量
    static const int kMinDataLength = 10;
    static const int kMinRulesLength = 20;
};

#endif // AIPROTOCOLINPUTDIALOG_H
