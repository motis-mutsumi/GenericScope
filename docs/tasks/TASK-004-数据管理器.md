# TASK-004: 实现数据管理器

## 任务信息

- **任务ID**: TASK-004
- **任务名称**: 实现数据管理器
- **优先级**: P0（必须完成）
- **预估工作量**: 2小时
- **依赖任务**: TASK-001
- **状态**: 待开始

## 任务描述

创建MonitorDataManager单例类，负责接收ProtocolParser的解析数据，并通过信号分发给所有订阅的MonitorChart。实现观察者模式的数据分发机制。

## 具体目标

1. 创建MonitorDataManager单例类
2. 实现onProtocolDataParsed()接收解析数据
3. 定义dataUpdated信号（字段名、值、时间戳）
4. 在onProtocolDataParsed()中遍历字段，emit信号
5. 使用QDateTime::currentMSecsSinceEpoch()生成时间戳

## 涉及文件

### 新增文件
- `common_component/plot/monitordatamanager.h`
- `common_component/plot/monitordatamanager.cpp`

### 修改文件
- `common_component/plot.pro`

## 实现细节

### 1. MonitorDataManager.h

```cpp
#ifndef MONITORDATAMANAGER_H
#define MONITORDATAMANAGER_H

#include <QObject>
#include <QMap>

/**
 * @brief 监控数据管理器（单例）
 *
 * 负责接收ProtocolParser解析的数据，并分发给所有订阅的MonitorChart。
 * 使用观察者模式，通过信号/槽实现解耦。
 */
class MonitorDataManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief 获取单例实例
     */
    static MonitorDataManager* instance();

    /**
     * @brief 接收协议解析数据
     * @param fieldValues 字段值映射 <字段名, 数值>
     *
     * 此方法会遍历所有字段，为每个字段生成时间戳并emit dataUpdated信号
     */
    void onProtocolDataParsed(const QMap<QString, double> &fieldValues);

signals:
    /**
     * @brief 数据更新信号
     * @param fieldName 字段名
     * @param value 数值
     * @param timestamp 时间戳（毫秒）
     *
     * 所有MonitorChart订阅此信号，根据字段名过滤自己需要的数据
     */
    void dataUpdated(const QString &fieldName, double value, qint64 timestamp);

private:
    explicit MonitorDataManager(QObject *parent = nullptr);
    ~MonitorDataManager();

    static MonitorDataManager *s_instance;
    static QMutex s_mutex;  // 线程安全
};

#endif // MONITORDATAMANAGER_H
```

### 2. MonitorDataManager.cpp

```cpp
#include "monitordatamanager.h"
#include <QDateTime>
#include <QMutexLocker>

// 静态成员初始化
MonitorDataManager* MonitorDataManager::s_instance = nullptr;
QMutex MonitorDataManager::s_mutex;

MonitorDataManager::MonitorDataManager(QObject *parent)
    : QObject(parent)
{
}

MonitorDataManager::~MonitorDataManager()
{
}

MonitorDataManager* MonitorDataManager::instance()
{
    if (s_instance == nullptr) {
        QMutexLocker locker(&s_mutex);
        if (s_instance == nullptr) {
            s_instance = new MonitorDataManager();
        }
    }
    return s_instance;
}

void MonitorDataManager::onProtocolDataParsed(const QMap<QString, double> &fieldValues)
{
    if (fieldValues.isEmpty()) {
        return;
    }

    // 生成时间戳（所有字段使用同一时间戳，表示同一批数据）
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    // 遍历所有字段，分发数据
    for (auto it = fieldValues.begin(); it != fieldValues.end(); ++it) {
        const QString &fieldName = it.key();
        double value = it.value();

        // 发送数据更新信号
        emit dataUpdated(fieldName, value, timestamp);
    }
}
```

### 3. 更新plot.pro

```pro
# 在HEADERS添加
HEADERS += \
    # ... existing headers ...
    $$PWD/monitordatamanager.h

# 在SOURCES添加
SOURCES += \
    # ... existing sources ...
    $$PWD/monitordatamanager.cpp
```

## 验收标准

- [ ] 单例模式正确实现（线程安全）
- [ ] instance()能获取唯一实例
- [ ] onProtocolDataParsed()能接收QMap<QString, double>数据
- [ ] 每个字段触发一次dataUpdated信号
- [ ] 时间戳生成正确（毫秒级）
- [ ] 同一批数据使用同一时间戳
- [ ] 编译通过，无警告

