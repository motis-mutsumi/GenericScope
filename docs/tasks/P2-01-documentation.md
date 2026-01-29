# P2-01: 文档完善

**优先级**: P2（低优先级）
**预计工作量**: 2-3天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

完善项目文档，包括API文档、用户手册、插件开发教程等。

### 当前状态
- ✅ CLAUDE.md项目指南完整
- ✅ 协议配置系统规范文档完成
- ✅ AI协议生成用户指南完成
- ⚠️ **缺少API文档**
- ⚠️ **缺少用户手册**
- ⚠️ **缺少插件开发教程**

### 任务目标
1. 使用Doxygen生成API文档
2. 编写用户手册
3. 编写插件开发教程
4. 更新README.md
5. 添加架构图和流程图

---

## 🎯 验收标准

### API文档
- [ ] 使用Doxygen生成HTML文档
- [ ] 所有公共类有详细说明
- [ ] 所有公共方法有参数和返回值说明
- [ ] 包含使用示例

### 用户手册
- [ ] 安装和配置说明
- [ ] 功能使用指南（含截图）
- [ ] 常见问题FAQ
- [ ] 故障排除指南

### 插件开发教程
- [ ] 插件系统架构说明
- [ ] 完整的开发示例
- [ ] 调试和测试指南
- [ ] 最佳实践

---

## 📝 文档结构

### 1. API文档（使用Doxygen）

#### 1.1 Doxyfile配置

```bash
# 创建Doxyfile
doxygen -g Doxyfile

# 关键配置项
PROJECT_NAME           = "GenericScope"
PROJECT_NUMBER         = 1.0.0
OUTPUT_DIRECTORY       = docs/api
INPUT                  = algorithm util protocol transfer common_component core_plugin app
RECURSIVE              = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = NO
EXTRACT_STATIC         = YES
GENERATE_HTML          = YES
GENERATE_LATEX         = NO
```

#### 1.2 代码注释示例

```cpp
/**
 * @brief 协议解析器类
 *
 * ProtocolParser用于根据ProtocolConfig动态解析二进制数据帧。
 * 支持多种校验方式和数据类型。
 *
 * @code
 * // 创建解析器
 * ProtocolManager *manager = ProtocolManager::instance();
 * QSharedPointer<ProtocolParser> parser = manager->createParser("IMU_Protocol");
 *
 * // 解析数据
 * QByteArray data = receiveData();
 * ParseResult result = parser->parse(data);
 *
 * if (result.success) {
 *     qDebug() << "Roll:" << result.fieldValues["Roll"].toDouble();
 * }
 * @endcode
 *
 * @see ProtocolConfig
 * @see ProtocolManager
 */
class ProtocolParser {
    // ...
};
```

### 2. 用户手册

#### 目录结构

```
docs/user-manual/
├── 00-overview.md              # 概览
├── 01-installation.md          # 安装和配置
├── 02-quick-start.md           # 快速开始
├── 03-device-connection.md     # 设备连接
├── 04-protocol-config.md       # 协议配置
├── 05-ai-generation.md         # AI协议生成
├── 06-data-visualization.md    # 数据可视化
├── 07-data-recording.md        # 数据录制
├── 08-plugins.md               # 插件使用
├── 09-faq.md                   # 常见问题
└── 10-troubleshooting.md       # 故障排除
```

#### 示例章节（03-device-connection.md）

```markdown
# 设备连接

本章介绍如何连接和配置设备。

## 串口连接

### 1. 选择COM口

1. 打开主界面
2. 在顶部工具栏找到COM口下拉框
3. 选择设备对应的串口（例如：COM7）

![COM口选择](images/com-selection.png)

### 2. 设置波特率

1. 点击波特率下拉框
2. 选择与设备匹配的波特率（常用：115200）

> **提示**：请查阅设备手册确认正确的波特率

### 3. 连接设备

点击"Connect"按钮，状态栏显示"已连接"表示成功。

### 常见问题

**Q: 为什么找不到COM口？**

A: 可能的原因：
- 设备未连接或驱动未安装
- 设备被其他程序占用
- 刷新COM口列表

**Q: 连接后无数据？**

A: 请检查：
- 波特率是否正确
- 设备是否已启动
- 协议配置是否正确
```

### 3. 插件开发教程

#### 目录结构

```
docs/plugin-development-guide.md
├── 第1章：插件系统概述
├── 第2章：开发环境准备
├── 第3章：创建第一个插件
├── 第4章：插件间通信
├── 第5章：UI设计
├── 第6章：调试和测试
├── 第7章：最佳实践
└── 第8章：示例插件分析
```

