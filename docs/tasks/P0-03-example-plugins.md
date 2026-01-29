# P0-03: 示例插件开发

**优先级**: P0（高优先级）
**预计工作量**: 1-2天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

开发至少2个示例插件，展示GenericScope插件系统的能力，为第三方开发者提供参考。

### 当前状态
- ✅ 插件系统框架完整（Core_PluginsManager）
- ✅ 插件基类定义（CorePluginsBase）
- ❌ **缺少具体插件实例**

### 任务目标
1. 开发"数据监控插件" - 实时监控传感器数据
2. 开发"设备控制插件" - 发送控制指令
3. 编写插件开发教程文档

---

## 🎯 验收标准

### 功能验收
- [ ] 至少2个插件正常加载
- [ ] 插件能接收主窗口数据
- [ ] 插件能发送消息到主窗口
- [ ] 插件UI正常显示
- [ ] 插件间能通过管理器通信

### 代码质量
- [ ] 符合Qt插件规范
- [ ] JSON元数据完整
- [ ] 代码注释详细
- [ ] 编译无警告

### 文档要求
- [ ] 插件开发教程完整
- [ ] 包含完整示例代码
- [ ] 说明插件加载流程

---

## 🔧 技术方案

### 插件1: 数据监控插件（DataMonitorPlugin）

#### 功能描述
实时监控IMU传感器数据，当数值超过阈值时发出警报。

#### 文件结构
```
core_plugin/core_plugins/datamonitor/
├── datamonitor.pro                 # 构建配置
├── datamonitorplugin.h             # 插件头文件
├── datamonitorplugin.cpp           # 插件实现
├── datamonitorwidget.h             # UI组件
├── datamonitorwidget.cpp           # UI实现
├── datamonitorwidget.ui            # UI布局
└── metadata.json                   # 插件元数据
```

#### 实现代码

**datamonitorplugin.h**:
```cpp
#ifndef DATAMONITORPLUGIN_H
#define DATAMONITORPLUGIN_H

#include "core_pluginsbase.h"
#include <QObject>

class DataMonitorWidget;

class DataMonitorPlugin : public QObject, public CorePluginsBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.genericscope.plugin.datamonitor" FILE "metadata.json")
    Q_INTERFACES(CorePluginsBase)

public:
    explicit DataMonitorPlugin(QObject *parent = nullptr);
    ~DataMonitorPlugin() override;

    // CorePluginsBase接口实现
    QString pluginName() const override;
    QString pluginVersion() const override;
    QString pluginDescription() const override;
    QWidget* pluginWidget() override;

    void handleManagerMessage(const CorePluginMetaData &data) override;

private slots:
    void onThresholdExceeded(const QString &fieldName, double value);

private:
    DataMonitorWidget *m_widget;

    // 阈值配置
    QMap<QString, double> m_thresholds;
};

#endif // DATAMONITORPLUGIN_H
```

**datamonitorplugin.cpp**:
```cpp
#include "datamonitorplugin.h"
#include "datamonitorwidget.h"
#include <QDebug>

DataMonitorPlugin::DataMonitorPlugin(QObject *parent)
    : QObject(parent)
    , m_widget(nullptr)
{
    // 设置默认阈值
    m_thresholds["Roll"] = 45.0;      // 横滚角超过45度报警
    m_thresholds["Pitch"] = 45.0;     // 俯仰角超过45度报警
    m_thresholds["Temperature"] = 60.0; // 温度超过60度报警
}

DataMonitorPlugin::~DataMonitorPlugin()
{
    if (m_widget) {
        delete m_widget;
    }
}

QString DataMonitorPlugin::pluginName() const
{
    return "数据监控插件";
}

QString DataMonitorPlugin::pluginVersion() const
{
    return "1.0.0";
}

QString DataMonitorPlugin::pluginDescription() const
{
    return "实时监控传感器数据，超过阈值时发出警报";
}

QWidget* DataMonitorPlugin::pluginWidget()
{
    if (!m_widget) {
        m_widget = new DataMonitorWidget();
        m_widget->setThresholds(m_thresholds);

        connect(m_widget, &DataMonitorWidget::thresholdExceeded,
                this, &DataMonitorPlugin::onThresholdExceeded);
    }
    return m_widget;
}

void DataMonitorPlugin::handleManagerMessage(const CorePluginMetaData &data)
{
    if (data.type == MessageType::Data) {
        // 接收来自主窗口的数据
        QVariantMap fieldData = data.data.toMap();

        // 更新UI显示
        if (m_widget) {
            m_widget->updateData(fieldData);
        }

        // 检查阈值
        for (auto it = fieldData.begin(); it != fieldData.end(); ++it) {
            QString fieldName = it.key();
            double value = it.value().toDouble();

            if (m_thresholds.contains(fieldName)) {
                if (qAbs(value) > m_thresholds[fieldName]) {
                    onThresholdExceeded(fieldName, value);
                }
            }
        }
    }
}

void DataMonitorPlugin::onThresholdExceeded(const QString &fieldName, double value)
{
    qWarning() << "DataMonitor: Threshold exceeded!" << fieldName << "=" << value;

    // 发送警报消息到主窗口
    CorePluginMetaData alertData;
    alertData.type = MessageType::Alert;
    alertData.data = QVariantMap{
        {"field", fieldName},
        {"value", value},
        {"threshold", m_thresholds[fieldName]},
        {"message", QString("%1超过阈值: %2 > %3")
                        .arg(fieldName)
                        .arg(value)
                        .arg(m_thresholds[fieldName])}
    };

    sendMessageToMain(alertData);
}
```

