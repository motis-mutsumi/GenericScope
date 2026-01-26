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
   - UI 界面（基于 DS_RVision 设计）
   - 设备管理
   - 配置管理
   - 主控制逻辑
   - 数据可视化（表格、图表、3D 姿态显示）

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

### 主界面设计

**位置：** `app/ui/mainwindow.h/cpp/ui`

主界面采用三栏布局，参考 DS_RVision IMU 可视化界面设计：

```
┌────────────────────────────────────────────────────────────┐
│ GENERICSCOPE          DS_GenericScope            [⚙] [🌙] │
├────────────────────────────────────────────────────────────┤
│ [COM7 ▼] [115200 ▼] [Connect] [□ RecordLog] [≡]          │
├──────────┬─────────────────────────┬─────────────────────┤
│ 🔽       │   🎯                    │ 📊 X_Range[60▼]    │
│          │   Roll Pitch Yaw        │    X_Dot[2▼] [⚙]   │
│ Message  │   -      -      -       │                     │
│ Value    │                         │                     │
│ Unit     │   [3D Visualization]    │   [Real-time Chart] │
│          │                         │                     │
│          │   IMU Status            │                     │
│          │   - Status: Ready       │   00:00:00.000      │
│          │   - Data Rate: 0 Hz     │      Time           │
│          │   - Error: None         │   00:00:00.000      │
└──────────┴─────────────────────────┴─────────────────────┘
```

**界面组成：**

1. **顶部标题栏**
   - 品牌标识：GENERICSCOPE
   - 应用标题：DS_GenericScope
   - 设置按钮：打开设置菜单（指令设置、设备配置、显示选项、关于）
   - 暗色模式切换：切换亮色/暗色主题

2. **设备控制栏**
   - COM 端口选择：自动检测可用串口
   - 波特率选择：9600-115200
   - Connect/Disconnect 切换按钮
   - RecordLog 复选框：启用/停止数据录制
   - 菜单按钮：显示上下文菜单

3. **左侧：数据表格面板**
   - 筛选工具栏
   - Message/Value/Unit 三列表格
   - 显示传感器原始数据：
     - 加速度计（AccX/Y/Z）
     - 陀螺仪（GyroX/Y/Z）
     - 磁力计（MagX/Y/Z）
     - 温度（Temperature）

4. **中间：3D 可视化面板**
   - 姿态角度显示：Roll/Pitch/Yaw
   - 3D 可视化容器（待实现 3D 模型）
   - IMU 状态分组框：
     - 连接状态
     - 数据速率（Hz）
     - 错误信息

5. **右侧：图表面板**
   - 图表参数设置：
     - X_Range：时间轴范围（10/30/60/120 秒）
     - X_Dot：点密度（1/2/5/10）
   - 实时曲线图（LinePlot）
   - 时间戳显示：开始时间 - Time - 当前时间

**数据格式：**

应用程序期望接收 CSV 格式的 IMU 数据：
```
ROLL,PITCH,YAW,AX,AY,AZ,GX,GY,GZ,MX,MY,MZ,TEMP
```

示例数据包：
```
1.23,4.56,7.89,0.1,0.2,9.8,0.01,0.02,0.03,25.4,12.3,8.9,25.5
```

**主要功能：**

- **设备连接**
  - 自动检测可用串口
  - 支持多种波特率
  - 连接状态实时反馈

- **数据可视化**
  - 实时更新数据表格
  - 姿态角度显示
  - 实时曲线绘制
  - 可配置图表范围和密度

- **数据录制**
  - CSV 格式保存
  - 自动生成时间戳文件名
  - 记录完整的 IMU 数据和元数据

- **主题切换**
  - 亮色主题（默认）
  - 暗色主题（护眼）
  - 实时切换，无需重启

- **性能优化**
  - 多定时器分离不同频率的更新
  - UI 更新定时器：100ms
  - 数据处理定时器：100ms
  - 时间显示定时器：100ms

- **指令设置**
  - 16进制指令管理
  - 支持手动填写和外部导入
  - 指令列表管理（添加、删除、编辑）
  - 指令详情编辑（名称、内容、描述）
  - 自动生成指令功能（待实现）
  - 指令测试发送功能（待实现）

### 指令设置对话框

**位置：** `app/ui/commandsettingsdialog.h/cpp`

指令设置对话框用于管理16进制格式的设备控制指令：

