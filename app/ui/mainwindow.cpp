#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config/config.h"
#include "commandsettingsdialog.h"
#include "common_component/log/logmanager.h"
#include <QMessageBox>
#include <QDateTime>
#include <QSerialPortInfo>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_deviceManager(new DeviceManager(this))
    , m_linePlot(nullptr)
    , m_logWidget(nullptr)
    , m_dataRecorder(new DataRecorder(this))
    , m_updateTimer(new QTimer(this))
    , m_dataTimer(new QTimer(this))
    , m_timeDisplayTimer(new QTimer(this))
    , m_contextMenu(new QMenu(this))
    , m_settingsMenu(new QMenu(this))
    , m_dataCounter(0)
    , m_currentRoll(0.0)
    , m_currentPitch(0.0)
    , m_currentYaw(0.0)
    , m_startTime(0)
    , m_packetCount(0)
    , m_errorCount(0)
    , m_lastDataRateTime(0)
    , m_lastPacketCount(0)
    , m_isDarkMode(false)
    , m_xRange(kDefaultXRange)
    , m_xDot(kDefaultXDot)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    setupMenu();
    loadPlugins();
    loadStyleSheet(m_isDarkMode);

    // 初始状态
    updateConnectionStatus(false);
    refreshAvailablePorts();

    // 启动时间显示定时器
    m_timeDisplayTimer->start(kTimeDisplayInterval);
}

MainWindow::~MainWindow()
{
    m_deviceManager->disconnectDevice();
    delete ui;
}

void MainWindow::setupUI()
{
    // 设置窗口标题和图标
    setWindowTitle("GenericScope - IMU Data Acquisition");

    // 设置数据表格
    setupDataTable();

    // 设置3D可视化
    setup3DVisualization();

    // 设置图表
    setupChart();

    // 设置初始值
    ui->xRangeComboBox->setCurrentText("60");
    ui->xDotComboBox->setCurrentText("2");
    ui->portComboBox->setCurrentText("COM7");
    ui->baudRateComboBox->setCurrentText("115200");

    // 初始化姿态显示
    updateAttitudeDisplay(0, 0, 0);

    // 初始化IMU状态
    updateIMUStatus("Ready", 0, "None");

    // 设置状态栏
    statusBar()->showMessage("Ready");
}

void MainWindow::setupConnections()
{
    // 连接设备管理器信号
    bool ok = true;
    ok &= static_cast<bool>(connect(m_deviceManager, &DeviceManager::connectionChanged,
                                    this, &MainWindow::onDeviceConnectionChanged));
    ok &= static_cast<bool>(connect(m_deviceManager, &DeviceManager::dataReceived,
                                    this, &MainWindow::onDeviceDataReceived));
    ok &= static_cast<bool>(connect(m_deviceManager, &DeviceManager::errorOccurred,
                                    this, &MainWindow::onDeviceError));
    Q_ASSERT(ok && "Failed to connect device manager signals");

    // 连接定时器
    ok = true;
    ok &= static_cast<bool>(connect(m_updateTimer, &QTimer::timeout, this, &MainWindow::onUpdateTimer));
    ok &= static_cast<bool>(connect(m_dataTimer, &QTimer::timeout, this, &MainWindow::onDataUpdateTimer));
    ok &= static_cast<bool>(connect(m_timeDisplayTimer, &QTimer::timeout, this, &MainWindow::updateTimeDisplay));
    Q_ASSERT(ok && "Failed to connect timer signals");
}

