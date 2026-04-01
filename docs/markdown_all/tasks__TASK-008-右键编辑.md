# TASK-008: 实现右键编辑

## 任务信息

- **任务ID**: TASK-008
- **任务名称**: 实现右键编辑
- **优先级**: P1（重要功能）
- **预估工作量**: 2小时
- **依赖任务**: TASK-007
- **状态**: 待开始

## 任务描述

实现图表的编辑功能，允许用户修改监控字段、X轴范围和刻度数量。编辑后图表立即更新，无需删除重建。

## 具体目标

1. 完善onEditAction()槽函数
2. 在MonitorPanel中实现onChartEditRequested()
3. 实现MonitorChart的配置更新方法
   - setFieldName()：更新字段，重新订阅数据
   - setXRange()：更新X轴范围
   - setXTickCount()：更新刻度数量
4. 更新图表标题

## 涉及文件

### 修改文件
- `common_component/plot/monitorchart.cpp`
- `common_component/plot/monitorpanel.cpp`

## 实现细节

### 1. 完善MonitorChart::onEditAction()

在`monitorchart.cpp`中：

```cpp
void MonitorChart::onEditAction()
{
    // 发射编辑请求信号
    emit editRequested(this);
}
```

### 2. 实现MonitorPanel::onChartEditRequested()

在`monitorpanel.cpp`中：

```cpp
void MonitorPanel::onChartEditRequested(MonitorChart *chart)
{
    if (!chart) {
        return;
    }

    // 获取当前协议的字段列表（同addChart逻辑）
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

    // 创建配置对话框（编辑模式）
    MonitorConfigDialog dialog(this);
    dialog.setAvailableFields(fields);
    dialog.setEditMode(chart->fieldName(),
                       chart->unit(),
                       chart->xRangeSeconds(),
                       chart->xTickCount());

    if (dialog.exec() == QDialog::Accepted) {
        // 更新图表配置
        chart->setFieldName(dialog.selectedField());
        chart->setUnit(dialog.selectedUnit());
        chart->setXRange(dialog.xRangeSeconds());
        chart->setXTickCount(dialog.xTickCount());
    }
}
```

### 3. 实现MonitorChart::setFieldName()

在`monitorchart.cpp`中完善：

```cpp
void MonitorChart::setFieldName(const QString &fieldName)
{
    if (m_fieldName == fieldName) {
        return;  // 没有变化
    }

    m_fieldName = fieldName;

    // 更新标题
    updateTitle();

    // 清空旧数据（字段变了，旧数据无意义）
    clearData();
}
```

### 4. 新增setUnit()方法

在`monitorchart.h`中添加声明：

```cpp
/**
 * @brief 设置单位
 * @param unit 新单位
 */
void setUnit(const QString &unit);
```

在`monitorchart.cpp`中实现：

```cpp
void MonitorChart::setUnit(const QString &unit)
{
    if (m_unit == unit) {
        return;
    }

    m_unit = unit;

    // 更新标题和Y轴标签
    updateTitle();
    m_plot->yAxis->setLabel(m_unit.isEmpty() ? "数值" : m_unit);
    m_plot->replot();
}
```

### 5. 实现setXRange()

在`monitorchart.cpp`中完善：

```cpp
void MonitorChart::setXRange(int seconds)
{
    if (m_xRangeSeconds == seconds) {
        return;
    }

    m_xRangeSeconds = seconds;

    // 清理超出新范围的旧数据
    if (!m_timestamps.isEmpty()) {
        qint64 latestTime = m_timestamps.last();
        qint64 cutoffTime = latestTime - m_xRangeSeconds * 1000;

        while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
            m_timestamps.removeFirst();
            m_values.removeFirst();
        }
    }

    // 更新图表
    updatePlot();
}
```

### 6. 实现setXTickCount()

在`monitorchart.cpp`中完善：

```cpp
void MonitorChart::setXTickCount(int count)
{
    if (m_xTickCount == count) {
        return;
    }

    m_xTickCount = count;

    // 更新图表
    updatePlot();
}
```

### 7. 新增updateTitle()方法

在`monitorchart.h`中添加：

```cpp
private:
    void updateTitle();
```

在`monitorchart.cpp`中实现：

```cpp
void MonitorChart::updateTitle()
{
    QString title = m_unit.isEmpty()
                    ? m_fieldName
                    : QString("%1 (%2)").arg(m_fieldName, m_unit);

    // 找到标题Label并更新（假设它是第一个子控件）
    QLabel *titleLabel = findChild<QLabel*>("titleLabel");
    if (titleLabel) {
        titleLabel->setText(title);
    }
}
```

**注意：** 需要在setupUI()中给标题Label设置对象名：

```cpp
void MonitorChart::setupUI()
{
    // ...
    QLabel *titleLabel = new QLabel(title, this);
    titleLabel->setObjectName("titleLabel");  // 新增
    // ...
}
```

## 验收标准

