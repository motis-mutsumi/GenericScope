# 动态多通道实时数据监控面板 - 技术规范文档

> **文档版本**: v1.0
> **创建日期**: 2026-02-03
> **项目**: GenericScope
> **模块**: common_component/plot

---

## 1. 需求概述

### 1.1 需求背景

GenericScope主界面右侧的实时图表功能过于简陋，无法满足多通道数据监控需求。需要实现一个功能完善的监控面板，支持动态添加多个图表，实时显示协议解析后的字段数据。

### 1.2 业务价值

- **多通道监控**: 同时监控多个传感器字段（如IMU的Roll/Pitch/Yaw）
- **灵活配置**: 用户自由选择监控字段和图表参数
- **直观显示**: 实时曲线图，直观展示数据变化趋势
- **调试辅助**: 设备调试时快速定位异常数据

### 1.3 目标用户

普通用户和技术人员，使用GenericScope进行设备调试、数据采集、测试测量等工作。

---

## 2. 功能描述

### 2.1 用户故事

**作为** GenericScope的用户
**我想要** 在主界面右侧动态添加多个实时监控图表，每个图表独立显示一个协议字段的数值变化
**以便** 在设备调试和数据采集时，能够直观地多通道同步监控关键数据的实时变化趋势

### 2.2 功能列表

#### 核心功能（P0）
1. **动态添加图表**: 点击"+"按钮，配置并添加新图表
2. **字段选择**: 从当前激活协议的字段列表中选择监控对象
3. **参数配置**: 设置X轴时间范围、X轴刻度数量
4. **实时绘制**: 图表实时显示字段值的时间序列曲线
5. **自动缩放**: Y轴自动缩放适应数据范围
6. **纵向布局**: 多个图表纵向堆叠，可滚动查看
7. **删除图表**: 右键菜单删除不需要的图表

#### 重要功能（P1）
8. **编辑图表**: 右键菜单修改图表配置
9. **暂停/恢复**: 右键菜单暂停数据更新
10. **清空数据**: 右键菜单清空数据缓存
11. **异常处理**: 解析失败时显示错误提示

#### 优化功能（P2）
12. **图表美化**: 网格线、图例、鼠标悬停提示
13. **主题支持**: 支持亮色/暗色主题

### 2.3 使用场景

#### 场景1：IMU姿态监控
1. 用户配置IMU协议（Roll、Pitch、Yaw、AccX、AccY、AccZ等字段）
2. 点击"+"添加Roll图表（X轴60秒，刻度2）
3. 点击"+"添加Pitch图表（X轴60秒，刻度2）
4. 点击"+"添加Yaw图表（X轴60秒，刻度2）
5. 开始接收数据，三个图表同步显示姿态角变化
6. 发现Roll数据异常，右键暂停Roll图表分析

#### 场景2：高频多通道采集
1. 用户配置8通道ADC协议（CH1~CH8）
2. 批量添加8个图表监控所有通道
3. 设备以100Hz发送数据，图表实时刷新
4. X轴范围设置为10秒，观察短时波形
5. 清空某个通道的数据重新观察

#### 场景3：数据对比分析
1. 监控温度和电压两个字段
2. 通过两个图表观察温度与电压的关联性
3. 暂停温度图表，标记关键时刻
4. 编辑电压图表，调整X轴范围放大观察

---

## 3. 技术方案

### 3.1 架构设计

#### 3.1.1 模块划分

```
common_component/plot/
├── MonitorPanel (新增) - 监控面板主容器
│   ├── 管理多个MonitorChart
│   ├── "+"按钮添加图表
│   ├── 纵向滚动布局
│
├── MonitorChart (新增) - 单个监控图表组件
│   ├── 图表绘制（使用QCustomPlot）
│   ├── 数据缓存管理
│   ├── 右键菜单
│   ├── 标题栏（字段名+单位）
│
├── MonitorConfigDialog (新增) - 图表配置对话框
│   ├── 字段选择下拉框
│   ├── X轴范围输入
│   ├── X轴点数输入
│
└── MonitorDataManager (新增) - 数据分发管理器（单例）
    ├── 接收ProtocolParser解析数据
    ├── 分发给订阅的MonitorChart
    ├── 信号：dataUpdated(fieldName, value, timestamp)
```

