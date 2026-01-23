# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目概述

GenericScope 是一个基于 Qt 的通用上位机框架，使用 C++17 开发。它支持设备控制与监控、数据采集与分析、测试测量等多种应用场景。应用程序采用模块化和基于插件的架构，提供可扩展的设备控制和数据处理能力。

**核心技术栈：** Qt 5.14, C++17, spdlog

**设计理念：** 参考 MidScope 项目的成熟架构，提供通用化、可扩展的上位机开发框架。

## 构建命令

### 构建项目

```bash
# 完整构建（从项目根目录）
cd script
build.bat

# 手动构建步骤
qmake GenericScope.pro -spec win32-msvc
jom qmake_all
jom.exe -f Makefile.Release
```

**前置要求：**
- MSVC 编译器（vcvarsall.bat 必须在 PATH 中）
- Qt 5.14 及 qmake
- jom（Qt 的并行构建工具）

**构建输出：** `Bin/x64/Release/GenericScope.exe`（调试版本在 `Bin/x64/Debug/`）

### 架构检测

构建系统通过 `global.pri` 自动检测 x86 vs x64 架构。所有中间文件存放在 `temp/` 目录（obj, moc, rcc, ui）。

## 架构概览

### 模块结构

项目采用模块化架构，具有有序的依赖关系：

1. **algorithm** - 核心数据处理库（`libalgorithm`）
   - 数据处理算法
   - 信号处理
   - 数据分析工具

2. **util** - 工具类库（`libutil`）
   - 异步队列（AsyncQueue）
   - 时间辅助工具
   - 通用工具函数

3. **transfer** - 硬件通信抽象层（`libtransfer`）
   - 统一的传输接口
   - 多种通信协议支持
   - 工厂模式创建传输对象

4. **common_component** - 共享 UI 组件库（`libcommon_component`）
   - Graphics - 图形显示组件
   - Plot - 绘图组件
   - Log - 日志管理
   - Record - 数据录制

5. **core_plugin** - 插件系统
   - core_plugins_manager - 插件管理器（`libcore_plugins_manager`）
   - core_plugins - 具体插件实现（动态库）

6. **app** - 主应用程序可执行文件（`GenericScope.exe`）
   - UI 界面
   - 设备管理
   - 配置管理
   - 主控制逻辑

### 插件系统

应用程序使用由 `Core_PluginsManager`（单例）管理的动态插件架构：

**插件管理器：** `core_plugin/core_plugins_manager/core_pluginsmanager.h`
- `loadAllPlugins()` - 从 `Bin/x64/Release/core_plugins/` 加载插件
- `getPlugin(name)` - 获取已加载的插件
- `sendMessageToPlugin(name, message)` - 向插件发送消息
- 通过 `CorePluginMetaData` 结构体进行消息传递

**插件基类：** `core_plugin/core_plugins_manager/core_pluginsbase.h`
- 所有插件继承此抽象接口
- 实现 `handleManagerMessage()` - 处理管理器消息
- 实现 `sendMessageToManager()` - 向管理器发送消息
- 实现 `sendMessageToMain()` - 向主窗口发送消息
- 使用 Qt 插件元数据系统和 JSON 文件

**插件开发流程：**
1. 在 `core_plugin/core_plugins/` 中创建子目录
2. 继承 `CorePluginsBase`
3. 实现必需的虚函数
4. 创建 UI 部件（可选）
5. 添加 JSON 元数据文件
6. 将其添加到 `core_plugins.pro` 的 SUBDIRS

**消息传递机制：**
```cpp
// 插件 -> 管理器
CorePluginMetaData data;
data.type = MessageType::Command;
data.data = QVariant::fromValue(commandData);
sendMessageToManager(data);

// 管理器 -> 插件
void handleManagerMessage(const CorePluginMetaData &data) override {
    // 处理消息
}

// 插件 -> 主窗口
sendMessageToMain(data);
```

### 传输层

`transfer` 模块提供硬件通信抽象，支持多种后端：

**基类：**
- `TransferBasic` - 所有传输的抽象基类
  - `open()` / `close()` - 连接管理
  - `send()` / `receive()` - 数据收发
  - `isConnected()` - 连接状态查询

- `ImageTransfer` - 图像流传输接口（继承自 TransferBasic）
  - `setImageCallback()` - 设置图像回调函数
  - `startAcquisition()` / `stopAcquisition()` - 采集控制

- `ControlTransfer` - 命令/响应接口（继承自 TransferBasic）
  - `sendCommand()` - 发送命令
  - `waitResponse()` - 等待响应

**实现：**
- `UartTransfer` - 串口通信（基于 Qt SerialPort）
  - 支持 RS232/RS485
  - 可配置波特率、数据位、停止位、校验位

- `TcpTransfer` - TCP 套接字通信
  - 支持 TCP 客户端/服务器模式
  - 自动重连机制

