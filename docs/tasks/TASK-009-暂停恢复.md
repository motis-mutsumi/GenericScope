# TASK-009: 实现暂停/恢复

## 任务信息

- **任务ID**: TASK-009
- **任务名称**: 实现暂停/恢复
- **优先级**: P1（重要功能）
- **预估工作量**: 1小时
- **依赖任务**: TASK-007
- **状态**: 待开始

## 任务描述

实现图表的暂停和恢复功能。暂停时停止数据接收和缓存，恢复后继续接收新数据。这是一个简单但实用的功能。

## 具体目标

1. 完善onPauseAction()槽函数
2. 更新右键菜单文本（暂停/恢复动态切换）
3. 在onDataReceived()中检查暂停状态
4. 测试暂停和恢复功能

## 涉及文件

### 修改文件
- `common_component/plot/monitorchart.cpp`

## 实现细节

### 1. 完善onPauseAction()

在`monitorchart.cpp`中（TASK-001已创建框架）：

```cpp
void MonitorChart::onPauseAction()
{
    // 切换暂停状态
    m_paused = !m_paused;

    // 可选：在标题上显示暂停标识
    updatePausedIndicator();
}
```

### 2. 新增暂停标识显示（可选）

在`monitorchart.h`中添加：

```cpp
private:
    void updatePausedIndicator();
```

在`monitorchart.cpp`中实现：

```cpp
void MonitorChart::updatePausedIndicator()
{
    QLabel *titleLabel = findChild<QLabel*>("titleLabel");
    if (!titleLabel) {
        return;
    }

    QString baseTitle = m_unit.isEmpty()
                        ? m_fieldName
                        : QString("%1 (%2)").arg(m_fieldName, m_unit);

    if (m_paused) {
        titleLabel->setText(baseTitle + " [已暂停]");
        titleLabel->setStyleSheet("color: gray;");
    } else {
        titleLabel->setText(baseTitle);
        titleLabel->setStyleSheet("");
    }
}
```

### 3. 确认onDataReceived()中的暂停检查

在`monitorchart.cpp`中（TASK-005已实现）：

```cpp
void MonitorChart::onDataReceived(const QString &fieldName, double value, qint64 timestamp)
{
    // 字段过滤
    if (fieldName != m_fieldName) {
        return;
    }

    // 暂停检查（关键！）
    if (m_paused) {
        return;  // 暂停时直接返回，不缓存数据
    }

    // 添加数据并绘制
    appendData(value, timestamp);
}
```

**说明：** 这段代码已经在TASK-005中实现，无需修改。

### 4. 更新contextMenuEvent()

在`monitorchart.cpp`中（TASK-001已实现）：

```cpp
void MonitorChart::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_editAction);
    menu.addAction(m_pauseAction);
    menu.addAction(m_clearAction);
    menu.addSeparator();
    menu.addAction(m_deleteAction);

    // 更新暂停按钮文本（关键！）
    m_pauseAction->setText(m_paused ? "恢复" : "暂停");

    menu.exec(event->globalPos());
}
```

**说明：** 这段代码已经在TASK-001中实现，无需修改。

## 验收标准

- [ ] 点击"暂停"，图表停止更新
- [ ] 菜单文本变为"恢复"
- [ ] 标题显示"[已暂停]"标识（如果实现）
- [ ] 暂停期间，数据不缓存（验证dataPointCount不增加）
- [ ] 点击"恢复"，图表继续更新
- [ ] 菜单文本变为"暂停"
- [ ] 标题移除"[已暂停]"标识
- [ ] 恢复后接收的是新数据（暂停期间的数据丢失）

## 测试方法

### 功能测试

**测试场景1：基本暂停恢复**

1. 添加Roll图表
2. 开始接收数据，观察曲线绘制
3. 右键点击"暂停"

**预期结果：**
- 曲线停止绘制
- 标题显示"Roll (度) [已暂停]"
- 菜单文本变为"恢复"

4. 继续接收数据（等待5秒）
5. 观察曲线

**预期结果：**
- 曲线保持静止，不更新

6. 右键点击"恢复"

**预期结果：**
- 曲线继续绘制新数据
- 标题恢复为"Roll (度)"
- 菜单文本变为"暂停"
- 暂停期间的数据丢失（曲线有间隙）

**测试场景2：暂停期间数据不缓存**

1. 添加Roll图表
2. 接收100个数据点
3. 暂停
4. 继续接收100个数据点
5. 验证数据点数量

**预期结果：**
- 数据点数量仍为100（暂停期间的100个点未缓存）

**测试场景3：多图表独立暂停**

1. 添加Roll和Pitch两个图表
2. 开始接收数据
3. 暂停Roll图表

**预期结果：**
- Roll图表停止更新
- Pitch图表继续更新
- 两个图表互不影响