void MainWindow::setupMenu()
{
    // 创建设置菜单
    m_settingsMenu->addAction("指令设置", this, &MainWindow::onCommandSettingsTriggered);
    m_settingsMenu->addSeparator();
    m_settingsMenu->addAction("设备配置", this, [this]() {
        QMessageBox::information(this, "设备配置", "设备配置功能待实现");
    });
    m_settingsMenu->addAction("显示选项", this, [this]() {
        QMessageBox::information(this, "显示选项", "显示选项功能待实现");
    });
    m_settingsMenu->addSeparator();
    m_settingsMenu->addAction("关于", this, [this]() {
        QMessageBox::about(this, "关于 GenericScope",
            "GenericScope v1.0\n"
            "通用上位机框架\n"
            "基于 Qt 5.14 和 C++17");
    });

    // 创建右键菜单
    m_contextMenu->addAction("Export Data", this, [this]() {
        QMessageBox::information(this, "Export", "Export data functionality");
    });
    m_contextMenu->addAction("Clear Chart", this, [this]() {
        if (m_linePlot) {
            m_linePlot->clearData();
        }
        m_xData.clear();
        m_yData.clear();
        m_dataCounter = 0;
    });
    m_contextMenu->addSeparator();
    m_contextMenu->addAction("About", this, [this]() {
        QMessageBox::about(this, "About GenericScope",
            "GenericScope v1.0\n"
            "A generic data acquisition and visualization tool\n"
            "Based on Qt 5.14 and C++17");
    });
}

void MainWindow::loadPlugins()
{
    QString pluginDir = Config::instance()->dirs.pluginDir;
    int count = Core_PluginsManager::instance()->loadAllPlugins(pluginDir);
    LOG_INFO(QString("Loaded %1 plugins").arg(count));
}

void MainWindow::loadStyleSheet(bool darkMode)
{
    QString qssFile = darkMode ? ":/qss/dark.qss" : ":/qss/light.qss";
    QFile file(qssFile);

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        // 如果无法加载QSS文件，使用内置样式
        QString builtInStyle = generateBuiltInStyle(darkMode);
        qApp->setStyleSheet(builtInStyle);
    }

    m_isDarkMode = darkMode;
}

void MainWindow::setupDataTable()
{
    // 配置数据表格
    ui->dataTableWidget->setColumnCount(3);
    ui->dataTableWidget->setHorizontalHeaderLabels({QStringLiteral("Message"),
                                                     QStringLiteral("Value"),
                                                     QStringLiteral("Unit")});
    ui->dataTableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->dataTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dataTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->dataTableWidget->setAlternatingRowColors(true);

    // 设置列宽
    ui->dataTableWidget->setColumnWidth(kDataTableMessageColumn, 120);
    ui->dataTableWidget->setColumnWidth(kDataTableValueColumn, 80);
    ui->dataTableWidget->setColumnWidth(kDataTableUnitColumn, 50);

    // 添加初始数据行
    QStringList messages = {QStringLiteral("AccX"), QStringLiteral("AccY"), QStringLiteral("AccZ"),
                           QStringLiteral("GyroX"), QStringLiteral("GyroY"), QStringLiteral("GyroZ"),
                           QStringLiteral("MagX"), QStringLiteral("MagY"), QStringLiteral("MagZ"),
                           QStringLiteral("Temperature")};

    for (int i = 0; i < messages.size(); ++i) {
        int row = ui->dataTableWidget->rowCount();
        ui->dataTableWidget->insertRow(row);

        ui->dataTableWidget->setItem(row, kDataTableMessageColumn, new QTableWidgetItem(messages[i]));
        ui->dataTableWidget->setItem(row, kDataTableValueColumn, new QTableWidgetItem(QStringLiteral("-")));
        ui->dataTableWidget->setItem(row, kDataTableUnitColumn, new QTableWidgetItem(QString()));

        m_tableRowMap[messages[i]] = row;
    }
}

void MainWindow::setup3DVisualization()
{
    // TODO: 实现3D可视化
    // 这里可以使用 Qt3D 或者 OpenGL 来实现IMU的3D姿态显示
    // 暂时显示占位信息
    QLabel *placeholder = new QLabel("3D Visualization\n(To be implemented)", this);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet("QLabel { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 4px; }");

    QVBoxLayout *layout = new QVBoxLayout(ui->visualization3DContainer);
    layout->addWidget(placeholder);
}