#### 3.1.2 类图

```cpp
┌─────────────────────────────────────────┐
│           MainWindow                    │
├─────────────────────────────────────────┤
│ - m_monitorPanel: MonitorPanel*         │
│ - m_protocolParser: ProtocolParser*     │
├─────────────────────────────────────────┤
│ + onProtocolDataReceived(data)          │
│   └─> parser->parse(data)               │
│       └─> MonitorDataManager::instance()│
│           ->onProtocolDataParsed()      │
└─────────────────────────────────────────┘
                    │
                    │ has-a
                    ↓
┌─────────────────────────────────────────┐
│          MonitorPanel                   │
├─────────────────────────────────────────┤
│ - m_addButton: QPushButton*             │
│ - m_scrollArea: QScrollArea*            │
│ - m_charts: QList<MonitorChart*>        │
├─────────────────────────────────────────┤
│ + addChart(field, unit, range, ticks)   │
│ + removeChart(chart)                    │
│ + clearAllCharts()                      │
│ - onAddButtonClicked()                  │
│ - onChartDeleteRequested(chart)         │
│ - onChartEditRequested(chart)           │
└─────────────────────────────────────────┘
                    │
                    │ has-many
                    ↓
┌─────────────────────────────────────────┐
│          MonitorChart                   │
├─────────────────────────────────────────┤
│ - m_fieldName: QString                  │
│ - m_unit: QString                       │
│ - m_xRangeSeconds: int                  │
│ - m_xTickCount: int                     │
│ - m_paused: bool                        │
│ - m_plot: QCustomPlot*                  │
│ - m_timestamps: QVector<qint64>         │
│ - m_values: QVector<double>             │
├─────────────────────────────────────────┤
│ + appendData(value, timestamp)          │
│ + clearData()                           │
│ + setPaused(paused)                     │
│ - onDataReceived(field, value, time)    │
│ - updatePlot()                          │
│ - contextMenuEvent(event)               │
└─────────────────────────────────────────┘
                    │
                    │ subscribes to
                    ↓
┌─────────────────────────────────────────┐
│      MonitorDataManager (Singleton)     │
├─────────────────────────────────────────┤
│ - static s_instance: MonitorDataManager*│
├─────────────────────────────────────────┤
│ + static instance(): MonitorDataManager*│
│ + onProtocolDataParsed(fieldValues)     │
│ signals:                                │
│   dataUpdated(field, value, timestamp)  │
└─────────────────────────────────────────┘
```

#### 3.1.3 数据流图

```
设备数据
   ↓
TransferLayer (UART/TCP/Modbus)
   ↓
ProtocolParser::parse()
   ↓
ParseResult {
  success: true,
  fieldValues: {
    "Roll": 45.2,
    "Pitch": -12.3,
    "Yaw": 180.5
  }
}
   ↓
MonitorDataManager::onProtocolDataParsed(fieldValues)
   ↓
for each (fieldName, value) in fieldValues:
  ├─> timestamp = QDateTime::currentMSecsSinceEpoch()
  └─> emit dataUpdated("Roll", 45.2, 1612345678901)
      emit dataUpdated("Pitch", -12.3, 1612345678901)
      emit dataUpdated("Yaw", 180.5, 1612345678901)
   ↓
   ↓ [Signal/Slot, Qt::QueuedConnection]
   ↓
MonitorChart::onDataReceived("Roll", 45.2, timestamp)
   │
   ├─> if (fieldName != m_fieldName) return;  // 字段过滤
   ├─> if (m_paused) return;                  // 暂停检查
   │
   └─> appendData(45.2, timestamp)
       ├─> m_timestamps.append(timestamp)
       ├─> m_values.append(45.2)
       ├─> 移除超出X轴范围的旧数据
       └─> updatePlot()
           ├─> 转换时间戳为相对时间
           ├─> m_plot->graph(0)->setData(xData, yData)
           ├─> m_plot->xAxis->setRange(-60, 0)
           ├─> m_plot->graph(0)->rescaleValueAxis()
           └─> m_plot->replot()
```

