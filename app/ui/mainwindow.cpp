#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config/config.h"
#include "commandsettingsdialog.h"
#include "commandsenddialog.h"
#include "deviceconfigdialog.h"
#include "turntablecontroldialog.h"
#include "ovencontroldialog.h"
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
#include <QSettings>
#include <QSignalBlocker>

namespace {
QString checksumTypeToString(ChecksumType type)
{
    switch (type) {
        case ChecksumType::None: return "None";
        case ChecksumType::Sum: return "Sum";
        case ChecksumType::XOR: return "XOR";
        case ChecksumType::CRC8: return "CRC8";
        case ChecksumType::CRC16_XMODEM: return "CRC16/XMODEM";
        case ChecksumType::CRC32: return "CRC32";
    }
    return "Unknown";
}

QString checksumScopeToString(ChecksumScope scope)
{
    switch (scope) {
        case ChecksumScope::FullFrame: return "FullFrame";
        case ChecksumScope::AfterHeader: return "AfterHeader";
        case ChecksumScope::DataOnly: return "DataOnly";
        case ChecksumScope::Custom: return "Custom";
    }
    return "Unknown";
}

QString byteOrderToString(ByteOrder order)
{
    return (order == ByteOrder::LittleEndian) ? "LittleEndian" : "BigEndian";
}

void logParserConfig(const QString &context, const ProtocolConfig &cfg)
{
    LOG_INFO(QString("[%1] Parser config: header=%2, footer=%3, lengthPos=%4, "
                     "checksumType=%5, checksumScope=%6, checksumStart=%7, checksumLength=%8, "
                     "checksumPos=%9, byteOrder=%10, checksumByteOrder=%11, fields=%12")
                 .arg(context)
                 .arg(QString::fromLatin1(cfg.frameHeader.toHex(' ').toUpper()))
                 .arg(QString::fromLatin1(cfg.frameFooter.toHex(' ').toUpper()))
                 .arg(cfg.lengthPosition)
                 .arg(checksumTypeToString(cfg.checksumType))
                 .arg(checksumScopeToString(cfg.checksumScope))
                 .arg(cfg.checksumStart)
                 .arg(cfg.checksumLength)
                 .arg(cfg.checksumPosition)
                 .arg(byteOrderToString(cfg.byteOrder))
                 .arg(byteOrderToString(cfg.checksumByteOrder))
                 .arg(cfg.fields.size()));
}

bool looksLikeTurntableFeedback(const QByteArray &data)
{
    return data.indexOf(QByteArrayLiteral("ZTB")) >= 0
        || data.indexOf(QByteArrayLiteral("ZTD")) >= 0;
}
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_deviceManager(new DeviceManager(this))
    , m_monitorPanel(nullptr)
    , m_3dView(nullptr)
    , m_histogramPlot(nullptr)
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
    , m_lastUiPacketCount(0)
    , m_lastDataRateTime(0)
    , m_lastPacketCount(0)
    , m_lastErrorDialogTime(0)
    , m_lastParseErrorLogTime(0)
    , m_suppressedParseErrorCount(0)
    , m_isDarkMode(false)
{
    ui->setupUi(this);
    setupUI();
    setupConnections();
    setupMenu();
    loadPlugins();
    const bool savedDarkMode = QSettings("GenericScope", "UI").value("theme/darkMode", false).toBool();
    {
        const QSignalBlocker blocker(ui->darkModeButton);
        ui->darkModeButton->setChecked(savedDarkMode);
    }
    loadStyleSheet(savedDarkMode);

    // 预加载协议配置（修复：即使不打开协议配置对话框，也能添加图表）
    preloadProtocols();
    if (ensureProtocolParser("startup")) {
        LOG_INFO("Protocol parser initialized on startup");
    }

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
    ui->portComboBox->setCurrentText("COM7");
    ui->baudRateComboBox->setCurrentText("115200");

    // 初始化传输类型：默认串口，隐藏 UDP 控件
    ui->udpRemoteIpEdit->setVisible(false);
    ui->udpRemotePortSpinBox->setVisible(false);
    ui->udpLocalPortSpinBox->setVisible(false);

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
    m_settingsMenu->addSeparator();
    m_settingsMenu->addAction("指令发送", this, &MainWindow::onCommandSendTriggered);
    m_settingsMenu->addAction("转台控制", this, &MainWindow::onTurntableControlTriggered);
    m_settingsMenu->addAction("烘箱控制", this, &MainWindow::onOvenControlTriggered);

    m_contextMenu->addAction("Export Data", this, [this]() {
        QMessageBox::information(this, "Export", "Export data functionality");
    });
    m_contextMenu->addAction("Clear Chart", this, [this]() {
        if (m_monitorPanel) {
            m_monitorPanel->clearAllCharts();
        }
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

void MainWindow::preloadProtocols()
{
    ProtocolManager *manager = ProtocolManager::instance();
    if (!manager) {
        LOG_WARNING("ProtocolManager not available when preloading protocols");
        return;
    }

    const int protocolCount = manager->loadProtocolsFromSettings("GenericScope", "ProtocolConfig");
    if (protocolCount > 0) {
        qDebug() << QString("[MainWindow] 预加载了 %1 个协议配置").arg(protocolCount);
        QString currentProtocol = manager->getCurrentProtocol();
        if (currentProtocol.isEmpty() || !manager->hasProtocol(currentProtocol)) {
            const QStringList names = manager->getProtocolNames();
            if (!names.isEmpty()) {
                currentProtocol = names.first();
                manager->setCurrentProtocol(currentProtocol);
                LOG_INFO(QString("Auto-selected protocol on preload: %1").arg(currentProtocol));
            }
        }
    } else {
        qDebug() << "[MainWindow] 未找到已保存的协议配置";
    }
}

void MainWindow::loadStyleSheet(bool darkMode)
{
    QString qssFile = darkMode ? ":/qss/dark.qss" : ":/qss/light.qss";
    QFile file(qssFile);

    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        QString styleSheet = stream.readAll();
        qApp->setStyleSheet(styleSheet);
        file.close();
    } else {
        // 如果无法加载QSS文件，使用内置样式
        QString builtInStyle = generateBuiltInStyle(darkMode);
        qApp->setStyleSheet(builtInStyle);
        LOG_WARNING(QString("Failed to load %1, fallback to built-in style").arg(qssFile));
    }

    m_isDarkMode = darkMode;
    updateThemeToggleButton(darkMode);

    if (m_logWidget) {
        m_logWidget->setTheme(darkMode);
    }

    if (m_3dView) {
        m_3dView->setBackgroundColor(darkMode
            ? QColor(0x25, 0x2C, 0x34)
            : QColor(0xF7, 0xF9, 0xFB));
    }

    if (m_monitorPanel) {
        m_monitorPanel->update();
    }
}

void MainWindow::updateThemeToggleButton(bool darkMode)
{
    if (!ui || !ui->darkModeButton) {
        return;
    }

    ui->darkModeButton->setText(darkMode ? QStringLiteral("L") : QStringLiteral("D"));
    ui->darkModeButton->setToolTip(darkMode ? QStringLiteral("Switch to light mode")
                                            : QStringLiteral("Switch to dark mode"));
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

void MainWindow::populateDefaultDataTableRows()
{
    static const QStringList kDefaultMessages = {
        QStringLiteral("AccX"), QStringLiteral("AccY"), QStringLiteral("AccZ"),
        QStringLiteral("GyroX"), QStringLiteral("GyroY"), QStringLiteral("GyroZ"),
        QStringLiteral("MagX"), QStringLiteral("MagY"), QStringLiteral("MagZ"),
        QStringLiteral("Temperature")
    };

    for (const QString &message : kDefaultMessages) {
        const int row = ui->dataTableWidget->rowCount();
        ui->dataTableWidget->insertRow(row);
        ui->dataTableWidget->setItem(row, kDataTableMessageColumn, new QTableWidgetItem(message));
        ui->dataTableWidget->setItem(row, kDataTableValueColumn, new QTableWidgetItem(QStringLiteral("-")));
        ui->dataTableWidget->setItem(row, kDataTableUnitColumn, new QTableWidgetItem(QString()));
        m_tableRowMap[message] = row;
    }
}

void MainWindow::rebuildDataTableFromProtocol()
{
    LOG_INFO("Starting rebuildDataTableFromProtocol");

    // 清空现有行
    ui->dataTableWidget->setRowCount(0);
    m_tableRowMap.clear();

    // 添加空指针检查
    ProtocolManager *manager = ProtocolManager::instance();
    if (!manager) {
        LOG_INFO("ProtocolManager not initialized, using default IMU fields");
        // 使用默认字段
        populateDefaultDataTableRows();
        return;
    }

    // 获取当前协议
    const QString currentProtocol = manager->getCurrentProtocol();

    if (!currentProtocol.isEmpty() && manager->hasProtocol(currentProtocol)) {
        // 从协议配置获取字段
        const ProtocolConfig config = manager->getProtocol(currentProtocol);

        LOG_INFO(QString("Rebuilding data table from protocol: %1 with %2 fields")
                     .arg(currentProtocol)
                     .arg(config.fields.size()));

        for (const FieldConfig &field : config.fields) {
            const int row = ui->dataTableWidget->rowCount();
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

        populateDefaultDataTableRows();
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
        Config *cfg = Config::instance();
        const bool isUdp = (ui->transferTypeComboBox->currentIndex() == 1);

        if (isUdp) {
            cfg->device.type = "UDP";
            cfg->device.udpRemoteIp = ui->udpRemoteIpEdit->text();
            cfg->device.udpRemotePort = ui->udpRemotePortSpinBox->value();
            cfg->device.udpLocalPort  = ui->udpLocalPortSpinBox->value();
        } else {
            cfg->device.type = "UART";
            cfg->device.port = ui->portComboBox->currentText();
            cfg->device.baudRate = ui->baudRateComboBox->currentText().toInt();
        }

        if (m_deviceManager->connectDevice()) {
            ui->connectToggleButton->setText("Disconnect");
            ui->transferTypeComboBox->setEnabled(false);
            ui->portComboBox->setEnabled(false);
            ui->baudRateComboBox->setEnabled(false);
            ui->udpRemoteIpEdit->setEnabled(false);
            ui->udpRemotePortSpinBox->setEnabled(false);
            ui->udpLocalPortSpinBox->setEnabled(false);

            if (isUdp) {
                LOG_INFO(QString("UDP connected: local=%1, remote=%2:%3")
                             .arg(cfg->device.udpLocalPort)
                             .arg(cfg->device.udpRemoteIp)
                             .arg(cfg->device.udpRemotePort));
            } else {
                LOG_INFO(QString("Connected to %1 @ %2 baud")
                             .arg(cfg->device.port)
                             .arg(cfg->device.baudRate));
            }

            m_deviceManager->startPolling();
            m_dataTimer->start(kStatusRefreshIntervalMs);
            resetRuntimeState(QDateTime::currentMSecsSinceEpoch());
        } else {
            ui->connectToggleButton->setChecked(false);
            QString errorMsg = isUdp
                ? QString("Failed to open UDP local port: %1").arg(cfg->device.udpLocalPort)
                : QString("Failed to connect to %1 at %2 baud")
                      .arg(cfg->device.port).arg(cfg->device.baudRate);
            LOG_ERROR(errorMsg);
            QMessageBox::critical(this, "Connection Error", errorMsg);
        }
    } else {
        // 断开连接
        m_deviceManager->stopPolling();
        m_deviceManager->disconnectDevice();
        m_dataTimer->stop();
        resetRuntimeState(0);

        ui->connectToggleButton->setText("Connect");
        ui->transferTypeComboBox->setEnabled(true);
        const bool isUdp = (ui->transferTypeComboBox->currentIndex() == 1);
        ui->portComboBox->setEnabled(!isUdp);
        ui->baudRateComboBox->setEnabled(!isUdp);
        ui->udpRemoteIpEdit->setEnabled(isUdp);
        ui->udpRemotePortSpinBox->setEnabled(isUdp);
        ui->udpLocalPortSpinBox->setEnabled(isUdp);
        // 根据当前类型恢复对应控件
        on_transferTypeComboBox_currentIndexChanged(ui->transferTypeComboBox->currentIndex());

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

void MainWindow::on_transferTypeComboBox_currentIndexChanged(int index)
{
    const bool isUdp = (index == 1);
    // 串口控件
    ui->portComboBox->setVisible(!isUdp);
    ui->baudRateComboBox->setVisible(!isUdp);
    // UDP 控件
    ui->udpRemoteIpEdit->setVisible(isUdp);
    ui->udpRemotePortSpinBox->setVisible(isUdp);
    ui->udpLocalPortSpinBox->setVisible(isUdp);
    const bool connected = ui->connectToggleButton->isChecked();
    ui->portComboBox->setEnabled(!isUdp && !connected);
    ui->baudRateComboBox->setEnabled(!isUdp && !connected);
    ui->udpRemoteIpEdit->setEnabled(isUdp && !connected);
    ui->udpRemotePortSpinBox->setEnabled(isUdp && !connected);
    ui->udpLocalPortSpinBox->setEnabled(isUdp && !connected);
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
    QSettings("GenericScope", "UI").setValue("theme/darkMode", checked);

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

void MainWindow::onCommandSendTriggered()
{
    CommandSendDialog dialog(m_deviceManager, this);
    dialog.exec();
}

void MainWindow::onTurntableControlTriggered()
{
    TurntableControlDialog dialog(this);
    dialog.exec();
}

void MainWindow::onOvenControlTriggered()
{
    OvenControlDialog dialog(this);
    dialog.exec();
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

    // 重建协议解析器
    if (!createProtocolParser(name, "protocol-changed")) {
        m_protocolParser.reset();
        LOG_WARNING(QString("Protocol %1 not found, parser not created").arg(name));
    }

    statusBar()->showMessage(QString("已切换到协议: %1").arg(name), 3000);
    m_rxBuffer.clear();  // 切协议后清空缓冲，避免旧协议残留字节导致误解析
    resetParseErrorState();

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

void MainWindow::onDeviceConnectionChanged(bool connected)
{
    updateConnectionStatus(connected);
}

void MainWindow::onDeviceDataReceived(const QByteArray &data)
{
    if (looksLikeTurntableFeedback(data)) {
        return;
    }

    processData(data);
}

void MainWindow::onDeviceError(const QString &error)
{
    m_errorCount++;
    updateIMUStatus("Error", 0, error);
    LOG_ERROR(QString("Device error: %1").arg(error));

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const bool isRepeated = (error == m_lastErrorMessage);
    const bool shouldShowDialog = !isRepeated || (now - m_lastErrorDialogTime >= kErrorDialogThrottleMs);
    if (shouldShowDialog) {
        QMessageBox::warning(this, "Device Error", error);
        m_lastErrorDialogTime = now;
        m_lastErrorMessage = error;
    }
}

void MainWindow::onUpdateTimer()
{
    flushPendingDataTableUpdates();

    // 性能优化：仅在收到新数据包时刷新图表区域，避免空转重绘导致卡顿
    if (m_packetCount == m_lastUiPacketCount) {
        return;
    }

    if (m_monitorPanel) {
        m_monitorPanel->update();
    }
    m_lastUiPacketCount = m_packetCount;
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

void MainWindow::resetParseErrorState()
{
    m_lastParseErrorLogTime = 0;
    m_lastParseErrorMessage.clear();
    m_suppressedParseErrorCount = 0;
}

void MainWindow::resetRuntimeState(qint64 startTimeMs)
{
    m_startTime = startTimeMs;
    m_packetCount = 0;
    m_lastUiPacketCount = 0;
    m_errorCount = 0;
    m_lastPacketCount = 0;
    m_lastDataRateTime = 0;
    m_lastErrorDialogTime = 0;
    m_lastErrorMessage.clear();
    m_pendingTableValues.clear();
    m_pendingTableUnits.clear();
    resetParseErrorState();
}

void MainWindow::flushPendingDataTableUpdates()
{
    if (m_pendingTableValues.isEmpty()) {
        return;
    }

    for (auto it = m_pendingTableValues.begin(); it != m_pendingTableValues.end(); ++it) {
        const QString &message = it.key();
        if (!m_tableRowMap.contains(message)) {
            continue;
        }

        const int row = m_tableRowMap.value(message);
        if (QTableWidgetItem *valueItem = ui->dataTableWidget->item(row, kDataTableValueColumn)) {
            valueItem->setText(QString::number(it.value().toDouble(), 'f', 3));
        }

        const QString unit = m_pendingTableUnits.value(message);
        if (!unit.isEmpty()) {
            if (QTableWidgetItem *unitItem = ui->dataTableWidget->item(row, kDataTableUnitColumn)) {
                if (unitItem->text().isEmpty()) {
                    unitItem->setText(unit);
                }
            }
        }
    }

    m_pendingTableValues.clear();
    m_pendingTableUnits.clear();
}

bool MainWindow::createProtocolParser(const QString &protocolName, const QString &logContext)
{
    ProtocolManager *manager = ProtocolManager::instance();
    if (!manager || protocolName.isEmpty() || !manager->hasProtocol(protocolName)) {
        return false;
    }

    const ProtocolConfig config = manager->getProtocol(protocolName);
    m_protocolParser.reset(new ProtocolParser(config));
    LOG_INFO(QString("Protocol parser created for: %1").arg(protocolName));
    logParserConfig(logContext, config);
    return true;
}

bool MainWindow::ensureProtocolParser(const QString &logContext)
{
    if (m_protocolParser) {
        return true;
    }

    ProtocolManager *manager = ProtocolManager::instance();
    if (!manager) {
        return false;
    }

    QString currentProtocol = manager->getCurrentProtocol();
    if (currentProtocol.isEmpty() || !manager->hasProtocol(currentProtocol)) {
        const QStringList names = manager->getProtocolNames();
        if (!names.isEmpty()) {
            currentProtocol = names.first();
            manager->setCurrentProtocol(currentProtocol);
            LOG_INFO(QString("Auto-recovered current protocol: %1").arg(currentProtocol));
        }
    }

    return createProtocolParser(currentProtocol, logContext);
}

void MainWindow::updateConnectionStatus(bool connected)
{
    if (connected) {
        statusBar()->showMessage("Device Connected");
        m_updateTimer->start(kUiRefreshIntervalMs);
    } else {
        statusBar()->showMessage("Device Disconnected");
        m_updateTimer->stop();
        updateIMUStatus("Disconnected", 0, "None");
        m_pendingTableValues.clear();
        m_pendingTableUnits.clear();
        resetParseErrorState();
        m_rxBuffer.clear();  // 断开连接时清空缓冲
        resetParseErrorState();
    }
}

void MainWindow::processData(const QByteArray &data)
{
    // ========== 使用ProtocolParser动态解析 ==========
    if (!ensureProtocolParser("auto-create")) {
        // No protocol parser available, fallback to legacy parsing for compatibility.
        processDataLegacy(data);
        return;
    }

    // 增量缓冲：解决串口分包/粘包
    if (!data.isEmpty()) {
        m_rxBuffer.append(data);
    }

    static constexpr int kMaxRxBufferBytes = 64 * 1024;
    if (m_rxBuffer.size() > kMaxRxBufferBytes) {
        m_rxBuffer = m_rxBuffer.right(kMaxRxBufferBytes / 2);
        LOG_WARNING("RX buffer too large, trimmed to prevent memory growth");
    }

    while (!m_rxBuffer.isEmpty()) {
        ParseResult result = m_protocolParser->parse(m_rxBuffer);

        if (!result.success) {
            if (result.errorMsg == "Incomplete frame") {
                break;
                // 数据还没收全，等待下一包
                break;
            }

            if (result.errorMsg == "Frame header not found") {
                // 保留帧头长度-1个字节，防止截断帧头
                int keepBytes = 0;
                const ProtocolConfig &cfg = m_protocolParser->config();
                if (!cfg.frameHeader.isEmpty()) {
                    keepBytes = qMax(0, cfg.frameHeader.size() - 1);
                }
                if (m_rxBuffer.size() > keepBytes) {
                    m_rxBuffer = m_rxBuffer.right(keepBytes);
                }
                break;
            }

            // 其它错误：丢弃1字节继续搜帧（重同步时可能出现大量可恢复错误）
            const bool isChecksumMismatch = (result.errorMsg == "Checksum verification failed");
            const qint64 now = QDateTime::currentMSecsSinceEpoch();
            const int throttleMs = isChecksumMismatch ? kChecksumResyncWarnIntervalMs
                                                      : kParseErrorLogThrottleMs;
            const bool shouldThrottle = ((now - m_lastParseErrorLogTime) < throttleMs);
            const QByteArray inspectData = !result.rawData.isEmpty()
                ? result.rawData
                : m_rxBuffer.left(qMin(m_rxBuffer.size(), 64));
            const QString inspectHex = QString::fromLatin1(inspectData.toHex(' ').toUpper());

            if (shouldThrottle) {
                ++m_suppressedParseErrorCount;
            } else {
                if (isChecksumMismatch) {
                    const bool hasSuppressed = (m_suppressedParseErrorCount > 0);
                    const QString message = hasSuppressed
                        ? QString("Protocol parse issue: %1 (resyncing stream, suppressed %2 logs). checksum_data[%3B]=%4")
                              .arg(result.errorMsg)
                              .arg(m_suppressedParseErrorCount)
                              .arg(inspectData.size())
                              .arg(inspectHex)
                        : QString("Protocol parse issue: %1 (resyncing stream). checksum_data[%2B]=%3")
                              .arg(result.errorMsg)
                              .arg(inspectData.size())
                              .arg(inspectHex);
                    LOG_WARNING(message);
                } else {
                    if (m_suppressedParseErrorCount > 0) {
                        LOG_ERROR(QString("Protocol parse failed: %1 (suppressed %2 logs)")
                                      .arg(result.errorMsg)
                                      .arg(m_suppressedParseErrorCount));
                    } else {
                        LOG_ERROR(QString("Protocol parse failed: %1").arg(result.errorMsg));
                    }
                }

                m_lastParseErrorLogTime = now;
                m_lastParseErrorMessage = result.errorMsg;
                m_suppressedParseErrorCount = 0;
            }

            m_errorCount++;
            if (!isChecksumMismatch) {
                updateIMUStatus("Error", 0, result.errorMsg);
            }

            if (isChecksumMismatch) {
                const QByteArray frameHeader = m_protocolParser->config().frameHeader;
                // 重同步优化：CRC失败时尽快跳到下一个帧头，避免逐字节扫描导致大量无效告警
                // CRC mismatch: jump directly to the next header if possible.
                if (!frameHeader.isEmpty()) {
                    const int nextHeaderPos = m_rxBuffer.indexOf(frameHeader, 1);
                    if (nextHeaderPos > 0) {
                        m_rxBuffer.remove(0, nextHeaderPos);
                        continue;
                    }
                }
            }

            m_rxBuffer.remove(0, 1);
            continue;
        }

        int consumed = result.consumedBytes;
        if (consumed <= 0 || consumed > m_rxBuffer.size()) {
            consumed = 1;
        }
        m_rxBuffer.remove(0, consumed);

        QMap<QString, double> fieldValues;
        for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
            const QString &fieldName = it.key();
            const QVariant value = it.value();

            bool ok = false;
            const double doubleValue = value.toDouble(&ok);
            if (ok) {
                fieldValues[fieldName] = doubleValue;
                const QString unit = getFieldUnit(fieldName);
                updateDataTable(fieldName, doubleValue, unit);
            } else {
                LOG_WARNING(QString("Field %1 cannot be converted to double: %2")
                           .arg(fieldName).arg(value.toString()));
            }
        }

        if (!fieldValues.isEmpty()) {
            MonitorDataManager::instance()->onProtocolDataParsed(fieldValues);
        }

        if (fieldValues.contains("Roll") && fieldValues.contains("Pitch") && fieldValues.contains("Yaw")) {
            updateAttitudeDisplay(fieldValues["Roll"], fieldValues["Pitch"], fieldValues["Yaw"]);
        }

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
    if (!m_tableRowMap.contains(message)) {
        return;
    }

    m_pendingTableValues[message] = value;
    if (!unit.isEmpty()) {
        m_pendingTableUnits[message] = unit;
    }
    return;
#if 0

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
#endif
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