**datamonitorwidget.ui** (关键部分):
```xml
<ui version="4.0">
 <class>DataMonitorWidget</class>
 <widget class="QWidget" name="DataMonitorWidget">
  <layout class="QVBoxLayout">
   <!-- 标题 -->
   <widget class="QLabel" name="titleLabel">
    <property name="text">
     <string>数据监控</string>
    </property>
   </widget>

   <!-- 阈值设置表格 -->
   <widget class="QTableWidget" name="thresholdTable">
    <column>
     <property name="text"><string>字段名</string></property>
    </column>
    <column>
     <property name="text"><string>当前值</string></property>
    </column>
    <column>
     <property name="text"><string>阈值</string></property>
    </column>
    <column>
     <property name="text"><string>状态</string></property>
    </column>
   </widget>

   <!-- 警报日志 -->
   <widget class="QTextEdit" name="alertLogEdit">
    <property name="readOnly"><bool>true</bool></property>
   </widget>
  </layout>
 </widget>
</ui>
```

**metadata.json**:
```json
{
    "id": "com.genericscope.plugin.datamonitor",
    "name": "数据监控插件",
    "version": "1.0.0",
    "author": "GenericScope Team",
    "description": "实时监控传感器数据，超过阈值时发出警报",
    "dependencies": [],
    "category": "monitoring"
}
```

**datamonitor.pro**:
```pro
TEMPLATE = lib
CONFIG += plugin
QT += core gui widgets

TARGET = datamonitor
DESTDIR = $$PWD/../../../Bin/x64/Release/core_plugins

# 包含路径
INCLUDEPATH += $$PWD/../../core_plugins_manager

# 头文件
HEADERS += \
    datamonitorplugin.h \
    datamonitorwidget.h

# 源文件
SOURCES += \
    datamonitorplugin.cpp \
    datamonitorwidget.cpp

# UI文件
FORMS += \
    datamonitorwidget.ui

# 元数据
OTHER_FILES += \
    metadata.json
```

---

### 插件2: 设备控制插件（DeviceControlPlugin）

#### 功能描述
发送控制指令到设备，例如校准命令、复位命令等。

#### 文件结构
```
core_plugin/core_plugins/devicecontrol/
├── devicecontrol.pro
├── devicecontrolplugin.h
├── devicecontrolplugin.cpp
├── devicecontrolwidget.h
├── devicecontrolwidget.cpp
├── devicecontrolwidget.ui
└── metadata.json
```

#### 实现代码

**devicecontrolplugin.h**:
```cpp
#ifndef DEVICECONTROLPLUGIN_H
#define DEVICECONTROLPLUGIN_H

#include "core_pluginsbase.h"
#include <QObject>

class DeviceControlWidget;

class DeviceControlPlugin : public QObject, public CorePluginsBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.genericscope.plugin.devicecontrol" FILE "metadata.json")
    Q_INTERFACES(CorePluginsBase)

public:
    explicit DeviceControlPlugin(QObject *parent = nullptr);
    ~DeviceControlPlugin() override;

    // CorePluginsBase接口实现
    QString pluginName() const override;
    QString pluginVersion() const override;
    QString pluginDescription() const override;
    QWidget* pluginWidget() override;

    void handleManagerMessage(const CorePluginMetaData &data) override;

private slots:
    void onSendCommand(const QString &commandName, const QByteArray &commandData);

private:
    DeviceControlWidget *m_widget;
};

#endif // DEVICECONTROLPLUGIN_H
```

