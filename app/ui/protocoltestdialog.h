#ifndef PROTOCOLTESTDIALOG_H
#define PROTOCOLTESTDIALOG_H

#include <QDialog>
#include <QSharedPointer>
#include "commandsettingsdialog.h"
#include "protocol/protocolparser.h"

// 前向声明
namespace Ui {
class ProtocolTestDialog;
}

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
    Ui::ProtocolTestDialog *ui;

    CommandSettingsDialog::ProtocolConfig m_config;
    QSharedPointer<ProtocolParser> m_parser;  // 协议解析器
};

#endif // PROTOCOLTESTDIALOG_H
