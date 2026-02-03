#ifndef NEWPROTOCOLDIALOG_H
#define NEWPROTOCOLDIALOG_H

#include <QDialog>
#include <QVector>
#include "commandsettingsdialog.h"

// 前向声明
namespace Ui {
class NewProtocolDialog;
}

/**
 * @brief 新建协议向导对话框
 *
 * 提供更友好的协议创建体验：
 * - 配置协议基本信息
 * - 选择协议模板（空协议、串口协议、MODBUS协议等）
 * - 选择是否添加默认字段
 */
class NewProtocolDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 协议模板类型
     */
    enum class ProtocolTemplate {
        Empty,              // 空协议
        SerialBasic,        // 基础串口协议（帧头+数据+校验）
        ModbusRTU,          // MODBUS RTU协议
        CustomIMU,          // 自定义IMU协议
        TextCSV,            // 文本CSV协议
        AIGenerated         // AI智能生成（新增）
    };

    explicit NewProtocolDialog(QWidget *parent = nullptr);
    ~NewProtocolDialog();

    // 获取创建的协议配置
    CommandSettingsDialog::ProtocolConfig getProtocolConfig() const;
    QString getProtocolName() const;

private slots:
    void onTemplateChanged(int index);
    void onAccepted();
    void onNameChanged(const QString &text);

private:
    void setupUI();
    void setupConnections();
    void updateTemplateDescription();
    bool validateInput();

    // 模板创建函数
    CommandSettingsDialog::ProtocolConfig createEmptyProtocol() const;
    CommandSettingsDialog::ProtocolConfig createSerialBasicProtocol() const;
    CommandSettingsDialog::ProtocolConfig createModbusRTUProtocol() const;
    CommandSettingsDialog::ProtocolConfig createCustomIMUProtocol() const;
    CommandSettingsDialog::ProtocolConfig createTextCSVProtocol() const;
    CommandSettingsDialog::ProtocolConfig createAIGeneratedProtocol() const;

private:
    Ui::NewProtocolDialog *ui;

    // 数据
    CommandSettingsDialog::ProtocolConfig m_config;
    QString m_protocolName;
};

#endif // NEWPROTOCOLDIALOG_H
