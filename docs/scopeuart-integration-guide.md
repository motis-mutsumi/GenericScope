# ScopeUart集成ProtocolParser示例

## 概述

本文档说明如何将ProtocolParser集成到ScopeUart中，实现动态协议解析功能。

## 集成步骤

### 1. 修改scopeuart.h

在头文件中添加ProtocolParser相关的成员：

```cpp
#ifndef SCOPEUART_H
#define SCOPEUART_H

#include "scopecontroltransfer.h"
#include "protocol/protocolparser.h"  // 添加
#include "protocol/protocolmanager.h" // 添加
#include <QSharedPointer>             // 添加
#include "windows.h"
#include <string>
#include <mutex>
#include <shared_mutex>
#include <functional>

// ... 现有代码 ...

class TRANSFER_EXPORT ScopeUart : public ScopeControlTransfer
{
public:
    ScopeUart();
    ~ScopeUart();

    // ... 现有函数 ...

    // 新增：设置协议解析器
    void setProtocolParser(QSharedPointer<ProtocolParser> parser);

    // 新增：设置协议（通过名称）
    void setProtocol(const QString &protocolName);

    // 新增：获取当前协议名称
    QString getCurrentProtocol() const;

    // 新增：设置解析结果回调
    void setParseResultCallback(std::function<void(const ParseResult&)> callback);

private:
    // ... 现有成员 ...

    // 新增：协议解析器
    QSharedPointer<ProtocolParser> m_protocolParser;
    QString m_currentProtocolName;

    // 新增：解析结果回调
    std::function<void(const ParseResult&)> m_parseResultCallback;

    // 新增：接收缓冲区（用于协议解析）
    QByteArray m_receiveBuffer;

    // 新增：解析接收到的数据
    void parseReceivedData(const QByteArray &data);
};

#endif // SCOPEUART_H
```

### 2. 修改scopeuart.cpp

在实现文件中添加协议解析功能：

```cpp
#include "scopeuart.h"
#include <iostream>
#include <process.h>

using namespace std;

ScopeUart::ScopeUart()
    : m_protocolParser(nullptr)
    , m_parseResultCallback(nullptr)
{
}

ScopeUart::~ScopeUart()
{
}

// 新增：设置协议解析器
void ScopeUart::setProtocolParser(QSharedPointer<ProtocolParser> parser)
{
    std::unique_lock<std::shared_mutex> lock(m_dataMutex);
    m_protocolParser = parser;
}

// 新增：设置协议（通过名称）
void ScopeUart::setProtocol(const QString &protocolName)
{
    ProtocolManager *manager = ProtocolManager::instance();

    if (!manager->hasProtocol(protocolName)) {
        qWarning() << "Protocol not found:" << protocolName;
        return;
    }

    QSharedPointer<ProtocolParser> parser = manager->createParser(protocolName);
    if (parser) {
        setProtocolParser(parser);
        m_currentProtocolName = protocolName;
        qDebug() << "ScopeUart: Protocol set to" << protocolName;
    }
}

// 新增：获取当前协议名称
QString ScopeUart::getCurrentProtocol() const
{
    return m_currentProtocolName;
}

// 新增：设置解析结果回调
void ScopeUart::setParseResultCallback(std::function<void(const ParseResult&)> callback)
{
    std::unique_lock<std::shared_mutex> lock(m_dataMutex);
    m_parseResultCallback = callback;
}

// 新增：解析接收到的数据
void ScopeUart::parseReceivedData(const QByteArray &data)
{
    if (!m_protocolParser) {
        return;
    }

    // 将新数据添加到缓冲区
    m_receiveBuffer.append(data);

    // 尝试解析
    ParseResult result = m_protocolParser->parse(m_receiveBuffer);

    if (result.success) {
        // 解析成功，调用回调
        if (m_parseResultCallback) {
            m_parseResultCallback(result);
        }

        // 从缓冲区移除已解析的数据
        int parsedLength = result.rawData.size();
        m_receiveBuffer.remove(0, parsedLength);

        qDebug() << "Parsed frame successfully, fields:" << result.fieldValues.size();
    } else {
        // 解析失败，检查缓冲区大小
        if (m_receiveBuffer.size() > 4096) {
            // 缓冲区过大，清空避免内存溢出
            qWarning() << "Receive buffer overflow, clearing...";
            m_receiveBuffer.clear();
        }
    }
}

// 修改现有的comRecv函数，添加协议解析
unsigned int __stdcall comRecv(void* param)
{
    ScopeUart* uart = (ScopeUart*)param;
    uint8_t buffer[1024];
    DWORD bytesRead = 0;

    while (uart->m_open) {
        BOOL success = ReadFile(uart->m_hcom, buffer, sizeof(buffer),
                               &bytesRead, &uart->m_ovRead);

        if (success && bytesRead > 0) {
            // 原有回调
            if (uart->m_xferCallBackFunction) {
                uart->m_xferCallBackFunction(buffer, bytesRead);
            }

            // 新增：协议解析
            QByteArray data(reinterpret_cast<const char*>(buffer), bytesRead);
            uart->parseReceivedData(data);
        }
    }

    return 0;
}

// ... 其他现有函数保持不变 ...
```

