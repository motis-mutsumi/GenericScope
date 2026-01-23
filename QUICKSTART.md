# GenericScope 快速开始指南

## 1. 环境准备

### 安装 Qt

1. 下载 Qt 在线安装器：https://www.qt.io/download-qt-installer
2. 安装 Qt 5.15 或更高版本
3. 选择组件：
   - Qt 5.15.x
   - MSVC 2019 64-bit
   - Qt Creator（可选）

### 安装 Visual Studio

1. 下载 Visual Studio 2019 或 2022
2. 安装时选择"使用 C++ 的桌面开发"工作负载

## 2. 构建项目

### 方法一：使用构建脚本（推荐）

1. 打开"开始菜单" -> "Qt" -> "Qt 5.15.x for Desktop (MSVC 2019 64-bit)"
2. 在 Qt 命令提示符中运行：
   ```batch
   cd D:\app\App_MidScope\GenericScope\script
   build.bat
   ```

### 方法二：使用 Qt Creator

1. 打开 Qt Creator
2. 文件 -> 打开文件或项目
3. 选择 `GenericScope.pro`
4. 配置项目（选择 MSVC 2019 64-bit 套件）
5. 点击"构建"按钮

### 方法三：手动构建

```batch
REM 1. 设置 MSVC 环境
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM 2. 设置 Qt 环境
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%

REM 3. 构建项目
cd D:\app\App_MidScope\GenericScope
qmake GenericScope.pro -spec win32-msvc
jom qmake_all
jom.exe -f Makefile.Release
```

## 3. 运行程序

构建成功后，可执行文件位于：
```
D:\app\App_MidScope\GenericScope\Bin\x64\Release\GenericScope.exe
```

双击运行即可。

## 4. 基本配置

### 配置串口设备

1. 打开 `Settings.ini` 文件（首次运行后自动生成）
2. 修改设备配置：
   ```ini
   [Device]
   type=UART
   port=COM3          # 修改为你的串口号
   baudRate=115200    # 修改为你的波特率
   dataBits=8
   stopBits=1
   parity=None
   ```

### 查看可用串口

在程序中可以通过以下代码查看可用串口：
```cpp
QStringList ports = UartTransfer::availablePorts();
```

## 5. 测试数据采集

### 使用虚拟串口测试

如果没有实际设备，可以使用虚拟串口工具测试：

1. 下载虚拟串口工具（如 com0com）
2. 创建一对虚拟串口（如 COM10 和 COM11）
3. 在 GenericScope 中连接 COM10
4. 使用串口调试助手向 COM11 发送数据

### 数据格式

默认情况下，程序期望接收文本格式的数值数据，例如：
```
25.6\r\n
26.1\r\n
25.9\r\n
```

如果你的设备使用不同的数据格式，需要修改 `MainWindow::processData()` 函数。

## 6. 自定义数据解析

编辑 `app/ui/mainwindow.cpp` 中的 `processData()` 函数：

```cpp
void MainWindow::processData(const QByteArray &data)
{
    // 示例：解析 JSON 格式数据
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        double value = obj["value"].toDouble();
        double time = obj["time"].toDouble();

        m_linePlot->addDataPoint(time, value);
    }

    // 示例：解析二进制数据
    if (data.size() >= 4) {
        float value;
        memcpy(&value, data.constData(), sizeof(float));
        m_linePlot->addDataPoint(m_dataCounter * 0.1, value);
    }
}
```

## 7. 开发自己的插件

### 创建插件项目

1. 在 `core_plugin/core_plugins/` 创建新目录，如 `my_plugin`
2. 创建 `my_plugin.pro`：
   ```qmake
   TEMPLATE = lib
   CONFIG += plugin
   TARGET = my_plugin

   include(../../global.pri)

   HEADERS += myplugin.h
   SOURCES += myplugin.cpp

   INCLUDEPATH += ../core_plugins_manager
   ```

3. 创建插件类 `myplugin.h`：
   ```cpp
   #include "core_pluginsbase.h"

   class MyPlugin : public QObject, public CorePluginsBase
   {
       Q_OBJECT
       Q_PLUGIN_METADATA(IID "com.genericscope.MyPlugin" FILE "myplugin.json")
       Q_INTERFACES(CorePluginsBase)

   public:
       QString pluginName() const override { return "MyPlugin"; }
       QString pluginVersion() const override { return "1.0.0"; }
       QString pluginDescription() const override { return "My custom plugin"; }
       QWidget* pluginWidget() override { return nullptr; }
       bool initialize() override { return true; }
       void uninitialize() override {}
       void handleManagerMessage(const CorePluginMetaData &data) override {}
   };
   ```

4. 创建 `myplugin.json`：
   ```json
   {
       "name": "MyPlugin",
       "version": "1.0.0",
       "description": "My custom plugin"
   }
   ```

5. 将插件添加到 `core_plugins.pro` 的 SUBDIRS

## 8. 常用功能示例

### 数据滤波

```cpp
#include "algorithm/algorithmprocessor.h"

QVector<double> rawData = {1.0, 2.5, 1.8, 3.2, 2.1};
QVector<double> filtered = AlgorithmProcessor::filterData(rawData, 0, 3);
```

### 数据录制

```cpp
#include "common_component/record/datarecorder.h"

DataRecorder recorder;
recorder.startRecording("data.csv", DataRecorder::CSV);

QVariantMap data;
data["temperature"] = 25.6;
data["humidity"] = 60.5;
recorder.recordData(data);

recorder.stopRecording();
```

### 日志记录

```cpp
#include "common_component/log/logmanager.h"

LOG_INFO("Application started");
LOG_WARNING("Temperature too high");
LOG_ERROR("Connection failed");
```

## 9. 故障排除

### 问题：找不到 qmake
**解决**：确保在 Qt Command Prompt 中运行，或将 Qt bin 目录添加到 PATH

### 问题：找不到 MSVC 编译器
**解决**：运行 vcvarsall.bat 设置 MSVC 环境

### 问题：串口打开失败
**解决**：
- 检查串口号是否正确
- 确认设备已连接
- 检查是否被其他程序占用
- 尝试以管理员权限运行

### 问题：程序崩溃
**解决**：
- 检查日志文件 `logs/log_*.txt`
- 使用 Debug 版本调试
- 检查数据解析逻辑

## 10. 下一步

- 阅读 `CLAUDE.md` 了解详细架构
- 查看 `README.md` 了解完整功能
- 开发自定义插件扩展功能
- 根据实际设备修改数据解析逻辑

## 需要帮助？

- 查看项目文档
- 提交 Issue
- 联系开发团队
