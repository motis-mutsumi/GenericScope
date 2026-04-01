# 代码合理性问题统一修复记录（2026-03-31）

## 文档范围
- `app/device/devicemanager.cpp`
- `app/ui/mainwindow.h`
- `app/ui/mainwindow.cpp`
- `protocol/protocolmanager.h`
- `protocol/protocolmanager.cpp`
- `transfer/scopetransfermanager.h`
- `transfer/scopetransfermanager.cpp`

## 修复总览
| 编号 | 问题 | 修复动作 | 效果 |
| --- | --- | --- | --- |
| 1 | UDP/串口生命周期不一致 | `ScopeUdp` 改为无 parent 创建；`disconnectDevice()` 统一 `delete m_transfer` | 断开后不残留传输对象 |
| 2 | 未知设备类型静默回退 UART | 仅允许 `UART/UDP`，其余显式报错并发 `errorOccurred` | 配置错误可观测 |
| 3 | 协议解析器使用裸指针 | 改为 `QScopedPointer<ProtocolParser>` | 避免手动释放遗漏 |
| 4 | 设备错误弹窗无节流 | 增加重复错误 3 秒节流 | 减少弹窗轰炸 |
| 5 | 协议预加载依赖 UI 副作用 | 新增 `ProtocolManager::loadProtocolsFromSettings()` | 业务层直连配置，职责更清晰 |
| 6 | 会话统计未完整重置 | 连接/断开时统一重置统计字段 | 数据速率按会话独立计算 |
| 7 | `m_linePlot` 为无效遗留路径 | 移除 `m_linePlot` 路径，统一刷新 `m_monitorPanel` | 清理死代码 |
| 8 | `destoryTransfer` 拼写错误 | 新增 `destroyTransfer`，保留兼容转发 | 命名规范且兼容旧调用 |

## 详细修复说明

### 1. 传输对象释放一致性
- 问题：UDP 与 UART 的释放路径不一致，可能导致 UDP 对象残留。
- 修复：
1. `connectUdp()` 改为 `new ScopeUdp()`。
2. `disconnectDevice()` 统一 `m_transfer->close(); delete m_transfer;`。
- 影响：多次连接/断开后内存与对象树更稳定。

### 2. 设备类型分发显式化
- 问题：未知类型会默认走 UART。
- 修复：`connectDevice()` 仅处理 `UDP` 与 `UART`，其余类型记录日志并抛出错误事件。
- 影响：错误配置可快速定位。

### 3. 协议解析器内存管理
- 问题：`m_protocolParser` 为裸指针，切换协议靠手动 `delete`。
- 修复：改用 `QScopedPointer`，切换时 `reset(new ProtocolParser(...))`。
- 影响：降低内存泄漏风险。

### 4. 错误弹窗节流
- 问题：同类错误高频重复弹窗影响使用。
- 修复：新增 `kErrorDialogThrottleMs = 3000`，重复错误在窗口期内不重复弹。
- 影响：保留告警同时减少干扰。

### 5. 协议预加载去 UI 依赖
- 问题：通过临时构造 `CommandSettingsDialog` 触发加载，存在副作用。
- 修复：
1. `ProtocolManager` 新增 `loadProtocolsFromSettings(organization, application)`。
2. `MainWindow::preloadProtocols()` 直接调用该方法。
- 影响：预加载逻辑更纯粹、可复用。

### 6. 会话统计重置
- 问题：重连后可能沿用上次计数基线。
- 修复：在连接成功与断开流程中，重置 `m_packetCount`、`m_lastPacketCount`、`m_lastDataRateTime`、错误节流状态。
- 影响：状态展示准确。

### 7. 图表刷新路径收敛
- 问题：`LinePlot` 已被 `MonitorPanel` 替代，旧刷新代码无效。
- 修复：删除 `m_linePlot` 成员与调用，刷新统一改为 `m_monitorPanel->update()`。
- 影响：逻辑与当前 UI 架构一致。

### 8. API 命名规范化
- 问题：`destoryTransfer` 拼写错误。
- 修复：新增 `destroyTransfer`，旧接口保留为兼容转发。
- 影响：新代码可用规范命名，不破坏旧调用。

## 验证与后续
- 已完成代码差异检查。
- 当前环境无 `qmake`，无法本地全量编译。
- 建议在 Qt 构建环境执行：
1. 全量编译一次。
2. UART/UDP 各做一次连接-收数-断开回归。
3. 连续触发同一错误，确认弹窗节流生效。
