# TASK-006: 实现图表绘制

## 任务信息

- **任务ID**: TASK-006
- **任务名称**: 实现图表绘制
- **优先级**: P0（必须完成）
- **预估工作量**: 5小时
- **依赖任务**: TASK-005
- **状态**: 待开始

## 任务描述

这是监控功能的核心任务！集成QCustomPlot到MonitorChart，实现实时数据绘制功能。包括数据缓存管理、时间戳转换、Y轴自动缩放等关键算法。

## 具体目标

1. 集成QCustomPlot到MonitorChart
2. 实现appendData()数据缓存逻辑
   - 添加新数据点到m_timestamps和m_values
   - 移除超出X轴范围的旧数据
3. 实现updatePlot()绘图逻辑
   - 转换时间戳为相对时间
   - 设置QCustomPlot数据
   - 配置X轴范围和刻度数量
   - Y轴自动缩放
4. 完善onDataReceived()
   - 调用appendData()

## 涉及文件

### 修改文件
- `common_component/plot/monitorchart.h`
- `common_component/plot/monitorchart.cpp`
- `common_component/plot.pro`（添加QCustomPlot依赖）

### 依赖库
- **QCustomPlot 2.1.0**（需要确认项目中是否已集成）

## 前置检查：QCustomPlot集成

在开始之前，请确认QCustomPlot已集成到项目中：

### 检查方法1：查找文件
```bash
# 在项目根目录搜索
find . -name "qcustomplot.h"
find . -name "qcustomplot.cpp"
```

### 检查方法2：查看.pro文件
```pro
# 查找是否有QCustomPlot相关配置
grep -r "qcustomplot" *.pro
```

### 如果未集成，需要添加：

1. 下载QCustomPlot 2.1.0（兼容Qt 5.14）
2. 将qcustomplot.h和qcustomplot.cpp添加到项目
3. 在plot.pro中添加：
```pro
HEADERS += $$PWD/qcustomplot.h
SOURCES += $$PWD/qcustomplot.cpp
```

## 实现细节

### 1. 修改MonitorChart.h

```cpp
// 添加包含
#include "qcustomplot.h"

class MonitorChart : public QWidget {
    // ... existing code ...

private:
    // ... existing members ...

    QCustomPlot *m_plot;               // 绘图组件
    QVector<qint64> m_timestamps;      // 时间戳缓存（毫秒）
    QVector<double> m_values;          // 数值缓存

    // 可选：添加用于测试的getter
    int dataPointCount() const { return m_timestamps.size(); }
};
```

### 2. 修改MonitorChart::setupUI()

```cpp
void MonitorChart::setupUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 标题栏
    QString title = m_unit.isEmpty() ? m_fieldName : QString("%1 (%2)").arg(m_fieldName, m_unit);
    QLabel *titleLabel = new QLabel(title, this);
    QFont font = titleLabel->font();
    font.setBold(true);
    titleLabel->setFont(font);
    mainLayout->addWidget(titleLabel);

    // ========== 替换占位Label为QCustomPlot ==========
    m_plot = new QCustomPlot(this);
    m_plot->setFixedHeight(200);  // 固定高度200像素
    mainLayout->addWidget(m_plot);

    // 配置QCustomPlot
    setupPlot();

    // 设置边框
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
}

void MonitorChart::setupPlot()
{
    // 添加图形
    m_plot->addGraph();

    // 设置曲线样式
    QPen pen;
    pen.setColor(QColor(0, 120, 215));  // 蓝色
    pen.setWidth(2);
    m_plot->graph(0)->setPen(pen);

    // 配置坐标轴
    m_plot->xAxis->setLabel("时间 (秒)");
    m_plot->yAxis->setLabel(m_unit.isEmpty() ? "数值" : m_unit);

    // X轴范围
    m_plot->xAxis->setRange(-m_xRangeSeconds, 0);

    // 启用抗锯齿
    m_plot->setAntialiasedElements(QCP::aeAll);

    // 设置背景色
    m_plot->setBackground(QBrush(Qt::white));

    // 允许用户交互（可选）
    m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}
```

### 3. 实现appendData()