**界面布局：**
```
┌──────────────────────────────────────────────────────────┐
│ 指令设置                                         [_][□][×]│
├──────────────────────────────────────────────────────────┤
│ ┌─指令列表────────────────┐ ┌─指令详情──────────────────┐│
│ │ 序号  名称      指令内容 │ │ 指令名称:                 ││
│ │ ──────────────────────│ │ [____________________]    ││
│ │  1   复位      FF AA 01 │ │                           ││
│ │  2   查询      FF AA 02 │ │ 指令内容(HEX):            ││
│ │  3   设置      FF AA 03 │ │ [____________________]    ││
│ │                         │ │                           ││
│ │                         │ │ 描述:                     ││
│ │                         │ │ [____________________]    ││
│ │                         │ │ [____________________]    ││
│ │ [添加] [删除] [导入]    │ │                           ││
│ └────────────────────────┘ │ [清空] [生成]             ││
│                             └───────────────────────────┘│
│                             [确定] [取消] [应用]         │
└──────────────────────────────────────────────────────────┘
```

**主要功能：**

- **指令列表管理**
  - 表格显示：序号、名称、指令内容(HEX)
  - 支持选择行查看详情
  - 交替行颜色显示

- **指令编辑**
  - 指令名称输入（如：复位指令）
  - 指令内容输入（16进制格式，如：FF AA 01 02 03）
  - 指令描述输入（多行文本）

- **操作按钮**
  - 添加：将当前编辑的指令添加到列表
  - 删除：删除选中的指令（需确认）
  - 导入：从外部文件导入指令（JSON/TXT格式，待实现）
  - 清空：清空当前编辑区域
  - 生成：自动生成指令（待实现）
  - 确定：保存并关闭对话框
  - 取消：放弃修改并关闭
  - 应用：保存但不关闭对话框

**使用方法：**

1. 点击主界面右上角"设置"按钮（⚙）
2. 在弹出菜单中选择"指令设置"
3. 在右侧填写指令信息
4. 点击"添加"按钮添加到列表
5. 选择列表中的指令可查看和编辑
6. 点击"确定"保存设置

**扩展功能（待实现）：**
- 指令导入/导出（JSON/TXT格式）
- 指令自动生成逻辑
- 指令发送测试功能
- 指令配置持久化
- 16进制格式验证
- 指令模板管理

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
| 指令设置 | `app/ui/commandsettingsdialog.h/cpp` |
| 设备管理 | `app/device/devicemanager.h` |
| 插件管理 | `core_plugin/core_plugins_manager/core_pluginsmanager.h` |
| 插件基类 | `core_plugin/core_plugins_manager/core_pluginsbase.h` |
| 算法处理 | `algorithm/algorithmprocessor.h` |
| 传输基类 | `transfer/transferbasic.h` |
| 传输工厂 | `transfer/transfermanager.h` |
| UI 组件 | `common_component/graphics/`, `common_component/plot/` |
| 配置管理 | `app/config/config.h` |
| 工具类 | `util/async_queue.h`, `util/timehelper.h` |
| 样式表 | `app/qss/light.qss`, `app/qss/dark.qss` |
| 资源文件 | `app/resources.qrc` |

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

### 代码审查记录

#### 2026-01-26: MainWindow Qt 5.14代码审查与优化

对 `app/ui/mainwindow.h/cpp` 进行了全面的Qt 5.14规范审查和代码优化。

**✅ 主要改进：**

1. **添加常量定义**（`mainwindow.h:125-135`）
   - 定义了11个静态常量，消除magic numbers
   - 包括定时器间隔、数据字段数、表格列索引等
   - 提高代码可读性和可维护性

2. **信号槽连接检查**（`mainwindow.cpp:85-103`）
   - 添加`connect()`返回值检查
   - 使用`Q_ASSERT`确保连接成功
   - 在开发阶段及时发现连接错误

3. **数据解析错误处理**（`mainwindow.cpp:419-486`）
   - 添加空数据检查
   - 验证字段数量与期望值匹配
   - 使用`toDouble(&ok)`检查每个数值转换
   - 记录详细错误日志
   - 更新IMU状态显示错误信息

4. **性能优化**
   - **容器预分配**（`mainwindow.cpp:203-205`）：使用`reserve()`预分配QVector容量
   - **时间显示优化**（`mainwindow.cpp:514-539`）：避免创建QTime对象，直接计算时分秒
   - **字符串优化**（`mainwindow.cpp:150-183`）：使用`QStringLiteral`优化常量字符串

