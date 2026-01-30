#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTableWidgetItem>
#include <QMenu>
#include "device/devicemanager.h"
#include "common_component/plot/lineplot.h"
#include "common_component/log/logwidget.h"
#include "common_component/record/datarecorder.h"
#include "core_plugin/core_plugins_manager/core_pluginsmanager.h"
#include "imu3dview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief 主窗口类
 *
 * 基于 DS_RVision 界面布局的主界面
 * - 顶部：品牌标题栏 + 设置按钮
 * - 控制栏：端口选择、连接控制、录制控制
 * - 主体三栏：数据表格 + 3D可视化 + 实时图表
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 设备控制
    void on_connectToggleButton_toggled(bool checked);
    void on_portComboBox_currentTextChanged(const QString &text);
    void on_baudRateComboBox_currentTextChanged(const QString &text);

    // 录制控制
    void on_recordLogCheckBox_toggled(bool checked);

    // 界面控制
    void on_settingsButton_clicked();
    void on_darkModeButton_toggled(bool checked);
    void on_menuButton_clicked();
    void on_filterButton_clicked();

    // 设置菜单
    void onCommandSettingsTriggered();

    // 协议管理
    void onProtocolChanged(const QString &name);

    // 图表参数
    void on_xRangeComboBox_currentTextChanged(const QString &text);
    void on_xDotComboBox_currentTextChanged(const QString &text);
    void on_chartSettingsButton_clicked();

    // 设备回调
    void onDeviceConnectionChanged(bool connected);
    void onDeviceDataReceived(const QByteArray &data);
    void onDeviceError(const QString &error);

    // 定时器
    void onUpdateTimer();
    void onDataUpdateTimer();

private:
    void setupUI();
    void setupConnections();
    void setupMenu();
    void loadPlugins();
    void loadStyleSheet(bool darkMode = false);
    void updateConnectionStatus(bool connected);
    void processData(const QByteArray &data);
    void updateAttitudeDisplay(double roll, double pitch, double yaw);
    void updateDataTable(const QString &message, const QVariant &value, const QString &unit);
    void updateIMUStatus(const QString &status, double dataRate, const QString &error);
    void updateTimeDisplay();
    void setupDataTable();
    void rebuildDataTableFromProtocol();
    void setup3DVisualization();
    void setupChart();
    void setupLogWidget();
    void refreshAvailablePorts();
    QString generateBuiltInStyle(bool darkMode);

private:
    Ui::MainWindow *ui;

    // 设备管理
    DeviceManager *m_deviceManager;

    // 绘图组件
    LinePlot *m_linePlot;
    IMU3DView *m_3dView;        // 3D可视化组件
    class HistogramPlot *m_histogramPlot;  // 直方图组件（前向声明）

    // 日志和录制
    LogWidget *m_logWidget;
    DataRecorder *m_dataRecorder;

    // 定时器
    QTimer *m_updateTimer;      // UI 更新定时器
    QTimer *m_dataTimer;        // 数据更新定时器
    QTimer *m_timeDisplayTimer; // 时间显示定时器

    // 菜单
    QMenu *m_contextMenu;
    QMenu *m_settingsMenu;      // 设置菜单

    // 数据
    QVector<double> m_xData;
    QVector<double> m_yData;
    int m_dataCounter;

    // 姿态数据
    double m_currentRoll;
    double m_currentPitch;
    double m_currentYaw;

    // 统计数据
    qint64 m_startTime;
    int m_packetCount;
    int m_errorCount;
    qint64 m_lastDataRateTime;      // 上次计算数据速率的时间
    int m_lastPacketCount;          // 上次的数据包计数

    // 配置
    bool m_isDarkMode;
    int m_xRange;      // X轴范围（秒）
    int m_xDot;        // X轴点密度

    // 数据表格行索引映射
    QMap<QString, int> m_tableRowMap;

    // 常量定义
    static constexpr int kUpdateTimerInterval = 100;     // UI更新定时器间隔（毫秒）
    static constexpr int kDataTimerInterval = 100;       // 数据更新定时器间隔（毫秒）
    static constexpr int kTimeDisplayInterval = 100;     // 时间显示定时器间隔（毫秒）
    static constexpr int kExpectedDataFields = 13;       // 期望的数据字段数
    static constexpr int kDefaultXRange = 60;            // 默认X轴范围（秒）
    static constexpr int kDefaultXDot = 2;               // 默认X轴点密度
    static constexpr int kMaxDataPoints = 1000;          // 最大数据点数
    static constexpr int kDataTableMessageColumn = 0;    // 数据表格消息列
    static constexpr int kDataTableValueColumn = 1;      // 数据表格值列
    static constexpr int kDataTableUnitColumn = 2;       // 数据表格单位列
};

#endif // MAINWINDOW_H
