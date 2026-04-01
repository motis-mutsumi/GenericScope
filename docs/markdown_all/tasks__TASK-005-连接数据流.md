# TASK-005: 连接数据流

## 任务信息

- **任务ID**: TASK-005
- **任务名称**: 连接数据流
- **优先级**: P0（必须完成）
- **预估工作量**: 3小时
- **依赖任务**: TASK-003, TASK-004
- **状态**: 待开始

## 任务描述

将ProtocolParser的解析结果连接到MonitorDataManager，建立完整的数据流通道。同时在MonitorChart中订阅dataUpdated信号，为后续图表绘制做准备。

## 具体目标

1. 在MainWindow的数据接收处理函数中
2. 调用ProtocolParser::parse()后
3. 将result.fieldValues传递给MonitorDataManager
4. 确保线程安全（Qt::QueuedConnection）
5. 在MonitorChart构造函数中订阅dataUpdated信号
6. 实现onDataReceived()方法框架

## 涉及文件

### 修改文件
- `app/ui/mainwindow.cpp`
- `common_component/plot/monitorchart.h`
- `common_component/plot/monitorchart.cpp`

## 实现细节

### 1. MainWindow中连接数据流

找到MainWindow中处理协议数据的位置，添加MonitorDataManager调用：

```cpp
#include "common_component/plot/monitordatamanager.h"

// 假设现有的数据接收函数是这样的
void MainWindow::onProtocolDataReceived(const QByteArray &data)
{
    // 解析数据
    ParseResult result = m_protocolParser->parse(data);

    if (result.success) {
        // ========== 新增：分发给MonitorDataManager ==========
        QMap<QString, double> fieldValues;

        // 转换ParseResult中的字段值为QMap<QString, double>
        // 根据实际ParseResult结构调整
        for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
            const QString &fieldName = it.key();
            QVariant value = it.value();

            // 转换为double（根据实际类型调整）
            bool ok = false;
            double doubleValue = value.toDouble(&ok);
            if (ok) {
                fieldValues[fieldName] = doubleValue;
            }
        }

        // 分发数据
        MonitorDataManager::instance()->onProtocolDataParsed(fieldValues);
        // ===================================================

        // 原有逻辑...
        // 更新UI表格、3D视图等
    } else {
        // 解析失败处理
        // TODO: TASK-011中添加错误信号
    }
}
```

### 2. MonitorChart订阅数据信号

在`monitorchart.cpp`的构造函数中：

```cpp
#include "monitordatamanager.h"

MonitorChart::MonitorChart(const QString &fieldName,
                           const QString &unit,
                           int xRangeSeconds,
                           int xTickCount,
                           QWidget *parent)
    : QWidget(parent)
    , m_fieldName(fieldName)
    , m_unit(unit)
    , m_xRangeSeconds(xRangeSeconds)
    , m_xTickCount(xTickCount)
    , m_paused(false)
    , m_plot(nullptr)
{
    setupUI();
    createActions();

    // ========== 新增：订阅数据更新信号 ==========
    // 使用Qt::QueuedConnection确保线程安全
    connect(MonitorDataManager::instance(), &MonitorDataManager::dataUpdated,
            this, &MonitorChart::onDataReceived,
            Qt::QueuedConnection);
    // ==========================================
}
```

### 3. 实现onDataReceived()框架

在`monitorchart.cpp`中：

```cpp
void MonitorChart::onDataReceived(const QString &fieldName, double value, qint64 timestamp)
{
    // 字段过滤：只处理匹配的字段
    if (fieldName != m_fieldName) {
        return;
    }

    // 暂停检查
    if (m_paused) {
        return;
    }

    // TODO: TASK-006中实现数据添加
    // appendData(value, timestamp);

    // 临时调试输出
    qDebug() << QString("[%1] 收到数据: %2 @ %3")
                .arg(m_fieldName)
                .arg(value)
                .arg(timestamp);
}
```

## 验收标准

- [ ] MainWindow正确调用MonitorDataManager::onProtocolDataParsed()
- [ ] 字段值类型转换正确（QVariant → double）
- [ ] MonitorChart成功订阅dataUpdated信号
- [ ] 使用Qt::QueuedConnection确保线程安全
- [ ] onDataReceived()能收到数据（通过qDebug验证）
- [ ] 字段过滤正确（只接收匹配字段的数据）
- [ ] 暂停状态检查正确
- [ ] 编译通过，无警告

## 测试方法

### 集成测试

**测试场景1：数据流验证**

1. 配置IMU协议（包含Roll、Pitch、Yaw字段）
2. 添加Roll图表
3. 开始接收数据
4. 观察控制台输出

**预期结果：**
```
[Roll] 收到数据: 45.2 @ 1612345678901
[Roll] 收到数据: 45.3 @ 1612345678911
[Roll] 收到数据: 45.1 @ 1612345678921
...
```