### 3. 使用示例

在MainWindow或其他地方使用：

```cpp
// 在MainWindow中
void MainWindow::setupUartWithProtocol()
{
    // 创建ScopeUart实例
    ScopeUart *uart = new ScopeUart();

    // 配置串口参数
    UartInfo info;
    info.port_name = "COM7";
    info.dcb.BaudRate = 115200;
    info.dcb.ByteSize = 8;
    info.dcb.Parity = NOPARITY;
    info.dcb.StopBits = ONESTOPBIT;
    info.async = true;
    uart->setUartInfo(info);

    // 设置协议（假设已经在CommandSettingsDialog中配置好）
    uart->setProtocol("IMU_Protocol_V1");

    // 设置解析结果回调
    uart->setParseResultCallback([this](const ParseResult &result) {
        // 处理解析结果
        qDebug() << "Received parsed data:";
        for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
            qDebug() << "  " << it.key() << ":" << it.value();
        }

        // 更新UI
        QMetaObject::invokeMethod(this, [this, result]() {
            updateDataDisplay(result);
        }, Qt::QueuedConnection);
    });

    // 打开串口
    if (uart->open() == ScopeTransferStatus::Ok) {
        qDebug() << "UART opened successfully";
    }
}

void MainWindow::updateDataDisplay(const ParseResult &result)
{
    // 更新数据表格
    for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
        QString fieldName = it.key();
        QVariant value = it.value();

        // 在表格中查找对应的行并更新
        for (int row = 0; row < m_dataTable->rowCount(); ++row) {
            if (m_dataTable->item(row, 0)->text() == fieldName) {
                m_dataTable->item(row, 1)->setText(value.toString());
                break;
            }
        }
    }

    // 更新图表
    if (result.fieldValues.contains("Roll")) {
        double roll = result.fieldValues["Roll"].toDouble();
        m_linePlot->addData(roll);
    }
}
```

### 4. 动态切换协议

```cpp
// 在运行时切换协议
void MainWindow::onProtocolChanged(const QString &protocolName)
{
    if (m_uart) {
        m_uart->setProtocol(protocolName);
        qDebug() << "Switched to protocol:" << protocolName;
    }
}
```

## 注意事项

1. **线程安全**：parseReceivedData在接收线程中调用，回调函数中更新UI需要使用QMetaObject::invokeMethod

2. **缓冲区管理**：接收缓冲区需要定期清理，避免内存溢出

3. **错误处理**：解析失败时需要记录日志，便于调试

4. **性能优化**：高频数据时考虑使用对象池减少内存分配

5. **协议切换**：切换协议时需要清空接收缓冲区

## 完整集成清单

- [x] 创建ProtocolTypeConverter类型转换工具
- [x] 实现CommandSettingsDialog与ProtocolManager的同步
- [x] 修改app.pro添加protocol模块依赖
- [ ] 修改scopeuart.h添加协议解析成员
- [ ] 修改scopeuart.cpp实现协议解析功能
- [ ] 在MainWindow中使用协议解析功能
- [ ] 测试完整的数据流程

## 测试建议

1. 使用虚拟串口工具测试协议解析
2. 发送标准格式的测试数据
3. 验证字段解析的正确性
4. 测试错误数据的处理
5. 测试协议动态切换功能