- `ModbusTransfer` - Modbus 协议
  - 支持 Modbus RTU/TCP
  - 标准功能码实现

- `AcqTransfer` - 图像采集（可扩展支持多种相机）
  - 基于回调的帧传递
  - 支持触发模式和连续模式

**工厂模式：**
```cpp
TransferManager::createTransfer(TransferType type, const QVariantMap &config);
```

**使用示例：**
```cpp
// 创建串口传输
QVariantMap config;
config["port"] = "COM1";
config["baudRate"] = 115200;
auto uart = TransferManager::createTransfer(TransferType::UART, config);
uart->open();
uart->send(data);

// 创建图像传输
auto acq = TransferManager::createTransfer(TransferType::ACQ, config);
acq->setImageCallback([](const QImage &image) {
    // 处理图像
});
acq->startAcquisition();
```

### 异步处理

**AsyncQueue**（`util/async_queue.h`）：
- 基于模板的线程安全队列，可配置工作线程数
- 用于非阻塞数据处理
- 特性：
  - 容量管理和溢出处理
  - 统计跟踪（队列大小、处理速度）
  - 优先级队列支持（可选）
  - 批处理模式（可选）

**使用示例：**
```cpp
// 创建异步队列
AsyncQueue<DataPacket> queue(4, 100);  // 4个工作线程，容量100

// 设置处理函数
queue.setProcessor([](const DataPacket &packet) {
    // 处理数据
    processData(packet);
});

// 启动队列
queue.start();

// 添加数据
queue.push(packet);

// 停止队列
queue.stop();
```

### 配置系统

**位置：** `app/config/config.h`

**配置类：**
```cpp
class Config {
public:
    // 目录配置
    struct Dirs {
        QString dataDir;      // 数据目录
        QString logDir;       // 日志目录
        QString configDir;    // 配置目录
        QString pluginDir;    // 插件目录
    };

    // 应用配置
    struct App {
        QString version;      // 版本号
        QString language;     // 语言
        bool autoStart;       // 自动启动
    };

    // 设备配置
    struct Device {
        QString type;         // 设备类型
        QVariantMap params;   // 设备参数
    };

    // 算法参数
    struct AlgorithmParams {
        // 根据具体应用添加参数
    };

    // 单例访问
    static Config* instance();

    // 加载/保存配置
    void load();
    void save();

    // 配置访问
    Dirs dirs;
    App app;
    Device device;
    AlgorithmParams algorithmParams;
};

// 全局配置访问
extern Config *pConfig;
```

**持久化：** 设置通过 QSettings 保存到 `Settings.ini`

### 通用组件

**Graphics**（`common_component/graphics/`）：
- `GraphicsView` - 基于 QGraphicsView 的显示组件
- `GraphicsScene` - 场景管理
- `GraphicsItem` - 交互式测量工具（点、线、矩形、圆形）
- 支持缩放、平移、标注

**Plot**（`common_component/plot/`）：
- `BasicPlot` - 基础绘图部件（基于 QCustomPlot）
- `LinePlot` - 线图
- `BarPlot` - 柱状图
- `ScatterPlot` - 散点图
- `HistogramPlot` - 直方图
- 支持实时更新、多曲线、图例

**Log**（`common_component/log/`）：
- `LogManager` - 使用 spdlog 的单例日志管理器
- `LogWidget` - 日志显示的 UI 部件
- 日志级别：Trace, Debug, Info, Warning, Error, Critical
- 支持文件日志和控制台日志
- 日志轮转和归档

**Record**（`common_component/record/`）：
- `DataRecorder` - 数据录制管理器
- 支持多种格式：CSV, JSON, Binary
- 单帧捕获和连续录制
- 带时间戳和元数据
- 数据回放功能

## 关键设计模式

- **单例模式：** `Core_PluginsManager`, `LogManager`, `Config`, `TransferManager`
- **工厂模式：** `TransferManager` 用于创建传输对象
- **观察者模式：** Qt 信号/槽，插件消息传递
- **模板模式：** `AsyncQueue<T>` 用于通用异步处理
- **策略模式：** 多种传输后端（UART, TCP, Modbus, ACQ）
- **插件架构：** 通过 Qt 插件系统动态加载
- **MVC 模式：** UI 与业务逻辑分离

## 重要文件位置

| 组件 | 关键文件 |
|------|---------|
| 主应用 | `app/main.cpp`, `app/ui/mainwindow.h/cpp` |
| 设备管理 | `app/device/devicemanager.h` |
| 插件管理 | `core_plugin/core_plugins_manager/core_pluginsmanager.h` |
| 插件基类 | `core_plugin/core_plugins_manager/core_pluginsbase.h` |
| 算法处理 | `algorithm/algorithmprocessor.h` |
| 传输基类 | `transfer/transferbasic.h` |
| 传输工厂 | `transfer/transfermanager.h` |
| UI 组件 | `common_component/graphics/`, `common_component/plot/` |
| 配置管理 | `app/config/config.h` |
| 工具类 | `util/async_queue.h`, `util/timehelper.h` |