**测试场景2：字段过滤验证**

1. 配置协议（包含Roll、Pitch、Yaw）
2. 添加Roll图表
3. 发送包含所有字段的数据
4. 观察控制台输出

**预期结果：**
- 只输出Roll的数据
- Pitch和Yaw的数据被过滤

**测试场景3：暂停验证**

1. 添加Roll图表
2. 接收几条数据（有输出）
3. 右键暂停图表
4. 继续接收数据

**预期结果：**
- 暂停前有输出
- 暂停后无输出

**测试场景4：多图表验证**

1. 添加Roll、Pitch、Yaw三个图表
2. 接收数据

**预期结果：**
```
[Roll] 收到数据: 45.2 @ 1612345678901
[Pitch] 收到数据: -12.3 @ 1612345678901
[Yaw] 收到数据: 180.5 @ 1612345678901
```

### 单元测试

```cpp
void TestDataFlow::testMonitorChartReceivesData()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 创建信号监听器（监听内部的appendData调用）
    QSignalSpy spy(&chart, &MonitorChart::dataReceived);  // 假设有此信号

    // 发送数据
    MonitorDataManager::instance()->onProtocolDataParsed({
        {"Roll", 45.2},
        {"Pitch", -12.3}
    });

    // 验证Roll图表收到1条数据
    QCOMPARE(spy.count(), 1);
}
```

## 注意事项

### 1. 线程安全

**问题：** ProtocolParser可能在非主线程解析数据

**解决方案：**
```cpp
// 使用Qt::QueuedConnection
connect(MonitorDataManager::instance(), &MonitorDataManager::dataUpdated,
        this, &MonitorChart::onDataReceived,
        Qt::QueuedConnection);  // 关键！
```

这样即使dataUpdated在非主线程emit，onDataReceived也会在主线程执行。

### 2. 数据类型转换

**问题：** ParseResult中的字段值可能是QVariant类型

**解决方案：**
```cpp
bool ok = false;
double value = variantValue.toDouble(&ok);
if (ok) {
    fieldValues[fieldName] = value;
} else {
    // 转换失败，记录日志
    qWarning() << "字段" << fieldName << "无法转换为double";
}
```

### 3. 性能考虑

**问题：** 高频数据（如100Hz）会频繁触发信号

**当前策略：** 暂不优化，先验证功能

**后期优化方案（P2）：**
- 批量处理：累积10个数据点再更新
- 降采样：超过阈值自动抽样

### 4. 内存管理

**注意：** MonitorDataManager是单例，生命周期贯穿整个程序

**建议：**
- 不需要手动delete
- 程序退出时Qt会自动清理

### 5. 调试输出

**开发阶段：** 保留qDebug输出，便于调试

**发布版本：** 使用条件编译移除：
```cpp
#ifdef QT_DEBUG
    qDebug() << "收到数据:" << value;
#endif
```

## 实际适配指南

由于不同项目的ParseResult结构可能不同，请根据实际情况调整：

### 方案A：ParseResult包含QMap

```cpp
// 假设ParseResult定义如下：
struct ParseResult {
    bool success;
    QMap<QString, QVariant> fieldValues;
};

// 直接使用
QMap<QString, double> doubleValues;
for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
    doubleValues[it.key()] = it.value().toDouble();
}
MonitorDataManager::instance()->onProtocolDataParsed(doubleValues);
```

### 方案B：ParseResult包含自定义结构

```cpp
// 假设ParseResult包含QList<FieldResult>
struct FieldResult {
    QString name;
    QVariant value;
};

struct ParseResult {
    bool success;
    QList<FieldResult> fields;
};

// 转换
QMap<QString, double> fieldValues;
for (const auto &field : result.fields) {
    fieldValues[field.name] = field.value.toDouble();
}
MonitorDataManager::instance()->onProtocolDataParsed(fieldValues);
```

### 方案C：直接传递ParseResult

如果不想转换，可以修改MonitorDataManager接口：

```cpp
// monitordatamanager.h
void onProtocolDataParsed(const ParseResult &result);

// monitordatamanager.cpp
void MonitorDataManager::onProtocolDataParsed(const ParseResult &result) {
    if (!result.success) return;

    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
        emit dataUpdated(it.key(), it.value().toDouble(), timestamp);
    }
}
```

**请在实施时根据项目实际结构选择合适方案。**

## 后续任务

完成此任务后，继续执行：
- TASK-006: 实现图表绘制（数据流已打通，可以开始绘图）

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-003: 集成到MainWindow](./TASK-003-集成MainWindow.md)
- [TASK-004: 实现数据管理器](./TASK-004-数据管理器.md)
- [Qt 线程与事件循环](https://doc.qt.io/qt-5.14/threads-qobject.html)
