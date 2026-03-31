# CLAUDE.md

GenericScope 通用上位机框架 - Qt 5.14 / C++17 / MSVC / spdlog，约95%完成。

## 构建

```bash
cd script && build.bat
# 或手动：qmake GenericScope.pro -spec win32-msvc && jom.exe -f Makefile.Release
# 输出：Bin/x64/Release/GenericScope.exe
```

## 架构

```
app → algorithm / util / protocol / transfer / common_component / core_plugin
protocol: ProtocolConfig, ProtocolParser, ChecksumCalculator(6种), DataTypeConverter(10种), ProtocolManager
transfer: ScopeUart / ScopeTcp / ScopeModbus
core_plugin: core_plugins_manager + datamonitor/devicecontrol 插件
```

## 关键文件

| 组件 | 路径 |
|------|------|
| 主窗口 | `app/ui/mainwindow.h/cpp` |
| 协议配置UI | `app/ui/commandsettingsdialog.h/cpp` |
| 协议管理器 | `protocol/protocolmanager.h/cpp` |
| 协议解析器 | `protocol/protocolparser.h/cpp` |
| 串口传输 | `transfer/uart/scopeuart.h/cpp` |
| 插件基类 | `core_plugin/core_plugins_manager/core_pluginsbase.h` |

## 开发规范

- 命名：类 CamelCase，方法/变量 camelCase，成员 m_ 前缀，常量 k 前缀
- Qt 5.14：`resize+fill` 分两步，不能 `resize(size, val)`
- 线程安全：QMutexLocker；UI更新用 QMetaObject::invokeMethod + QueuedConnection
- 内存：优先 Qt 父子关系，其次 unique_ptr / QScopedPointer
- 设计模式：单例(ProtocolManager/LogManager/Config)、工厂(TransferManager)、Qt插件架构

## 日志调试

```cpp
LogManager::instance()->info("msg");
LogManager::instance()->error("err: {}", msg);
// 日志文件：Bin/x64/Release/logs/log_yyyyMMdd_hhmmss.txt
```

## 文档

- 用户手册：`docs/user-manual.md`
- 协议规范：`docs/protocol-config-system-spec.md`
- 任务文档：`docs/tasks/`
