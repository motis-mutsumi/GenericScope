# TASK-003: 集成到MainWindow

## 任务信息

- **任务ID**: TASK-003
- **任务名称**: 集成到MainWindow
- **优先级**: P0（必须完成）
- **预估工作量**: 2小时
- **依赖任务**: TASK-001, TASK-002
- **状态**: 待开始

## 任务描述

将MonitorPanel集成到MainWindow的右侧区域，替换原有的简陋图表功能。连接"+"按钮点击事件，弹出配置对话框，获取当前协议的字段列表。

## 具体目标

1. 在MainWindow添加m_monitorPanel成员
2. 在setupUI()中创建MonitorPanel
3. 替换原有简陋图表区域的布局
4. 连接"+"按钮点击事件到onAddButtonClicked()
5. 实现onAddButtonClicked()：
   - 获取当前协议配置的字段列表
   - 弹出配置对话框
   - 调用addChart()添加图表

## 涉及文件

### 修改文件
- `app/ui/mainwindow.h`
- `app/ui/mainwindow.cpp`

## 实现细节

### 1. mainwindow.h 修改

```cpp
// 在文件顶部添加
#include "common_component/plot/monitorpanel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    // ... existing methods ...

private slots:
    // ... existing slots ...

private:
    // ... existing members ...

    // 新增成员
    MonitorPanel *m_monitorPanel;  // 监控面板
};
```

### 2. mainwindow.cpp 修改

#### 2.1 在setupUI()中创建MonitorPanel

```cpp
void MainWindow::setupUI() {
    // ... existing code ...

    // 创建监控面板（替换原有简陋图表区域）
    m_monitorPanel = new MonitorPanel(this);

    // 假设右侧布局是rightLayout（QVBoxLayout）
    // 移除旧的图表组件（如果有）
    // rightLayout->removeWidget(oldChartWidget);
    // delete oldChartWidget;

    // 添加监控面板到右侧布局
    rightLayout->addWidget(m_monitorPanel);

    // ... existing code ...
}
```

#### 2.2 实现MonitorPanel::onAddButtonClicked()

在`monitorpanel.cpp`中完善此方法：

```cpp
#include "monitorconfigdialog.h"
#include "protocol/protocolmanager.h"  // 假设协议管理器路径

void MonitorPanel::onAddButtonClicked()
{
    // 获取当前激活的协议配置
    auto *protocolManager = ProtocolManager::instance();
    if (!protocolManager) {
        QMessageBox::warning(this, "提示", "协议管理器未初始化");
        return;
    }

    auto currentProtocol = protocolManager->currentProtocol();
    if (!currentProtocol) {
        QMessageBox::warning(this, "提示", "请先配置并激活协议");
        return;
    }

    // 构建字段列表
    QMap<QString, QString> fields;
    for (const auto &field : currentProtocol->fields) {
        fields[field.name] = field.unit;
    }

    if (fields.isEmpty()) {
        QMessageBox::warning(this, "提示", "当前协议没有可监控的字段");
        return;
    }

    // 创建并显示配置对话框
    MonitorConfigDialog dialog(this);
    dialog.setAvailableFields(fields);

    if (dialog.exec() == QDialog::Accepted) {
        // 添加图表
        addChart(dialog.selectedField(),
                 dialog.selectedUnit(),
                 dialog.xRangeSeconds(),
                 dialog.xTickCount());
    }
}
```

#### 2.3 处理协议切换

```cpp
// 在MainWindow中，当协议切换时清空监控面板
void MainWindow::onProtocolChanged()
{
    // ... existing code ...

    // 清空所有监控图表
    if (m_monitorPanel) {
        m_monitorPanel->clearAllCharts();
    }

    // ... existing code ...
}
```

## 验收标准

- [ ] 主界面右侧显示MonitorPanel
- [ ] MonitorPanel替换了原有简陋图表区域
- [ ] 点击"+"按钮弹出配置对话框
- [ ] 配置对话框显示当前协议的所有字段
- [ ] 字段列表格式正确（字段名+单位）
- [ ] 选择字段并配置参数后，点击确定
- [ ] 图表成功添加到监控面板（纵向堆叠）
- [ ] 如果没有激活协议，点击"+"显示提示
- [ ] 编译通过，无警告

## 测试方法

### 集成测试

1. **测试场景：添加图表**
   - 启动GenericScope
   - 配置IMU协议（包含Roll、Pitch、Yaw字段）
   - 激活协议
   - 点击右侧监控面板的"+"按钮
   - 验证：配置对话框弹出，显示Roll、Pitch、Yaw三个字段
   - 选择Roll，设置X轴60秒，点数2
   - 点击确定
   - 验证：监控面板显示Roll图表框

2. **测试场景：无协议提示**
   - 启动GenericScope（未配置协议）
   - 点击"+"按钮
   - 验证：显示警告提示"请先配置并激活协议"

3. **测试场景：多图表添加**
   - 连续添加Roll、Pitch、Yaw三个图表
   - 验证：三个图表纵向堆叠排列
   - 验证：滚动条正常工作

4. **测试场景：协议切换**
   - 添加几个图表
   - 切换协议（如果支持）
   - 验证：监控面板清空所有图表

## 注意事项

1. **协议管理器接口**：
   - 根据实际项目调整ProtocolManager的获取方式
   - 可能是单例、也可能在MainWindow中持有
   - 字段结构可能与假设不同，需要适配

2. **布局替换**：
   - 仔细查找原有图表区域的变量名
   - 确保正确移除旧组件
   - 避免内存泄漏

3. **错误处理**：
   - 协议未配置时的友好提示
   - 字段列表为空时的提示

4. **用户体验**：
   - 对话框居中显示
   - 提示信息清晰
   - 操作流程顺畅

## 实际适配建议

由于不同项目的协议管理实现可能不同，请根据实际情况调整：

### 方案A：使用ProtocolManager单例

```cpp
auto *manager = ProtocolManager::instance();
auto config = manager->currentProtocol();
```

### 方案B：MainWindow持有协议配置

```cpp
// 在MainWindow中
void MonitorPanel::onAddButtonClicked()
{
    // 通过父窗口获取
    MainWindow *mainWin = qobject_cast<MainWindow*>(window());
    if (mainWin) {
        auto config = mainWin->currentProtocolConfig();
        // ...
    }
}
```

### 方案C：通过信号槽传递

```cpp
// MonitorPanel添加信号
signals:
    void requestCurrentProtocol();

// MainWindow连接
connect(m_monitorPanel, &MonitorPanel::requestCurrentProtocol,
        this, &MainWindow::onProtocolRequested);
```

**请在实施时根据项目实际架构选择合适方案。**

## 后续任务

完成此任务后，继续执行：
- TASK-005: 连接数据流（需要TASK-004完成）

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-001: 搭建基础框架](./TASK-001-基础框架.md)
- [TASK-002: 实现配置对话框](./TASK-002-配置对话框.md)
- [GenericScope协议系统](../../CLAUDE.md#协议系统)
