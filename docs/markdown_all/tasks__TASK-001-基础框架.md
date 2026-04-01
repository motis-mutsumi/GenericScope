# TASK-001: 搭建基础框架

## 任务信息

- **任务ID**: TASK-001
- **任务名称**: 搭建基础框架
- **优先级**: P0（必须完成）
- **预估工作量**: 4小时
- **依赖任务**: 无
- **状态**: 待开始

## 任务描述

创建MonitorPanel和MonitorChart的基础UI框架，搭建监控面板的骨架结构。这是整个监控功能的基础，所有后续任务都依赖于此。

## 具体目标

1. 创建MonitorPanel类，实现基本UI框架
   - 顶部"+"按钮
   - QScrollArea滚动区域
   - QVBoxLayout纵向布局

2. 创建MonitorChart类，实现图表容器
   - 标题栏（显示字段名+单位）
   - QCustomPlot占位（暂时显示空白）
   - 基本样式设置

3. 实现addChart()和removeChart()基本功能

4. 更新plot.pro添加新文件

## 涉及文件

### 新增文件
- `common_component/plot/monitorpanel.h`
- `common_component/plot/monitorpanel.cpp`
- `common_component/plot/monitorchart.h`
- `common_component/plot/monitorchart.cpp`

### 修改文件
- `common_component/plot.pro`

## 实现细节

### 1. MonitorPanel.h

```cpp
#ifndef MONITORPANEL_H
#define MONITORPANEL_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QList>

class MonitorChart;

class MonitorPanel : public QWidget {
    Q_OBJECT
public:
    explicit MonitorPanel(QWidget *parent = nullptr);
    ~MonitorPanel();

    /**
     * @brief 添加监控图表
     * @param fieldName 字段名称
     * @param unit 单位
     * @param xRangeSeconds X轴时间范围（秒）
     * @param xTickCount X轴刻度数量
     */
    void addChart(const QString &fieldName,
                  const QString &unit,
                  int xRangeSeconds,
                  int xTickCount);

    /**
     * @brief 移除指定图表
     * @param chart 要移除的图表指针
     */
    void removeChart(MonitorChart *chart);

    /**
     * @brief 清空所有图表
     */
    void clearAllCharts();

private slots:
    void onAddButtonClicked();
    void onChartDeleteRequested(MonitorChart *chart);
    void onChartEditRequested(MonitorChart *chart);

private:
    void setupUI();

    QPushButton *m_addButton;          // 添加按钮
    QScrollArea *m_scrollArea;         // 滚动区域
    QWidget *m_contentWidget;          // 内容容器
    QVBoxLayout *m_contentLayout;      // 纵向布局
    QList<MonitorChart*> m_charts;     // 图表列表
};

#endif // MONITORPANEL_H
```

### 2. MonitorPanel.cpp

```cpp
#include "monitorpanel.h"
#include "monitorchart.h"
#include <QVBoxLayout>

MonitorPanel::MonitorPanel(QWidget *parent)
    : QWidget(parent)
    , m_addButton(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_contentLayout(nullptr)
{
    setupUI();
}

MonitorPanel::~MonitorPanel()
{
    clearAllCharts();
}

void MonitorPanel::setupUI()
{
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    // 添加按钮
    m_addButton = new QPushButton("+ 添加监控", this);
    mainLayout->addWidget(m_addButton);

    // 滚动区域
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainLayout->addWidget(m_scrollArea);

    // 内容容器
    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(10);
    m_contentLayout->addStretch();  // 底部弹簧，保持图表紧凑

    m_scrollArea->setWidget(m_contentWidget);

    // 连接信号
    connect(m_addButton, &QPushButton::clicked,
            this, &MonitorPanel::onAddButtonClicked);
}

void MonitorPanel::addChart(const QString &fieldName,
                            const QString &unit,
                            int xRangeSeconds,
                            int xTickCount)
{
    auto *chart = new MonitorChart(fieldName, unit, xRangeSeconds, xTickCount, this);

    // 连接信号
    connect(chart, &MonitorChart::deleteRequested,
            this, &MonitorPanel::onChartDeleteRequested);
    connect(chart, &MonitorChart::editRequested,
            this, &MonitorPanel::onChartEditRequested);

    // 添加到布局（在弹簧之前）
    m_contentLayout->insertWidget(m_contentLayout->count() - 1, chart);
    m_charts.append(chart);
}

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

void MonitorPanel::clearAllCharts()
{
    // 复制列表，避免遍历时修改
    QList<MonitorChart*> charts = m_charts;
    for (auto *chart : charts) {
        removeChart(chart);
    }
}

void MonitorPanel::onAddButtonClicked()
{
    // TODO: TASK-003中实现
    // 弹出配置对话框，添加图表
}

void MonitorPanel::onChartDeleteRequested(MonitorChart *chart)
{
    removeChart(chart);
}

void MonitorPanel::onChartEditRequested(MonitorChart *chart)
{
    // TODO: TASK-008中实现
    // 弹出编辑对话框
}
```