void MainWindow::setupChart()
{
    // 创建实时曲线图
    m_linePlot = new LinePlot(this);
    m_linePlot->setTitle("Real-time Data");
    m_linePlot->setAxisLabels("Time (s)", "Value");
    m_linePlot->setMaxDataPoints(kMaxDataPoints);
    m_linePlot->setAutoScale(true);

    // 预分配容器容量以提高性能
    m_xData.reserve(kMaxDataPoints);
    m_yData.reserve(kMaxDataPoints);

    // 添加到图表容器
    QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_linePlot);
}

void MainWindow::refreshAvailablePorts()
{
    // 刷新可用串口列表
    ui->portComboBox->clear();

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port : ports) {
        ui->portComboBox->addItem(port.portName());
    }

    // 如果没有找到串口，添加默认项
    if (ui->portComboBox->count() == 0) {
        for (int i = 1; i <= 10; ++i) {
            ui->portComboBox->addItem(QString("COM%1").arg(i));
        }
    }
}

void MainWindow::on_connectToggleButton_toggled(bool checked)
{
    if (checked) {
        // 连接设备
        QString port = ui->portComboBox->currentText();
        int baudRate = ui->baudRateComboBox->currentText().toInt();

        // 配置设备
        Config::instance()->device.port = port;
        Config::instance()->device.baudRate = baudRate;

        if (m_deviceManager->connectDevice()) {
            ui->connectToggleButton->setText("Disconnect");
            ui->portComboBox->setEnabled(false);
            ui->baudRateComboBox->setEnabled(false);
            LOG_INFO(QString("Connected to %1 @ %2 baud").arg(port).arg(baudRate));

            // 启动数据定时器
            m_deviceManager->startPolling();
            m_dataTimer->start(kDataTimerInterval);

            m_startTime = QDateTime::currentMSecsSinceEpoch();
        } else {
            ui->connectToggleButton->setChecked(false);
            QString errorMsg = QString("Failed to connect to %1 at %2 baud").arg(port).arg(baudRate);
            LOG_ERROR(errorMsg);
            QMessageBox::critical(this, "Connection Error", errorMsg);
        }
    } else {
        // 断开连接
        m_deviceManager->stopPolling();
        m_deviceManager->disconnectDevice();
        m_dataTimer->stop();

        ui->connectToggleButton->setText("Connect");
        ui->portComboBox->setEnabled(true);
        ui->baudRateComboBox->setEnabled(true);

        LOG_INFO("Device disconnected");
    }
}

void MainWindow::on_portComboBox_currentTextChanged(const QString &text)
{
    LOG_DEBUG(QString("Port changed to: %1").arg(text));
}

void MainWindow::on_baudRateComboBox_currentTextChanged(const QString &text)
{
    LOG_DEBUG(QString("Baud rate changed to: %1").arg(text));
}

void MainWindow::on_recordLogCheckBox_toggled(bool checked)
{
    if (checked) {
        // 开始录制
        QString dataDir = Config::instance()->dirs.dataDir;

        // 确保数据目录存在
        QDir dir;
        if (!dir.exists(dataDir)) {
            if (!dir.mkpath(dataDir)) {
                LOG_ERROR(QString("Failed to create data directory: %1").arg(dataDir));
                ui->recordLogCheckBox->setChecked(false);
                QMessageBox::critical(this, "Error", QString("Failed to create data directory:\n%1").arg(dataDir));
                return;
            }
        }

        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
        QString fileName = dataDir + "/imu_data_" + timestamp + ".csv";

        QVariantMap metadata;
        metadata["device"] = ui->portComboBox->currentText();
        metadata["baudRate"] = ui->baudRateComboBox->currentText();
        metadata["timestamp"] = timestamp;

        if (m_dataRecorder->startRecording(fileName, DataRecorder::CSV, metadata)) {
            LOG_INFO(QString("Recording started: %1").arg(fileName));
            statusBar()->showMessage(QString("Recording to: %1").arg(fileName));
        } else {
            ui->recordLogCheckBox->setChecked(false);
            QMessageBox::critical(this, "Error", "Failed to start recording");
        }
    } else {
        // 停止录制
        m_dataRecorder->stopRecording();
        int count = m_dataRecorder->recordedCount();
        LOG_INFO(QString("Recording stopped. Total records: %1").arg(count));
        statusBar()->showMessage(QString("Recording stopped. %1 records saved.").arg(count), 3000);
    }
}

