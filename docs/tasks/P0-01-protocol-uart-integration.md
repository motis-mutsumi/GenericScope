# P0-01: Protocol与ScopeUart集成验证

**优先级**: P0（高优先级）
**预计工作量**: 0.5-1天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

验证并完善Protocol模块与ScopeUart的集成，确保串口接收的数据能通过ProtocolParser正确解析。

### 当前状态
- ✅ Protocol模块已完整实现（ProtocolConfig/ProtocolParser/ProtocolManager）
- ✅ ScopeUart串口通信已实现
- ✅ 集成指南文档已完成（`docs/scopeuart-integration-guide.md`）
- ⚠️ **代码集成待验证**：ScopeUart中使用ProtocolParser的实际代码未确认

### 任务目标
1. 在ScopeUart中集成ProtocolParser
2. 实现数据接收后自动解析
3. 通过回调机制传递解析结果
4. 端到端测试数据解析流程

---

## 🎯 验收标准

### 功能验收
- [ ] ScopeUart能够加载指定的协议配置
- [ ] 串口接收数据后自动调用ProtocolParser解析
- [ ] 解析成功后通过信号/回调传递ParseResult
- [ ] 解析失败时输出详细错误日志
- [ ] 支持动态切换协议配置

### 测试验收
- [ ] 使用IMU协议测试数据验证解析正确性
- [ ] 测试帧头/帧尾识别
- [ ] 测试CRC校验验证
- [ ] 测试字段数据提取和类型转换
- [ ] 测试大小端转换

### 代码质量
- [ ] 代码符合Qt 5.14规范
- [ ] 添加详细注释
- [ ] 无内存泄漏
- [ ] 线程安全（跨线程信号连接）

---

## 🔧 技术方案

### 1. ScopeUart类修改

#### 1.1 添加成员变量（`transfer/uart/scopeuart.h`）

```cpp
#include "protocol/protocolparser.h"
#include "protocol/protocolmanager.h"

class ScopeUart : public ScopeTransferBasic {
    Q_OBJECT
public:
    // ... 现有代码 ...

    // 新增：协议管理接口
    void setProtocol(const QString &protocolName);
    QString currentProtocol() const;

signals:
    // 新增：解析结果信号
    void parseResultReady(const ParseResult &result);
    void parseError(const QString &error);

private slots:
    // 修改：串口数据接收槽函数
    void onReadyRead();

private:
    // 新增成员变量
    QSharedPointer<ProtocolParser> m_parser;
    QString m_currentProtocolName;
    QByteArray m_receiveBuffer;  // 数据缓冲区
};
```

#### 1.2 实现协议设置（`transfer/uart/scopeuart.cpp`）

```cpp
void ScopeUart::setProtocol(const QString &protocolName) {
    ProtocolManager *manager = ProtocolManager::instance();

    if (!manager->hasProtocol(protocolName)) {
        qWarning() << "Protocol not found:" << protocolName;
        emit parseError(QString("协议不存在: %1").arg(protocolName));
        return;
    }

    // 创建解析器
    m_parser = manager->createParser(protocolName);
    m_currentProtocolName = protocolName;

    qDebug() << "ScopeUart: Protocol set to" << protocolName;
}

QString ScopeUart::currentProtocol() const {
    return m_currentProtocolName;
}
```

#### 1.3 修改数据接收处理

