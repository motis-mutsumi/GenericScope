# TASK-010: 实现Clear数据

## 任务信息

- **任务ID**: TASK-010
- **任务名称**: 实现Clear数据
- **优先级**: P1（重要功能）
- **预估工作量**: 1小时
- **依赖任务**: TASK-007
- **状态**: 待开始

## 任务描述

实现图表的数据清空功能。点击"Clear数据"后，清空当前图表的数据缓存，图表重新开始绘制。这是一个简单但实用的功能，用于重置观察窗口。

## 具体目标

1. 完善onClearAction()槽函数
2. 实现clearData()方法
3. 测试清空功能

## 涉及文件

### 修改文件
- `common_component/plot/monitorchart.cpp`

## 实现细节

### 1. 完善onClearAction()

在`monitorchart.cpp`中（TASK-001已创建框架）：

```cpp
void MonitorChart::onClearAction()
{
    clearData();
}
```

### 2. 实现clearData()

在`monitorchart.cpp`中（TASK-001已创建框架）：

```cpp
void MonitorChart::clearData()
{
    // 清空数据缓存
    m_timestamps.clear();
    m_values.clear();

    // 更新图表（显示空白）
    updatePlot();
}
```

**说明：** 实现非常简单，清空两个QVector即可。

## 验收标准

- [ ] 点击"Clear数据"，图表清空
- [ ] 清空后图表显示空白（无曲线）
- [ ] 继续接收数据，图表重新绘制
- [ ] Clear操作不影响其他图表
- [ ] 清空暂停中的图表也正常工作

## 测试方法

### 功能测试

**测试场景1：基本清空**

1. 添加Roll图表
2. 接收数据，观察曲线绘制
3. 右键点击"Clear数据"

**预期结果：**
- 曲线立即消失
- 图表显示空白
- 坐标轴仍然存在

4. 继续接收数据

**预期结果：**
- 曲线重新开始绘制
- 从空白状态开始

**测试场景2：清空后数据点数量**

1. 添加Roll图表
2. 接收100个数据点
3. Clear数据
4. 验证数据点数量

**预期结果：**
- 数据点数量为0

**测试场景3：多次Clear**

1. 添加图表
2. 接收数据
3. Clear
4. 接收数据
5. Clear
6. 重复5次

**预期结果：**
- 每次Clear后都能正常重新绘制
- 无内存泄漏

**测试场景4：暂停状态下Clear**

1. 添加图表
2. 接收数据
3. 暂停
4. Clear数据

**预期结果：**
- 数据清空
- 仍保持暂停状态
- 恢复后接收新数据

### 单元测试

```cpp
void TestMonitorChart::testClearData()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 添加数据
    chart.appendData(10.0, 1000);
    chart.appendData(20.0, 2000);
    chart.appendData(30.0, 3000);
    QCOMPARE(chart.dataPointCount(), 3);

    // 清空
    chart.clearData();

    // 验证数据被清空
    QCOMPARE(chart.dataPointCount(), 0);

    // 添加新数据
    chart.appendData(40.0, 4000);
    QCOMPARE(chart.dataPointCount(), 1);
}

void TestMonitorChart::testClearWhilePaused()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 添加数据并暂停
    chart.appendData(10.0, 1000);
    chart.setPaused(true);

    // 清空
    chart.clearData();

    // 验证数据被清空
    QCOMPARE(chart.dataPointCount(), 0);

    // 验证仍保持暂停状态
    QVERIFY(chart.isPaused());
}
```

## 注意事项

### 1. QVector::clear()的行为

**Qt文档：**
```cpp
void QVector::clear()
```
- 移除所有元素
- 容量可能保留（不释放内存）
- 如需释放内存，使用squeeze()

**当前实现：**
```cpp
m_timestamps.clear();  // 足够
```

**如需释放内存：**
```cpp
m_timestamps.clear();
m_timestamps.squeeze();  // 释放内存
```

### 2. updatePlot()处理空数据

**在updatePlot()中（TASK-006已实现）：**

```cpp
void MonitorChart::updatePlot()
{
    if (m_timestamps.isEmpty()) {
        m_plot->graph(0)->data()->clear();  // 清空图形数据
        m_plot->replot();
        return;
    }

    // 正常绘制逻辑...
}
```

**说明：** 已正确处理空数据情况，无需修改。

### 3. Clear不改变配置

**Clear操作只清空数据，不改变：**
- 字段名
- X轴范围
- 刻度数量
- 暂停状态

**正确行为：**
```cpp
void MonitorChart::clearData()
{
    m_timestamps.clear();
    m_values.clear();
    updatePlot();

    // ❌ 不要这样做：
    // m_paused = false;  // 错误！不应改变暂停状态
    // m_xRangeSeconds = 60;  // 错误！不应改变配置
}
```

### 4. 与Record模块的关系

**说明：**
- Clear只清空图表缓存
- 不清空Record模块的录制数据
- Record模块独立管理数据

**确认：** 符合需求（TASK-005中确认）

## 扩展功能（可选）

### 1. 清空确认对话框

```cpp
void MonitorChart::onClearAction()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this,
                                   "确认清空",
                                   "确定要清空当前图表的数据吗？",
                                   QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        clearData();
    }
}
```

**建议：** 不添加确认对话框，Clear是快速操作，确认会降低体验。

### 2. 清空统计

```cpp
// 记录清空次数
int m_clearCount;

void MonitorChart::clearData()
{
    m_clearCount++;
    qDebug() << "清空次数:" << m_clearCount;

    m_timestamps.clear();
    m_values.clear();
    updatePlot();
}
```

### 3. 清空动画

```cpp
void MonitorChart::clearData()
{
    // 渐隐动画
    QPropertyAnimation *animation = new QPropertyAnimation(m_plot, "opacity");
    animation->setDuration(300);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    connect(animation, &QPropertyAnimation::finished, [this]() {
        m_timestamps.clear();
        m_values.clear();
        updatePlot();
        m_plot->setOpacity(1.0);  // 恢复
    });
}
```

**建议：** 简单实现即可，不需要动画。

## 常见问题

### Q1: Clear后图表仍显示旧数据？

**检查：**
- updatePlot()是否调用
- m_plot->replot()是否执行
- QCustomPlot数据是否清空

### Q2: Clear后内存未释放？

**解决方案：**
```cpp
m_timestamps.clear();
m_timestamps.squeeze();  // 释放内存
m_values.clear();
m_values.squeeze();
```

### Q3: Clear后Y轴范围错误？

**说明：** updatePlot()中的rescaleValueAxis()会处理空数据。

**如有问题，手动重置：**
```cpp
if (m_timestamps.isEmpty()) {
    m_plot->yAxis->setRange(0, 1);  // 默认范围
}
```

## 性能考虑

**Clear操作的性能：**
- QVector::clear()是O(1)操作（只修改size，不释放内存）
- updatePlot()在空数据时很快
- 总耗时 < 1ms

**结论：** 性能无问题。

## 后续任务

完成此任务后，继续执行：
- TASK-011: 异常处理

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-006: 实现图表绘制](./TASK-006-图表绘制.md)
- [TASK-007: 实现右键删除](./TASK-007-右键删除.md)
- [Qt QVector文档](https://doc.qt.io/qt-5.14/qvector.html)