### 3.2 接口设计

#### 3.2.1 MonitorPanel

```cpp
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
    QPushButton *m_addButton;          // 添加按钮
    QScrollArea *m_scrollArea;         // 滚动区域
    QWidget *m_contentWidget;          // 内容容器
    QVBoxLayout *m_contentLayout;      // 纵向布局
    QList<MonitorChart*> m_charts;     // 图表列表
};
```

#### 3.2.2 MonitorChart

```cpp
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
```

#### 3.2.3 MonitorConfigDialog

```cpp
class MonitorConfigDialog : public QDialog {
    Q_OBJECT
public:
    explicit MonitorConfigDialog(QWidget *parent = nullptr);

    /**
     * @brief 设置可用字段列表
     * @param fields 字段映射 <字段名, 单位>
     */
    void setAvailableFields(const QMap<QString, QString> &fields);

    /**
     * @brief 获取选中的字段名
     */
    QString selectedField() const;

    /**
     * @brief 获取选中字段的单位
     */
    QString selectedUnit() const;

    /**
     * @brief 获取X轴时间范围
     */
    int xRangeSeconds() const;

    /**
     * @brief 获取X轴刻度数量
     */
    int xTickCount() const;

    /**
     * @brief 设置编辑模式（填充当前配置）
     */
    void setEditMode(const QString &fieldName,
                     const QString &unit,
                     int xRangeSeconds,
                     int xTickCount);

private:
    QComboBox *m_fieldComboBox;        // 字段选择
    QSpinBox *m_xRangeSpinBox;         // X轴范围
    QSpinBox *m_xTickSpinBox;          // X轴刻度数
    QMap<QString, QString> m_fieldUnitMap;  // 字段-单位映射
};
```

#### 3.2.4 MonitorDataManager

```cpp
class MonitorDataManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief 获取单例实例
     */
    static MonitorDataManager* instance();

    /**
     * @brief 接收协议解析数据
     * @param fieldValues 字段值映射 <字段名, 数值>
     */
    void onProtocolDataParsed(const QMap<QString, double> &fieldValues);

signals:
    /**
     * @brief 数据更新信号
     * @param fieldName 字段名
     * @param value 数值
     * @param timestamp 时间戳（毫秒）
     */
    void dataUpdated(const QString &fieldName, double value, qint64 timestamp);

private:
    explicit MonitorDataManager(QObject *parent = nullptr);
    static MonitorDataManager *s_instance;
};
```

### 3.3 UI设计

#### 3.3.1 MonitorPanel布局

```
┌───────────────────────────────────────────────────┐
│  MonitorPanel                                     │
│  ┌─────────────────────────────────────────────┐  │
│  │ [+ 添加监控]                      (按钮)    │  │
│  └─────────────────────────────────────────────┘  │
│  ┌─────────────────────────────────────────────┐  │
│  │ QScrollArea (可滚动)                        │  │
│  │ ┌─────────────────────────────────────────┐ │  │
│  │ │ MonitorChart #1: Roll (度)             │ │  │
│  │ │ ┌────────────────────────────────────┐  │ │  │
│  │ │ │        (QCustomPlot 曲线图)        │  │ │  │
│  │ │ │                                    │  │ │  │
│  │ │ └────────────────────────────────────┘  │ │  │
│  │ └─────────────────────────────────────────┘ │  │
│  │ ┌─────────────────────────────────────────┐ │  │
│  │ │ MonitorChart #2: Pitch (度)            │ │  │
│  │ │ ┌────────────────────────────────────┐  │ │  │
│  │ │ │        (QCustomPlot 曲线图)        │  │ │  │
│  │ │ │                                    │  │ │  │
│  │ │ └────────────────────────────────────┘  │ │  │
│  │ └─────────────────────────────────────────┘ │  │
│  │ ┌─────────────────────────────────────────┐ │  │
│  │ │ MonitorChart #3: Yaw (度)              │ │  │
│  │ │ ┌────────────────────────────────────┐  │ │  │
│  │ │ │        (QCustomPlot 曲线图)        │  │ │  │
│  │ │ │                                    │  │ │  │
│  │ │ └────────────────────────────────────┘  │ │  │
│  │ └─────────────────────────────────────────┘ │  │
│  └─────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────┘
```

