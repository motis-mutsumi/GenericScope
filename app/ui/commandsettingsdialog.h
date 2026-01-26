#ifndef COMMANDSETTINGSDIALOG_H
#define COMMANDSETTINGSDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>

namespace Ui {
class CommandSettingsDialog;
}

/**
 * @brief 指令设置对话框
 *
 * 功能：
 * - 管理16进制指令列表
 * - 支持手动填写指令
 * - 支持从外部文件导入指令
 * - 自动生成指令
 */
class CommandSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommandSettingsDialog(QWidget *parent = nullptr);
    ~CommandSettingsDialog();

private slots:
    // 按钮槽函数
    void onAddCommand();
    void onDeleteCommand();
    void onImportCommands();
    void onClearCommand();
    void onGenerateCommand();

    // 表格选择变化
    void onTableSelectionChanged();

    // 对话框按钮
    void onAccepted();
    void onRejected();
    void onApplyClicked();

private:
    void setupUI();
    void setupConnections();
    void loadCommands();
    void saveCommands();
    void updateCommandDetails(int row);
    void clearCommandDetails();
    void applyStyles();

private:
    Ui::CommandSettingsDialog *ui;
};

#endif // COMMANDSETTINGSDIALOG_H
