# CLAUDE.md

本文档为 Claude Code 提供 GenericScope 项目的核心指导信息。

## 项目概述

GenericScope 是一个基于 Qt 5.14 的通用上位机框架，使用 C++17 开发。支持设备控制、数据采集、测试测量等场景。

**核心技术栈：** Qt 5.14, C++17, MSVC, spdlog

**实现状态：** 约95%完成，接近生产就绪

## 快速开始

### 构建项目

```bash
# 完整构建
cd script
build.bat

# 手动构建
qmake GenericScope.pro -spec win32-msvc
jom qmake_all
jom.exe -f Makefile.Release
```

**前置要求：** MSVC 编译器, Qt 5.14, jom

**输出目录：** `Bin/x64/Release/GenericScope.exe`

## 架构概览

### 模块依赖关系

```
app (主程序)
 ├── algorithm (数据处理)
 ├── util (工具类)
 ├── protocol (协议解析) ⭐核心模块
 │    ├── ProtocolConfig - 协议配置
 │    ├── ProtocolParser - 动态解析器
 │    ├── ChecksumCalculator - 6种校验算法
 │    ├── DataTypeConverter - 10种数据类型
 │    └── ProtocolManager - 单例管理器
 ├── transfer (硬件通信)
 │    ├── ScopeUart - 串口
 │    ├── ScopeTcp - TCP
 │    └── ScopeModbus - Modbus
 ├── common_component (通用组件)
 │    ├── plot - 绘图组件
 │    ├── log - 日志管理
 │    └── record - 数据录制
 └── core_plugin (插件系统)
      ├── core_plugins_manager - 插件管理器
      └── core_plugins - 具体插件实现
           ├── datamonitor - 数据监控插件
           └── devicecontrol - 设备控制插件
```

### 核心特性

**1. 协议系统（企业级）**
- 6种校验算法：Sum/XOR/CRC8/CRC16-MODBUS/CRC16-CCITT/CRC32
- 10种数据类型：int8/uint8/int16/uint16/int32/uint32/float/double/mbyte/string
- 大小端转换、缩放因子、独立校验码字节序
- AI辅助生成、协议测试、JSON导入导出

**2. 插件系统**
- 动态加载插件（Qt插件机制）
- 标准化消息传递（插件↔管理器↔主窗口）
- 已实现2个示例插件（DataMonitor, DeviceControl）

**3. 传输层**
- 统一抽象接口（ScopeTransferBasic）
- 工厂模式创建（ScopeTransferManager）
- 支持UART/TCP/Modbus/控制传输

**4. UI界面**
- 三栏布局（数据表格 + 3D可视化 + 实时图表）
- 主题切换（亮色/暗色）
- IMU 3D姿态显示
- 实时曲线图和直方图

## 核心组件使用

### 协议配置与解析

```cpp
// 1. 创建协议配置
ProtocolConfig config;
config.name = "IMU_Protocol_V1";
config.frameHeader = "FF AA";
config.frameFooter = "0D 0A";
config.checksumType = ChecksumType::CRC16_MODBUS;
config.checksumScope = ChecksumScope::AfterHeader;
config.byteOrder = ByteOrder::LittleEndian;

// 2. 添加字段
FieldConfig field;
field.name = "Roll";
field.type = DataType::Float;
field.elementHead = 0;
field.byteLength = 4;
config.fields.append(field);

// 3. 保存并创建解析器
ProtocolManager::instance()->addProtocol(config);
auto parser = ProtocolManager::instance()->createParser("IMU_Protocol_V1");

// 4. 解析数据
ParseResult result = parser->parse(rawData);
if (result.success) {
    double roll = result.fieldValues["Roll"].toDouble();
}
```

### 插件开发

```cpp
class MyPlugin : public QObject, public CorePluginsBase {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.genericscope.plugin" FILE "metadata.json")
    Q_INTERFACES(CorePluginsBase)

public:
    void handleManagerMessage(const CorePluginMetaData &data) override;
    void sendMessageToManager(const CorePluginMetaData &data) override;
};
```

### 传输层使用

```cpp
// 创建串口传输
PScopeTransferBasic uart = nullptr;
ScopeTransferManager::createTransfer(TransferType::Uart, &uart);
uart->open();
uart->send(data);
ScopeTransferManager::destoryTransfer(uart);
```

## 关键文件位置

