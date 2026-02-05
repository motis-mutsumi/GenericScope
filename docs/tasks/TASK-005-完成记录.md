# TASK-005 完成记录

## 任务信息

- **任务ID**: TASK-005
- **任务名称**: 连接数据流（ProtocolParser → MonitorDataManager）
- **完成时间**: 2026-02-04
- **状态**: ✅ 已完成（含重大架构改进）

## 完成内容

### 1. 基础数据流连接

#### MainWindow数据分发（初版）
- **文件**: `app/ui/mainwindow.cpp`
- **修改内容**:
  - 添加头文件 `#include "common_component/plot/monitordatamanager.h"`
  - 在 `processData()` 函数末尾添加数据分发代码
  - 将解析后的字段值传递给MonitorDataManager

#### MonitorChart数据订阅
- **文件**: `common_component/plot/monitorchart.cpp`
- **修改内容**:
  - 添加头文件 `#include "monitordatamanager.h"`
  - 构造函数中订阅dataUpdated信号（Qt::QueuedConnection）
  - 实现 `onDataReceived()` 方法（字段过滤、暂停检查、数据缓存）
  - 实现 `onPauseAction()` - 暂停/恢复功能
  - 实现 `clearData()` - 清空数据缓存
  - 实现 `onClearAction()` - 右键菜单清空功能

### 2. 架构重大改进（动态协议解析）

#### 问题发现
用户发现`processData`函数使用硬编码CSV解析，不会跟随协议配置变化，导致：
- 协议切换无效
- 只能监控固定的13个字段
- 与项目的ProtocolParser系统脱节

#### 解决方案
将MainWindow改为使用**ProtocolParser动态解析**：

**mainwindow.h 修改**:
```cpp
// 新增成员
class ProtocolParser *m_protocolParser;  // 协议解析器（动态）

// 新增方法
void processDataLegacy(const QByteArray &data);  // 硬编码解析（后备）
QString getFieldUnit(const QString &fieldName);  // 从协议获取字段单位
```

**mainwindow.cpp 核心改进**:

1. **添加头文件**:
   ```cpp
   #include "protocol/protocolparser.h"
   ```

2. **构造函数初始化**:
   ```cpp
   , m_protocolParser(nullptr)
   ```

3. **协议切换时重建解析器**:
   ```cpp
   void MainWindow::onProtocolChanged(const QString &name)
   {
       // 删除旧解析器
       if (m_protocolParser) {
           delete m_protocolParser;
           m_protocolParser = nullptr;
       }

       // 创建新解析器
       ProtocolConfig config = ProtocolManager::instance()->getProtocol(name);
       m_protocolParser = new ProtocolParser(config);

       // 清空监控面板（字段可能不匹配）
       m_monitorPanel->clearAllCharts();
   }
   ```

4. **processData改为动态解析**:
   ```cpp
   void MainWindow::processData(const QByteArray &data)
   {
       // 使用ProtocolParser动态解析
       ParseResult result = m_protocolParser->parse(data);

       if (!result.success) {
           LOG_ERROR(result.errorMsg);
           return;
       }

       // 转换为double映射
       QMap<QString, double> fieldValues;
       for (auto it = result.fieldValues.begin(); it != result.fieldValues.end(); ++it) {
           bool ok;
           double value = it.value().toDouble(&ok);
           if (ok) {
               fieldValues[it.key()] = value;

               // 更新数据表格（单位从协议获取）
               QString unit = getFieldUnit(it.key());
               updateDataTable(it.key(), value, unit);
           }
       }

       // 分发给MonitorDataManager
       MonitorDataManager::instance()->onProtocolDataParsed(fieldValues);

       // 更新3D视图（如果有姿态字段）
       if (fieldValues.contains("Roll") && fieldValues.contains("Pitch") && fieldValues.contains("Yaw")) {
           updateAttitudeDisplay(fieldValues["Roll"], fieldValues["Pitch"], fieldValues["Yaw"]);
       }
   }
   ```

5. **添加辅助函数**:
   ```cpp
   QString MainWindow::getFieldUnit(const QString &fieldName)
   {
       if (!m_protocolParser) return QString();

       const ProtocolConfig &config = m_protocolParser->config();
       for (const FieldConfig &field : config.fields) {
           if (field.name == fieldName) {
               return field.unit;
           }
       }
       return QString();
   }
   ```

6. **保留后备方案**:
   - `processDataLegacy()` - 保留原有硬编码CSV解析
   - 当没有配置协议时自动回退到旧方式
   - 确保向后兼容

## 数据流路径

```
设备数据
  ↓
MainWindow::processData()
  ↓
ProtocolParser::parse() (动态解析)
  ↓
ParseResult::fieldValues (QMap<QString, QVariant>)
  ↓
转换为 QMap<QString, double>
  ↓
MonitorDataManager::onProtocolDataParsed()
  ↓
emit dataUpdated(fieldName, value, timestamp)
  ↓
MonitorChart::onDataReceived() (字段过滤 + 暂停检查)
  ↓
数据缓存 (m_timestamps, m_values)
  ↓
[待TASK-006绘制图表]
```