#### 3.3.2 MonitorConfigDialog布局

```
┌─────────────────────────────────────────┐
│  添加监控图表                           │
├─────────────────────────────────────────┤
│                                         │
│  监控字段:  [Roll            ▼]         │
│                                         │
│  X轴范围:   [  60  ] 秒                 │
│                                         │
│  X轴点数:   [   2  ]                    │
│                                         │
│                                         │
│           [  确定  ]  [  取消  ]        │
└─────────────────────────────────────────┘
```

#### 3.3.3 MonitorChart右键菜单

```
┌─────────────┐
│  编辑       │
│  暂停       │
│  Clear数据  │
├─────────────┤
│  删除       │
└─────────────┘
```

### 3.4 数据结构

#### 3.4.1 数据缓存

```cpp
// MonitorChart内部数据缓存
struct DataCache {
    QVector<qint64> timestamps;  // 时间戳（毫秒）
    QVector<double> values;      // 数值

    // 示例数据：
    // timestamps: [1612345678901, 1612345678911, 1612345678921, ...]
    // values:     [45.2,           45.3,           45.1,           ...]
};
```

#### 3.4.2 配置数据

```cpp
// 图表配置
struct ChartConfig {
    QString fieldName;      // 字段名，如"Roll"
    QString unit;           // 单位，如"度"
    int xRangeSeconds;      // X轴时间范围（秒），如60
    int xTickCount;         // X轴刻度数量，如2
};
```

---

## 4. 实现细节

### 4.1 核心算法

#### 4.1.1 数据缓存管理

```cpp
void MonitorChart::appendData(double value, qint64 timestamp) {
    // 添加新数据点
    m_timestamps.append(timestamp);
    m_values.append(value);

    // 移除超出X轴范围的旧数据
    qint64 cutoffTime = timestamp - m_xRangeSeconds * 1000;  // 毫秒
    while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
        m_timestamps.removeFirst();
        m_values.removeFirst();
    }

    // 更新图表
    updatePlot();
}
```

**算法说明：**
- 使用滑动窗口策略，只保留X轴范围内的数据
- 时间复杂度：O(n)，n为需要移除的数据点数
- 空间复杂度：O(m)，m为X轴范围内的数据点数

**示例：**
- X轴范围60秒，数据频率100Hz
- 最大缓存点数：60 × 100 = 6000点
- 新数据到达时，移除60秒前的数据

#### 4.1.2 时间戳转换

```cpp
void MonitorChart::updatePlot() {
    if (m_timestamps.isEmpty()) {
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

    // 更新QCustomPlot
    m_plot->graph(0)->setData(xData, yData);
    m_plot->xAxis->setRange(-m_xRangeSeconds, 0);
    m_plot->graph(0)->rescaleValueAxis();
    m_plot->xAxis->ticker()->setTickCount(m_xTickCount);
    m_plot->replot();
}
```

**转换示例：**
```
当前时间: 17:17:18.418 (timestamp = 1612345678418)

原始时间戳          相对时间
1612345618418  →  -60.000秒  (60秒前)
1612345648418  →  -30.000秒  (30秒前)
1612345678408  →   -0.010秒  (10毫秒前)
1612345678418  →    0.000秒  (当前)
```

#### 4.1.3 Y轴自动缩放

```cpp
// QCustomPlot内置方法
m_plot->graph(0)->rescaleValueAxis();
```