void MainWindow::on_settingsButton_clicked()
{
    // 显示设置菜单
    QPoint globalPos = ui->settingsButton->mapToGlobal(QPoint(0, ui->settingsButton->height()));
    m_settingsMenu->exec(globalPos);
}

void MainWindow::on_darkModeButton_toggled(bool checked)
{
    loadStyleSheet(checked);
    LOG_INFO(QString("Dark mode: %1").arg(checked ? "ON" : "OFF"));
}

void MainWindow::onCommandSettingsTriggered()
{
    // 创建并显示指令设置对话框
    CommandSettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        LOG_INFO("Command settings saved");
        statusBar()->showMessage("指令设置已保存", 3000);
    }
}

void MainWindow::on_menuButton_clicked()
{
    // 显示上下文菜单
    QPoint pos = ui->menuButton->mapToGlobal(QPoint(0, ui->menuButton->height()));
    m_contextMenu->exec(pos);
}

void MainWindow::on_filterButton_clicked()
{
    // TODO: 实现数据过滤功能
    QMessageBox::information(this, "Filter", "Data filter functionality (To be implemented)");
}

void MainWindow::on_xRangeComboBox_currentTextChanged(const QString &text)
{
    m_xRange = text.toInt();
    if (m_linePlot) {
        // 根据 X 范围和数据采样率计算最大点数
        // 数据采样率 = 1000ms / kDataTimerInterval = 10 Hz
        int samplingRate = 1000 / kDataTimerInterval;  // 每秒点数
        int maxPoints = m_xRange * samplingRate / m_xDot;  // 考虑点密度
        m_linePlot->setMaxDataPoints(maxPoints);
    }
    LOG_DEBUG(QString("X Range changed to: %1s, max points: %2")
              .arg(m_xRange)
              .arg(m_xRange * (1000 / kDataTimerInterval) / m_xDot));
}

void MainWindow::on_xDotComboBox_currentTextChanged(const QString &text)
{
    m_xDot = text.toInt();
    if (m_linePlot) {
        // 更新最大点数（考虑点密度）
        int samplingRate = 1000 / kDataTimerInterval;  // 每秒点数
        int maxPoints = m_xRange * samplingRate / m_xDot;
        m_linePlot->setMaxDataPoints(maxPoints);
    }
    LOG_DEBUG(QString("X Dot changed to: %1, max points: %2")
              .arg(m_xDot)
              .arg(m_xRange * (1000 / kDataTimerInterval) / m_xDot));
}

void MainWindow::on_chartSettingsButton_clicked()
{
    // TODO: 打开图表设置对话框
    QMessageBox::information(this, "Chart Settings",
        "Chart configuration\n"
        "- Y axis range\n"
        "- Line colors\n"
        "- Grid options\n"
        "(To be implemented)");
}

void MainWindow::onDeviceConnectionChanged(bool connected)
{
    updateConnectionStatus(connected);
}

void MainWindow::onDeviceDataReceived(const QByteArray &data)
{
    processData(data);
}

void MainWindow::onDeviceError(const QString &error)
{
    m_errorCount++;
    updateIMUStatus("Error", 0, error);
    LOG_ERROR(QString("Device error: %1").arg(error));

    QMessageBox::warning(this, "Device Error", error);
}

void MainWindow::onUpdateTimer()
{
    // 更新UI（较低频率）
    if (m_linePlot) {
        m_linePlot->refresh();
    }
}

