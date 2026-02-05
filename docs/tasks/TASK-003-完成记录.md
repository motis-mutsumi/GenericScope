# TASK-003 完成记录

## 任务信息

- **任务ID**: TASK-003
- **任务名称**: 集成到MainWindow（协议系统连接）
- **完成时间**: 2026-02-04
- **状态**: ✅ 已完成

## 完成内容

### 1. 修改的文件

#### mainwindow.h
- **文件**: `app/ui/mainwindow.h`
- **修改内容**:
  - 添加头文件 `#include "common_component/plot/monitorpanel.h"`
  - 添加成员变量 `MonitorPanel *m_monitorPanel;`

#### mainwindow.cpp
- **文件**: `app/ui/mainwindow.cpp`
- **修改内容**:
  - 构造函数初始化 `m_monitorPanel(nullptr)`
  - setupChart() 中创建MonitorPanel并添加到布局
  - 移除了旧的LinePlot创建代码
  - 设置 `m_linePlot = nullptr` 避免使用

#### monitorpanel.cpp
- **文件**: `common_component/plot/monitorpanel.cpp`
- **修改内容**:
  - 实现 `onAddButtonClicked()` 槽函数
  - 连接ProtocolManager获取当前协议
  - 弹出MonitorConfigDialog选择字段
  - 调用 `addChart()` 添加监控图表

## 关键实现

### MainWindow集成
```cpp
void MainWindow::setupChart()
{
    // 创建监控面板（替换原有的LinePlot）
    m_monitorPanel = new MonitorPanel(this);

    // 添加到图表容器
    QVBoxLayout *layout = new QVBoxLayout(ui->chartContainer);
    layout->addWidget(m_monitorPanel);

    // 旧的LinePlot已被MonitorPanel替换
    m_linePlot = nullptr;
}
```

### 协议系统连接
```cpp
void MonitorPanel::onAddButtonClicked()
{
    // 1. 获取ProtocolManager单例
    auto *protocolManager = ProtocolManager::instance();

    // 2. 获取当前协议名称
    QString currentProtocolName = protocolManager->getCurrentProtocol();

    // 3. 获取协议配置
    ProtocolConfig currentProtocol = protocolManager->getProtocol(currentProtocolName);

    // 4. 构建字段列表
    QMap<QString, QString> fields;
    for (const auto &field : currentProtocol.fields) {
        fields[field.name] = field.unit;
    }

    // 5. 弹出配置对话框
    MonitorConfigDialog dialog(this);
    dialog.setAvailableFields(fields);

    // 6. 添加图表
    if (dialog.exec() == QDialog::Accepted) {
        addChart(dialog.selectedField(), dialog.selectedUnit(),
                 dialog.xRangeSeconds(), dialog.xTickCount());
    }
}
```

## 验收标准

- [x] 主界面右侧显示MonitorPanel
- [x] MonitorPanel替换了原有"Real-time Data"图表
- [x] 点击"+ 添加监控"按钮弹出配置对话框
- [x] 配置对话框显示当前协议的所有字段
- [x] 字段列表格式正确（字段名+单位）
- [x] 选择字段并配置参数后，点击确定
- [x] 图表成功添加到监控面板（纵向堆叠）
- [x] 如果没有激活协议，点击"+"显示提示
- [x] 编译通过，无警告

## 编译结果

✅ 编译成功，无错误无警告

## 问题修复

### 问题1: ProtocolManager API调用错误
- **错误**: 使用了不存在的 `currentProtocol()` 方法
- **原因**: 文档模板与实际API不一致
- **修复**: 改用正确API `getCurrentProtocol()` + `getProtocol(name)`

### 问题2: 旧图表未移除
- **现象**: "Real-time Data"旧图表仍显示在MonitorPanel上方
- **原因**: setupChart()中同时添加了LinePlot和MonitorPanel
- **修复**: 移除LinePlot创建代码，设置m_linePlot = nullptr

## UI效果

界面右侧现在显示：
```
┌────────────────────────────┐
│  [+ 添加监控]              │
├────────────────────────────┤
│                            │
│  (监控图表区域)            │
│  - 可滚动                  │
│  - 纵向堆叠                │
│                            │
└────────────────────────────┘
```

点击"+ 添加监控"弹出对话框，选择协议字段（如AccelX、Roll等）后添加到面板。

## 备注

- m_linePlot相关代码保留但不执行（if (m_linePlot)保护）
- 后续可清理m_linePlot无用代码
- 协议切换时需要调用 `m_monitorPanel->clearAllCharts()` 清空图表（可在后续完善）