**缩放策略：**
- 扫描当前显示的所有数据点
- 找到最小值和最大值
- 设置Y轴范围为[min, max]，并添加10%的边距

### 4.2 线程安全

#### 4.2.1 信号连接方式

```cpp
// MonitorChart构造函数
MonitorChart::MonitorChart(...) {
    // 使用Qt::QueuedConnection确保跨线程安全
    connect(MonitorDataManager::instance(), &MonitorDataManager::dataUpdated,
            this, &MonitorChart::onDataReceived,
            Qt::QueuedConnection);  // 关键：队列连接
}
```

**说明：**
- ProtocolParser可能在非主线程解析数据
- Qt::QueuedConnection确保onDataReceived()在主线程执行
- 避免跨线程访问UI导致崩溃

#### 4.2.2 数据管理器实现

```cpp
void MonitorDataManager::onProtocolDataParsed(const QMap<QString, double> &fieldValues) {
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    // 遍历所有字段，分发数据
    for (auto it = fieldValues.begin(); it != fieldValues.end(); ++it) {
        emit dataUpdated(it.key(), it.value(), timestamp);
    }
}
```

### 4.3 内存管理

#### 4.3.1 图表生命周期

```cpp
void MonitorPanel::removeChart(MonitorChart *chart) {
    // 从布局移除
    m_contentLayout->removeWidget(chart);

    // 从列表移除
    m_charts.removeOne(chart);

    // 安全删除（事件循环结束后）
    chart->deleteLater();
}
```

#### 4.3.2 数据缓存限制

```cpp
void MonitorChart::appendData(double value, qint64 timestamp) {
    m_timestamps.append(timestamp);
    m_values.append(value);

    // 移除超出时间范围的数据
    qint64 cutoffTime = timestamp - m_xRangeSeconds * 1000;
    while (!m_timestamps.isEmpty() && m_timestamps.first() < cutoffTime) {
        m_timestamps.removeFirst();
        m_values.removeFirst();
    }

    // 可选：添加最大点数限制（防止极端情况）
    const int MAX_POINTS = 10000;
    if (m_timestamps.size() > MAX_POINTS) {
        m_timestamps.remove(0, m_timestamps.size() - MAX_POINTS);
        m_values.remove(0, m_values.size() - MAX_POINTS);
    }

    updatePlot();
}
```

### 4.4 Qt 5.14兼容性

#### 4.4.1 容器初始化

```cpp
// ✅ 正确：Qt 5.14兼容
QVector<double> data;
data.resize(size);
data.fill(0.0);

// ❌ 错误：Qt 6+才支持
// QVector<double> data(size, 0.0);  // 编译错误
```

#### 4.4.2 QCustomPlot版本

- 使用QCustomPlot 2.1.0（兼容Qt 5.6+）
- 避免使用QCustomPlot 3.x（需要Qt 6）

---

## 5. 测试方案

### 5.1 单元测试

#### 测试用例1：数据缓存管理
```cpp
// 测试目标：验证数据缓存正确移除旧数据
void TestMonitorChart::testDataCache() {
    MonitorChart chart("Roll", "度", 10, 2);  // 10秒窗口

    // 添加11秒的数据（每秒1个点）
    qint64 baseTime = 1000000000000;
    for (int i = 0; i <= 11; ++i) {
        chart.appendData(i * 1.0, baseTime + i * 1000);
    }

    // 验证：只保留10秒内的数据（最后10个点）
    QCOMPARE(chart.dataPointCount(), 10);
}
```

#### 测试用例2：字段过滤
```cpp
// 测试目标：验证图表只接收匹配字段的数据
void TestMonitorChart::testFieldFilter() {
    MonitorChart chart("Roll", "度", 60, 2);

    // 发送不匹配的数据
    MonitorDataManager::instance()->onProtocolDataParsed({
        {"Pitch", 10.0},
        {"Yaw", 20.0}
    });

    // 验证：图表无数据
    QCOMPARE(chart.dataPointCount(), 0);

    // 发送匹配的数据
    MonitorDataManager::instance()->onProtocolDataParsed({
        {"Roll", 45.2}
    });

    // 验证：图表有数据
    QCOMPARE(chart.dataPointCount(), 1);
}
```