```cpp
void ScopeUart::onReadyRead() {
    if (!m_serialPort || !m_serialPort->isOpen()) {
        return;
    }

    // 读取数据到缓冲区
    QByteArray newData = m_serialPort->readAll();
    m_receiveBuffer.append(newData);

    // 原始数据信号（保留向后兼容）
    emit dataReceived(newData);

    // 如果配置了协议解析器，进行解析
    if (m_parser) {
        // 尝试从缓冲区解析完整帧
        ParseResult result = m_parser->parse(m_receiveBuffer);

        if (result.success) {
            // 解析成功
            qDebug() << "ScopeUart: Frame parsed successfully";
            qDebug() << "  Fields:" << result.fieldValues.size();

            // 发送解析结果
            emit parseResultReady(result);

            // 从缓冲区移除已解析的数据
            // 注意：需要ProtocolParser返回已消耗的字节数
            // 这里假设ParseResult包含consumedBytes字段
            if (result.consumedBytes > 0) {
                m_receiveBuffer.remove(0, result.consumedBytes);
            }
        } else {
            // 解析失败 - 可能是数据不完整，继续等待
            qDebug() << "ScopeUart: Parse failed:" << result.errorMessage;

            // 如果缓冲区过大，清理旧数据
            if (m_receiveBuffer.size() > 4096) {
                qWarning() << "ScopeUart: Buffer overflow, clearing old data";
                m_receiveBuffer.clear();
                emit parseError("数据缓冲区溢出");
            }
        }
    }
}
```

### 2. ParseResult结构增强（`protocol/protocolparser.h`）

```cpp
struct ParseResult {
    bool success;                           // 解析是否成功
    QString errorMessage;                   // 错误信息
    QMap<QString, QVariant> fieldValues;    // 字段名 -> 字段值
    QByteArray rawFrame;                    // 原始帧数据
    int consumedBytes;                      // 已消耗的字节数（新增）

    ParseResult() : success(false), consumedBytes(0) {}
};
```

### 3. ProtocolParser修改（`protocol/protocolparser.cpp`）

在`parse()`方法中添加`consumedBytes`计算：

```cpp
ParseResult ProtocolParser::parse(const QByteArray &data) {
    ParseResult result;

    // ... 现有解析逻辑 ...

    if (result.success) {
        // 计算已消耗的字节数（帧结束位置+1）
        result.consumedBytes = frameEndPos + 1;
    }

    return result;
}
```

### 4. MainWindow集成示例（`app/ui/mainwindow.cpp`）

```cpp
void MainWindow::setupUartWithProtocol() {
    // 创建串口传输
    QVariantMap config;
    config["port"] = ui->comPortComboBox->currentText();
    config["baudRate"] = ui->baudRateComboBox->currentText().toInt();

    m_uart = new ScopeUart(this);
    m_uart->setConfig(config);

    // 设置协议
    m_uart->setProtocol("IMU_Protocol_V1");

    // 连接解析结果信号
    connect(m_uart, &ScopeUart::parseResultReady,
            this, &MainWindow::onParseResultReady);
    connect(m_uart, &ScopeUart::parseError,
            this, &MainWindow::onParseError);

    // 打开串口
    if (!m_uart->open()) {
        qWarning() << "Failed to open UART";
    }
}

void MainWindow::onParseResultReady(const ParseResult &result) {
    // 更新UI显示
    for (auto it = result.fieldValues.begin();
         it != result.fieldValues.end(); ++it) {
        QString fieldName = it.key();
        QVariant fieldValue = it.value();

        // 更新数据表格
        updateDataTable(fieldName, fieldValue.toDouble());
    }

    // 更新图表
    if (result.fieldValues.contains("Roll")) {
        double roll = result.fieldValues["Roll"].toDouble();
        m_plot->addData(roll);
    }
}

void MainWindow::onParseError(const QString &error) {
    qWarning() << "Parse error:" << error;
    // 在状态栏显示错误
    statusBar()->showMessage(error, 3000);
}
```

---

## 📝 实施步骤

### Step 1: 修改ParseResult结构（15分钟）
1. 打开 `protocol/protocolparser.h`
2. 在`ParseResult`结构中添加`int consumedBytes;`字段
3. 在构造函数中初始化为0

### Step 2: 修改ProtocolParser（30分钟）
1. 打开 `protocol/protocolparser.cpp`
2. 在`parse()`方法成功分支中计算`consumedBytes`
3. 添加详细调试日志

