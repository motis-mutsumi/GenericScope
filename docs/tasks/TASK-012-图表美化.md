# TASK-012: 图表美化

## 任务信息

- **任务ID**: TASK-012
- **任务名称**: 图表美化
- **优先级**: P2（优化功能）
- **预估工作量**: 3小时
- **依赖任务**: TASK-006
- **状态**: 待开始

## 任务描述

优化图表的视觉效果，提升用户体验。包括添加网格线、图例、鼠标悬停提示、优化配色等。这是P2阶段的优化任务，非必需但能显著提升用户体验。

## 具体目标

1. 添加网格线（X轴和Y轴）
2. 添加图例（显示字段名+单位）
3. 添加鼠标悬停提示（显示当前点的时间和数值）
4. 优化配色（支持亮色/暗色主题）
5. 曲线颜色自动分配（多图表时颜色不同）

## 涉及文件

### 修改文件
- `common_component/plot/monitorchart.cpp`

## 实现细节

### 1. 添加网格线

在`monitorchart.cpp`的setupPlot()中添加：

```cpp
void MonitorChart::setupPlot()
{
    // ... existing code ...

    // ========== 新增：网格线 ==========
    // X轴网格线
    m_plot->xAxis->grid()->setVisible(true);
    m_plot->xAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));

    // Y轴网格线
    m_plot->yAxis->grid()->setVisible(true);
    m_plot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));

    // 子网格线（可选）
    m_plot->xAxis->grid()->setSubGridVisible(true);
    m_plot->xAxis->grid()->setSubGridPen(QPen(QColor(230, 230, 230), 1, Qt::DotLine));

    m_plot->yAxis->grid()->setSubGridVisible(true);
    m_plot->yAxis->grid()->setSubGridPen(QPen(QColor(230, 230, 230), 1, Qt::DotLine));
    // ==================================

    // ... existing code ...
}
```

### 2. 添加图例

```cpp
void MonitorChart::setupPlot()
{
    // ... existing code ...

    // ========== 新增：图例 ==========
    m_plot->legend->setVisible(true);
    m_plot->legend->setFont(QFont("sans", 9));
    m_plot->legend->setRowSpacing(-3);

    // 图例位置（右上角）
    m_plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

    // 图例内容
    QString legendText = m_unit.isEmpty()
                         ? m_fieldName
                         : QString("%1 (%2)").arg(m_fieldName, m_unit);
    m_plot->graph(0)->setName(legendText);
    // ==================================

    // ... existing code ...
}
```

### 3. 添加鼠标悬停提示

在`monitorchart.h`中添加：

```cpp
private slots:
    void showTooltip(QMouseEvent *event);

private:
    QCPItemTracer *m_tracer;  // 跟踪器
    QCPItemText *m_tooltip;   // 提示文本
```

在`monitorchart.cpp`中：

```cpp
void MonitorChart::setupPlot()
{
    // ... existing code ...

    // ========== 新增：鼠标悬停提示 ==========
    // 创建跟踪器
    m_tracer = new QCPItemTracer(m_plot);
    m_tracer->setGraph(m_plot->graph(0));
    m_tracer->setInterpolating(true);
    m_tracer->setStyle(QCPItemTracer::tsCircle);
    m_tracer->setPen(QPen(Qt::red));
    m_tracer->setBrush(Qt::red);
    m_tracer->setSize(7);
    m_tracer->setVisible(false);

    // 创建提示文本
    m_tooltip = new QCPItemText(m_plot);
    m_tooltip->setLayer("overlay");
    m_tooltip->setClipToAxisRect(false);
    m_tooltip->setPadding(QMargins(8, 4, 8, 4));
    m_tooltip->setBrush(QBrush(QColor(255, 255, 220, 200)));
    m_tooltip->setPen(QPen(Qt::black));
    m_tooltip->setFont(QFont("sans", 9));
    m_tooltip->setVisible(false);

    // 连接鼠标移动事件
    connect(m_plot, &QCustomPlot::mouseMove,
            this, &MonitorChart::showTooltip);
    // ========================================
}

void MonitorChart::showTooltip(QMouseEvent *event)
{
    if (m_timestamps.isEmpty()) {
        m_tracer->setVisible(false);
        m_tooltip->setVisible(false);
        m_plot->replot();
        return;
    }

    // 获取鼠标位置对应的坐标
    double x = m_plot->xAxis->pixelToCoord(event->pos().x());
    double y = m_plot->yAxis->pixelToCoord(event->pos().y());

    // 查找最近的数据点
    m_tracer->setGraphKey(x);
    m_tracer->updatePosition();

    // 获取数据点的值
    double valueX = m_tracer->position->key();
    double valueY = m_tracer->position->value();

    // 检查鼠标是否在图表范围内
    if (m_plot->xAxis->range().contains(valueX) &&
        m_plot->yAxis->range().contains(valueY)) {

        // 显示跟踪器和提示
        m_tracer->setVisible(true);
        m_tooltip->setVisible(true);

        // 设置提示文本
        QString tooltipText = QString("时间: %1s\n数值: %2 %3")
                              .arg(valueX, 0, 'f', 2)
                              .arg(valueY, 0, 'f', 3)
                              .arg(m_unit);
        m_tooltip->setText(tooltipText);

        // 设置提示位置（鼠标右上方）
        m_tooltip->position->setType(QCPItemPosition::ptPlotCoords);
        m_tooltip->position->setCoords(valueX, valueY);
        m_tooltip->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);

        m_plot->replot();
    } else {
        m_tracer->setVisible(false);
        m_tooltip->setVisible(false);
        m_plot->replot();
    }
}
```

