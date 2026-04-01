# TASK-004 完成记录

## 任务信息

- **任务ID**: TASK-004
- **任务名称**: 实现数据管理器（MonitorDataManager单例）
- **完成时间**: 2026-02-04
- **状态**: ✅ 已完成

## 完成内容

### 1. 创建的文件

#### MonitorDataManager（监控数据管理器）
- **文件**: `common_component/plot/monitordatamanager.h`
- **文件**: `common_component/plot/monitordatamanager.cpp`
- **功能**:
  - 单例模式数据管理器（线程安全）
  - 接收ProtocolParser解析数据
  - 通过信号分发数据给所有MonitorChart
  - 观察者模式实现解耦

### 2. 修改的文件

#### plot.pro
- **文件**: `common_component/plot/plot.pro`
- **修改内容**: 添加了 monitordatamanager.h/cpp

## 关键实现

### 单例模式（线程安全）
```cpp
// 双重检查锁定
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
```

### 数据分发机制
```cpp
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

### 核心API
```cpp
// 获取单例
static MonitorDataManager* instance();

// 接收协议数据
void onProtocolDataParsed(const QMap<QString, double> &fieldValues);

// 数据更新信号
signals:
    void dataUpdated(const QString &fieldName, double value, qint64 timestamp);
```

## 设计模式

### 1. 单例模式
- **实现**: 双重检查锁定（Double-Checked Locking）
- **线程安全**: QMutex保护实例创建
- **目的**: 全局唯一数据管理器

### 2. 观察者模式
- **发布者**: MonitorDataManager
- **订阅者**: MonitorChart（多个）
- **消息**: dataUpdated(fieldName, value, timestamp)
- **优势**: 解耦、扩展性强

## 验收标准

- [x] 单例模式正确实现（线程安全）
- [x] instance()能获取唯一实例
- [x] onProtocolDataParsed()能接收QMap<QString, double>数据
- [x] 每个字段触发一次dataUpdated信号
- [x] 时间戳生成正确（毫秒级）
- [x] 同一批数据使用同一时间戳
- [x] 编译通过，无警告

## 编译结果

✅ 编译成功，无错误无警告

## 架构优势

### 解耦设计
```
ProtocolParser
    ↓ (解析数据)
MonitorDataManager (中介者)
    ↓ (dataUpdated信号)
MonitorChart1, MonitorChart2, ... (订阅者)
```

- ProtocolParser不需要知道MonitorChart的存在
- MonitorChart不需要知道ProtocolParser的存在
- 通过MonitorDataManager中介，实现松耦合

### 扩展性
- 新增MonitorChart只需订阅dataUpdated信号
- 不需要修改现有代码
- 支持动态添加/移除订阅者

### 一致性
- 同一批数据使用相同时间戳
- 保证图表时间轴对齐
- 避免时间戳不一致导致的显示问题

## 下一步集成

TASK-005将连接数据流：
1. 在MainWindow中连接ProtocolParser解析完成信号
2. 将解析结果转换为QMap<QString, double>
3. 调用MonitorDataManager::instance()->onProtocolDataParsed()
4. MonitorChart订阅dataUpdated信号并更新数据

## 备注

- 目前只支持double类型数据
- 如需支持其他类型，可改用QVariant
- 空数据会被过滤，避免无意义的信号发射