### 3. MonitorChart.h

```cpp
#ifndef MONITORCHART_H
#define MONITORCHART_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QMenu>
#include <QAction>

class QCustomPlot;

class MonitorChart : public QWidget {
    Q_OBJECT
public:
    explicit MonitorChart(const QString &fieldName,
                          const QString &unit,
                          int xRangeSeconds,
                          int xTickCount,
                          QWidget *parent = nullptr);
    ~MonitorChart();

    // 配置访问器
    QString fieldName() const { return m_fieldName; }
    QString unit() const { return m_unit; }
    int xRangeSeconds() const { return m_xRangeSeconds; }
    int xTickCount() const { return m_xTickCount; }
    bool isPaused() const { return m_paused; }

    /**
     * @brief 设置监控字段
     * @param fieldName 新字段名
     */
    void setFieldName(const QString &fieldName);

    /**
     * @brief 设置X轴时间范围
     * @param seconds 时间范围（秒）
     */
    void setXRange(int seconds);

    /**
     * @brief 设置X轴刻度数量
     * @param count 刻度数量
     */
    void setXTickCount(int count);

    /**
     * @brief 添加数据点
     * @param value 数值
     * @param timestamp 时间戳（毫秒）
     */
    void appendData(double value, qint64 timestamp);

    /**
     * @brief 清空数据缓存
     */
    void clearData();

    /**
     * @brief 设置暂停状态
     * @param paused true=暂停，false=恢复
     */
    void setPaused(bool paused);

signals:
    void deleteRequested(MonitorChart *chart);
    void editRequested(MonitorChart *chart);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void onDataReceived(const QString &fieldName, double value, qint64 timestamp);
    void onDeleteAction();
    void onEditAction();
    void onPauseAction();
    void onClearAction();
    void updatePlot();

private:
    void setupUI();
    void createActions();

    QString m_fieldName;               // 字段名
    QString m_unit;                    // 单位
    int m_xRangeSeconds;               // X轴时间范围（秒）
    int m_xTickCount;                  // X轴刻度数量
    bool m_paused;                     // 暂停状态

    QCustomPlot *m_plot;               // 绘图组件
    QVector<qint64> m_timestamps;      // 时间戳缓存
    QVector<double> m_values;          // 数值缓存

    QAction *m_deleteAction;
    QAction *m_editAction;
    QAction *m_pauseAction;
    QAction *m_clearAction;
};

#endif // MONITORCHART_H
```

### 4. MonitorChart.cpp

