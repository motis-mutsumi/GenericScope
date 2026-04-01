# ScopeUart 协议解析集成使用示例

本文档提供 ScopeUart 与 ProtocolParser 集成的完整使用示例。

## 快速开始

### 1. 创建并配置协议

```cpp
#include "protocol/protocolmanager.h"
#include "transfer/uart/scopeuart.h"

// 方式1: 从 ProtocolManager 加载已有协议
ProtocolManager *manager = ProtocolManager::instance();
// 假设已通过UI界面或代码添加了名为 "IMU_Protocol_V1" 的协议

// 方式2: 代码创建协议配置
ProtocolConfig config;
config.name = "IMU_Protocol_V1";
config.version = "1.0.0";
config.frameHeader = QByteArray::fromHex("FFAA");
config.frameFooter = QByteArray::fromHex("0D0A");
config.checksumType = ChecksumType::CRC16_MODBUS;
config.checksumScope = ChecksumScope::AfterHeader;
config.byteOrder = ByteOrder::LittleEndian;

// 添加字段
FieldConfig rollField;
rollField.index = 1;
rollField.elementHead = 0;
rollField.name = "Roll";
rollField.type = DataType::Float;
rollField.byteLength = 4;
rollField.scale = 1.0;
rollField.unit = "°";
config.fields.append(rollField);

// 保存到 ProtocolManager
manager->addProtocol(config);
```

### 2. 配置串口并启用协议解析

```cpp
// 创建串口传输对象
ScopeUart *uart = new ScopeUart();

// 配置串口参数
UartInfo info;
info.port_name = "COM3";
info.async = true;  // 必须启用异步模式才能使用协议解析
info.dcb.BaudRate = CBR_115200;
info.dcb.ByteSize = 8;
info.dcb.Parity = NOPARITY;
info.dcb.StopBits = ONESTOPBIT;
uart->setUartInfo(info);

// 设置协议
uart->setProtocol("IMU_Protocol_V1");

// 设置解析结果回调
uart->setParseResultCallback([](const ParseResult &result) {
    if (!result.success) {
        qWarning() << "Parse failed:" << result.errorMsg;
        return;
    }

    // 访问解析的字段值
    for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
        qDebug() << it.key() << ":" << it.value();
    }

    // 或者直接访问特定字段
    if (result.fieldValues.contains("Roll")) {
        double roll = result.fieldValues["Roll"].toDouble();
        qDebug() << "Roll angle:" << roll << "°";
    }
});

// 打开串口
if (uart->open() != ScopeTransferStatus::Ok) {
    qCritical() << "Failed to open serial port";
    return;
}

// 数据会自动接收和解析，解析结果通过回调通知
```

### 3. 完整应用示例

```cpp
#include "transfer/uart/scopeuart.h"
#include "protocol/protocolmanager.h"
#include <QCoreApplication>

class IMUDataReceiver : public QObject
{
    Q_OBJECT
public:
    IMUDataReceiver() : m_uart(new ScopeUart())
    {
        setupProtocol();
        setupUart();
    }

    ~IMUDataReceiver()
    {
        if (m_uart) {
            m_uart->close();
            delete m_uart;
        }
    }

    bool start(const QString &portName, int baudRate = 115200)
    {
        UartInfo info;
        info.port_name = portName.toStdString();
        info.async = true;
        info.dcb.BaudRate = baudRate;
        info.dcb.ByteSize = 8;
        info.dcb.Parity = NOPARITY;
        info.dcb.StopBits = ONESTOPBIT;
        m_uart->setUartInfo(info);

        return m_uart->open() == ScopeTransferStatus::Ok;
    }

signals:
    void imuDataReceived(double roll, double pitch, double yaw);
    void parseError(const QString &error);

private:
    void setupProtocol()
    {
        // 设置协议（假设已在UI中配置或代码中添加）
        m_uart->setProtocol("IMU_Protocol_V1");

        // 设置解析回调
        m_uart->setParseResultCallback([this](const ParseResult &result) {
            if (!result.success) {
                emit parseError(result.errorMsg);
                return;
            }

            // 提取IMU数据
            double roll = result.fieldValues.value("Roll", 0.0).toDouble();
            double pitch = result.fieldValues.value("Pitch", 0.0).toDouble();
            double yaw = result.fieldValues.value("Yaw", 0.0).toDouble();

            emit imuDataReceived(roll, pitch, yaw);
        });
    }

    void setupUart()
    {
        // 其他配置...
    }

private:
    ScopeUart *m_uart;
};

// 使用
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    IMUDataReceiver receiver;

    // 连接信号
    QObject::connect(&receiver, &IMUDataReceiver::imuDataReceived,
                     [](double roll, double pitch, double yaw) {
        qDebug() << "IMU Data - Roll:" << roll
                 << "Pitch:" << pitch
                 << "Yaw:" << yaw;
    });

    QObject::connect(&receiver, &IMUDataReceiver::parseError,
                     [](const QString &error) {
        qWarning() << "Parse error:" << error;
    });

    // 启动接收
    if (!receiver.start("COM3", 115200)) {
        qCritical() << "Failed to start receiver";
        return 1;
    }

    qDebug() << "IMU receiver started on COM3";

    return app.exec();
}
```