- [ ] 点击"编辑"弹出配置对话框
- [ ] 对话框显示当前配置（字段、X轴范围、刻度数）
- [ ] 修改字段后，图表显示新字段名称
- [ ] 修改字段后，清空旧数据，开始接收新字段数据
- [ ] 修改X轴范围后，图表范围立即变化
- [ ] 修改刻度数后，刻度显示立即变化
- [ ] 修改单位后，标题和Y轴标签更新
- [ ] 点击取消，配置不变

## 测试方法

### 功能测试

**测试场景1：修改字段**

1. 添加Roll图表
2. 接收数据，观察曲线
3. 右键编辑，将字段改为Pitch
4. 确认

**预期结果：**
- 标题变为"Pitch (度)"
- 旧数据清空
- 开始显示Pitch数据

**测试场景2：修改X轴范围**

1. 添加Roll图表（X轴60秒）
2. 接收数据
3. 右键编辑，将X轴改为30秒
4. 确认

**预期结果：**
- X轴范围变为-30~0秒
- 超出30秒的旧数据被移除
- 继续接收新数据

**测试场景3：修改刻度数**

1. 添加图表（刻度数2）
2. 右键编辑，改为5
3. 确认

**预期结果：**
- X轴显示5个刻度标签

**测试场景4：取消编辑**

1. 添加Roll图表
2. 右键编辑，修改配置
3. 点击取消

**预期结果：**
- 配置不变
- 图表继续显示Roll数据

### 单元测试

```cpp
void TestMonitorChart::testEditField()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 添加一些数据
    chart.appendData(45.2, 1000);
    chart.appendData(45.3, 2000);
    QCOMPARE(chart.dataPointCount(), 2);

    // 修改字段
    chart.setFieldName("Pitch");

    // 验证字段名变化
    QCOMPARE(chart.fieldName(), "Pitch");

    // 验证数据被清空
    QCOMPARE(chart.dataPointCount(), 0);
}

void TestMonitorChart::testEditXRange()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 添加60秒的数据
    qint64 baseTime = QDateTime::currentMSecsSinceEpoch();
    for (int i = 0; i < 60; ++i) {
        chart.appendData(i, baseTime + i * 1000);
    }

    // 验证有60个点
    QCOMPARE(chart.dataPointCount(), 60);

    // 修改X轴范围为30秒
    chart.setXRange(30);

    // 验证只保留30个点
    QCOMPARE(chart.dataPointCount(), 30);
}

void TestMonitorChart::testEditXTickCount()
{
    MonitorChart chart("Roll", "度", 60, 2);

    QCOMPARE(chart.xTickCount(), 2);

    chart.setXTickCount(5);

    QCOMPARE(chart.xTickCount(), 5);
}
```

## 注意事项

### 1. 字段变更时清空数据

**为什么要清空？**
- 字段变了，旧数据无意义
- 避免Y轴范围错误

**实现：**
```cpp
void MonitorChart::setFieldName(const QString &fieldName) {
    // ...
    clearData();  // 清空
}
```

### 2. X轴范围变更时清理数据

**只移除超出范围的数据，保留有效数据：**

```cpp
void MonitorChart::setXRange(int seconds) {
    // ...
    // 只移除超出新范围的数据
    while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
        m_timestamps.removeFirst();
        m_values.removeFirst();
    }
}
```

### 3. 刻度数变更

**只需更新配置，重绘即可：**

```cpp
void MonitorChart::setXTickCount(int count) {
    m_xTickCount = count;
    updatePlot();  // 重绘时会应用新刻度数
}
```

### 4. 标题更新

**使用findChild查找Label：**

```cpp
QLabel *titleLabel = findChild<QLabel*>("titleLabel");
```

**前提：** 必须在创建时设置objectName：

```cpp
titleLabel->setObjectName("titleLabel");
```

### 5. 数据订阅不需要重新连接

**原因：**
- MonitorChart订阅的是所有字段的dataUpdated信号
- 在onDataReceived()中通过字段名过滤
- 字段变更后，自动接收新字段数据

**不需要：**
```cpp
// ❌ 不需要
disconnect(...);
connect(...);
```

## 常见问题

### Q1: 编辑后数据不更新？

**检查：**
- onDataReceived()的字段过滤逻辑
- setFieldName()是否正确更新m_fieldName

### Q2: X轴范围变化不生效？

**检查：**
- updatePlot()是否调用
- m_plot->xAxis->setRange()是否正确

### Q3: 标题不更新？

**检查：**
- titleLabel的objectName是否设置
- findChild()是否找到Label
- setText()是否调用

## 后续任务

完成此任务后，继续执行：
- TASK-009: 实现暂停/恢复
- TASK-010: 实现Clear数据

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-002: 实现配置对话框](./TASK-002-配置对话框.md)
- [TASK-007: 实现右键删除](./TASK-007-右键删除.md)
- [Qt findChild()文档](https://doc.qt.io/qt-5.14/qobject.html#findChild)
