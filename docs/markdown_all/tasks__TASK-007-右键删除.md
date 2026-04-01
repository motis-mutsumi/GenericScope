# TASK-007: 实现右键删除

## 任务信息

- **任务ID**: TASK-007
- **任务名称**: 实现右键删除
- **优先级**: P0（必须完成）
- **预估工作量**: 2小时
- **依赖任务**: TASK-006
- **状态**: 待开始

## 任务描述

完善MonitorChart的右键菜单功能，实现删除操作。这是P0阶段的最后一个任务，完成后即达到MVP（最小可行产品）里程碑。

## 具体目标

1. 确认contextMenuEvent()已正确实现（TASK-001已创建框架）
2. 完善onDeleteAction()槽函数
3. 确认MonitorPanel::removeChart()正确工作
4. 测试删除功能

## 涉及文件

### 修改文件
- `common_component/plot/monitorchart.cpp`（已在TASK-001创建，需完善）
- `common_component/plot/monitorpanel.cpp`（已在TASK-001创建，需确认）

## 实现细节

### 1. 确认contextMenuEvent()实现

在`monitorchart.cpp`中，TASK-001已创建框架，确认代码正确：

```cpp
void MonitorChart::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_editAction);      // 编辑（TASK-008）
    menu.addAction(m_pauseAction);     // 暂停（TASK-009）
    menu.addAction(m_clearAction);     // Clear（TASK-010）
    menu.addSeparator();
    menu.addAction(m_deleteAction);    // 删除

    // 更新暂停按钮文本
    m_pauseAction->setText(m_paused ? "恢复" : "暂停");

    menu.exec(event->globalPos());
}
```

### 2. 完善onDeleteAction()

在`monitorchart.cpp`中，TASK-001已创建框架：

```cpp
void MonitorChart::onDeleteAction()
{
    // 发射删除请求信号
    emit deleteRequested(this);
}
```

**说明：** 这个实现已经完整，无需修改。信号会被MonitorPanel接收并处理删除。

### 3. 确认MonitorPanel::removeChart()

在`monitorpanel.cpp`中，TASK-001已创建：

```cpp
void MonitorPanel::removeChart(MonitorChart *chart)
{
    if (!chart) {
        return;
    }

    // 从布局移除
    m_contentLayout->removeWidget(chart);

    // 从列表移除
    m_charts.removeOne(chart);

    // 安全删除（事件循环结束后）
    chart->deleteLater();
}
```

**说明：** 实现已完整，无需修改。

### 4. 确认信号连接

在`monitorpanel.cpp`的addChart()中，TASK-001已创建：

```cpp
void MonitorPanel::addChart(const QString &fieldName,
                            const QString &unit,
                            int xRangeSeconds,
                            int xTickCount)
{
    auto *chart = new MonitorChart(fieldName, unit, xRangeSeconds, xTickCount, this);

    // 连接删除信号
    connect(chart, &MonitorChart::deleteRequested,
            this, &MonitorPanel::onChartDeleteRequested);

    // 连接编辑信号（TASK-008）
    connect(chart, &MonitorChart::editRequested,
            this, &MonitorPanel::onChartEditRequested);

    // 添加到布局（在弹簧之前）
    m_contentLayout->insertWidget(m_contentLayout->count() - 1, chart);
    m_charts.append(chart);
}
```

### 5. 确认槽函数

在`monitorpanel.cpp`中：

```cpp
void MonitorPanel::onChartDeleteRequested(MonitorChart *chart)
{
    removeChart(chart);
}
```

**说明：** 所有相关代码在TASK-001中已经创建完成，本任务主要是验证和测试。

## 验收标准

- [ ] 右键图表显示菜单
- [ ] 菜单包含：编辑、暂停、Clear数据、（分隔符）、删除
- [ ] 点击"删除"，图表从界面移除
- [ ] 其他图表不受影响，位置自动调整
- [ ] 删除后无内存泄漏
- [ ] 删除所有图表后，面板显示空白
- [ ] 删除图表后，数据信号不再接收（自动取消订阅）

## 测试方法

### 功能测试

**测试场景1：删除单个图表**

1. 添加Roll图表
2. 开始接收数据，观察曲线
3. 右键图表
4. 验证菜单显示
5. 点击"删除"

**预期结果：**
- 图表立即从界面消失
- 面板显示空白
- 不再接收Roll数据

**测试场景2：删除多图表中的一个**

1. 添加Roll、Pitch、Yaw三个图表
2. 开始接收数据
3. 右键Pitch图表，点击"删除"

**预期结果：**
- Pitch图表消失
- Roll和Yaw图表保持显示
- 布局自动调整（Roll和Yaw紧凑排列）

**测试场景3：删除所有图表**

1. 添加多个图表
2. 逐个右键删除

**预期结果：**
- 所有图表依次消失
- 最后面板显示空白（只有"+"按钮）