### 4. 优化配色

```cpp
void MonitorChart::setupPlot()
{
    // ... existing code ...

    // ========== 新增：配色优化 ==========
    // 背景色
    m_plot->setBackground(QBrush(Qt::white));

    // 坐标轴颜色
    m_plot->xAxis->setBasePen(QPen(Qt::black, 1));
    m_plot->xAxis->setTickPen(QPen(Qt::black, 1));
    m_plot->xAxis->setSubTickPen(QPen(Qt::black, 1));
    m_plot->xAxis->setTickLabelColor(Qt::black);

    m_plot->yAxis->setBasePen(QPen(Qt::black, 1));
    m_plot->yAxis->setTickPen(QPen(Qt::black, 1));
    m_plot->yAxis->setSubTickPen(QPen(Qt::black, 1));
    m_plot->yAxis->setTickLabelColor(Qt::black);

    // 坐标轴标签字体
    QFont labelFont;
    labelFont.setPointSize(10);
    m_plot->xAxis->setLabelFont(labelFont);
    m_plot->yAxis->setLabelFont(labelFont);

    // 刻度标签字体
    QFont tickFont;
    tickFont.setPointSize(9);
    m_plot->xAxis->setTickLabelFont(tickFont);
    m_plot->yAxis->setTickLabelFont(tickFont);
    // ==================================
}
```

### 5. 曲线颜色自动分配

在`monitorpanel.cpp`中添加颜色管理：

```cpp
class MonitorPanel {
private:
    static const QVector<QColor> kChartColors;
    int m_nextColorIndex;
};

// 定义颜色表
const QVector<QColor> MonitorPanel::kChartColors = {
    QColor(0, 120, 215),    // 蓝色
    QColor(232, 17, 35),    // 红色
    QColor(0, 153, 68),     // 绿色
    QColor(255, 140, 0),    // 橙色
    QColor(142, 68, 173),   // 紫色
    QColor(0, 174, 219),    // 青色
    QColor(255, 185, 0),    // 黄色
    QColor(136, 23, 152),   // 深紫色
};

MonitorPanel::MonitorPanel(QWidget *parent)
    : QWidget(parent)
    , m_nextColorIndex(0)
{
    setupUI();
}

void MonitorPanel::addChart(const QString &fieldName,
                            const QString &unit,
                            int xRangeSeconds,
                            int xTickCount)
{
    auto *chart = new MonitorChart(fieldName, unit, xRangeSeconds, xTickCount, this);

    // 分配颜色
    QColor color = kChartColors[m_nextColorIndex % kChartColors.size()];
    chart->setLineColor(color);
    m_nextColorIndex++;

    // ... existing code ...
}
```

在`monitorchart.h`中添加：

```cpp
public:
    void setLineColor(const QColor &color);

private:
    QColor m_lineColor;
```

在`monitorchart.cpp`中：

```cpp
void MonitorChart::setLineColor(const QColor &color)
{
    m_lineColor = color;

    QPen pen;
    pen.setColor(color);
    pen.setWidth(2);
    m_plot->graph(0)->setPen(pen);
    m_plot->replot();
}
```

## 验收标准

- [ ] 图表显示X轴和Y轴网格线
- [ ] 网格线颜色浅灰色，不抢眼
- [ ] 右上角显示图例（字段名+单位）
- [ ] 鼠标悬停在曲线上，显示数据点信息
- [ ] 提示框显示时间和数值
- [ ] 多个图表使用不同颜色
- [ ] 配色协调美观
- [ ] 支持亮色主题

## 测试方法

### 视觉测试

**测试场景1：网格线**
- 添加图表
- 观察网格线是否显示
- 验证颜色浅灰色
- 验证线型为虚线

**测试场景2：图例**
- 添加图表
- 观察右上角图例
- 验证显示字段名和单位

**测试场景3：鼠标悬停**
- 添加图表，接收数据
- 鼠标移动到曲线上
- 验证显示提示框
- 验证提示内容正确

**测试场景4：多图表颜色**
- 添加5个图表
- 验证颜色各不相同
- 验证颜色搭配协调

## 注意事项

### 1. 性能影响

**鼠标悬停提示可能影响性能：**
- 每次鼠标移动都会触发
- 需要重绘图表

**优化方案：**
```cpp
// 限制刷新频率
QTimer *tooltipTimer = new QTimer(this);
tooltipTimer->setSingleShot(true);
connect(m_plot, &QCustomPlot::mouseMove, [this, tooltipTimer](QMouseEvent *event) {
    if (!tooltipTimer->isActive()) {
        showTooltip(event);
        tooltipTimer->start(50);  // 50ms内不重复处理
    }
});
```

### 2. 暗色主题支持

```cpp
void MonitorChart::setDarkTheme(bool dark)
{
    if (dark) {
        m_plot->setBackground(QBrush(QColor(30, 30, 30)));
        m_plot->xAxis->setBasePen(QPen(Qt::white, 1));
        m_plot->xAxis->setTickLabelColor(Qt::white);
        // ...
    } else {
        // 亮色主题
    }
}
```

## 后续任务

完成此任务后，继续执行：
- TASK-013: 集成测试

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-006: 实现图表绘制](./TASK-006-图表绘制.md)
- [QCustomPlot美化示例](https://www.qcustomplot.com/index.php/demos/barchartdemo)