#### 测试用例3：暂停功能
```cpp
// 测试目标：验证暂停时停止数据缓存
void TestMonitorChart::testPause() {
    MonitorChart chart("Roll", "度", 60, 2);

    // 添加1个数据点
    chart.appendData(10.0, 1000);
    QCOMPARE(chart.dataPointCount(), 1);

    // 暂停
    chart.setPaused(true);

    // 尝试添加数据
    chart.appendData(20.0, 2000);

    // 验证：数据未增加
    QCOMPARE(chart.dataPointCount(), 1);

    // 恢复
    chart.setPaused(false);
    chart.appendData(30.0, 3000);

    // 验证：数据增加
    QCOMPARE(chart.dataPointCount(), 2);
}
```

### 5.2 集成测试

#### 测试场景1：单通道监控
**步骤：**
1. 启动GenericScope
2. 配置IMU协议（包含Roll字段）
3. 点击"+"添加Roll图表
4. 开始接收数据
5. 观察图表是否实时更新

**预期结果：**
- 图表标题显示"Roll (度)"
- 曲线实时绘制
- Y轴自动缩放
- 无卡顿

#### 测试场景2：多通道监控
**步骤：**
1. 添加Roll、Pitch、Yaw、AccX、AccY共5个图表
2. 开始接收100Hz数据
3. 观察5个图表是否同步更新

**预期结果：**
- 5个图表纵向堆叠
- 同步更新，无延迟
- 滚动条正常工作
- 内存占用稳定

#### 测试场景3：编辑与删除
**步骤：**
1. 添加Roll图表（X轴60秒）
2. 接收数据观察
3. 右键编辑，修改X轴范围为30秒
4. 观察图表变化
5. 右键删除图表

**预期结果：**
- 编辑后X轴范围立即变为30秒
- 删除后图表从界面消失
- 其他图表不受影响

#### 测试场景4：暂停与清空
**步骤：**
1. 添加Roll图表
2. 接收数据，观察曲线
3. 右键暂停
4. 继续接收数据，观察图表是否停止更新
5. 右键恢复
6. 右键Clear数据

**预期结果：**
- 暂停时曲线停止绘制
- 恢复后继续绘制
- Clear后曲线清空，重新绘制

#### 测试场景5：高频数据
**步骤：**
1. 配置100Hz协议
2. 添加3个图表，X轴60秒
3. 持续接收数据5分钟
4. 观察性能和内存

**预期结果：**
- 图表流畅更新，无明显卡顿
- 内存占用稳定（不持续增长）
- CPU占用合理（<20%）

### 5.3 异常测试

#### 测试用例1：协议解析失败
**步骤：**
1. 添加图表
2. 发送格式错误的数据
3. 观察图表是否显示错误提示

**预期结果：**
- 图表显示"解析失败"提示
- 不崩溃

#### 测试用例2：字段不存在
**步骤：**
1. 配置协议A（包含Roll字段）
2. 添加Roll图表
3. 切换到协议B（不包含Roll字段）
4. 接收数据

**预期结果：**
- 图表不更新（字段不匹配）
- 或显示警告

#### 测试用例3：极端参数
**步骤：**
1. 添加图表，X轴范围1秒，X轴点数10
2. 添加图表，X轴范围300秒，X轴点数2
3. 接收高频数据

**预期结果：**
- 图表正常显示
- 无崩溃

---

## 6. 文件清单

### 6.1 新增文件