void MainWindow::onDataUpdateTimer()
{
    // 计算数据速率
    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    if (m_lastDataRateTime > 0) {
        double dt = (currentTime - m_lastDataRateTime) / 1000.0;  // 秒
        int deltaPackets = m_packetCount - m_lastPacketCount;
        double dataRate = deltaPackets / dt;

        updateIMUStatus("Running", dataRate, "None");
    }

    m_lastDataRateTime = currentTime;
    m_lastPacketCount = m_packetCount;
}

void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        statusBar()->showMessage("Device Connected");
        m_updateTimer->start(kUpdateTimerInterval);
    } else {
        statusBar()->showMessage("Device Disconnected");
        m_updateTimer->stop();
        updateIMUStatus("Disconnected", 0, "None");
    }
}

void MainWindow::processData(const QByteArray &data)
{
    // 解析数据协议
    // 数据格式：ROLL,PITCH,YAW,AX,AY,AZ,GX,GY,GZ,MX,MY,MZ,TEMP

    QString dataStr = QString::fromUtf8(data).trimmed();

    // 检查空数据
    if (dataStr.isEmpty()) {
        LOG_WARNING("Received empty data");
        return;
    }

    QStringList values = dataStr.split(',');

    // 验证数据字段数量
    if (values.size() < kExpectedDataFields) {
        LOG_ERROR(QString("Invalid data format. Expected %1 fields, got %2. Data: %3")
                  .arg(kExpectedDataFields)
                  .arg(values.size())
                  .arg(dataStr));
        m_errorCount++;
        updateIMUStatus("Error", 0, QString("Invalid format (%1 fields)").arg(values.size()));
        return;
    }

    // 解析姿态角并进行错误检查
    bool ok = true;
    double roll = values[0].toDouble(&ok);
    if (!ok) {
        LOG_ERROR(QString("Invalid roll value: %1").arg(values[0]));
        m_errorCount++;
        updateIMUStatus("Error", 0, QString("Invalid roll: %1").arg(values[0]));
        return;
    }

    double pitch = values[1].toDouble(&ok);
    if (!ok) {
        LOG_ERROR(QString("Invalid pitch value: %1").arg(values[1]));
        m_errorCount++;
        updateIMUStatus("Error", 0, QString("Invalid pitch: %1").arg(values[1]));
        return;
    }

    double yaw = values[2].toDouble(&ok);
    if (!ok) {
        LOG_ERROR(QString("Invalid yaw value: %1").arg(values[2]));
        m_errorCount++;
        updateIMUStatus("Error", 0, QString("Invalid yaw: %1").arg(values[2]));
        return;
    }

    updateAttitudeDisplay(roll, pitch, yaw);

    // 更新数据表格 - 添加错误检查
    double accX = values[3].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid AccX: %1").arg(values[3])); return; }
    double accY = values[4].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid AccY: %1").arg(values[4])); return; }
    double accZ = values[5].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid AccZ: %1").arg(values[5])); return; }
    double gyroX = values[6].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid GyroX: %1").arg(values[6])); return; }
    double gyroY = values[7].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid GyroY: %1").arg(values[7])); return; }
    double gyroZ = values[8].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid GyroZ: %1").arg(values[8])); return; }
    double magX = values[9].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid MagX: %1").arg(values[9])); return; }
    double magY = values[10].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid MagY: %1").arg(values[10])); return; }
    double magZ = values[11].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid MagZ: %1").arg(values[11])); return; }
    double temp = values[12].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid Temperature: %1").arg(values[12])); return; }

    updateDataTable("AccX", accX, "m/s²");
    updateDataTable("AccY", accY, "m/s²");
    updateDataTable("AccZ", accZ, "m/s²");
    updateDataTable("GyroX", gyroX, "°/s");
    updateDataTable("GyroY", gyroY, "°/s");
    updateDataTable("GyroZ", gyroZ, "°/s");
    updateDataTable("MagX", magX, "μT");
    updateDataTable("MagY", magY, "μT");
    updateDataTable("MagZ", magZ, "μT");
    updateDataTable("Temperature", temp, "°C");

    // 添加到图表（以Roll为例）
    double time = (QDateTime::currentMSecsSinceEpoch() - m_startTime) / 1000.0;

    // 限制容器大小，避免内存无限增长
    if (m_xData.size() >= kMaxDataPoints) {
        m_xData.removeFirst();
        m_yData.removeFirst();
    }

    m_xData.append(time);
    m_yData.append(roll);

    if (m_linePlot) {
        m_linePlot->addDataPoint(time, roll);
    }

    // 录制数据
    if (m_dataRecorder->isRecording()) {
        QVariantMap dataMap;
        dataMap["time"] = time;
        dataMap["roll"] = roll;
        dataMap["pitch"] = pitch;
        dataMap["yaw"] = yaw;
        dataMap["ax"] = accX;
        dataMap["ay"] = accY;
        dataMap["az"] = accZ;
        dataMap["gx"] = gyroX;
        dataMap["gy"] = gyroY;
        dataMap["gz"] = gyroZ;
        dataMap["mx"] = magX;
        dataMap["my"] = magY;
        dataMap["mz"] = magZ;
        dataMap["temp"] = temp;
        m_dataRecorder->recordData(dataMap);
    }

    m_packetCount++;
}

