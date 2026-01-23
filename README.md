# GenericScope - 通用传感器数据采集上位机

## 项目简介

GenericScope 是一个基于 Qt 的通用传感器数据采集上位机框架，参考 MidScope 项目的成熟架构设计。它支持：

- **设备控制与监控**：通过串口、TCP、Modbus 等多种方式连接传感器设备
- **数据采集与分析**：实时数据处理、滤波、统计分析
- **数据可视化**：实时曲线图、直方图等多种图表
- **数据录制**：支持 CSV、JSON、Binary 多种格式
- **插件系统**：可扩展的插件架构，方便添加新功能

## 技术栈

- **Qt 5+**：跨平台 GUI 框架
- **C++17**：现代 C++ 标准
- **MSVC**：Windows 平台编译器

## 项目结构

```
GenericScope/
├── algorithm/              # 算法处理模块
│   ├── algorithmprocessor.h/cpp
│   └── algorithm.pro
├── util/                   # 工具类模块
│   ├── async_queue.h
│   ├── timehelper.h/cpp
│   └── util.pro
├── transfer/               # 传输层模块
│   ├── transferbasic.h/cpp
│   ├── controltransfer.h/cpp
│   ├── transfermanager.h/cpp
│   ├── uart/              # 串口传输
│   ├── tcp/               # TCP 传输
│   ├── modbus/            # Modbus 传输
│   └── transfer.pro
├── common_component/       # 通用组件
│   ├── plot/              # 绘图组件
│   ├── log/               # 日志组件
│   ├── record/            # 录制组件
│   └── common_component.pro
├── core_plugin/            # 插件系统
│   ├── core_plugins_manager/
│   ├── core_plugins/
│   └── core_plugin.pro
├── app/                    # 主应用程序
│   ├── main.cpp
│   ├── ui/                # 界面
│   ├── config/            # 配置管理
│   ├── device/            # 设备管理
│   └── app.pro
├── script/                 # 构建脚本
│   └── build.bat
├── Bin/                    # 输出目录
├── libs/                   # 第三方库
├── GenericScope.pro        # 主项目文件
├── global.pri              # 全局配置
├── CLAUDE.md               # 架构文档
└── README.md               # 本文件
```

## 构建说明

### 前置要求

1. **Qt 5+**
   - 下载并安装 Qt（推荐 Qt 5.15 或更高版本）
   - 确保 qmake 和 jom 在 PATH 中

2. **MSVC 编译器**
   - 安装 Visual Studio 2019 或更高版本
   - 运行 vcvarsall.bat 设置编译环境

### 构建步骤

1. 打开 Qt Command Prompt（或配置好 Qt 环境的命令行）

2. 运行 MSVC 环境设置：
   ```batch
   "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
   ```

3. 进入项目目录并构建：
   ```batch
   cd GenericScope\script
   build.bat
   ```

4. 构建成功后，可执行文件位于：
   ```
   Bin\x64\Release\GenericScope.exe
   ```

### 手动构建

如果需要手动构建：

```batch
cd GenericScope
qmake GenericScope.pro -spec win32-msvc
jom qmake_all
jom.exe -f Makefile.Release
```

## 使用说明

### 基本使用

1. **启动程序**
   - 运行 `GenericScope.exe`
   - 首次运行会创建默认配置文件 `Settings.ini`

2. **连接设备**
   - 点击 "Connect" 按钮连接设备
   - 默认使用串口 COM1，波特率 115200
   - 可在设置中修改连接参数

3. **数据采集**
   - 点击 "Start Polling" 开始自动轮询数据
   - 实时曲线图显示数据变化
   - 直方图显示数据分布

4. **数据录制**
   - 点击 "Start Record" 开始录制数据
   - 数据保存在 `data/` 目录下
   - 支持 CSV 格式，可用 Excel 打开

### 配置文件

配置文件 `Settings.ini` 包含以下配置项：

```ini
[Device]
type=UART
port=COM1
baudRate=115200
dataBits=8
stopBits=1
parity=None
autoPolling=false
pollingInterval=100

[Display]
maxDataPoints=1000
autoScale=true
refreshRate=50

[AlgorithmParams]
filterType=0
filterKernelSize=3
smoothWindowSize=5
peakThreshold=0.5
```

## 开发指南

### 添加新的传输协议

1. 在 `transfer/` 目录创建新的传输类
2. 继承 `TransferBasic` 或 `ControlTransfer`
3. 实现必需的虚函数
4. 在 `TransferManager` 中注册

示例：
```cpp
class MyTransfer : public ControlTransfer {
    // 实现接口
};
```

### 添加新的插件

1. 在 `core_plugin/core_plugins/` 创建插件目录
2. 继承 `CorePluginsBase`
3. 实现插件接口
4. 创建 JSON 元数据文件
5. 添加到 `core_plugins.pro`

### 数据处理

使用 `AlgorithmProcessor` 进行数据处理：

```cpp
// 滤波
QVector<double> filtered = AlgorithmProcessor::filterData(data, 0, 3);

// 统计
double mean, stdDev, min, max;
AlgorithmProcessor::calculateStatistics(data, mean, stdDev, min, max);

// 峰值检测
QVector<int> peaks = AlgorithmProcessor::detectPeaks(data, threshold);
```

### 异步处理

使用 `AsyncQueue` 进行异步数据处理：

```cpp
AsyncQueue<DataPacket> queue(4, 100);  // 4个线程，容量100

queue.setProcessor([](const DataPacket &packet) {
    // 处理数据
});

queue.start();
queue.push(packet);
```

## 常见问题

### Q: 编译时找不到 Qt 头文件
A: 确保在 Qt Command Prompt 中运行构建脚本，或手动设置 Qt 环境变量。

### Q: 串口连接失败
A: 检查串口号是否正确，设备是否已连接，是否被其他程序占用。

### Q: 数据显示不正常
A: 检查数据解析逻辑是否正确，确认数据格式与设备协议一致。

### Q: 如何添加新的图表类型
A: 继承 `BasicPlot` 类，实现 `drawPlot()` 方法。

## 许可证

待定

## 贡献

欢迎提交 Issue 和 Pull Request。

## 联系方式

待定

## 更新日志

### v1.0.0 (2026-01-23)
- 初始版本
- 基础架构搭建
- 串口通信支持
- 实时数据可视化
- 数据录制功能
- 插件系统框架
