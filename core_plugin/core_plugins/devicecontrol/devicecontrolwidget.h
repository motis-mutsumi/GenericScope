#ifndef DEVICECONTROLWIDGET_H
#define DEVICECONTROLWIDGET_H

#include <QWidget>
#include <QMap>

namespace Ui {
class DeviceControlWidget;
}

/**
 * @brief 设备控制插件UI组件
 *
 * 提供预定义指令按钮和自定义指令输入功能。
 */
class DeviceControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceControlWidget(QWidget *parent = nullptr);
    ~DeviceControlWidget();

    /**
     * @brief 设置预定义指令
     * @param commands 指令名称->指令数据映射
     */
    void setPredefinedCommands(const QMap<QString, QByteArray> &commands);

    /**
     * @brief 添加历史记录
     * @param message 历史消息
     */
    void addHistory(const QString &message);

signals:
    /**
     * @brief 发送指令信号
     * @param commandName 指令名称
     * @param commandData 指令数据
     */
    void sendCommand(const QString &commandName, const QByteArray &commandData);

private slots:
    void onCalibrateClicked();
    void onResetClicked();
    void onQueryClicked();
    void onStartClicked();
    void onStopClicked();
    void onSendCustomClicked();

private:
    Ui::DeviceControlWidget *ui;
    QMap<QString, QByteArray> m_commands;
};

#endif // DEVICECONTROLWIDGET_H