## 高级功能

### 1. 缓冲区管理

```cpp
// 获取缓冲区大小
size_t bufferSize = uart->getBufferSize();
qDebug() << "Receive buffer size:" << bufferSize << "bytes";

// 清空缓冲区（在协议切换或错误恢复时使用）
uart->clearBuffer();
```

### 2. 动态切换协议

```cpp
// 停止当前协议解析
uart->clearBuffer();

// 切换到新协议
uart->setProtocol("New_Protocol_V2");

// 设置新的回调
uart->setParseResultCallback([](const ParseResult &result) {
    // 新协议的处理逻辑
});
```

### 3. 与原始回调共存

```cpp
// ScopeUart 支持同时使用协议解析和原始数据回调

// 原始数据回调（向后兼容）
uart->setDataCallBackFunction([](uint8_t *data, size_t len) {
    // 处理原始数据
    qDebug() << "Received" << len << "bytes";
});

// 协议解析回调（新功能）
uart->setParseResultCallback([](const ParseResult &result) {
    // 处理解析后的数据
    qDebug() << "Parsed fields:" << result.fieldValues.size();
});

// 两个回调会同时触发
```

## 错误处理

### 1. 协议不存在

```cpp
uart->setProtocol("NonExistent_Protocol");
// 输出警告: "ScopeUart: Protocol not found: NonExistent_Protocol"
// 协议解析不会启用
```

### 2. 解析失败

```cpp
uart->setParseResultCallback([](const ParseResult &result) {
    if (!result.success) {
        // 解析失败的常见原因:
        // 1. 数据不完整（等待更多数据）
        // 2. 帧头未找到
        // 3. 校验码错误
        // 4. 字段读取越界
        qWarning() << "Parse failed:" << result.errorMsg;

        // 查看原始数据
        qDebug() << "Raw data:" << result.rawData.toHex();
        return;
    }

    // 处理成功的结果...
});
```

### 3. 缓冲区溢出

```cpp
// ScopeUart 内置缓冲区溢出保护
// 当缓冲区超过 4096 字节时，会自动清空并输出警告
// 输出: "ScopeUart: Buffer overflow (5120 bytes), clearing old data"

// 可以通过监控缓冲区大小来预防:
QTimer *timer = new QTimer();
QObject::connect(timer, &QTimer::timeout, [uart]() {
    size_t size = uart->getBufferSize();
    if (size > 2048) {
        qWarning() << "Buffer size high:" << size;
        // 可以选择清空或调整协议配置
    }
});
timer->start(1000);  // 每秒检查一次
```

## 性能优化建议

### 1. 异步模式必选
```cpp
// ✅ 正确：异步模式下协议解析才能正常工作
UartInfo info;
info.async = true;  // 必须启用

// ❌ 错误：同步模式不支持协议解析
info.async = false;
```