### Step 3: 修改ScopeUart类（1小时）
1. 打开 `transfer/uart/scopeuart.h`
2. 添加协议相关的成员变量和方法声明
3. 打开 `transfer/uart/scopeuart.cpp`
4. 实现`setProtocol()`方法
5. 修改`onReadyRead()`槽函数，添加解析逻辑
6. 添加缓冲区管理

### Step 4: 更新transfer.pro（5分钟）
```pro
# 添加protocol模块依赖
LIBS += -L$$DESTDIR -lprotocol
INCLUDEPATH += $$PWD/../protocol
```

### Step 5: MainWindow集成测试（30分钟）
1. 在`mainwindow.cpp`中添加测试代码
2. 编译运行
3. 使用真实串口数据或模拟数据测试

### Step 6: 验证测试（1-2小时）
1. 准备测试数据（参考集成指南文档）
2. 测试各种协议配置
3. 验证错误处理
4. 性能测试（高速数据流）

---

## 🧪 测试方案

### 测试用例1：基本解析功能

**测试数据**（IMU协议）:
```
AA 55 01 01 2E 00 C7 F6 29 01 60 1C 00 00 02 66
F7 FF 38 DA FF FF 95 9B 01 00 C9 C9 0E 00 61 04
ED FF BF 46 18 10 D4 FF 00 00 E1 FF 00 00 40 FF
00 00 00 00 DC 7E
```

**预期结果**:
- `success = true`
- 解析出11个字段（时间戳、温度、角速度x3、加速度x3、欧拉角x3）
- `consumedBytes = 56`（完整帧长度）

### 测试用例2：不完整数据处理

**测试数据**:
```
AA 55 01 01 2E 00 C7 F6  // 只有前8字节
```

**预期结果**:
- `success = false`
- 数据保留在缓冲区
- 等待更多数据到达

### 测试用例3：CRC错误处理

**测试数据**（故意修改CRC）:
```
AA 55 ... FF FF  // 错误的CRC
```

**预期结果**:
- `success = false`
- `errorMessage` 包含"CRC校验失败"

### 测试用例4：多帧连续解析

**测试数据**（两个完整帧）:
```
[帧1数据...] [帧2数据...]
```

**预期结果**:
- 第一次parse()解析帧1，返回帧1的consumedBytes
- 缓冲区移除帧1数据
- 第二次parse()解析帧2

---

## 📚 参考资料

### 相关文档
- [ScopeUart集成指南](../scopeuart-integration-guide.md)
- [协议配置系统规范](../protocol-config-system-spec.md)
- [CLAUDE.md - Protocol模块](../../CLAUDE.md#协议配置与解析系统)

### 相关代码文件
- `protocol/protocolparser.h/cpp` - 协议解析器
- `protocol/protocolmanager.h/cpp` - 协议管理器
- `transfer/uart/scopeuart.h/cpp` - 串口传输
- `app/ui/mainwindow.h/cpp` - 主窗口

---

## ⚠️ 注意事项

### 线程安全
- ScopeUart的串口读取在单独的线程
- 信号槽连接应使用`Qt::QueuedConnection`
- ProtocolParser本身是线程安全的（无状态操作）

### 性能考虑
- 缓冲区大小限制（建议4KB）
- 避免频繁内存分配
- 考虑使用对象池优化ParseResult创建

### 错误处理
- 缓冲区溢出检测
- 无效数据快速丢弃
- 详细的错误日志

---

## ✅ 完成检查清单

开发完成后，请确认：

- [ ] 代码编译通过，无警告
- [ ] 所有测试用例通过
- [ ] 添加了充足的注释
- [ ] 更新了相关文档
- [ ] 进行了代码审查
- [ ] 性能测试通过（1000Hz数据流）
- [ ] 内存泄漏检查通过

---

**创建日期**: 2026-01-29
**最后更新**: 2026-01-29
**预计完成**: 2026-01-30
