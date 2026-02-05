#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config/config.h"
#include "commandsettingsdialog.h"
#include "deviceconfigdialog.h"
#include "common_component/log/logmanager.h"
#include "common_component/plot/monitordatamanager.h"
#include "protocol/protocolmanager.h"
#include "protocol/protocolparser.h"
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
    , m_monitorPanel(nullptr)
    , m_3dView(nullptr)
    , m_logWidget(nullptr)
    , m_dataRecorder(new DataRecorder(this))
    , m_protocolParser(nullptr)
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

    // 设置日志窗口
    setupLogWidget();

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

    // 连接协议管理器信号（使用队列连接避免死锁）
    if (ProtocolManager::instance()) {
        ok = static_cast<bool>(connect(ProtocolManager::instance(), &ProtocolManager::currentProtocolChanged,
                                        this, &MainWindow::onProtocolChanged, Qt::QueuedConnection));
        Q_ASSERT(ok && "Failed to connect protocol manager signals");
    }
}

void MainWindow::setupMenu()
{
    // 创建设置菜单
    m_settingsMenu->addAction("指令设置", this, &MainWindow::onCommandSettingsTriggered);
    m_settingsMenu->addSeparator();
    m_settingsMenu->addAction("设备配置", this, [this]() {
        // 打开设备配置对话框
        DeviceConfigDialog dialog(m_deviceManager, this);
        dialog.exec();
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

    // 尝试从当前协议加载字段，否则使用默认字段
    rebuildDataTableFromProtocol();
}

void MainWindow::rebuildDataTableFromProtocol()
{
    LOG_INFO("Starting rebuildDataTableFromProtocol");

    // 清空现有行
    ui->dataTableWidget->setRowCount(0);
    m_tableRowMap.clear();

    // 添加空指针检查
    if (!ProtocolManager::instance()) {
        LOG_INFO("ProtocolManager not initialized, using default IMU fields");
        // 使用默认字段
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
        return;
    }

    // 获取当前协议
    QString currentProtocol = ProtocolManager::instance()->getCurrentProtocol();

    if (!currentProtocol.isEmpty() && ProtocolManager::instance()->hasProtocol(currentProtocol)) {
        // 从协议配置获取字段
        ProtocolConfig config = ProtocolManager::instance()->getProtocol(currentProtocol);

        LOG_INFO(QString("Rebuilding data table from protocol: %1 with %2 fields")
                     .arg(currentProtocol)
                     .arg(config.fields.size()));

        for (const FieldConfig &field : config.fields) {
            int row = ui->dataTableWidget->rowCount();
            ui->dataTableWidget->insertRow(row);

            // 字段名称
            ui->dataTableWidget->setItem(row, kDataTableMessageColumn,
                                         new QTableWidgetItem(field.name));
            // 初始值
            ui->dataTableWidget->setItem(row, kDataTableValueColumn,
                                         new QTableWidgetItem(QStringLiteral("-")));
            // 单位
            ui->dataTableWidget->setItem(row, kDataTableUnitColumn,
                                         new QTableWidgetItem(field.unit));

            // 建立字段名称到行号的映射
            m_tableRowMap[field.name] = row;
        }

        statusBar()->showMessage(QString("已加载协议 %1 的 %2 个字段")
                                     .arg(currentProtocol)
                                     .arg(config.fields.size()), 3000);
    } else {
        // 使用默认字段（IMU数据）
        LOG_INFO("No protocol configured, using default IMU fields");

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
}

void MainWindow::setup3DVisualization()
{
    // 创建3D可视化组件
    m_3dView = new IMU3DView(this);

    // 将3D视图添加到中间面板容器
    QVBoxLayout *layout = new QVBoxLayout(ui->visualization3DContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_3dView);

    // 连接信号（可选，用于调试）
    connect(m_3dView, &IMU3DView::angleChanged, this, [](double roll, double pitch, double yaw) {
        qDebug() << "3D View angle changed - Roll:" << roll << "Pitch:" << pitch << "Yaw:" << yaw;
    });
}

void MainWindow::setupChart()
{
    // 创建监控面板（替换原有的LinePlot）
    m_monitorPanel = new MonitorPanel(this);

    // 添加到图表容器
    QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addWidget(m_monitorPanel);

    // 保留原有数据容器（其他地方可能使用）
    m_xData.reserve(kMaxDataPoints);
    m_yData.reserve(kMaxDataPoints);

    // 旧的LinePlot已被MonitorPanel替换
    m_linePlot = nullptr;
}

void MainWindow::setupLogWidget()
{
    // 创建日志窗口
    m_logWidget = new LogWidget(this);
    m_logWidget->setMinimumHeight(150);
    m_logWidget->setMaximumHeight(250);

    // 设置初始主题
    m_logWidget->setTheme(m_isDarkMode);

    // 将日志窗口添加到主布局的底部
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(ui->centralwidget->layout());
    if (mainLayout) {
        mainLayout->addWidget(m_logWidget);
    }

    // 添加初始日志
    LogManager::instance()->info("GenericScope started");
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

    // 更新日志窗口主题
    if (m_logWidget) {
        m_logWidget->setTheme(checked);
    }

    // 更新3D视图背景色
    if (m_3dView) {
        QPalette pal = QApplication::palette();
        QColor bgColor = pal.color(QPalette::Window);
        if (bgColor.lightness() < 128) {
            m_3dView->setBackgroundColor(QColor(0x1E, 0x1E, 0x1E)); // 深色主题
        } else {
            m_3dView->setBackgroundColor(QColor(0xF5, 0xF5, 0xF5)); // 浅色主题
        }
    }

    // 强制刷新所有图表组件以应用新颜色
    if (m_linePlot) {
        m_linePlot->update();
    }

    LOG_INFO(QString("Dark mode: %1").arg(checked ? "ON" : "OFF"));
}

void MainWindow::onCommandSettingsTriggered()
{
    // 创建并显示指令设置对话框
    CommandSettingsDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        LOG_INFO("Command settings saved");
        statusBar()->showMessage("指令设置已保存", 3000);

        // 同步协议字段到数据表格
        rebuildDataTableFromProtocol();
    }
}

void MainWindow::onProtocolChanged(const QString &name)
{
    LOG_INFO(QString("Protocol changed to: %1").arg(name));

    // 防止空协议名导致的问题
    if (name.isEmpty()) {
        LOG_INFO("Protocol name is empty, skipping rebuild");
        return;
    }

    // 重建数据表格以反映新的协议字段
    rebuildDataTableFromProtocol();

    // ========== 新增：重建协议解析器 ==========
    // 删除旧解析器
    if (m_protocolParser) {
        delete m_protocolParser;
        m_protocolParser = nullptr;
    }

    // 创建新解析器
    if (ProtocolManager::instance() && ProtocolManager::instance()->hasProtocol(name)) {
        ProtocolConfig config = ProtocolManager::instance()->getProtocol(name);
        m_protocolParser = new ProtocolParser(config);
        LOG_INFO(QString("Protocol parser created for: %1").arg(name));
    } else {
        LOG_WARNING(QString("Protocol %1 not found, parser not created").arg(name));
    }
    // ==========================================

    statusBar()->showMessage(QString("已切换到协议: %1").arg(name), 3000);

    // 清空监控面板图表（协议变了，字段可能不匹配）
    if (m_monitorPanel) {
        m_monitorPanel->clearAllCharts();
        LOG_INFO("Monitor panel cleared due to protocol change");
    }

    LOG_INFO("Data table rebuild completed");
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
    // ========== 使用ProtocolParser动态解析 ==========
    if (!m_protocolParser) {
        // 如果没有解析器，尝试创建（兼容旧行为）
        QString currentProtocol = ProtocolManager::instance()->getCurrentProtocol();
        if (!currentProtocol.isEmpty() && ProtocolManager::instance()->hasProtocol(currentProtocol)) {
            ProtocolConfig config = ProtocolManager::instance()->getProtocol(currentProtocol);
            m_protocolParser = new ProtocolParser(config);
            LOG_INFO(QString("Auto-created protocol parser for: %1").arg(currentProtocol));
        } else {
            // 回退到硬编码解析（兼容性）
            processDataLegacy(data);
            return;
        }
    }

    // 使用ProtocolParser解析
    ParseResult result = m_protocolParser->parse(data);

    if (!result.success) {
        LOG_ERROR(QString("Protocol parse failed: %1").arg(result.errorMsg));
        m_errorCount++;
        updateIMUStatus("Error", 0, result.errorMsg);
        return;
    }

    // 转换为double映射并分发给MonitorDataManager
    QMap<QString, double> fieldValues;

    for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
        const QString &fieldName = it.key();
        QVariant value = it.value();

        // 转换为double
        bool ok = false;
        double doubleValue = value.toDouble(&ok);
        if (ok) {
            fieldValues[fieldName] = doubleValue;

            // 更新数据表格（从协议获取单位）
            QString unit = getFieldUnit(fieldName);
            updateDataTable(fieldName, doubleValue, unit);
        } else {
            LOG_WARNING(QString("Field %1 cannot be converted to double: %2")
                       .arg(fieldName).arg(value.toString()));
        }
    }

    // 分发数据给监控面板
    if (!fieldValues.isEmpty()) {
        MonitorDataManager::instance()->onProtocolDataParsed(fieldValues);
    }

    // 更新3D视图（如果有Roll/Pitch/Yaw字段）
    if (fieldValues.contains("Roll") && fieldValues.contains("Pitch") && fieldValues.contains("Yaw")) {
        updateAttitudeDisplay(fieldValues["Roll"], fieldValues["Pitch"], fieldValues["Yaw"]);
    }

    // 录制数据
    if (m_dataRecorder->isRecording()) {
        QVariantMap dataMap;
        dataMap["timestamp"] = QDateTime::currentMSecsSinceEpoch();
        for (auto it = fieldValues.begin(); it != fieldValues.end(); ++it) {
            dataMap[it.key()] = it.value();
        }
        m_dataRecorder->recordData(dataMap);
    }

    m_packetCount++;
}

// 辅助函数：从协议配置获取字段单位
QString MainWindow::getFieldUnit(const QString &fieldName)
{
    if (!m_protocolParser) {
        return QString();
    }

    const ProtocolConfig &config = m_protocolParser->config();
    for (const FieldConfig &field : config.fields) {
        if (field.name == fieldName) {
            return field.unit;
        }
    }
    return QString();
}

// 兼容旧的硬编码解析（作为后备）
void MainWindow::processDataLegacy(const QByteArray &data)
{
    LOG_WARNING("Using legacy hardcoded CSV parsing (no protocol configured)");

    // 原有的硬编码解析逻辑
    QString dataStr = QString::fromUtf8(data).trimmed();

    if (dataStr.isEmpty()) {
        LOG_WARNING("Received empty data");
        return;
    }

    QStringList values = dataStr.split(',');

    if (values.size() < kExpectedDataFields) {
        LOG_ERROR(QString("Invalid data format. Expected %1 fields, got %2. Data: %3")
                  .arg(kExpectedDataFields)
                  .arg(values.size())
                  .arg(dataStr));
        m_errorCount++;
        updateIMUStatus("Error", 0, QString("Invalid format (%1 fields)").arg(values.size()));
        return;
    }

    // 解析姿态角
    bool ok = true;
    double roll = values[0].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid roll: %1").arg(values[0])); return; }

    double pitch = values[1].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid pitch: %1").arg(values[1])); return; }

    double yaw = values[2].toDouble(&ok);
    if (!ok) { LOG_ERROR(QString("Invalid yaw: %1").arg(values[2])); return; }

    updateAttitudeDisplay(roll, pitch, yaw);

    // 解析其他字段
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

    // 构建字段值映射并分发
    QMap<QString, double> fieldValues;
    fieldValues["Roll"] = roll;
    fieldValues["Pitch"] = pitch;
    fieldValues["Yaw"] = yaw;
    fieldValues["AccelX"] = accX;
    fieldValues["AccelY"] = accY;
    fieldValues["AccelZ"] = accZ;
    fieldValues["GyroX"] = gyroX;
    fieldValues["GyroY"] = gyroY;
    fieldValues["GyroZ"] = gyroZ;
    fieldValues["MagX"] = magX;
    fieldValues["MagY"] = magY;
    fieldValues["MagZ"] = magZ;
    fieldValues["Temperature"] = temp;

    MonitorDataManager::instance()->onProtocolDataParsed(fieldValues);

    // 录制数据
    if (m_dataRecorder->isRecording()) {
        QVariantMap dataMap;
        dataMap["time"] = (QDateTime::currentMSecsSinceEpoch() - m_startTime) / 1000.0;
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

    // 更新3D视图
    if (m_3dView) {
        m_3dView->setAttitude(roll, pitch, yaw);
    }
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
