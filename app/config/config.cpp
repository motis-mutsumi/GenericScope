#include "config.h"
#include <QCoreApplication>
#include <QDir>

Config* Config::s_instance = nullptr;
Config* pConfig = nullptr;

Config::Config(QObject *parent)
    : QObject(parent)
{
    QString configPath = QCoreApplication::applicationDirPath() + "/Settings.ini";
    m_settings = new QSettings(configPath, QSettings::IniFormat);
    loadDefaults();
    pConfig = this;
}

Config::~Config()
{
    delete m_settings;
}

Config* Config::instance()
{
    if (!s_instance) {
        s_instance = new Config();
    }
    return s_instance;
}

void Config::loadDefaults()
{
    // 目录配置
    QString appDir = QCoreApplication::applicationDirPath();
    dirs.dataDir = appDir + "/data";
    dirs.logDir = appDir + "/logs";
    dirs.configDir = appDir + "/config";
    dirs.pluginDir = appDir + "/core_plugins";

    // 应用配置
    app.version = "1.0.0";
    app.language = "zh_CN";
    app.autoStart = false;
    app.autoConnect = false;

    // 设备配置
    device.type = "UART";
    device.port = "COM1";
    device.baudRate = 115200;
    device.dataBits = 8;
    device.stopBits = 1;
    device.parity = "None";
    device.timeout = 1000;
    device.autoPolling = false;
    device.pollingInterval = 100;
    device.udpRemoteIp = "192.168.1.100";
    device.udpRemotePort = 8888;
    device.udpLocalPort = 9999;

    // 算法参数
    algorithmParams.filterType = 0;
    algorithmParams.filterKernelSize = 3;
    algorithmParams.smoothWindowSize = 5;
    algorithmParams.peakThreshold = 0.5;

    // 显示参数
    display.maxDataPoints = 1000;
    display.autoScale = true;
    display.yMin = 0.0;
    display.yMax = 100.0;
    display.refreshRate = 50;
}

void Config::load()
{
    m_settings->beginGroup("Dirs");
    dirs.dataDir = m_settings->value("dataDir", dirs.dataDir).toString();
    dirs.logDir = m_settings->value("logDir", dirs.logDir).toString();
    dirs.configDir = m_settings->value("configDir", dirs.configDir).toString();
    dirs.pluginDir = m_settings->value("pluginDir", dirs.pluginDir).toString();
    m_settings->endGroup();

    m_settings->beginGroup("App");
    app.version = m_settings->value("version", app.version).toString();
    app.language = m_settings->value("language", app.language).toString();
    app.autoStart = m_settings->value("autoStart", app.autoStart).toBool();
    app.autoConnect = m_settings->value("autoConnect", app.autoConnect).toBool();
    m_settings->endGroup();

    m_settings->beginGroup("Device");
    device.type = m_settings->value("type", device.type).toString();
    device.port = m_settings->value("port", device.port).toString();
    device.baudRate = m_settings->value("baudRate", device.baudRate).toInt();
    device.dataBits = m_settings->value("dataBits", device.dataBits).toInt();
    device.stopBits = m_settings->value("stopBits", device.stopBits).toInt();
    device.parity = m_settings->value("parity", device.parity).toString();
    device.timeout = m_settings->value("timeout", device.timeout).toInt();
    device.autoPolling = m_settings->value("autoPolling", device.autoPolling).toBool();
    device.pollingInterval = m_settings->value("pollingInterval", device.pollingInterval).toInt();
    device.udpRemoteIp = m_settings->value("udpRemoteIp", device.udpRemoteIp).toString();
    device.udpRemotePort = m_settings->value("udpRemotePort", device.udpRemotePort).toInt();
    device.udpLocalPort = m_settings->value("udpLocalPort", device.udpLocalPort).toInt();
    m_settings->endGroup();

    m_settings->beginGroup("AlgorithmParams");
    algorithmParams.filterType = m_settings->value("filterType", algorithmParams.filterType).toInt();
    algorithmParams.filterKernelSize = m_settings->value("filterKernelSize", algorithmParams.filterKernelSize).toInt();
    algorithmParams.smoothWindowSize = m_settings->value("smoothWindowSize", algorithmParams.smoothWindowSize).toInt();
    algorithmParams.peakThreshold = m_settings->value("peakThreshold", algorithmParams.peakThreshold).toDouble();
    m_settings->endGroup();

    m_settings->beginGroup("Display");
    display.maxDataPoints = m_settings->value("maxDataPoints", display.maxDataPoints).toInt();
    display.autoScale = m_settings->value("autoScale", display.autoScale).toBool();
    display.yMin = m_settings->value("yMin", display.yMin).toDouble();
    display.yMax = m_settings->value("yMax", display.yMax).toDouble();
    display.refreshRate = m_settings->value("refreshRate", display.refreshRate).toInt();
    m_settings->endGroup();

    // 创建必要的目录
    QDir dir;
    dir.mkpath(dirs.dataDir);
    dir.mkpath(dirs.logDir);
    dir.mkpath(dirs.configDir);
}

void Config::save()
{
    m_settings->beginGroup("Dirs");
    m_settings->setValue("dataDir", dirs.dataDir);
    m_settings->setValue("logDir", dirs.logDir);
    m_settings->setValue("configDir", dirs.configDir);
    m_settings->setValue("pluginDir", dirs.pluginDir);
    m_settings->endGroup();

    m_settings->beginGroup("App");
    m_settings->setValue("version", app.version);
    m_settings->setValue("language", app.language);
    m_settings->setValue("autoStart", app.autoStart);
    m_settings->setValue("autoConnect", app.autoConnect);
    m_settings->endGroup();

    m_settings->beginGroup("Device");
    m_settings->setValue("type", device.type);
    m_settings->setValue("port", device.port);
    m_settings->setValue("baudRate", device.baudRate);
    m_settings->setValue("dataBits", device.dataBits);
    m_settings->setValue("stopBits", device.stopBits);
    m_settings->setValue("parity", device.parity);
    m_settings->setValue("timeout", device.timeout);
    m_settings->setValue("autoPolling", device.autoPolling);
    m_settings->setValue("pollingInterval", device.pollingInterval);
    m_settings->setValue("udpRemoteIp", device.udpRemoteIp);
    m_settings->setValue("udpRemotePort", device.udpRemotePort);
    m_settings->setValue("udpLocalPort", device.udpLocalPort);
    m_settings->endGroup();

    m_settings->beginGroup("AlgorithmParams");
    m_settings->setValue("filterType", algorithmParams.filterType);
    m_settings->setValue("filterKernelSize", algorithmParams.filterKernelSize);
    m_settings->setValue("smoothWindowSize", algorithmParams.smoothWindowSize);
    m_settings->setValue("peakThreshold", algorithmParams.peakThreshold);
    m_settings->endGroup();

    m_settings->beginGroup("Display");
    m_settings->setValue("maxDataPoints", display.maxDataPoints);
    m_settings->setValue("autoScale", display.autoScale);
    m_settings->setValue("yMin", display.yMin);
    m_settings->setValue("yMax", display.yMax);
    m_settings->setValue("refreshRate", display.refreshRate);
    m_settings->endGroup();

    m_settings->sync();
    emit configChanged();
}

void Config::reset()
{
    loadDefaults();
    save();
}
