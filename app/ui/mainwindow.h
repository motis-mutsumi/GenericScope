#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "device/devicemanager.h"
#include "common_component/plot/lineplot.h"
#include "common_component/plot/histogramplot.h"
#include "common_component/log/logwidget.h"
#include "common_component/record/datarecorder.h"
#include "core_plugin/core_plugins_manager/core_pluginsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief 主窗口类
 *
 * 应用程序的主界面
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 设备控制
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void on_startPollingButton_clicked();
    void on_stopPollingButton_clicked();

    // 数据录制
    void on_startRecordButton_clicked();
    void on_stopRecordButton_clicked();

    // 配置
    void on_settingsButton_clicked();

    // 设备回调
    void onDeviceConnectionChanged(bool connected);
    void onDeviceDataReceived(const QByteArray &data);
    void onDeviceError(const QString &error);

    // 定时器
    void onUpdateTimer();

private:
    void setupUI();
    void loadPlugins();
    void updateConnectionStatus(bool connected);
    void processData(const QByteArray &data);

private:
    Ui::MainWindow *ui;
    DeviceManager *m_deviceManager;
    LinePlot *m_linePlot;
    HistogramPlot *m_histogramPlot;
    LogWidget *m_logWidget;
    DataRecorder *m_dataRecorder;
    QTimer *m_updateTimer;

    QVector<double> m_xData;
    QVector<double> m_yData;
    int m_dataCounter;
};

#endif // MAINWINDOW_H