| 组件 | 文件路径 |
|------|---------|
| 主窗口 | `app/ui/mainwindow.h/cpp` |
| 3D可视化 | `app/ui/imu3dview.h/cpp` |
| 协议配置UI | `app/ui/commandsettingsdialog.h/cpp` |
| AI生成器 | `app/ui/protocolaigenerator.h/cpp` |
| 协议解析器 | `protocol/protocolparser.h/cpp` |
| 校验计算 | `protocol/checksumcalculator.h/cpp` |
| 协议管理器 | `protocol/protocolmanager.h/cpp` |
| 串口传输 | `transfer/uart/scopeuart.h/cpp` |
| 插件管理器 | `core_plugin/core_plugins_manager/core_pluginsmanager.h` |
| 插件基类 | `core_plugin/core_plugins_manager/core_pluginsbase.h` |

## 开发规范

### Qt 5.14 兼容性

```cpp
// ✅ 正确：容器初始化
QVector<int> vec(size, 0);

// ❌ 错误：Qt 5.14不支持
m_vector.resize(size, 0);  // 编译错误

// ✅ 正确：分两步
m_vector.resize(size);
m_vector.fill(0);
```

### 命名约定

- **类名：** CamelCase (例: `DeviceManager`)
- **方法/变量：** camelCase (例: `sendData()`)
- **成员变量：** m_前缀 (例: `m_data`)
- **常量：** k前缀 (例: `kDefaultTimeout`)

### 线程安全

```cpp
// 使用QMutexLocker实现RAII
void updateData(const Data &data) {
    QMutexLocker locker(&m_mutex);
    m_data = data;
}

// UI更新必须在主线程
QMetaObject::invokeMethod(this, [this, data]() {
    ui->label->setText(data);
}, Qt::QueuedConnection);
```

### 内存管理

```cpp
// 优先使用Qt父子关系
QWidget *widget = new QWidget(parent);

// 或使用智能指针
std::unique_ptr<Data> data = std::make_unique<Data>();
QScopedPointer<File> file(new QFile());
```

## 设计模式

- **单例：** ProtocolManager, LogManager, Config
- **工厂：** TransferManager, ProtocolManager::createParser()
- **适配器：** ProtocolTypeConverter (UI ↔ Protocol)
- **观察者：** Qt信号/槽
- **策略：** 多种校验算法、传输类型
- **插件架构：** Qt插件系统

## 项目状态

**总体完成度：约95%**

✅ **已完成核心功能：**
- 完整协议系统（6种校验、10种数据类型、AI生成）
- 插件架构（2个示例插件）
- 传输层（UART/TCP/Modbus）
- UI界面（三栏布局、3D可视化、主题切换）
- 数据可视化（表格、图表、直方图）

✅ **已完成核心功能：**
- 完整协议系统（6种校验、10种数据类型、AI生成）
- 插件架构（2个示例插件）
- 传输层（UART/TCP/Modbus）
- UI界面（三栏布局、3D可视化、主题切换）
- 数据可视化（表格、图表、直方图）
- ScopeUart协议集成
- **用户手册**（完整文档）

🔄 **待完善：**
- 单元测试和集成测试
- API文档（Doxygen生成）

## 常见工作流程

### 添加新插件
1. 在`core_plugin/core_plugins/`创建目录
2. 继承`CorePluginsBase`
3. 创建UI部件和metadata.json
4. 更新`core_plugins.pro`

### 添加新传输协议
1. 继承`ScopeTransferBasic`
2. 实现open/close/send/receive
3. 添加到`TransferManager`工厂
4. 更新`transfer.pro`

### 配置新协议
1. 打开主界面"设置"→"协议配置"
2. 使用AI生成或手动配置
3. 测试协议解析
4. 生成并保存

## 调试技巧

```cpp
// 使用日志系统
LogManager::instance()->info("操作成功");
LogManager::instance()->error("错误: {}", errorMsg);

// 信号槽连接检查
bool ok = connect(sender, &Sender::signal, receiver, &Receiver::slot);
Q_ASSERT(ok);

// 检查日志文件
// logs/log_yyyyMMdd_hhmmss.txt
```

## 详细文档

- **用户手册：** `docs/user-manual.md` ⭐
- **ScopeUart协议集成示例：** `docs/scopeuart-protocol-integration-example.md`
- **协议系统设计规范：** `docs/protocol-config-system-spec.md`
- **ScopeUart集成指南：** `docs/scopeuart-integration-guide.md`
- **任务文档：** `docs/tasks/`

## 项目路线图

### ✅ 已完成（95%）
- [x] 核心框架（主应用、协议系统、基础模块、插件系统）
- [x] 通用组件（Plot/Log/Record）
- [x] 传输层（UART/TCP/Modbus）
- [x] 示例插件（DataMonitor/DeviceControl）
- [x] 高级功能（AI生成、协议测试、新建向导）

### 🔄 进行中
- [ ] ScopeUart完整集成
- [ ] 单元测试
- [ ] API文档
- [ ] 用户手册

---

**项目定位：** 工业级通用上位机框架，支持设备控制、数据采集、测试测量等多种应用场景。