### 2. 合理的协议配置
```cpp
// 帧头/帧尾应足够独特，避免误识别
config.frameHeader = QByteArray::fromHex("FFAA");  // ✅ 好
config.frameHeader = QByteArray::fromHex("00");     // ❌ 太常见

// 使用校验码确保数据完整性
config.checksumType = ChecksumType::CRC16_MODBUS;  // ✅ 推荐
config.checksumType = ChecksumType::None;          // ❌ 不推荐
```

### 3. 回调函数优化
```cpp
// ✅ 好：快速处理，避免阻塞
uart->setParseResultCallback([](const ParseResult &result) {
    // 快速提取数据
    double value = result.fieldValues["Sensor"].toDouble();

    // 通过信号槽异步处理
    emit dataReady(value);
});

// ❌ 差：在回调中执行耗时操作
uart->setParseResultCallback([](const ParseResult &result) {
    // 会阻塞接收线程！
    heavyComputation();
    saveToDatabase();
});
```

## 调试技巧

### 1. 启用详细日志
```cpp
// protocol/protocolparser.cpp 中已包含详细的 qDebug/qWarning 输出
// 确保在 .pro 文件中未定义 QT_NO_DEBUG_OUTPUT

// 查看解析过程
uart->setParseResultCallback([](const ParseResult &result) {
    qDebug() << "=== Parse Result ===";
    qDebug() << "Success:" << result.success;
    qDebug() << "Error:" << result.errorMsg;
    qDebug() << "Consumed bytes:" << result.consumedBytes;
    qDebug() << "Field count:" << result.fieldValues.size();
    qDebug() << "Raw data:" << result.rawData.toHex();
});
```

### 2. 测试协议配置
```cpp
// 在 ProtocolTestDialog 中测试协议
// 或手动测试:
QByteArray testData = QByteArray::fromHex("FFAA0C0102030405060708090A0B0C0D0A");
ParseResult result = parser->parse(testData);
if (!result.success) {
    qDebug() << "Test failed:" << result.errorMsg;
}
```

## 常见问题

**Q: 为什么协议解析回调不触发？**
A: 检查以下几点：
1. 串口是否设置为异步模式 (`info.async = true`)
2. 协议是否正确加载 (`uart->currentProtocol()`)
3. 回调是否正确设置 (`setParseResultCallback`)
4. 数据是否符合协议格式（帧头/帧尾/校验码）

**Q: 如何处理多协议设备？**
A: 使用协议自动识别或手动切换：
```cpp
// 方式1: 根据设备响应切换
uart->setProtocol("Device_Query_Protocol");
// ... 查询设备类型 ...
uart->setProtocol("Device_Specific_Protocol");

// 方式2: 尝试多个协议
// （需要自己实现逻辑判断哪个协议解析成功）
```

**Q: consumedBytes 是什么？**
A: 表示本次解析消耗了多少字节的缓冲区数据。这个值用于从缓冲区移除已处理的数据，为后续数据腾出空间。

## 完整的数据流图

```
硬件设备
    ↓ (串口发送二进制数据)
Windows COM Port
    ↓ (ReadFile 异步读取)
comRecv 接收线程
    ↓ (原始数据)
┌────────────────────────┐
│ m_xferCallBackFunction │ ← 原始回调（可选）
│  (原始数据回调)         │
└────────────────────────┘
    ↓
接收缓冲区 (m_receiveBuffer)
    ↓
ProtocolParser::parse()
    ├─ findFrameHeader()    // 查找帧头
    ├─ extractFrame()       // 提取完整帧
    ├─ verifyChecksum()     // 校验
    └─ parseField()         // 解析字段
    ↓
ParseResult
    ↓
┌────────────────────────┐
│ m_parseResultCallback  │ ← 协议解析回调
│  (解析后的数据)        │
└────────────────────────┘
    ↓
应用程序业务逻辑
```

## 参考文档

- **协议配置：** `docs/protocol-config-system-spec.md`
- **ProtocolParser API：** `protocol/protocolparser.h`
- **ScopeUart API：** `transfer/uart/scopeuart.h`
- **使用示例：** `app/ui/mainwindow.cpp` (主界面集成示例)