```cpp
void MonitorChart::appendData(double value, qint64 timestamp)
{
    // 添加新数据点
    m_timestamps.append(timestamp);
    m_values.append(value);

    // 移除超出X轴范围的旧数据
    qint64 cutoffTime = timestamp - m_xRangeSeconds * 1000;  // 转换为毫秒
    while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
        m_timestamps.removeFirst();
        m_values.removeFirst();
    }

    // 可选：限制最大缓存点数（防止极端情况）
    const int MAX_POINTS = 10000;
    if (m_timestamps.size() > MAX_POINTS) {
        int removeCount = m_timestamps.size() - MAX_POINTS;
        m_timestamps.remove(0, removeCount);
        m_values.remove(0, removeCount);
    }

    // 更新图表
    updatePlot();
}
```

### 4. 实现updatePlot()

```cpp
void MonitorChart::updatePlot()
{
    if (m_timestamps.isEmpty()) {
        m_plot->graph(0)->data()->clear();
        m_plot->replot();
        return;
    }

    // 转换时间戳为相对时间（秒）
    QVector<double> xData, yData;
    qint64 latestTime = m_timestamps.last();

    for (int i = 0; i < m_timestamps.size(); ++i) {
        // 相对时间（秒），负数表示过去的时间
        double relativeTime = (m_timestamps[i] - latestTime) / 1000.0;
        xData.append(relativeTime);
        yData.append(m_values[i]);
    }

    // 更新QCustomPlot数据
    m_plot->graph(0)->setData(xData, yData);

    // 设置X轴范围
    m_plot->xAxis->setRange(-m_xRangeSeconds, 0);

    // Y轴自动缩放
    m_plot->graph(0)->rescaleValueAxis();

    // 设置X轴刻度数量
    QSharedPointer<QCPAxisTicker> ticker = m_plot->xAxis->ticker();
    ticker->setTickCount(m_xTickCount);

    // 重绘
    m_plot->replot();
}
```

### 5. 完善onDataReceived()

```cpp
void MonitorChart::onDataReceived(const QString &fieldName, double value, qint64 timestamp)
{
    // 字段过滤
    if (fieldName != m_fieldName) {
        return;
    }

    // 暂停检查
    if (m_paused) {
        return;
    }

    // 添加数据并绘制
    appendData(value, timestamp);
}
```

## 验收标准

- [ ] QCustomPlot成功集成，编译通过
- [ ] 接收数据后，图表实时绘制曲线
- [ ] X轴显示相对时间（如-60到0秒）
- [ ] X轴刻度数量符合配置（如2个刻度）
- [ ] Y轴自动缩放，跟随数据范围
- [ ] 超出X轴范围的数据自动移除
- [ ] 曲线平滑，无跳变
- [ ] 高频数据（100Hz）流畅显示
- [ ] 无内存泄漏

## 测试方法

### 功能测试

**测试场景1：基本绘制**
1. 配置IMU协议（Roll字段）
2. 添加Roll图表（X轴60秒，刻度2）
3. 开始接收数据
4. 观察图表

**预期结果：**
- 曲线从左向右绘制
- X轴显示-60到0秒
- X轴有2个刻度标签
- Y轴根据数据自动缩放

**测试场景2：数据缓存清理**
1. 添加图表，X轴范围10秒
2. 接收数据超过10秒
3. 观察内存占用

**预期结果：**
- 内存占用稳定，不持续增长
- 只保留10秒内的数据

**测试场景3：高频数据**
1. 配置100Hz协议
2. 添加图表
3. 接收数据1分钟

**预期结果：**
- 图表流畅刷新
- 无明显卡顿
- CPU占用合理（<20%）

**测试场景4：空数据处理**
1. 添加图表
2. 暂停数据接收
3. 观察图表

**预期结果：**
- 图表显示空白或保持最后状态
- 不崩溃

### 性能测试

```cpp
void TestMonitorChart::testPerformance()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 模拟100Hz数据，60秒 = 6000个点
    qint64 baseTime = QDateTime::currentMSecsSinceEpoch();
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 6000; ++i) {
        chart.appendData(qrand() % 360, baseTime + i * 10);
    }

    qint64 elapsed = timer.elapsed();
    qDebug() << "绘制6000个点耗时:" << elapsed << "ms";

    // 预期：<1000ms
    QVERIFY(elapsed < 1000);
}
```

### 内存测试

```cpp
void TestMonitorChart::testMemoryLeak()
{
    MonitorChart chart("Roll", "度", 10, 2);  // 10秒窗口

    size_t initialSize = chart.dataPointCount();

    // 添加100秒的数据（应该只保留10秒）
    qint64 baseTime = QDateTime::currentMSecsSinceEpoch();
    for (int i = 0; i < 10000; ++i) {
        chart.appendData(i, baseTime + i * 10);
    }

    size_t finalSize = chart.dataPointCount();

    // 验证：数据点数量不超过10秒@100Hz = 1000点
    QVERIFY(finalSize <= 1000);
    qDebug() << "最终数据点数:" << finalSize;
}
```

