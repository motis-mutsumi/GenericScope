#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config/config.h"
#include "common_component/log/logmanager.h"
#include <QMessageBox>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_deviceManager(new DeviceManager(this))
    , m_linePlot(new LinePlot(this))
    , m_histogramPlot(new HistogramPlot(this))
    , m_logWidget(new LogWidget(this))
    , m_dataRecorder(new DataRecorder(this))
    , m_updateTimer(new QTimer(this))
    , m_dataCounter(0)
{
    ui->setupUi(this);
    setupUI();
    loadPlugins();

    // 连接设备管理器信号
    connect(m_deviceManager, &DeviceManager::connectionChanged,
            this, &MainWindow::onDeviceConnectionChanged);
    connect(m_deviceManager, &DeviceManager::dataReceived,
            this, &MainWindow::onDeviceDataReceived);
    connect(m_deviceManager, &DeviceManager::errorOccurred,
            this, &MainWindow::onDeviceError);

    // 连接更新定时器
    connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimer);
    m_updateTimer->start(Config::instance()->display.refreshRate);

    // 初始状态
    updateConnectionStatus(false);

    // 自动连接
    if (Config::instance()->app.autoConnect) {
        on_connectButton_clicked();
    }
}

MainWindow::~MainWindow()
{
    m_deviceManager->disconnectDevice();
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("GenericScope - Sensor Data Acquisition");
    resize(1200, 800);

    // 设置绘图部件
    m_linePlot->setTitle("Real-time Data");
    m_linePlot->setAxisLabels("Time (s)", "Value");
    m_linePlot->setMaxDataPoints(Config::instance()->display.maxDataPoints);
    m_linePlot->setAutoScale(Config::instance()->display.autoScale);

    m_histogramPlot->setTitle("Data Distribution");
    m_histogramPlot->setAxisLabels("Value", "Count");

    // TODO: 将绘图部件添加到布局中
    // 这需要在 .ui 文件中定义相应的容器
}

void MainWindow::loadPlugins()
{
    QString pluginDir = Config::instance()->dirs.pluginDir;
    int count = Core_PluginsManager::instance()->loadAllPlugins(pluginDir);
    LOG_INFO(QString("Loaded %1 plugins").arg(count));

    // TODO: 将插件 UI 添加到主窗口
}

void MainWindow::on_connectButton_clicked()
{
    if (m_deviceManager->connectDevice()) {
        LOG_INFO("Device connected successfully");
    } else {
        QMessageBox::critical(this, "Error", "Failed to connect device");
    }
}

void MainWindow::on_disconnectButton_clicked()
{
    m_deviceManager->disconnectDevice();
}

void MainWindow::on_startPollingButton_clicked()
{
    m_deviceManager->startPolling();
}

void MainWindow::on_stopPollingButton_clicked()
{
    m_deviceManager->stopPolling();
}

void MainWindow::on_startRecordButton_clicked()
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString fileName = Config::instance()->dirs.dataDir + "/data_" + timestamp + ".csv";

    QVariantMap metadata;
    metadata["device"] = Config::instance()->device.port;
    metadata["timestamp"] = timestamp;

    if (m_dataRecorder->startRecording(fileName, DataRecorder::CSV, metadata)) {
        LOG_INFO(QString("Recording started: %1").arg(fileName));
    } else {
        QMessageBox::critical(this, "Error", "Failed to start recording");
    }
}

void MainWindow::on_stopRecordButton_clicked()
{
    m_dataRecorder->stopRecording();
    LOG_INFO(QString("Recording stopped. Total records: %1").arg(m_dataRecorder->recordedCount()));
}

void MainWindow::on_settingsButton_clicked()
{
    // TODO: 打开设置对话框
    QMessageBox::information(this, "Settings", "Settings dialog not implemented yet");
}

void MainWindow::onDeviceConnectionChanged(bool connected)
{
    updateConnectionStatus(connected);

    if (connected && Config::instance()->device.autoPolling) {
        m_deviceManager->startPolling();
    }
}

void MainWindow::onDeviceDataReceived(const QByteArray &data)
{
    processData(data);
}

void MainWindow::onDeviceError(const QString &error)
{
    QMessageBox::warning(this, "Device Error", error);
}

void MainWindow::onUpdateTimer()
{
    // 更新绘图
    m_linePlot->refresh();
    m_histogramPlot->refresh();
}

void MainWindow::updateConnectionStatus(bool connected)
{
    // TODO: 更新 UI 状态
    // ui->connectButton->setEnabled(!connected);
    // ui->disconnectButton->setEnabled(connected);
    // ui->startPollingButton->setEnabled(connected);
    // ui->stopPollingButton->setEnabled(connected);

    if (connected) {
        statusBar()->showMessage("Device Connected");
    } else {
        statusBar()->showMessage("Device Disconnected");
    }
}

void MainWindow::processData(const QByteArray &data)
{
    // TODO: 根据实际数据协议解析数据
    // 这里提供一个示例实现

    // 假设数据是文本格式的数值
    bool ok;
    double value = QString::fromUtf8(data).trimmed().toDouble(&ok);

    if (ok) {
        // 添加到绘图数据
        double time = m_dataCounter * 0.1;  // 假设采样间隔 100ms
        m_xData.append(time);
        m_yData.append(value);

        m_linePlot->addDataPoint(time, value);
        m_histogramPlot->setRawData(m_yData);

        // 录制数据
        if (m_dataRecorder->isRecording()) {
            QVariantMap dataMap;
            dataMap["time"] = time;
            dataMap["value"] = value;
            m_dataRecorder->recordData(dataMap);
        }

        m_dataCounter++;
    }
}