```cpp
#include "monitorchart.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QContextMenuEvent>
#include <QMenu>

MonitorChart::MonitorChart(const QString &fieldName,
                           const QString &unit,
                           int xRangeSeconds,
                           int xTickCount,
                           QWidget *parent)
    : QWidget(parent)
    , m_fieldName(fieldName)
    , m_unit(unit)
    , m_xRangeSeconds(xRangeSeconds)
    , m_xTickCount(xTickCount)
    , m_paused(false)
    , m_plot(nullptr)
{
    setupUI();
    createActions();
}

MonitorChart::~MonitorChart()
{
}

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

    // QCustomPlot占位（暂时用QLabel代替）
    // TODO: TASK-006中替换为真正的QCustomPlot
    QLabel *plotPlaceholder = new QLabel("(图表占位)", this);
    plotPlaceholder->setFixedHeight(200);
    plotPlaceholder->setFrameShape(QFrame::Box);
    plotPlaceholder->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(plotPlaceholder);

    // 设置边框
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
}

void MonitorChart::createActions()
{
    m_editAction = new QAction("编辑", this);
    connect(m_editAction, &QAction::triggered, this, &MonitorChart::onEditAction);

    m_pauseAction = new QAction("暂停", this);
    connect(m_pauseAction, &QAction::triggered, this, &MonitorChart::onPauseAction);

    m_clearAction = new QAction("Clear数据", this);
    connect(m_clearAction, &QAction::triggered, this, &MonitorChart::onClearAction);

    m_deleteAction = new QAction("删除", this);
    connect(m_deleteAction, &QAction::triggered, this, &MonitorChart::onDeleteAction);
}

void MonitorChart::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(m_editAction);
    menu.addAction(m_pauseAction);
    menu.addAction(m_clearAction);
    menu.addSeparator();
    menu.addAction(m_deleteAction);

    // 更新暂停按钮文本
    m_pauseAction->setText(m_paused ? "恢复" : "暂停");

    menu.exec(event->globalPos());
}

void MonitorChart::onDeleteAction()
{
    emit deleteRequested(this);
}

void MonitorChart::onEditAction()
{
    emit editRequested(this);
}

void MonitorChart::onPauseAction()
{
    // TODO: TASK-009中实现
}

void MonitorChart::onClearAction()
{
    // TODO: TASK-010中实现
}

// 以下方法在后续任务中实现

void MonitorChart::setFieldName(const QString &fieldName)
{
    m_fieldName = fieldName;
    // TODO: 更新标题
}

void MonitorChart::setXRange(int seconds)
{
    m_xRangeSeconds = seconds;
    // TODO: 更新图表
}

void MonitorChart::setXTickCount(int count)
{
    m_xTickCount = count;
    // TODO: 更新图表
}

void MonitorChart::appendData(double value, qint64 timestamp)
{
    // TODO: TASK-006中实现
}

void MonitorChart::clearData()
{
    // TODO: TASK-010中实现
}

void MonitorChart::setPaused(bool paused)
{
    m_paused = paused;
}

void MonitorChart::onDataReceived(const QString &fieldName, double value, qint64 timestamp)
{
    // TODO: TASK-006中实现
}

void MonitorChart::updatePlot()
{
    // TODO: TASK-006中实现
}
```

### 5. 更新plot.pro

```pro
# 在HEADERS添加
HEADERS += \
    # ... existing headers ...
    $$PWD/monitorpanel.h \
    $$PWD/monitorchart.h

# 在SOURCES添加
SOURCES += \
    # ... existing sources ...
    $$PWD/monitorpanel.cpp \
    $$PWD/monitorchart.cpp
```

## 验收标准

- [ ] MonitorPanel能显示，有"+"按钮和滚动区域
- [ ] 手动调用addChart()能添加空白图表框
- [ ] 图表纵向堆叠排列
- [ ] 图表显示标题（字段名+单位）
- [ ] 图表有占位区域（暂无真实绘图）
- [ ] 右键图表显示菜单（编辑、暂停、Clear、删除）
- [ ] 点击"删除"能移除图表
- [ ] 编译通过，无警告

## 测试方法

### 单元测试

```cpp
// 测试MonitorPanel基本功能
void TestMonitorPanel::testAddChart()
{
    MonitorPanel panel;

    // 添加图表
    panel.addChart("Roll", "度", 60, 2);

    // 验证图表数量
    QCOMPARE(panel.chartCount(), 1);
}

void TestMonitorPanel::testRemoveChart()
{
    MonitorPanel panel;
    panel.addChart("Roll", "度", 60, 2);

    auto *chart = panel.charts().first();
    panel.removeChart(chart);

    // 验证图表已移除
    QCOMPARE(panel.chartCount(), 0);
}
```

### 手动测试

1. 创建MonitorPanel实例
2. 调用addChart("Roll", "度", 60, 2)
3. 观察界面是否显示图表框
4. 右键图表，选择"删除"
5. 验证图表被移除

## 注意事项

1. **Qt 5.14兼容性**：
   - 不要使用Qt 6新增API
   - QVector操作需要注意

2. **内存管理**：
   - 使用deleteLater()安全删除Widget
   - 父子关系正确设置

3. **布局技巧**：
   - 使用QVBoxLayout::insertWidget()在弹簧之前插入
   - 避免布局重叠

4. **代码规范**：
   - 成员变量使用m_前缀
   - 方法名使用camelCase
   - 信号槽命名清晰

## 后续任务

完成此任务后，继续执行：
- TASK-002: 实现配置对话框
- TASK-004: 实现数据管理器

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [Qt QVBoxLayout文档](https://doc.qt.io/qt-5.14/qvboxlayout.html)
- [Qt QScrollArea文档](https://doc.qt.io/qt-5.14/qscrollarea.html)