| 文件路径 | 说明 | 代码量（行） |
|---------|------|-------------|
| `common_component/plot/monitorpanel.h` | MonitorPanel声明 | 50 |
| `common_component/plot/monitorpanel.cpp` | MonitorPanel实现 | 150 |
| `common_component/plot/monitorchart.h` | MonitorChart声明 | 80 |
| `common_component/plot/monitorchart.cpp` | MonitorChart实现 | 300 |
| `common_component/plot/monitorconfigdialog.h` | 配置对话框声明 | 40 |
| `common_component/plot/monitorconfigdialog.cpp` | 配置对话框实现 | 120 |
| `common_component/plot/monitordatamanager.h` | 数据管理器声明 | 30 |
| `common_component/plot/monitordatamanager.cpp` | 数据管理器实现 | 50 |

**总计新增代码：约820行**

### 6.2 修改文件

| 文件路径 | 修改内容 | 代码量（行） |
|---------|---------|-------------|
| `common_component/plot.pro` | 添加新文件到编译 | +8 |
| `app/ui/mainwindow.h` | 添加m_monitorPanel成员 | +3 |
| `app/ui/mainwindow.cpp` | 集成MonitorPanel，连接数据流 | +30 |

**总计修改代码：约41行**

### 6.3 依赖文件

| 文件 | 说明 | 来源 |
|-----|------|-----|
| `qcustomplot.h` | QCustomPlot头文件 | 第三方库 |
| `qcustomplot.cpp` | QCustomPlot实现 | 第三方库 |

**需要检查项目是否已集成QCustomPlot，如未集成需要添加。**

---

## 7. 风险与注意事项

### 7.1 性能风险

#### 风险1：高频数据 + 多图表卡顿
**描述：**
- 100Hz数据频率 × 8个图表 × 60秒 = 48000个数据点
- QCustomPlot重绘可能成为瓶颈

**缓解措施：**
- 阶段1（P0/P1）：暂不优化，先验证功能
- 阶段2（P2）：如出现卡顿，实施优化：
  - 降采样：超过1000点自动抽样
  - OpenGL加速：QCustomPlot::setOpenGl(true)
  - 延迟刷新：累积多个数据点再重绘

**优先级：P2（后期优化）**

#### 风险2：内存占用持续增长
**描述：**
- 长时间运行，数据缓存可能无限增长

**缓解措施：**
- 严格按X轴范围清理旧数据
- 添加最大缓存点数限制（10000点）
- 定期监控内存占用

**优先级：P1（重要）**

### 7.2 线程安全风险

#### 风险3：跨线程UI访问
**描述：**
- ProtocolParser可能在非主线程解析数据
- 直接调用UI更新会崩溃

**缓解措施：**
- 使用Qt::QueuedConnection连接信号
- 确保所有UI操作在主线程

**优先级：P0（必须）**

### 7.3 兼容性风险

#### 风险4：QCustomPlot版本不兼容
**描述：**
- QCustomPlot 3.x需要Qt 6
- 项目使用Qt 5.14

**缓解措施：**
- 使用QCustomPlot 2.1.0（兼容Qt 5.6+）
- 编译前检查QCustomPlot版本

**优先级：P0（必须）**

#### 风险5：Qt 5.14 API限制
**描述：**
- QVector::resize()只支持单参数
- 部分Qt 6 API不可用

**缓解措施：**
- 严格遵循Qt 5.14 API规范
- 代码审查时检查API兼容性

**优先级：P0（必须）**

### 7.4 业务逻辑风险

#### 风险6：协议切换时数据混乱
**描述：**
- 用户切换协议时，图表监控的字段可能不存在

**缓解措施：**
- 方案1：切换协议时清空所有图表（推荐）
- 方案2：图表显示警告，停止更新

**优先级：P1（重要）**

#### 风险7：字段单位缺失
**描述：**
- 协议配置中可能没有填写单位

**缓解措施：**
- 单位为空时显示字段名
- 或显示"字段名 (无单位)"

**优先级：P1（重要）**

---

## 8. 任务分解与开发计划

### 8.1 任务列表