## 注意事项

### 1. QCustomPlot版本兼容性

**重要：** 必须使用QCustomPlot 2.1.0（兼容Qt 5.14）

**检查版本：**
```cpp
// 在qcustomplot.h中查找
#define QCUSTOMPLOT_VERSION 0x020100  // 2.1.0
```

**如果版本不对：**
- QCustomPlot 1.x：API差异较大，需要调整代码
- QCustomPlot 3.x：需要Qt 6，不兼容

### 2. 时间戳转换算法

**关键点：**
```cpp
// 最新时间作为0点
qint64 latestTime = m_timestamps.last();

// 历史时间为负值
double relativeTime = (m_timestamps[i] - latestTime) / 1000.0;
// 例如：60秒前 = -60.0
```

**为什么这样设计？**
- X轴范围设置为[-60, 0]，0在右侧
- 新数据始终出现在右侧（0点）
- 旧数据向左移动，超出-60被移除

### 3. Y轴自动缩放

```cpp
m_plot->graph(0)->rescaleValueAxis();
```

**工作原理：**
- 扫描当前X轴范围内的所有Y值
- 找到min和max
- 设置Y轴范围为[min - margin, max + margin]

**注意：** 如果数据跳变大，Y轴会自动调整，可能导致视觉抖动。

### 4. 性能优化建议（P2阶段）

**当前实现：**
- 每个数据点都触发replot()
- 高频数据可能卡顿

**优化方案（暂不实施）：**
```cpp
// 方案1：延迟刷新
QTimer *refreshTimer = new QTimer(this);
connect(refreshTimer, &QTimer::timeout, this, &MonitorChart::updatePlot);
refreshTimer->start(50);  // 50ms刷新一次

void MonitorChart::appendData(double value, qint64 timestamp) {
    m_timestamps.append(timestamp);
    m_values.append(value);
    // 不立即调用updatePlot()，等定时器触发
}

// 方案2：OpenGL加速
m_plot->setOpenGl(true);
```

### 5. Qt 5.14兼容性

**避免使用：**
```cpp
// ❌ Qt 6新增
m_timestamps.resize(size, 0);

// ✅ Qt 5.14兼容
m_timestamps.resize(size);
m_timestamps.fill(0);
```

## 调试技巧

### 1. 验证数据缓存

```cpp
void MonitorChart::appendData(double value, qint64 timestamp) {
    m_timestamps.append(timestamp);
    m_values.append(value);

    // 调试输出
    qDebug() << QString("缓存: %1个点, 最新值: %2, 时间戳: %3")
                .arg(m_timestamps.size())
                .arg(value)
                .arg(timestamp);

    // ...
}
```

### 2. 验证时间转换

```cpp
void MonitorChart::updatePlot() {
    // ...

    // 调试输出X轴范围
    qDebug() << "X轴数据范围:" << xData.first() << "~" << xData.last();

    // ...
}
```

### 3. 检查绘图性能

```cpp
void MonitorChart::updatePlot() {
    QElapsedTimer timer;
    timer.start();

    // ... 绘图代码 ...

    qint64 elapsed = timer.elapsed();
    if (elapsed > 50) {  // 超过50ms警告
        qWarning() << "绘图耗时过长:" << elapsed << "ms";
    }
}
```

## 常见问题

### Q1: 图表不显示数据？

**检查清单：**
- [ ] QCustomPlot是否成功添加到布局？
- [ ] m_plot->addGraph()是否调用？
- [ ] setData()是否正确调用？
- [ ] replot()是否调用？
- [ ] 数据范围是否合理？

### Q2: 曲线显示断裂？

**可能原因：**
- 时间戳不连续
- 数据缺失

**解决方案：**
- 使用QCPGraph::lsLine保证连线
- 检查数据源

### Q3: 性能卡顿？

**优化步骤：**
1. 降低刷新率（50ms一次）
2. 启用OpenGL加速
3. 降采样数据

## 后续任务

完成此任务后，继续执行：
- TASK-007: 实现右键删除（MVP完成）

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-005: 连接数据流](./TASK-005-连接数据流.md)
- [QCustomPlot官方文档](https://www.qcustomplot.com/documentation/)
- [QCustomPlot示例](https://www.qcustomplot.com/index.php/demos/realtimedatademo)