5. **常量替代Magic Numbers**
   - 构造函数初始化：`m_xRange(kDefaultXRange)`, `m_xDot(kDefaultXDot)`
   - 定时器间隔：`start(kTimeDisplayInterval)`, `start(kDataTimerInterval)`
   - 表格列索引：`kDataTableMessageColumn`, `kDataTableValueColumn`, `kDataTableUnitColumn`
   - 数据验证：`kExpectedDataFields`

**📊 改进效果：**
- **安全性**：信号槽连接失败可在编译/运行时发现
- **错误处理**：数据解析错误易于调试和定位
- **性能**：减少内存重分配，每秒节约10次对象创建
- **可维护性**：常量命名清晰，代码可读性提升

**🔍 兼容性确认：**
- 所有修改完全兼容Qt 5.14
- 未使用任何Qt 6特性
- 使用C++11/17标准特性

**参考文件：**
- 修改的头文件：`app/ui/mainwindow.h`
- 修改的实现文件：`app/ui/mainwindow.cpp`

#### 2026-01-26: app.pro 构建配置优化

对 `app/app.pro` 进行了全面重构和规范化，提升可读性和可维护性。

**✅ 主要改进：**

1. **结构化注释**
   - 使用分隔线和标题组织各个配置段
   - 为每个库添加了用途说明注释
   - 添加文件头部说明和依赖信息

2. **版本信息**
   - 添加版本号定义：`VERSION = 1.0.0.0`
   - 定义版本宏：`APP_VERSION`
   - Windows版本信息：公司、产品、描述、版权

3. **平台特定配置**
   - **Windows**：应用程序类型、图标、版本信息
   - **macOS**：图标和Info.plist配置预留
   - **Linux**：配置段预留

4. **编译优化**
   - Release模式：O2优化，去除调试输出
   - Debug模式：W3警告级别
   - 预编译头文件支持（已注释，可选启用）

5. **部署和清理**
   - 添加安装目标配置
   - 添加清理规则

6. **代码组织**
   - 将库链接分行，每行一个库
   - 添加`$$PWD`到包含路径
   - 库按依赖顺序排列

**📁 文件结构：**
```
# ============================================================================
# GenericScope 主应用程序
# ============================================================================
├── 版本信息
├── 头文件
├── 源文件
├── UI 表单文件
├── 资源文件
├── 包含路径
├── 依赖库
├── 平台特定配置
├── 编译优化
├── 预编译头文件
├── 部署配置
└── 清理规则
```

**🔧 配置详情：**
- **依赖库**（7个）：algorithm, util, transfer, plot, log, record, core_plugins_manager
- **包含路径**（8个）：app目录及各依赖模块
- **Qt模块**（继承自global.pri）：core, gui, widgets, network, serialport
- **C++标准**（继承自global.pri）：C++17

**参考文件：**
- 修改的配置文件：`app/app.pro`
- 全局配置：`global.pri`

#### 2026-01-26: 指令设置功能实现

实现了16进制指令管理功能，用于设备控制指令的配置和管理。

**✅ 新增文件：**

1. **指令设置对话框头文件**（`app/ui/commandsettingsdialog.h`）
   - 定义 `CommandSettingsDialog` 类
   - 声明指令管理相关槽函数
   - 支持添加、删除、导入、生成指令

2. **指令设置对话框实现**（`app/ui/commandsettingsdialog.cpp`）
   - 左右分栏布局：指令列表 + 指令详情
   - 表格显示指令（序号、名称、内容）
   - 详情编辑区（名称、内容、描述）
   - 完整的样式表设计（绿色主题）

**✅ 修改文件：**

1. **主窗口头文件**（`app/ui/mainwindow.h:50,101`）
   - 添加 `onCommandSettingsTriggered()` 槽函数
   - 添加 `m_settingsMenu` 成员变量

2. **主窗口实现**（`app/ui/mainwindow.cpp`）
   - 包含 `commandsettingsdialog.h` 头文件
   - 初始化设置菜单 `m_settingsMenu`
   - 实现 `setupMenu()` 创建设置菜单项
   - 修改 `on_settingsButton_clicked()` 显示菜单
   - 实现 `onCommandSettingsTriggered()` 打开对话框