## 测试方法

### 单元测试

```cpp
void TestMonitorDataManager::testSingleton()
{
    auto *manager1 = MonitorDataManager::instance();
    auto *manager2 = MonitorDataManager::instance();

    // 验证单例
    QCOMPARE(manager1, manager2);
}

void TestMonitorDataManager::testDataDistribution()
{
    auto *manager = MonitorDataManager::instance();

    // 创建信号监听器
    QSignalSpy spy(manager, &MonitorDataManager::dataUpdated);

    // 发送数据
    QMap<QString, double> data;
    data["Roll"] = 45.2;
    data["Pitch"] = -12.3;
    data["Yaw"] = 180.5;

    manager->onProtocolDataParsed(data);

    // 验证信号触发3次
    QCOMPARE(spy.count(), 3);

    // 验证信号参数
    QList<QVariant> args1 = spy.at(0);
    QCOMPARE(args1.at(0).toString(), QString("Roll"));
    QCOMPARE(args1.at(1).toDouble(), 45.2);
    QVERIFY(args1.at(2).toLongLong() > 0);  // 时间戳有效
}

void TestMonitorDataManager::testTimestamp()
{
    auto *manager = MonitorDataManager::instance();

    QSignalSpy spy(manager, &MonitorDataManager::dataUpdated);

    QMap<QString, double> data;
    data["Field1"] = 1.0;
    data["Field2"] = 2.0;

    qint64 beforeTime = QDateTime::currentMSecsSinceEpoch();
    manager->onProtocolDataParsed(data);
    qint64 afterTime = QDateTime::currentMSecsSinceEpoch();

    // 验证两个字段的时间戳相同（同一批数据）
    qint64 timestamp1 = spy.at(0).at(2).toLongLong();
    qint64 timestamp2 = spy.at(1).at(2).toLongLong();
    QCOMPARE(timestamp1, timestamp2);

    // 验证时间戳在合理范围内
    QVERIFY(timestamp1 >= beforeTime);
    QVERIFY(timestamp1 <= afterTime);
}
```

### 手动测试

```cpp
// 创建测试代码
auto *manager = MonitorDataManager::instance();

// 连接信号
QObject::connect(manager, &MonitorDataManager::dataUpdated,
    [](const QString &fieldName, double value, qint64 timestamp) {
        qDebug() << "字段:" << fieldName
                 << "值:" << value
                 << "时间戳:" << timestamp;
    });

// 发送测试数据
QMap<QString, double> data;
data["Roll"] = 45.2;
data["Pitch"] = -12.3;
manager->onProtocolDataParsed(data);

// 预期输出：
// 字段: "Roll" 值: 45.2 时间戳: 1612345678901
// 字段: "Pitch" 值: -12.3 时间戳: 1612345678901
```

## 注意事项

1. **线程安全**：
   - 使用双重检查锁定实现单例
   - QMutex保护实例创建
   - 信号发射本身是线程安全的（Qt机制）

2. **时间戳精度**：
   - 使用QDateTime::currentMSecsSinceEpoch()（毫秒级）
   - 同一批数据使用相同时间戳，保证时间一致性

3. **性能考虑**：
   - 遍历map的开销很小（通常<10个字段）
   - emit信号的开销取决于订阅数量
   - 如果有性能问题，后期可优化

4. **数据类型**：
   - 目前只支持double类型
   - 如需支持其他类型，使用QVariant

5. **空数据处理**：
   - 空map时直接返回，避免无意义的时间戳生成

## 设计优势

### 解耦设计
- ProtocolParser不需要知道MonitorChart的存在
- MonitorChart不需要知道ProtocolParser的存在
- 通过MonitorDataManager中介，实现松耦合

### 扩展性
- 新增MonitorChart只需订阅dataUpdated信号
- 不需要修改现有代码
- 支持动态添加/移除订阅者

### 可测试性
- 单例可mock
- 信号可用QSignalSpy测试
- 业务逻辑清晰

## 后续任务

完成此任务后，继续执行：
- TASK-005: 连接数据流（需要TASK-003和TASK-004都完成）

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-001: 搭建基础框架](./TASK-001-基础框架.md)
- [Qt 信号与槽](https://doc.qt.io/qt-5.14/signalsandslots.html)
- [Qt 单例模式](https://doc.qt.io/qt-5.14/qobject.html#Q_GLOBAL_STATIC)