**devicecontrolwidget.ui** (关键部分):
```xml
<ui version="4.0">
 <class>DeviceControlWidget</class>
 <widget class="QWidget" name="DeviceControlWidget">
  <layout class="QVBoxLayout">
   <!-- 预定义指令按钮组 -->
   <widget class="QGroupBox" name="commandsGroupBox">
    <property name="title"><string>快捷指令</string></property>
    <layout class="QGridLayout">
     <item row="0" column="0">
      <widget class="QPushButton" name="calibrateButton">
       <property name="text"><string>校准</string></property>
      </widget>
     </item>
     <item row="0" column="1">
      <widget class="QPushButton" name="resetButton">
       <property name="text"><string>复位</string></property>
      </widget>
     </item>
     <item row="1" column="0">
      <widget class="QPushButton" name="queryButton">
       <property name="text"><string>查询状态</string></property>
      </widget>
     </item>
     <item row="1" column="1">
      <widget class="QPushButton" name="startButton">
       <property name="text"><string>开始采集</string></property>
      </widget>
     </item>
    </layout>
   </widget>

   <!-- 自定义指令输入 -->
   <widget class="QGroupBox" name="customGroupBox">
    <property name="title"><string>自定义指令</string></property>
    <layout class="QHBoxLayout">
     <widget class="QLineEdit" name="customCommandEdit">
      <property name="placeholderText"><string>输入16进制指令</string></property>
     </widget>
     <widget class="QPushButton" name="sendCustomButton">
      <property name="text"><string>发送</string></property>
     </widget>
    </layout>
   </widget>

   <!-- 指令历史 -->
   <widget class="QTextEdit" name="historyEdit">
    <property name="readOnly"><bool>true</bool></property>
   </widget>
  </layout>
 </widget>
</ui>
```

**metadata.json**:
```json
{
    "id": "com.genericscope.plugin.devicecontrol",
    "name": "设备控制插件",
    "version": "1.0.0",
    "author": "GenericScope Team",
    "description": "向设备发送控制指令",
    "dependencies": [],
    "category": "control"
}
```

---

## 📝 实施步骤

### Step 1: 更新core_plugins.pro（15分钟）

```pro
TEMPLATE = subdirs

SUBDIRS += \
    datamonitor \       # 新增
    devicecontrol       # 新增

# 如果有依赖关系，在这里配置
# devicecontrol.depends = datamonitor
```

### Step 2: 创建数据监控插件（4小时）
1. 创建目录和文件
2. 实现插件类和UI
3. 编译测试
4. 集成到主窗口

### Step 3: 创建设备控制插件（4小时）
1. 创建目录和文件
2. 实现插件类和UI
3. 编译测试
4. 集成到主窗口

### Step 4: 主窗口集成（2小时）

在MainWindow中加载插件：

```cpp
void MainWindow::loadPlugins()
{
    Core_PluginsManager *manager = Core_PluginsManager::instance();

    // 加载所有插件
    manager->loadAllPlugins();

    // 获取插件列表
    QStringList pluginNames = manager->getPluginNames();

    for (const QString &name : pluginNames) {
        CorePluginsBase *plugin = manager->getPlugin(name);
        if (plugin) {
            // 获取插件UI
            QWidget *pluginWidget = plugin->pluginWidget();

            // 添加到插件面板（例如：TabWidget或DockWidget）
            ui->pluginTabWidget->addTab(pluginWidget, plugin->pluginName());
        }
    }
}

void MainWindow::onParseResultReady(const ParseResult &result)
{
    // ... 现有UI更新 ...

    // 发送数据到所有插件
    CorePluginMetaData data;
    data.type = MessageType::Data;
    data.data = QVariant::fromValue(result.fieldValues);

    Core_PluginsManager::instance()->broadcastMessage(data);
}
```

### Step 5: 编写插件开发教程（2小时）

创建 `docs/plugin-development-guide.md`

---

## 🧪 测试方案

### 测试用例1: 插件加载

**操作**: 启动应用程序

**预期结果**:
- 控制台输出插件加载信息
- 插件Tab正常显示
- 插件UI正常渲染

### 测试用例2: 数据监控

**操作**:
1. 连接设备
2. 发送Roll角度超过阈值的数据

**预期结果**:
- 数据监控插件显示当前值
- 超过阈值时显示警报
- 主窗口收到警报消息

### 测试用例3: 设备控制

**操作**:
1. 点击"校准"按钮
2. 输入自定义指令并发送

**预期结果**:
- 指令通过插件管理器发送到主窗口
- 主窗口将指令发送到串口
- 指令历史正确记录

---

## 📚 参考资料

### Qt插件系统
- [How to Create Qt Plugins](https://doc.qt.io/qt-5/plugins-howto.html)
- [Qt Plugin Metadata](https://doc.qt.io/qt-5/qtplugin.html)

### 相关代码
- `core_plugin/core_plugins_manager/core_pluginsbase.h`
- `core_plugin/core_plugins_manager/core_pluginsmanager.h`
- `app/ui/mainwindow.h/cpp`

---

## ⚠️ 注意事项

### 插件接口版本兼容
- 确保插件IID与基类一致
- JSON元数据格式正确

### 内存管理
- 插件对象由管理器管理
- UI对象由插件管理（使用QObject父子关系）

### 跨插件通信
- 通过管理器中转消息
- 避免插件间直接引用

---

## ✅ 完成检查清单

- [ ] 数据监控插件完成并测试
- [ ] 设备控制插件完成并测试
- [ ] 插件正常加载和卸载
- [ ] 插件间通信正常
- [ ] 插件开发教程完成
- [ ] 代码审查通过
- [ ] 文档更新完成

---

**创建日期**: 2026-01-29
**最后更新**: 2026-01-29
**预计完成**: 2026-01-31