void MainWindow::updateAttitudeDisplay(double roll, double pitch, double yaw)
{
    m_currentRoll = roll;
    m_currentPitch = pitch;
    m_currentYaw = yaw;

    ui->rollValueLabel->setText(QString::number(roll, 'f', 2));
    ui->pitchValueLabel->setText(QString::number(pitch, 'f', 2));
    ui->yawValueLabel->setText(QString::number(yaw, 'f', 2));
}

void MainWindow::updateDataTable(const QString &message, const QVariant &value, const QString &unit)
{
    if (m_tableRowMap.contains(message)) {
        int row = m_tableRowMap[message];

        // 更新值
        QString valueStr = QString::number(value.toDouble(), 'f', 3);
        ui->dataTableWidget->item(row, kDataTableValueColumn)->setText(valueStr);

        // 更新单位
        if (!unit.isEmpty() && ui->dataTableWidget->item(row, kDataTableUnitColumn)->text().isEmpty()) {
            ui->dataTableWidget->item(row, kDataTableUnitColumn)->setText(unit);
        }
    }
}

void MainWindow::updateIMUStatus(const QString &status, double dataRate, const QString &error)
{
    ui->imuStatusLabel->setText(QString("Status: %1").arg(status));
    ui->imuDataRateLabel->setText(QString("Data Rate: %1 Hz").arg(dataRate, 0, 'f', 1));
    ui->imuErrorLabel->setText(QString("Error: %1").arg(error));
}

void MainWindow::updateTimeDisplay()
{
    static const QString kZeroTime = QStringLiteral("00:00:00.000");

    if (m_startTime > 0 && ui->connectToggleButton->isChecked()) {
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_startTime;

        // 直接计算时分秒毫秒，避免创建QTime对象
        int hours = elapsed / 3600000;
        int minutes = (elapsed % 3600000) / 60000;
        int seconds = (elapsed % 60000) / 1000;
        int msecs = elapsed % 1000;

        QString elapsedStr = QString("%1:%2:%3.%4")
            .arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
            .arg(msecs, 3, 10, QChar('0'));

        ui->startTimeLabel->setText(kZeroTime);
        ui->endTimeLabel->setText(elapsedStr);
    } else {
        ui->startTimeLabel->setText(kZeroTime);
        ui->endTimeLabel->setText(kZeroTime);
    }
}

