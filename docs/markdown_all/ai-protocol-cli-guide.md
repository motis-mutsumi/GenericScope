# AI协议生成功能 - CLI调用方式使用指南

本文档介绍如何使用CMD命令行调用Claude进行协议生成，无需配置API密钥。

---

## 📋 目录

- [功能概述](#功能概述)
- [前置准备](#前置准备)
- [快速开始](#快速开始)
- [使用方法](#使用方法)
- [常见问题](#常见问题)
- [技术原理](#技术原理)

---

## 功能概述

### 🎯 CLI调用方式 vs API调用方式

| 特性 | CLI调用方式 | API调用方式 |
|------|------------|------------|
| **配置复杂度** | ✅ 简单（一次性安装） | ⚠️ 复杂（需要API密钥） |
| **网络要求** | ✅ 自动处理 | ⚠️ 需要配置代理 |
| **费用** | 💰 使用Claude付费账号 | 💰 按API调用量计费 |
| **速度** | 🐌 稍慢（进程启动） | 🚀 较快（直接HTTP） |
| **适用场景** | 个人开发、测试 | 生产环境、批量生成 |

### ✨ 主要优势

- **无需API密钥**：使用Claude CLI登录一次即可
- **简单易用**：安装后即可使用，无需复杂配置
- **自动认证**：CLI工具自动管理认证token

---

## 前置准备

### 1️⃣ 安装Claude Code CLI

Claude Code CLI是Anthropic官方提供的命令行工具。

#### Windows安装方式

**方式A：使用安装包（推荐）**

1. 访问 [Claude Code下载页](https://claude.ai/download)
2. 下载Windows安装包（.exe或.msi）
3. 双击安装，按提示完成安装
4. 安装完成后，`claude`命令会自动添加到PATH

**方式B：使用命令行（需要管理员权限）**

```bash
# 使用winget（Windows 10/11）
winget install Anthropic.Claude

# 或使用scoop
scoop install claude
```

#### 验证安装

打开命令提示符（CMD）或PowerShell，运行：

```bash
claude --version
```

如果显示版本号，说明安装成功。

### 2️⃣ 登录Claude账号

第一次使用需要登录：

```bash
claude auth login
```

按提示完成浏览器登录流程。登录成功后，凭证会保存在本地，无需重复登录。

### 3️⃣ 测试连接

```bash
claude "你好，请回复一句话"
```

如果收到Claude的回复，说明配置成功。

---

## 快速开始

### 5分钟上手示例

1. **启动GenericScope** → 点击右上角 **设置⚙️** → **协议配置**

2. **新建协议** → 点击 **新建** 按钮

3. **选择生成方式** → 在下拉框中选择 **"🤖 AI智能生成（CLI）"**

4. **输入协议名称**：`IMU_Protocol_CLI`

5. **点击"创建"** → 进入AI输入对话框

6. **粘贴示例** → 点击 **粘贴示例** 按钮（自动填充演示数据）

7. **生成协议** → 点击 **生成协议** 按钮

8. **等待生成** → CLI调用中（约15-45秒）

9. **完成！** → 协议配置自动生成并填充到配置界面

---

## 使用方法

### 方法1：在GenericScope中使用

参考"快速开始"部分，在协议配置对话框中选择CLI生成方式。

### 方法2：直接使用代码集成

```cpp
#include "protocolcligenerator.h"

// 创建CLI生成器
ProtocolCLIGenerator *generator = new ProtocolCLIGenerator(this);

// 连接信号
connect(generator, &ProtocolCLIGenerator::generationComplete,
        this, &MyClass::onProtocolGenerated);
connect(generator, &ProtocolCLIGenerator::generationFailed,
        this, &MyClass::onGenerationFailed);
connect(generator, &ProtocolCLIGenerator::progressUpdate,
        this, &MyClass::onProgressUpdate);

// 生成协议
QString protocolName = "IMU_Protocol";
QString rawData = "AA55 0101 2E00 C7F6 2901 ...";
QString parseRules = "原始数据从第7字节开始，取44字节...";

generator->generateProtocol(protocolName, rawData, parseRules);
```

### 方法3：自定义Claude命令路径

如果Claude命令不在PATH中，可以指定完整路径：

```cpp
generator->setCommandPath("C:/Program Files/Claude/claude.exe");
```

---

## 常见问题

### ❓ 安装和配置

**Q1：如何确认Claude CLI是否正确安装？**

A：打开命令提示符，运行 `claude --version`。如果显示版本号，说明安装成功。

**Q2：提示"未找到Claude CLI命令"怎么办？**

A：可能的原因：
1. Claude CLI未正确安装
2. claude命令不在PATH环境变量中
3. 需要重启GenericScope以刷新PATH

**解决方案**：
- 重新安装Claude CLI
- 手动添加Claude安装目录到PATH
- 在代码中使用完整路径：`setCommandPath("C:/path/to/claude.exe")`

**Q3：登录失败怎么办？**

A：
1. 检查网络连接
2. 使用代理：`claude auth login --proxy http://proxy:port`
3. 清除旧凭证：`claude auth logout`，然后重新登录

---

### ❓ 使用过程问题

**Q4：生成速度慢？**

A：CLI方式相比API方式稍慢，正常情况下需要15-45秒。影响因素：
- 进程启动时间（约2-5秒）
- 网络延迟
- Claude服务器负载

**Q5：生成失败，提示"进程崩溃"？**

A：可能的原因：
1. Claude CLI版本过旧 → 升级到最新版本
2. 登录凭证过期 → 重新运行 `claude auth login`
3. 网络连接中断 → 检查网络

**Q6：能否在离线环境使用？**

A：不能。Claude CLI需要联网调用Anthropic服务器。

**Q7：CLI方式和API方式可以切换吗？**

A：可以！两种方式独立存在：
- CLI方式：使用 `ProtocolCLIGenerator` 类
- API方式：使用 `ProtocolAIGenerator` 类

在AIProtocolInputDialog中可以选择使用哪种方式。

**Q8：生成成功但界面卡住不响应？**

A：这是Qt信号槽连接问题，可能的原因：

1. **对象创建顺序错误**（最常见）
   - 症状：日志显示"发送generationComplete信号"，但槽函数从未被调用
   - 原因：在对象创建前就调用了`connect()`，导致连接失败
   - 解决方案：确保在`setupConnections()`之前创建生成器对象：
   ```cpp
   AIProtocolInputDialog::AIProtocolInputDialog(QWidget *parent)
   {
       qRegisterMetaType<CommandSettingsDialog::ProtocolConfig>(...);

       // ✅ 先创建对象
       m_apiGenerator = new ProtocolAIGenerator(this);
       m_cliGenerator = new ProtocolCLIGenerator(this);

       // ✅ 再建立连接
       setupUI();
       setupConnections();
   }
   ```

2. **自定义类型未注册**
   - 症状：编译通过，但运行时信号槽不工作
   - 解决方案：在构造函数中注册自定义类型：
   ```cpp
   qRegisterMetaType<CommandSettingsDialog::ProtocolConfig>(
       "CommandSettingsDialog::ProtocolConfig");
   ```

3. **调试方法**
   - 在槽函数开头添加qDebug()确认是否被调用
   - 检查connect()返回值（应为true）
   - 使用Qt::QueuedConnection显式指定连接类型

---

### ❓ 费用和限制

**Q8：CLI方式收费吗？**

A：是的。CLI方式使用你的Claude账号，按照Claude的订阅计划计费：
- **Claude Pro**: $20/月，无限对话
- **Claude Free**: 有使用限制

每次协议生成约消耗1次对话。

**Q9：有使用频率限制吗？**

A：Claude Free账号有频率限制（每8小时约20-30次对话）。Claude Pro账号无限制。

**Q10：CLI方式比API方式更便宜吗？**

A：对于轻度使用，CLI方式（Claude Pro $20/月）更划算。对于大量使用（每月生成1000+协议），API方式可能更经济。

---

## 技术原理

### 系统架构

```
┌─────────────────────────┐
│ ProtocolCLIGenerator    │  CLI生成器
└──────────┬──────────────┘
           │
           ▼
┌─────────────────────────┐
│ QProcess                │  进程管理
│ ├─ 创建临时文件          │
│ ├─ 调用claude命令        │
│ └─ 捕获标准输出          │
└──────────┬──────────────┘
           │
           ▼
┌─────────────────────────┐
│ claude CLI              │  Claude命令行工具
│ (本地安装)               │
└──────────┬──────────────┘
           │
           ▼
┌─────────────────────────┐
│ Anthropic API           │  远程AI服务
│ (Claude自动调用)         │
└──────────┬──────────────┘
           │
           ▼
┌─────────────────────────┐
│ ProtocolConfig          │  生成的协议配置
└─────────────────────────┘
```

### 调用流程

```
1. 构建提示词
   └─ 包含协议名称、原始数据、解析规则

2. 保存到临时文件
   └─ %TEMP%/claude_prompt_XXXXXX.txt

3. 启动Claude进程
   ├─ 命令: claude --dangerously-skip-permissions
   └─ 标准输入: 临时文件

4. 捕获输出
   ├─ 标准输出: AI生成的JSON
   └─ 标准错误: 错误信息

5. 解析JSON
   └─ 转换为ProtocolConfig

6. 清理临时文件
   └─ 删除提示词文件
```

### 与API方式的区别

| 步骤 | CLI方式 | API方式 |
|------|---------|---------|
| 认证 | CLI自动管理token | 需要配置API密钥 |
| 网络 | CLI处理网络请求 | 直接HTTP POST |
| 请求 | 通过进程stdin | QNetworkAccessManager |
| 响应 | 通过进程stdout | HTTP响应body |
| 错误 | stderr + 退出码 | HTTP状态码 + JSON |

---

## 文件说明

### 核心文件

| 文件 | 说明 |
|------|------|
| [protocolcligenerator.h](../app/ui/protocolcligenerator.h) | CLI生成器头文件 |
| [protocolcligenerator.cpp](../app/ui/protocolcligenerator.cpp) | CLI生成器实现 |
| [aiprotocolinputdialog.h](../app/ui/aiprotocolinputdialog.h) | AI输入对话框（支持CLI/API切换） |
| [aiprotocolinputdialog.cpp](../app/ui/aiprotocolinputdialog.cpp) | 对话框实现 |

### 临时文件

| 文件 | 位置 | 说明 |
|------|------|------|
| claude_prompt_*.txt | %TEMP% | 临时提示词文件，生成后自动删除 |

---

## 最佳实践

### ✅ 推荐使用场景

- **个人开发**：本地开发测试，无需管理API密钥
- **快速原型**：快速验证协议解析逻辑
- **教学演示**：展示AI辅助协议配置

### ✅ 性能优化建议

1. **保持Claude CLI登录状态**：避免重复登录
2. **使用稳定网络**：减少连接失败
3. **合理设置超时**：默认2分钟，可根据需要调整

### ✅ 安全建议

1. **不要在生产环境使用个人账号**：使用API方式+团队密钥
2. **定期更新Claude CLI**：获取最新功能和安全补丁
3. **保护登录凭证**：不要分享 `~/.claude` 目录

---

## 版本历史

### v1.0.0 (2026-01-28)

- ✨ 首次发布CLI调用功能
- ✅ 支持Windows平台
- ✅ 自动检测claude命令
- ✅ 临时文件自动清理
- ✅ 完整的错误处理和日志

---

## 附录

### A. Claude CLI常用命令

```bash
# 查看版本
claude --version

# 查看帮助
claude --help

# 登录
claude auth login

# 登出
claude auth logout

# 查看登录状态
claude auth status

# 发送消息
claude "你的问题"

# 从文件读取
claude < input.txt

# 保存输出
claude "问题" > output.txt
```

### B. 环境变量

| 变量 | 说明 | 示例 |
|------|------|------|
| PATH | Claude命令搜索路径 | C:\Program Files\Claude |
| ANTHROPIC_API_KEY | API密钥（CLI不需要） | sk-ant-... |

### C. 故障排查步骤

1. **确认Claude CLI安装**
   ```bash
   where claude
   claude --version
   ```

2. **检查登录状态**
   ```bash
   claude auth status
   ```

3. **测试基本功能**
   ```bash
   claude "测试连接"
   ```

4. **查看GenericScope日志**
   - 日志文件位置：`Bin/x64/Release/logs/`
   - 搜索关键词：`ProtocolCLIGenerator`

5. **启用调试输出**
   ```cpp
   // 在代码中添加
   qDebug() << "Claude命令:" << command;
   qDebug() << "进程输出:" << output;
   ```

---

**文档版本**：1.0.0
**更新日期**：2026-01-28
**作者**：GenericScope开发团队

---

🎉 祝您使用愉快！如有问题，欢迎反馈。
