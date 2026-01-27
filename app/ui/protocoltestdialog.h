#ifndef PROTOCOLTESTDIALOG_H
#define PROTOCOLTESTDIALOG_H

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QLabel>
#include "commandsettingsdialog.h"

/**
 * @brief 协议测试对话框
 *
 * 用于测试协议配置是否正确：
 * - 输入16进制测试数据
 * - 解析并显示结果
 * - 显示校验码验证结果
 */
class ProtocolTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProtocolTestDialog(const CommandSettingsDialog::ProtocolConfig &config,
                                QWidget *parent = nullptr);
    ~ProtocolTestDialog();

private slots:
    void onParseData();
    void onClearData();
    void onLoadSample();

private:
    void setupUI();
    void setupConnections();
    void displayParseResult(bool success, const QString &errorMsg,
                           const QMap<QString, QVariant> &fieldValues);
    QByteArray hexStringToByteArray(const QString &hexStr, bool *ok = nullptr);
    QString byteArrayToHexString(const QByteArray &data);

private:
    CommandSettingsDialog::ProtocolConfig m_config;

    // UI组件
    QTextEdit *m_inputEdit;          // 输入16进制数据
    QPushButton *m_parseBtn;         // 解析按钮
    QPushButton *m_clearBtn;         // 清空按钮
    QPushButton *m_sampleBtn;        // 加载示例按钮
    QLabel *m_statusLabel;           // 状态标签
    QTableWidget *m_resultTable;     // 解析结果表格
    QLabel *m_checksumLabel;         // 校验码验证结果
    QLabel *m_rawDataLabel;          // 原始数据显示
};

#endif // PROTOCOLTESTDIALOG_H