## 开发注意事项

### 线程安全
- 广泛使用原子变量和互斥锁
- AsyncQueue 提供线程安全的数据处理
- 传输层回调在独立线程上运行
- UI 更新必须在主线程执行（使用 QMetaObject::invokeMethod）

### 性能考虑
- 使用工作线程的 AsyncQueue 实现非阻塞处理
- 避免在主线程执行耗时操作
- 合理设置队列容量，避免内存溢出
- 使用对象池减少内存分配

### 外部依赖
- Qt 5.14（系统）
- spdlog（`libs/spdlog/`）
- 其他依赖根据具体应用添加

### Qt 5.14 API 使用规范
- **QVector API：** Qt 5.14 中 `QVector::resize()` 只支持单参数版本，需要配合 `fill()` 初始化元素
  ```cpp
  // 正确写法
  m_vector.resize(size);
  m_vector.fill(0);

  // 错误写法（Qt 5.14 不支持）
  m_vector.resize(size, 0);  // 编译错误
  ```
- **容器初始化：** 推荐使用构造函数进行初始化
  ```cpp
  QVector<int> vec(size, 0);  // 创建大小为 size，所有元素为 0 的向量
  ```
- **字符串处理：** 优先使用 Qt 5.14 支持的 API，避免使用 Qt 6 新增的 API

### 编译器设置
- 需要 C++17 标准
- MSVC 使用 UTF-8 编码（`/utf-8` 标志）
- 需要 Qt widgets, network, serialport 模块

### 单实例
应用程序使用 QSharedMemory 防止多个实例同时运行（参见 `app/main.cpp`）。

### 样式
- 使用 Qt Fusion 风格
- 从 `:/qss/default.qss` 加载 QSS 样式表
- 启用高 DPI 缩放

## 常见工作流程

### 添加新设备控制
1. 在 `core_plugin/core_plugins/your_plugin/` 创建新插件
2. 继承 `CorePluginsBase`
3. 创建 UI 部件（`.ui` 文件）
4. 实现消息处理逻辑
5. 添加 JSON 元数据文件
6. 更新 `core_plugins.pro`

### 添加新传输协议
1. 创建继承自 `TransferBasic` 的新类
2. 实现必需的虚函数（open, close, send, receive）
3. 添加到 `TransferManager` 工厂
4. 更新 `transfer.pro`

### 修改配置参数
1. 更新 `app/config/config.h` 中的配置结构
2. 修改 `app/ui/mainwindow.ui` 中的 UI 控件
3. 更新 `app/ui/mainwindow.cpp` 中的加载/保存逻辑
4. 设置自动持久化到 `Settings.ini`

### 添加数据处理算法
1. 在 `algorithm/algorithmprocessor.h` 中声明函数
2. 在 `algorithm/algorithmprocessor.cpp` 中实现
3. 在主窗口或插件中调用
4. 考虑使用 AsyncQueue 进行异步处理

### 调试技巧
- 使用 `LogManager` 进行全面日志记录
- 设置日志级别为 Debug 或 Trace
- 检查 `logs/` 目录下的日志文件
- 使用 Qt Creator 的调试器
- 检查插件加载状态

## 扩展指南

### 支持新的相机类型
1. 在 `transfer/acq/` 创建新的采集类
2. 继承 `ImageTransfer`
3. 实现相机 SDK 集成
4. 添加到 `TransferManager` 工厂

### 支持新的数据格式
1. 在 `common_component/record/` 添加新的编码器/解码器
2. 实现 `DataEncoder` 和 `DataDecoder` 接口
3. 注册到 `DataRecorder`

### 添加新的绘图类型
1. 在 `common_component/plot/` 创建新的绘图类
2. 继承 `BasicPlot`
3. 实现特定的绘图逻辑
4. 添加到主窗口或插件

## 项目路线图

### 第一阶段：核心框架
- [x] 项目结构搭建
- [ ] 基础模块实现（algorithm, util, transfer）
- [ ] 插件系统实现
- [ ] 主应用程序框架

### 第二阶段：通用组件
- [ ] Graphics 组件
- [ ] Plot 组件
- [ ] Log 组件
- [ ] Record 组件

### 第三阶段：传输层
- [ ] UART 传输
- [ ] TCP 传输
- [ ] Modbus 传输
- [ ] 图像采集传输

### 第四阶段：示例插件
- [ ] 设备控制插件
- [ ] 数据采集插件
- [ ] 测试测量插件

### 第五阶段：文档和测试
- [ ] API 文档
- [ ] 用户手册
- [ ] 单元测试
- [ ] 集成测试

## 许可证

待定

## 贡献指南

待定

## 联系方式

待定