| 任务ID | 任务名称 | 优先级 | 工作量 | 依赖任务 |
|--------|---------|-------|--------|---------|
| TASK-001 | 搭建基础框架 | P0 | 4h | 无 |
| TASK-002 | 实现配置对话框 | P0 | 3h | TASK-001 |
| TASK-003 | 集成到MainWindow | P0 | 2h | TASK-001, TASK-002 |
| TASK-004 | 实现数据管理器 | P0 | 2h | TASK-001 |
| TASK-005 | 连接数据流 | P0 | 3h | TASK-003, TASK-004 |
| TASK-006 | 实现图表绘制 | P0 | 5h | TASK-005 |
| TASK-007 | 实现右键删除 | P0 | 2h | TASK-006 |
| TASK-008 | 实现右键编辑 | P1 | 2h | TASK-007 |
| TASK-009 | 实现暂停/恢复 | P1 | 1h | TASK-007 |
| TASK-010 | 实现Clear数据 | P1 | 1h | TASK-007 |
| TASK-011 | 异常处理 | P1 | 2h | TASK-006 |
| TASK-012 | 图表美化 | P2 | 3h | TASK-006 |
| TASK-013 | 集成测试 | P1 | 2h | TASK-011 |

### 8.2 开发计划

#### 第1天（P0核心）
- [x] TASK-001：搭建基础框架 (4h)
- [x] TASK-002：实现配置对话框 (3h)
- [x] TASK-003：集成到MainWindow (2h)

**里程碑：基础UI框架完成**

#### 第2天（P0核心）
- [x] TASK-004：实现数据管理器 (2h)
- [x] TASK-005：连接数据流 (3h)
- [x] TASK-006：实现图表绘制 (5h)

**里程碑：数据流打通，图表能显示数据**

#### 第3天（P0完成）
- [x] TASK-007：实现右键删除 (2h)

**里程碑：MVP完成，可演示基本功能**

#### 第4天（P1功能）
- [x] TASK-008：实现右键编辑 (2h)
- [x] TASK-009：实现暂停/恢复 (1h)
- [x] TASK-010：实现Clear数据 (1h)
- [x] TASK-011：异常处理 (2h)

**里程碑：交互功能完善**

#### 第5天（测试与美化）
- [x] TASK-013：集成测试 (2h)
- [x] TASK-012：图表美化 (3h)

**里程碑：全功能完成，可交付**

### 8.3 里程碑

| 里程碑 | 日期 | 完成标志 | 可演示内容 |
|--------|------|---------|-----------|
| M1: 基础框架 | 第1天 | TASK-003完成 | 可添加空白图表 |
| M2: 数据显示 | 第2天 | TASK-006完成 | 图表实时显示数据 |
| M3: MVP完成 | 第3天 | TASK-007完成 | 可添加、删除、显示 |
| M4: 功能完善 | 第4天 | TASK-011完成 | 所有交互功能可用 |
| M5: 可交付 | 第5天 | TASK-013完成 | 测试通过，可发布 |

---

## 9. 附录

### 9.1 参考资料

- **QCustomPlot官方文档**: https://www.qcustomplot.com/documentation/
- **Qt 5.14文档**: https://doc.qt.io/qt-5.14/
- **GenericScope项目文档**: `docs/CLAUDE.md`
- **协议系统设计规范**: `docs/protocol-config-system-spec.md`

### 9.2 术语表

| 术语 | 说明 |
|------|------|
| 监控面板 | MonitorPanel，右侧的主容器组件 |
| 监控图表 | MonitorChart，单个图表组件 |
| 字段 | 协议解析后的数据项，如Roll、Pitch |
| X轴范围 | 图表显示的时间窗口，如60秒 |
| X轴点数 | X轴时间刻度的数量，如2个 |
| 数据缓存 | MonitorChart内部存储的历史数据 |
| 相对时间 | 相对于当前时间的偏移，负数表示过去 |

### 9.3 变更记录

| 版本 | 日期 | 作者 | 变更内容 |
|------|------|------|---------|
| v1.0 | 2026-02-03 | Requirements Analyst | 初始版本 |

---

**文档结束**