**测试场景4：删除后重新添加**

1. 添加Roll图表
2. 删除
3. 重新添加Roll图表

**预期结果：**
- 新图表正常显示
- 能接收数据并绘制

### 内存泄漏测试

使用Qt Creator的内存分析工具或Valgrind：

```bash
valgrind --leak-check=full ./GenericScope
```

**操作步骤：**
1. 添加10个图表
2. 删除所有图表
3. 重复10次
4. 观察内存占用

**预期结果：**
- 内存占用稳定，无持续增长
- Valgrind无内存泄漏报告

### 单元测试

```cpp
void TestMonitorPanel::testRemoveChart()
{
    MonitorPanel panel;

    // 添加3个图表
    panel.addChart("Roll", "度", 60, 2);
    panel.addChart("Pitch", "度", 60, 2);
    panel.addChart("Yaw", "度", 60, 2);

    QCOMPARE(panel.chartCount(), 3);

    // 删除中间的图表
    auto *chart = panel.charts().at(1);
    panel.removeChart(chart);

    QCOMPARE(panel.chartCount(), 2);

    // 验证剩余图表是Roll和Yaw
    QCOMPARE(panel.charts().at(0)->fieldName(), "Roll");
    QCOMPARE(panel.charts().at(1)->fieldName(), "Yaw");
}

void TestMonitorPanel::testRemoveAllCharts()
{
    MonitorPanel panel;
    panel.addChart("Roll", "度", 60, 2);
    panel.addChart("Pitch", "度", 60, 2);

    panel.clearAllCharts();

    QCOMPARE(panel.chartCount(), 0);
}
```

### 信号取消订阅测试

```cpp
void TestMonitorChart::testUnsubscribeOnDelete()
{
    MonitorPanel panel;
    panel.addChart("Roll", "度", 60, 2);

    auto *chart = panel.charts().first();

    // 创建信号监听器
    QSignalSpy spy(chart, &MonitorChart::dataReceived);

    // 发送数据（应该接收）
    MonitorDataManager::instance()->onProtocolDataParsed({{"Roll", 45.2}});
    QCOMPARE(spy.count(), 1);

    // 删除图表
    panel.removeChart(chart);

    // 再次发送数据（不应该接收，因为已删除）
    MonitorDataManager::instance()->onProtocolDataParsed({{"Roll", 50.0}});

    // 验证：删除后不再接收信号
    // （由于deleteLater()，需要等待事件循环）
    QTest::qWait(100);
    QCOMPARE(spy.count(), 1);  // 仍然是1，没有增加
}
```

## 注意事项

### 1. deleteLater()的使用

**为什么使用deleteLater()而不是delete？**

```cpp
// ❌ 错误：立即删除
delete chart;  // 可能导致崩溃

// ✅ 正确：延迟删除
chart->deleteLater();  // 等事件循环结束后删除
```

**原因：**
- 右键菜单是chart的子对象
- 立即delete会导致菜单访问无效指针
- deleteLater()确保菜单事件处理完成后再删除

### 2. 布局自动调整

**QVBoxLayout的行为：**
- removeWidget()后，布局自动重排
- 其他Widget向上移动填补空隙
- 不需要手动调整位置

### 3. 信号自动断开

**Qt的信号槽机制：**
- QObject被删除时，自动断开所有信号连接
- 不需要手动disconnect()
- 这是Qt的RAII特性

### 4. 父子关系

**确保正确的父子关系：**
```cpp
auto *chart = new MonitorChart(..., this);  // this是MonitorPanel
```

**好处：**
- MonitorPanel销毁时，自动删除所有子chart
- 避免内存泄漏

### 5. 空指针检查

```cpp
void MonitorPanel::removeChart(MonitorChart *chart)
{
    if (!chart) {  // 重要！
        return;
    }
    // ...
}
```

## 里程碑：MVP完成

完成此任务后，**P0核心功能全部完成**，达到MVP（最小可行产品）里程碑！

### MVP功能清单

- [x] 动态添加图表
- [x] 配置字段和参数
- [x] 实时数据绘制
- [x] Y轴自动缩放
- [x] 纵向布局
- [x] 删除图表

### 可演示内容

1. 配置IMU协议
2. 添加Roll、Pitch、Yaw图表
3. 开始接收数据，三个图表同步显示
4. 删除Pitch图表
5. Roll和Yaw继续正常显示

## 后续任务

完成此任务后，进入P1阶段：
- TASK-008: 实现右键编辑
- TASK-009: 实现暂停/恢复
- TASK-010: 实现Clear数据
- TASK-011: 异常处理

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-001: 搭建基础框架](./TASK-001-基础框架.md)
- [TASK-006: 实现图表绘制](./TASK-006-图表绘制.md)
- [Qt 对象树与内存管理](https://doc.qt.io/qt-5.14/objecttrees.html)
