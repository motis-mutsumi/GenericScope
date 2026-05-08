#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSettings>

/**
 * @brief 配置管理类（单例）
 *
 * 管理应用程序的所有配置参数
 */
class Config : public QObject
{
    Q_OBJECT

public:
    // 目录配置
    struct Dirs {
        QString dataDir;        // 数据目录
        QString logDir;         // 日志目录
        QString configDir;      // 配置目录
        QString pluginDir;      // 插件目录
    };

    // 应用配置
    struct App {
        QString version;        // 版本号
        QString language;       // 语言
        bool autoStart;         // 自动启动
        bool autoConnect;       // 自动连接设备
    };

    // 设备配置
    struct Device {
        QString type;           // 设备类型（UART/TCP/MODBUS/UDP）
        QString port;           // 串口号或IP地址
        int baudRate;           // 波特率
        int dataBits;           // 数据位
        int stopBits;           // 停止位
        QString parity;         // 校验位
        int timeout;            // 超时时间
        bool autoPolling;       // 自动轮询
        int pollingInterval;    // 轮询间隔
        // UDP 专属
        QString udpRemoteIp;    // UDP 远端 IP
        int udpRemotePort;      // UDP 远端端口
        int udpLocalPort;       // UDP 本地监听端口
    };

    // 转台配置
    struct Turntable {
        QString udpRemoteIp;     // 转台 UDP 远端 IP
        int udpRemotePort;       // 转台 UDP 远端端口
        int udpLocalPort;        // 转台 UDP 本地监听端口
        int axisCount;           // 转台轴数
    };

    // 烘箱配置
    struct Oven {
        QString port;            // 烘箱串口号
        int baudRate;            // 烘箱波特率
        int dataBits;            // 数据位
        int stopBits;            // 停止位
        QString parity;          // 校验位
        int timeout;             // 超时时间
        int deviceId;            // 设备编号
        double targetTemperature; // 目标温度
        int pollingIntervalMs;   // 轮询间隔
        bool autoQueryStatus;    // 自动查询状态
    };

    // 算法参数
    struct AlgorithmParams {
        int filterType;         // 滤波器类型
        int filterKernelSize;   // 滤波器大小
        int smoothWindowSize;   // 平滑窗口大小
        double peakThreshold;   // 峰值检测阈值
    };

    // 显示参数
    struct Display {
        int maxDataPoints;      // 最大显示数据点数
        bool autoScale;         // 自动缩放
        double yMin;            // Y轴最小值
        double yMax;            // Y轴最大值
        int refreshRate;        // 刷新率（毫秒）
    };

    static Config* instance();

    /**
     * @brief 加载配置
     */
    void load();

    /**
     * @brief 保存配置
     */
    void save();

    /**
     * @brief 重置为默认配置
     */
    void reset();

    // 配置访问
    Dirs dirs;
    App app;
    Device device;
    Turntable turntable;
    Oven oven;
    AlgorithmParams algorithmParams;
    Display display;

signals:
    void configChanged();

private:
    explicit Config(QObject *parent = nullptr);
    ~Config();
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void loadDefaults();

private:
    static Config *s_instance;
    QSettings *m_settings;
};

// 全局配置访问
extern Config *pConfig;

#endif // CONFIG_H