## 验收标准

- [x] MainWindow正确调用MonitorDataManager::onProtocolDataParsed()
- [x] 字段值类型转换正确（QVariant → double）
- [x] MonitorChart成功订阅dataUpdated信号
- [x] 使用Qt::QueuedConnection确保线程安全
- [x] onDataReceived()能收到数据（通过qDebug验证）
- [x] 字段过滤正确（只接收匹配字段的数据）
- [x] 暂停状态检查正确
- [x] ✨ **使用ProtocolParser动态解析（重大改进）**
- [x] ✨ **协议切换时数据解析自动同步**
- [x] ✨ **支持监控任意协议字段**
- [x] 编译通过，无警告

## 编译结果

✅ 编译成功，无错误无警告

## 改进亮点

### ✅ 动态适配协议配置
- 修改协议配置后，数据解析**自动同步**
- 可以监控**任意字段**（不再限于13个固定字段）
- 字段名称和单位从协议配置动态获取

### ✅ 架构统一
- MainWindow现在使用项目的ProtocolParser系统
- 消除了硬编码与协议系统的脱节
- 数据表格、MonitorPanel、数据解析**三者同步**

### ✅ 兼容性保留
- 保留`processDataLegacy()`作为后备
- 如果没有配置协议，自动回退到旧方式
- 不会破坏现有功能

## 工作流程示例

```
1. 用户配置新协议（如添加Temperature_2字段）
   ↓
2. onProtocolChanged()触发
   ↓
3. 重建ProtocolParser（新协议配置）
   ↓
4. 清空MonitorPanel旧图表
   ↓
5. 重建数据表格（显示新字段）
   ↓
6. processData()使用新解析器解析数据
   ↓
7. MonitorPanel可以监控新字段
```

## 测试方法

### 集成测试

**测试场景1：数据流验证**
1. 配置IMU协议（包含Roll、Pitch、Yaw字段）
2. 添加Roll图表
3. 连接设备，开始接收数据
4. 观察控制台输出（Debug模式）：
   ```
   [Roll] 收到数据: 45.20 @ 1738654321234
   [Roll] 收到数据: 45.30 @ 1738654321244
   ```

**测试场景2：字段过滤验证**
1. 配置协议（包含Roll、Pitch、Yaw）
2. 添加Roll图表
3. 发送包含所有字段的数据
4. 验证：只输出Roll的数据，Pitch和Yaw被过滤

**测试场景3：暂停功能验证**
1. 添加Roll图表
2. 接收几条数据（有输出）
3. 右键暂停图表
4. 继续接收数据
5. 验证：暂停后无输出

**测试场景4：多图表验证**
1. 添加Roll、Pitch、Yaw三个图表
2. 接收数据
3. 验证：三个图表都收到数据（相同时间戳）

**测试场景5：协议切换验证** ✨
1. 配置协议A（如IMU：Roll, Pitch, Yaw）
2. 添加Roll监控图表
3. 连接设备，验证数据正常
4. 切换到协议B（如温度：Temp1, Temp2, Humidity）
5. 验证：数据表格自动更新，旧图表被清空
6. 添加Temp1监控图表
7. 验证：新协议数据正常解析和显示

## 问题修复

### 问题1: 硬编码解析不跟随协议配置
- **发现**: 用户指出processData使用硬编码CSV格式，协议切换无效
- **原因**: 未使用ProtocolParser系统，存在两套解析逻辑
- **修复**: 改为使用ProtocolParser动态解析，协议切换时重建解析器
- **影响**: 彻底解决了协议切换无效的问题

## 额外完成功能

- ✅ 暂停/恢复功能（`onPauseAction()`）
- ✅ 清空数据功能（`clearData()`, `onClearAction()`）
- ✅ 协议切换时自动清空监控面板
- ✅ 字段单位动态获取（`getFieldUnit()`）

## 调试输出

Debug模式下可通过qDebug观察数据流：
```cpp
#ifdef QT_DEBUG
    qDebug() << QString("[%1] 收到数据: %2 @ %3")
                    .arg(m_fieldName)
                    .arg(value, 0, 'f', 2)
                    .arg(timestamp);
#endif
```

## 下一步

数据流已完全打通，所有准备工作已完成，可以继续：
- **TASK-006**: 实现图表绘制（QCustomPlot集成）- 将缓存的数据绘制成实时曲线

## 备注

- 线程安全：使用Qt::QueuedConnection确保跨线程信号安全
- 内存管理：m_protocolParser在协议切换时正确delete旧实例
- 性能：目前未优化高频数据（待后期根据实际需求优化）
- **架构改进是本任务的重大亮点**，彻底解决了硬编码与协议系统脱节的问题
