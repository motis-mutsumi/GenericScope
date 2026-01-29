# GenericScope 插件开发教程

**版本**: 1.0.0
**日期**: 2026-01-29
**作者**: GenericScope Team

---

## 📋 目录

1. [插件系统概述](#插件系统概述)
2. [开发环境准备](#开发环境准备)
3. [插件开发步骤](#插件开发步骤)
4. [完整示例](#完整示例)
5. [调试与测试](#调试与测试)
6. [常见问题](#常见问题)

---

## 插件系统概述

### 系统架构

GenericScope采用基于Qt插件系统的动态插件架构：

```
┌─────────────────────────────────────────────┐
│           GenericScope 主程序                │
│  ┌───────────────────────────────────────┐  │
│  │     Core_PluginsManager (单例)        │  │
│  │  ┌─────────────────────────────────┐  │  │
│  │  │  插件加载、消息路由、生命周期  │  │  │
│  │  └─────────────────────────────────┘  │  │
│  └───────────────────────────────────────┘  │
│              ↓     ↓     ↓                   │
│     ┌────────┬─────────┬────────┐            │
│     │ 插件1  │ 插件2   │ 插件3  │            │
│     └────────┴─────────┴────────┘            │
└─────────────────────────────────────────────┘
```

### 核心组件

1. **CorePluginsBase** - 插件基类接口
   - 所有插件必须继承此类
   - 定义插件标准接口

2. **Core_PluginsManager** - 插件管理器（单例）
   - 加载/卸载插件
   - 插件消息路由
   - 插件生命周期管理

3. **CorePluginMetaData** - 消息元数据
   - 插件间消息传递
   - 支持6种消息类型：Command/Response/Data/Event/Config/Alert

---

## 开发环境准备

### 必需工具

- **Qt 5.14** 及以上版本
- **MSVC 2019** 或更高版本编译器
- **qmake** 构建工具
- **jom** 并行构建工具（可选）

### 项目结构

```
GenericScope/
├── core_plugin/
│   ├── core_plugins_manager/      # 插件管理器
│   │   ├── core_pluginsbase.h     # 插件基类
│   │   ├── core_pluginsmanager.h  # 插件管理器
│   │   └── core_pluginmetadata.h  # 消息元数据
│   └── core_plugins/               # 插件实现目录
│       ├── datamonitor/            # 示例：数据监控插件
│       └── devicecontrol/          # 示例：设备控制插件
└── Bin/x64/Release/core_plugins/   # 插件输出目录
```

---

## 插件开发步骤

### Step 1: 创建插件目录

在`core_plugin/core_plugins/`下创建插件目录：

```bash
mkdir core_plugin/core_plugins/myplugin
```

### Step 2: 创建插件头文件

**mypluginplugin.h**:

```cpp
#ifndef MYPLUGINPLUGIN_H
#define MYPLUGINPLUGIN_H

#include "core_pluginsbase.h"
#include <QObject>

class MyPluginWidget;

class MyPluginPlugin : public QObject, public CorePluginsBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.genericscope.plugin.myplugin" FILE "metadata.json")
    Q_INTERFACES(CorePluginsBase)

public:
    explicit MyPluginPlugin(QObject *parent = nullptr);
    ~MyPluginPlugin() override;

    // CorePluginsBase接口实现
    QString pluginName() const override;
    QString pluginVersion() const override;
    QString pluginDescription() const override;
    QWidget* pluginWidget() override;
    bool initialize() override;
    void uninitialize() override;
    void handleManagerMessage(const CorePluginMetaData &data) override;

signals:
    /**
     * @brief 向管理器发送消息
     * @param data 消息数据
     */
    void sendMessageToManager(const CorePluginMetaData &data);

    /**
     * @brief 向主窗口发送消息
     * @param data 消息数据
     */
    void sendMessageToMain(const CorePluginMetaData &data);

private:
    MyPluginWidget *m_widget;
};

#endif // MYPLUGINPLUGIN_H
```

**关键要点**:
- 多重继承：`QObject` + `CorePluginsBase`
- `Q_PLUGIN_METADATA`宏指定IID和元数据文件
- `Q_INTERFACES`宏声明接口
- 实现所有纯虚函数
- **必须声明 signals**：`sendMessageToManager` 和 `sendMessageToMain`（因为 CorePluginsBase 是纯接口，不包含 signals）

### Step 3: 实现插件类

**mypluginplugin.cpp**:

```cpp
#include "mypluginplugin.h"
#include "mypluginwidget.h"
#include <QDebug>

MyPluginPlugin::MyPluginPlugin(QObject *parent)
    : QObject(parent)
    , m_widget(nullptr)
{
}

MyPluginPlugin::~MyPluginPlugin()
{
    if (m_widget) {
        delete m_widget;
        m_widget = nullptr;
    }
}

QString MyPluginPlugin::pluginName() const
{
    return "我的插件";
}

QString MyPluginPlugin::pluginVersion() const
{
    return "1.0.0";
}

QString MyPluginPlugin::pluginDescription() const
{
    return "这是一个示例插件";
}

QWidget* MyPluginPlugin::pluginWidget()
{
    if (!m_widget) {
        m_widget = new MyPluginWidget();
        // 连接信号槽
        connect(m_widget, &MyPluginWidget::someSignal,
                this, &MyPluginPlugin::onSomeSlot);
    }
    return m_widget;
}

bool MyPluginPlugin::initialize()
{
    qDebug() << "MyPlugin initialized";
    return true;
}

void MyPluginPlugin::uninitialize()
{
    qDebug() << "MyPlugin uninitialized";
}

void MyPluginPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    // 处理来自主窗口或管理器的消息
    switch (data.type) {
    case CorePluginMetaData::Data:
        // 处理数据消息
        break;
    case CorePluginMetaData::Command:
        // 处理命令消息
        break;
    default:
        break;
    }
}
```

### Step 4: 创建插件UI

**mypluginwidget.h**:

```cpp
#ifndef MYPLUGINWIDGET_H
#define MYPLUGINWIDGET_H

#include <QWidget>

namespace Ui {
class MyPluginWidget;
}

class MyPluginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MyPluginWidget(QWidget *parent = nullptr);
    ~MyPluginWidget();

signals:
    void someSignal();

private:
    Ui::MyPluginWidget *ui;
};

#endif // MYPLUGINWIDGET_H
```

**mypluginwidget.ui**:

使用Qt Designer创建UI文件，或手动编写XML格式的UI文件。

### Step 5: 创建元数据文件

**metadata.json**:

```json
{
    "id": "com.genericscope.plugin.myplugin",
    "name": "我的插件",
    "version": "1.0.0",
    "author": "Your Name",
    "description": "插件功能描述",
    "dependencies": [],
    "category": "custom"
}
```

### Step 6: 创建构建文件

**myplugin.pro**:

```pro
TEMPLATE = lib
CONFIG += plugin
TARGET = myplugin

include(../../../global.pri)

# 输出目录
DESTDIR = $$PWD/../../../Bin/$$ARCH/$$BUILD_MODE/core_plugins

# 包含路径
INCLUDEPATH += $$PWD/../../core_plugins_manager

# 头文件
HEADERS += \
    mypluginplugin.h \
    mypluginwidget.h

# 源文件
SOURCES += \
    mypluginplugin.cpp \
    mypluginwidget.cpp

# UI文件
FORMS += \
    mypluginwidget.ui

# 元数据
OTHER_FILES += \
    metadata.json
```

### Step 7: 注册插件到构建系统

编辑`core_plugin/core_plugins/core_plugins.pro`，添加插件：

```pro
TEMPLATE = subdirs

SUBDIRS += \
    datamonitor \
    devicecontrol \
    myplugin      # 新增

# 如果有依赖关系
# myplugin.depends = datamonitor
```

---

## 完整示例

### 示例1: 数据监控插件

**功能**: 监控传感器数据，超过阈值时发出警报。

**文件位置**: `core_plugin/core_plugins/datamonitor/`

**核心代码**:

```cpp
void DataMonitorPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    if (data.type == CorePluginMetaData::Data) {
        QVariantMap fieldData = data.data.toMap();

        // 检查阈值
        for (auto it = fieldData.begin(); it != fieldData.end(); ++it) {
            QString fieldName = it.key();
            double value = it.value().toDouble();

            if (m_thresholds.contains(fieldName)) {
                if (qAbs(value) > m_thresholds[fieldName]) {
                    // 发送警报
                    CorePluginMetaData alertData;
                    alertData.type = CorePluginMetaData::Alert;
                    alertData.source = pluginName();
                    alertData.data = QVariantMap{
                        {"field", fieldName},
                        {"value", value}
                    };
                    emit sendMessageToMain(alertData);
                }
            }
        }
    }
}
```

### 示例2: 设备控制插件

**功能**: 发送控制指令到设备。

**文件位置**: `core_plugin/core_plugins/devicecontrol/`

**核心代码**:

```cpp
void DeviceControlWidget::onSendCustomClicked()
{
    QString hexString = ui->customCommandEdit->text().trimmed();
    hexString.remove(QRegExp("[^0-9A-Fa-f]"));

    QByteArray commandData = QByteArray::fromHex(hexString.toLatin1());
    emit sendCommand("自定义指令", commandData);
}
```

---

## 插件消息通信

### 消息类型

```cpp
enum MessageType {
    Command,    // 命令消息（如：发送指令）
    Response,   // 响应消息（如：设备响应）
    Data,       // 数据消息（如：传感器数据）
    Event,      // 事件消息（如：连接状态变化）
    Config,     // 配置消息（如：参数更新）
    Alert       // 警报消息（如：阈值超限）
};
```

### 发送消息到主窗口

```cpp
CorePluginMetaData msg;
msg.type = CorePluginMetaData::Command;
msg.source = pluginName();
msg.command = "MyCommand";
msg.data = QVariant::fromValue(myData);

emit sendMessageToMain(msg);
```

### 接收来自主窗口的消息

```cpp
void MyPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    if (data.type == CorePluginMetaData::Data) {
        // 处理数据
        QVariantMap dataMap = data.data.toMap();
        // ...
    }
}
```

---

## 调试与测试

### 编译插件

```bash
cd core_plugin/core_plugins/myplugin
qmake myplugin.pro
jom release
```

### 检查输出

插件DLL应输出到：`Bin/x64/Release/core_plugins/myplugin.dll`

### 调试技巧

1. **使用qDebug输出日志**:
   ```cpp
   qDebug() << "MyPlugin: Processing data:" << data.toString();
   ```

2. **检查插件加载**:
   ```cpp
   // 在initialize()中添加日志
   bool MyPlugin::initialize()
   {
       qDebug() << "MyPlugin initialized successfully";
       return true;
   }
   ```

3. **验证消息传递**:
   ```cpp
   void MyPlugin::handleManagerMessage(const CorePluginMetaData &data)
   {
       qDebug() << "Received message type:" << data.type;
       qDebug() << "Message source:" << data.source;
   }
   ```

---

## 常见问题

### Q1: 插件无法加载

**可能原因**:
- IID不匹配
- 元数据JSON格式错误
- 插件DLL未输出到正确目录

**解决方法**:
```cpp
// 确认IID与基类定义一致
#define CorePluginsBase_iid "com.genericscope.CorePluginsBase"

// 检查Q_PLUGIN_METADATA宏
Q_PLUGIN_METADATA(IID "com.genericscope.plugin.myplugin" FILE "metadata.json")

// 检查元数据JSON格式
{
    "id": "com.genericscope.plugin.myplugin",  // 必须与IID匹配
    "name": "我的插件"
}
```

### Q2: 插件UI不显示

**可能原因**:
- `pluginWidget()`返回nullptr
- UI文件未正确加载

**解决方法**:
```cpp
QWidget* MyPlugin::pluginWidget()
{
    if (!m_widget) {
        m_widget = new MyPluginWidget();  // 确保创建UI
    }
    return m_widget;  // 不要返回nullptr
}
```

### Q3: 消息无法接收

**可能原因**:
- 未正确实现`handleManagerMessage()`
- 消息类型判断错误

**解决方法**:
```cpp
void MyPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    // 添加日志调试
    qDebug() << "handleManagerMessage called, type:" << data.type;

    // 使用switch而非if-else
    switch (data.type) {
    case CorePluginMetaData::Data:
        // 处理数据
        break;
    default:
        qDebug() << "Unhandled message type:" << data.type;
        break;
    }
}
```

### Q4: 内存泄漏

**原因**: 插件对象未正确删除

**解决方法**:
```cpp
// 在析构函数中显式删除UI对象
MyPlugin::~MyPlugin()
{
    if (m_widget) {
        delete m_widget;
        m_widget = nullptr;
    }
}
```

---

## 最佳实践

### 1. 命名规范

- 插件类名：`<PluginName>Plugin`
- UI类名：`<PluginName>Widget`
- 插件ID：`com.genericscope.plugin.<pluginname>`

### 2. 线程安全

插件消息处理在主线程，避免阻塞操作：

```cpp
void MyPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    // 使用异步处理耗时操作
    QtConcurrent::run([this, data]() {
        // 耗时操作
        processHeavyData(data);
    });
}
```

### 3. 错误处理

```cpp
bool MyPlugin::initialize()
{
    try {
        // 初始化逻辑
        setupResources();
        return true;
    } catch (const std::exception &e) {
        qCritical() << "MyPlugin initialization failed:" << e.what();
        return false;
    }
}
```

### 4. 资源管理

使用Qt父子对象关系自动管理内存：

```cpp
m_widget = new MyPluginWidget();  // 无需手动delete
m_widget->setParent(this);        // 设置父对象
```

---

## 参考资料

### Qt官方文档
- [How to Create Qt Plugins](https://doc.qt.io/qt-5/plugins-howto.html)
- [Qt Plugin Metadata](https://doc.qt.io/qt-5/qtplugin.html)
- [QPluginLoader Class](https://doc.qt.io/qt-5/qpluginloader.html)

### GenericScope文档
- [CLAUDE.md](../../CLAUDE.md) - 项目架构说明
- [插件管理器源码](../core_plugins_manager/core_pluginsmanager.h)
- [插件基类源码](../core_plugins_manager/core_pluginsbase.h)

---

**最后更新**: 2026-01-29
**文档版本**: 1.0.0