3. **构建配置**（`app/app.pro:12,19`）
   - 添加 `commandsettingsdialog.h` 到 HEADERS
   - 添加 `commandsettingsdialog.cpp` 到 SOURCES

**🎯 功能特性：**

- **设置菜单**：点击设置按钮弹出菜单，包含指令设置、设备配置、显示选项、关于
- **指令列表**：表格显示所有指令，支持选择查看详情
- **指令编辑**：右侧编辑区支持名称、内容(HEX)、描述输入
- **操作按钮**：添加、删除、导入、清空、生成
- **对话框按钮**：确定、取消、应用
- **样式设计**：统一的绿色主题，圆角边框，悬停效果

**📋 待实现功能：**

- 指令导入/导出（JSON/TXT格式）
- 指令自动生成逻辑
- 指令发送测试功能
- 指令配置持久化（保存到配置文件）
- 16进制格式验证
- 指令模板管理

**🔍 设计亮点：**

- 左右分栏布局，清晰直观
- 表格与详情联动，选择即显示
- 完整的错误提示和确认对话框
- 统一的UI风格和配色方案
- 可扩展的架构设计

**参考文件：**
- 新增头文件：`app/ui/commandsettingsdialog.h`
- 新增实现文件：`app/ui/commandsettingsdialog.cpp`
- 修改的头文件：`app/ui/mainwindow.h`
- 修改的实现文件：`app/ui/mainwindow.cpp`
- 修改的配置文件：`app/app.pro`

### 编译器设置
- 需要 C++17 标准
- MSVC 使用 UTF-8 编码（`/utf-8` 标志）
- 需要 Qt widgets, network, serialport 模块

### 单实例
应用程序使用 QSharedMemory 防止多个实例同时运行（参见 `app/main.cpp`）。

### 样式
- 支持亮色/暗色主题切换
- 亮色主题：`:/qss/light.qss`
- 暗色主题：`:/qss/dark.qss`
- 内置样式表作为后备方案
- 启用高 DPI 缩放

## 使用指南

### 启动应用程序

1. **运行程序**
   ```bash
   cd Bin/x64/Release
   GenericScope.exe
   ```

2. **连接设备**
   - 选择 COM 端口（自动检测可用端口）
   - 选择波特率（默认 115200）
   - 点击 "Connect" 按钮
   - 观察状态栏确认连接成功

3. **查看数据**
   - 左侧表格：传感器原始数据
   - 中间区域：姿态角度（Roll/Pitch/Yaw）
   - 右侧图表：实时曲线（默认显示 Roll）

4. **录制数据**
   - 勾选 "RecordLog" 复选框
   - 数据自动保存到 `data/imu_data_yyyyMMdd_hhmmss.csv`
   - 取消勾选停止录制

5. **调整图表**
   - X_Range：设置时间轴范围（秒）
   - X_Dot：设置点密度
   - 点击齿轮按钮打开高级设置（待实现）

6. **切换主题**
   - 点击右上角月亮按钮（🌙）
   - 在亮色/暗色主题之间切换

7. **指令设置**
   - 点击右上角设置按钮（⚙）
   - 选择"指令设置"菜单项
   - 添加、编辑、删除16进制指令
   - 支持从外部文件导入指令

### 数据协议

设备应发送 CSV 格式的数据包，每行包含 13 个字段：

```
ROLL,PITCH,YAW,AX,AY,AZ,GX,GY,GZ,MX,MY,MZ,TEMP
```

**字段说明：**
- ROLL：横滚角（度）
- PITCH：俯仰角（度）
- YAW：偏航角（度）
- AX/AY/AZ：加速度计 X/Y/Z 轴（m/s²）
- GX/GY/GZ：陀螺仪 X/Y/Z 轴（°/s）
- MX/MY/MZ：磁力计 X/Y/Z 轴（μT）
- TEMP：温度（°C）

**示例数据：**
```
0.12,1.45,89.23,0.05,0.12,9.81,0.01,0.02,0.00,24.5,10.3,8.7,25.3
```

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
- [x] 主应用程序框架
  - [x] 主界面设计（三栏布局）
  - [x] 设备控制界面
  - [x] 数据表格显示
  - [x] 实时图表绘制
  - [x] 主题切换功能
  - [ ] 3D 可视化实现
- [ ] 基础模块实现（algorithm, util, transfer）
- [ ] 插件系统实现

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