#### 示例章节（第3章）

```markdown
# 第3章：创建第一个插件

## 3.1 插件目录结构

```
core_plugin/core_plugins/hello/
├── hello.pro               # qmake构建配置
├── helloplugin.h          # 插件头文件
├── helloplugin.cpp        # 插件实现
├── hellowidget.h          # UI组件头文件
├── hellowidget.cpp        # UI实现
├── hellowidget.ui         # Qt Designer界面
└── metadata.json          # 插件元数据
```

## 3.2 实现步骤

### Step 1: 创建插件类

```cpp
// helloplugin.h
#ifndef HELLOPLUGIN_H
#define HELLOPLUGIN_H

#include "core_pluginsbase.h"

class HelloPlugin : public QObject, public CorePluginsBase
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.genericscope.plugin.hello" FILE "metadata.json")
    Q_INTERFACES(CorePluginsBase)

public:
    explicit HelloPlugin(QObject *parent = nullptr);

    QString pluginName() const override;
    QWidget* pluginWidget() override;
    void handleManagerMessage(const CorePluginMetaData &data) override;
};

#endif
```

### Step 2: 实现插件方法

```cpp
// helloplugin.cpp
QString HelloPlugin::pluginName() const {
    return "Hello插件";
}

void HelloPlugin::handleManagerMessage(const CorePluginMetaData &data) {
    qDebug() << "Received message:" << data.type;
}
```

### Step 3: 编写metadata.json

```json
{
    "id": "com.genericscope.plugin.hello",
    "name": "Hello插件",
    "version": "1.0.0",
    "author": "Your Name",
    "description": "我的第一个插件"
}
```

### Step 4: 配置hello.pro

```pro
TEMPLATE = lib
CONFIG += plugin
TARGET = hello
DESTDIR = $$PWD/../../../Bin/x64/Release/core_plugins
```

### Step 5: 编译和测试

```bash
cd core_plugin/core_plugins/hello
qmake hello.pro
jom
```

重启GenericScope，插件自动加载。
```

### 4. README.md更新

```markdown
# GenericScope

[![Build Status](badge)](link)
[![License](badge)](link)
[![Version](badge)](link)

GenericScope是一个基于Qt的通用上位机框架，专注于设备控制、数据采集和可视化。

## ✨ 核心特性

- 🔧 **协议配置系统** - 动态协议解析，支持10种数据类型和6种校验算法
- 🤖 **AI协议生成** - 基于Claude AI自动生成协议配置
- 🔌 **插件架构** - 灵活的插件系统，易于扩展
- 📊 **数据可视化** - 实时图表和3D姿态显示
- 💾 **数据录制** - CSV格式数据录制和回放

## 🚀 快速开始

### 安装

1. 下载最新版本
2. 解压到任意目录
3. 运行 `GenericScope.exe`

### 5分钟上手

[快速开始视频](link) | [详细教程](docs/user-manual/)

## 📚 文档

- [用户手册](docs/user-manual/)
- [API文档](docs/api/)
- [插件开发指南](docs/plugin-development-guide.md)
- [AI协议生成指南](docs/ai-protocol-generation-guide.md)

## 🛠️ 技术栈

- Qt 5.14
- C++17
- spdlog
- Qt3D

## 📄 许可证

MIT License

## 🤝 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md)
```

---

## 📝 实施步骤

### Step 1: 添加Doxygen注释（1天）

逐个模块添加：
1. Protocol模块
2. Transfer模块
3. Common Component模块
4. Core Plugin模块

### Step 2: 生成API文档（0.5天）

```bash
# 安装Doxygen
# Windows: 下载安装包
# Linux: sudo apt-get install doxygen

# 生成文档
doxygen Doxyfile

# 查看文档
# 打开 docs/api/html/index.html
```

### Step 3: 编写用户手册（1天）

- 每章2-3页
- 配合截图
- 包含实际操作步骤

### Step 4: 编写插件教程（0.5天）

- 基于P0-03的示例插件
- 详细的代码讲解

### Step 5: 更新README（0.5天）

- 添加徽章
- 更新特性列表
- 添加截图

---

## ✅ 完成检查清单

- [ ] Doxygen配置完成
- [ ] API文档生成成功
- [ ] 用户手册10章全部完成
- [ ] 插件开发教程完成
- [ ] README更新完成
- [ ] 所有文档经过审校
- [ ] 生成PDF版本（可选）

---

**创建日期**: 2026-01-29