QString MainWindow::generateBuiltInStyle(bool darkMode)
{
    if (darkMode) {
        return R"(
            /* Dark Mode */
            QMainWindow {
                background-color: #1e1e1e;
                color: #ffffff;
            }

            QWidget#titleBarWidget {
                background-color: #0d3b66;
                color: #ffffff;
            }

            QWidget#controlBarWidget {
                background-color: #2d2d2d;
                border-bottom: 1px solid #3d3d3d;
            }

            QPushButton {
                background-color: #4CAF50;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 6px 12px;
                font-size: 13px;
            }

            QPushButton:hover {
                background-color: #45a049;
            }

            QPushButton:pressed {
                background-color: #3d8b40;
            }

            QPushButton:checked {
                background-color: #f44336;
            }

            QComboBox, QSpinBox {
                border: 1px solid #3d3d3d;
                border-radius: 4px;
                padding: 4px;
                background-color: #2d2d2d;
                color: #ffffff;
            }

            QTableWidget {
                gridline-color: #3d3d3d;
                background-color: #2d2d2d;
                alternate-background-color: #353535;
                color: #ffffff;
            }

            QTableWidget::item:selected {
                background-color: #4CAF50;
                color: white;
            }

            QHeaderView::section {
                background-color: #1e1e1e;
                padding: 6px;
                border: 1px solid #3d3d3d;
                font-weight: bold;
                color: #ffffff;
            }

            QGroupBox {
                border: 2px solid #3d3d3d;
                border-radius: 6px;
                margin-top: 10px;
                font-weight: bold;
                padding-top: 10px;
                color: #ffffff;
            }

            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 5px;
                background-color: #1e1e1e;
            }

            QToolButton {
                background-color: transparent;
                border: 1px solid transparent;
                border-radius: 4px;
                padding: 4px;
                font-size: 16px;
            }

            QToolButton:hover {
                background-color: #3d3d3d;
                border: 1px solid #4d4d4d;
            }

            QStatusBar {
                background-color: #2d2d2d;
                color: #ffffff;
            }
        )";
    } else {
        return R"(
            /* Light Mode */
            QMainWindow {
                background-color: #f5f5f5;
                color: #212121;
            }

            QWidget#titleBarWidget {
                background-color: #0d3b66;
                color: #ffffff;
            }

            QWidget#controlBarWidget {
                background-color: #fafafa;
                border-bottom: 1px solid #e0e0e0;
            }

            QPushButton {
                background-color: #4CAF50;
                color: white;
                border: none;
                border-radius: 4px;
                padding: 6px 12px;
                font-size: 13px;
            }

            QPushButton:hover {
                background-color: #45a049;
            }

            QPushButton:pressed {
                background-color: #3d8b40;
            }

            QPushButton:checked {
                background-color: #f44336;
            }

            QComboBox, QSpinBox {
                border: 1px solid #ddd;
                border-radius: 4px;
                padding: 4px;
                background-color: white;
            }

            QTableWidget {
                gridline-color: #ddd;
                background-color: white;
                alternate-background-color: #f9f9f9;
            }

            QTableWidget::item:selected {
                background-color: #4CAF50;
                color: white;
            }

            QHeaderView::section {
                background-color: #f0f0f0;
                padding: 6px;
                border: 1px solid #ddd;
                font-weight: bold;
            }

            QGroupBox {
                border: 2px solid #ddd;
                border-radius: 6px;
                margin-top: 10px;
                font-weight: bold;
                padding-top: 10px;
            }

            QGroupBox::title {
                subcontrol-origin: margin;
                subcontrol-position: top left;
                padding: 0 5px;
                background-color: white;
            }

            QToolButton {
                background-color: transparent;
                border: 1px solid transparent;
                border-radius: 4px;
                padding: 4px;
                font-size: 16px;
            }

            QToolButton:hover {
                background-color: #e0e0e0;
                border: 1px solid #d0d0d0;
            }

            QStatusBar {
                background-color: #fafafa;
            }
        )";
    }
}