### 单元测试

```cpp
void TestMonitorChart::testPause()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 添加数据
    chart.appendData(10.0, 1000);
    QCOMPARE(chart.dataPointCount(), 1);

    // 暂停
    chart.setPaused(true);
    QVERIFY(chart.isPaused());

    // 尝试添加数据（通过onDataReceived）
    MonitorDataManager::instance()->onProtocolDataParsed({{"Roll", 20.0}});

    // 等待信号处理
    QTest::qWait(100);

    // 验证：数据未增加
    QCOMPARE(chart.dataPointCount(), 1);

    // 恢复
    chart.setPaused(false);
    QVERIFY(!chart.isPaused());

    // 添加数据
    MonitorDataManager::instance()->onProtocolDataParsed({{"Roll", 30.0}});
    QTest::qWait(100);

    // 验证：数据增加
    QCOMPARE(chart.dataPointCount(), 2);
}

void TestMonitorChart::testPauseToggle()
{
    MonitorChart chart("Roll", "度", 60, 2);

    QVERIFY(!chart.isPaused());

    // 模拟点击暂停
    chart.setPaused(true);
    QVERIFY(chart.isPaused());

    // 再次点击（恢复）
    chart.setPaused(false);
    QVERIFY(!chart.isPaused());
}
```

## 注意事项

### 1. 暂停时数据丢失

**行为：**
- 暂停期间，数据不缓存
- 恢复后只接收新数据
- 暂停期间的数据永久丢失

**这是正常的设计！**

**如果需要保留数据：**
```cpp
void MonitorChart::onDataReceived(...) {
    if (fieldName != m_fieldName) return;

    if (m_paused) {
        // 方案：仍然缓存，但不绘制
        m_timestamps.append(timestamp);
        m_values.append(value);
        return;  // 不调用updatePlot()
    }

    appendData(value, timestamp);
}
```

### 2. 暂停状态初始化

**确保初始状态为false：**

```cpp
MonitorChart::MonitorChart(...)
    : m_paused(false)  // 初始未暂停
{
    // ...
}
```

### 3. 菜单文本动态更新

**关键代码：**

```cpp
m_pauseAction->setText(m_paused ? "恢复" : "暂停");
```

**必须在contextMenuEvent()中更新，而不是onPauseAction()中！**

**原因：** 菜单每次右键时重新构建，需要实时读取状态。

### 4. 标题标识的样式

**可选实现：**

```cpp
if (m_paused) {
    titleLabel->setStyleSheet("color: gray;");  // 灰色表示暂停
}
```

**更丰富的样式：**

```cpp
if (m_paused) {
    titleLabel->setStyleSheet(
        "color: #888888; "
        "font-style: italic;"
    );
}
```

## 扩展功能（可选）

### 1. 快捷键支持

```cpp
// 在构造函数中
QShortcut *pauseShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
connect(pauseShortcut, &QShortcut::activated, this, &MonitorChart::onPauseAction);
```

### 2. 暂停时显示提示文本

```cpp
void MonitorChart::updatePlot()
{
    if (m_paused) {
        // 在图表上显示"已暂停"文本
        QCPItemText *textLabel = new QCPItemText(m_plot);
        textLabel->setText("已暂停");
        textLabel->setFont(QFont(font().family(), 16));
        textLabel->setColor(QColor(128, 128, 128));
        textLabel->position->setType(QCPItemPosition::ptPlotCoords);
        textLabel->position->setCoords(0, 0);  // 中心位置
        m_plot->replot();
        return;
    }

    // 正常绘制逻辑...
}
```

### 3. 暂停统计

```cpp
// 记录暂停次数和时长
int m_pauseCount;
QElapsedTimer m_pauseTimer;

void MonitorChart::onPauseAction() {
    m_paused = !m_paused;

    if (m_paused) {
        m_pauseCount++;
        m_pauseTimer.start();
    } else {
        qint64 pauseDuration = m_pauseTimer.elapsed();
        qDebug() << "暂停时长:" << pauseDuration << "ms";
    }
}
```

## 常见问题

### Q1: 恢复后曲线有断点？

**答：** 这是正常的！暂停期间数据丢失，曲线自然有间隙。

**如不希望有间隙：** 暂停时也缓存数据（见注意事项1）。

### Q2: 暂停后内存仍在增长？

**检查：** onDataReceived()中是否正确return。

### Q3: 菜单文本不更新？

**检查：** setText()是否在contextMenuEvent()中调用。

## 后续任务

完成此任务后，继续执行：
- TASK-010: 实现Clear数据

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-007: 实现右键删除](./TASK-007-右键删除.md)
- [TASK-010: 实现Clear数据](./TASK-010-Clear数据.md)
