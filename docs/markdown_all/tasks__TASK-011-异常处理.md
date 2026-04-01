# TASK-011: 异常处理

## 任务信息

- **任务ID**: TASK-011
- **任务名称**: 异常处理
- **优先级**: P1（重要功能）
- **预估工作量**: 2小时
- **依赖任务**: TASK-006
- **状态**: 待开始

## 任务描述

实现协议解析失败时的错误处理机制。当ProtocolParser解析失败时，MonitorChart应该显示错误提示，而不是崩溃或显示错误数据。

## 具体目标

1. 在MainWindow中检查ParseResult.success
2. 如果解析失败，发送错误信号
3. MonitorChart订阅错误信号
4. 在图表上显示错误提示
5. 错误恢复：下次解析成功时清除错误状态

## 涉及文件

### 修改文件
- `app/ui/mainwindow.cpp`
- `common_component/plot/monitordatamanager.h`
- `common_component/plot/monitordatamanager.cpp`
- `common_component/plot/monitorchart.h`
- `common_component/plot/monitorchart.cpp`

## 实现细节

### 1. 在MonitorDataManager中添加错误信号

在`monitordatamanager.h`中：

```cpp
class MonitorDataManager : public QObject {
    Q_OBJECT
public:
    // ... existing methods ...

    /**
     * @brief 通知解析错误
     * @param errorMessage 错误信息
     */
    void notifyParseError(const QString &errorMessage);

signals:
    void dataUpdated(const QString &fieldName, double value, qint64 timestamp);

    /**
     * @brief 解析错误信号
     * @param errorMessage 错误信息
     */
    void parseError(const QString &errorMessage);

    /**
     * @brief 解析恢复信号
     */
    void parseRecovered();

private:
    bool m_lastParseSuccess;  // 上次解析是否成功
};
```

在`monitordatamanager.cpp`中：

```cpp
MonitorDataManager::MonitorDataManager(QObject *parent)
    : QObject(parent)
    , m_lastParseSuccess(true)
{
}

void MonitorDataManager::onProtocolDataParsed(const QMap<QString, double> &fieldValues)
{
    if (fieldValues.isEmpty()) {
        return;
    }

    // 如果之前是错误状态，现在恢复了
    if (!m_lastParseSuccess) {
        emit parseRecovered();
        m_lastParseSuccess = true;
    }

    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    for (auto it = fieldValues.begin(); it != fieldValues.end(); ++it) {
        emit dataUpdated(it.key(), it.value(), timestamp);
    }
}

void MonitorDataManager::notifyParseError(const QString &errorMessage)
{
    // 只在状态变化时发送信号（避免重复）
    if (m_lastParseSuccess) {
        emit parseError(errorMessage);
        m_lastParseSuccess = false;
    }
}
```

### 2. 在MainWindow中调用错误通知

在`mainwindow.cpp`中：

```cpp
void MainWindow::onProtocolDataReceived(const QByteArray &data)
{
    // 解析数据
    ParseResult result = m_protocolParser->parse(data);

    if (result.success) {
        // 转换并分发数据
        QMap<QString, double> fieldValues;
        // ... 转换逻辑（TASK-005中已实现）...

        MonitorDataManager::instance()->onProtocolDataParsed(fieldValues);

        // 原有逻辑...
    } else {
        // ========== 新增：解析失败处理 ==========
        QString errorMsg = result.errorMessage.isEmpty()
                           ? "数据解析失败"
                           : result.errorMessage;

        MonitorDataManager::instance()->notifyParseError(errorMsg);
        // ========================================

        // 原有错误处理逻辑...
    }
}
```

### 3. 在MonitorChart中订阅错误信号

在`monitorchart.h`中添加：

```cpp
private slots:
    void onDataReceived(const QString &fieldName, double value, qint64 timestamp);
    void onParseError(const QString &errorMessage);   // 新增
    void onParseRecovered();                           // 新增

private:
    bool m_showingError;  // 是否正在显示错误
    QString m_currentError;  // 当前错误信息
    void showErrorState(const QString &errorMessage);
    void clearErrorState();
```

在`monitorchart.cpp`构造函数中：

```cpp
MonitorChart::MonitorChart(...)
    : // ... existing initializations ...
    , m_showingError(false)
{
    setupUI();
    createActions();

    // 订阅数据信号
    connect(MonitorDataManager::instance(), &MonitorDataManager::dataUpdated,
            this, &MonitorChart::onDataReceived,
            Qt::QueuedConnection);

    // ========== 新增：订阅错误信号 ==========
    connect(MonitorDataManager::instance(), &MonitorDataManager::parseError,
            this, &MonitorChart::onParseError,
            Qt::QueuedConnection);

    connect(MonitorDataManager::instance(), &MonitorDataManager::parseRecovered,
            this, &MonitorChart::onParseRecovered,
            Qt::QueuedConnection);
    // ========================================
}
```

### 4. 实现错误显示

在`monitorchart.cpp`中：

```cpp
void MonitorChart::onParseError(const QString &errorMessage)
{
    m_showingError = true;
    m_currentError = errorMessage;
    showErrorState(errorMessage);
}

void MonitorChart::onParseRecovered()
{
    if (m_showingError) {
        m_showingError = false;
        m_currentError.clear();
        clearErrorState();
    }
}

void MonitorChart::showErrorState(const QString &errorMessage)
{
    // 方案1：在图表上绘制错误文本
    if (!m_plot) {
        return;
    }

    // 清除旧的错误文本（如果有）
    QList<QCPItemText*> textItems = m_plot->findChildren<QCPItemText*>("errorText");
    for (auto *item : textItems) {
        m_plot->removeItem(item);
    }

    // 创建错误文本
    QCPItemText *errorText = new QCPItemText(m_plot);
    errorText->setObjectName("errorText");
    errorText->setText(QString("解析失败\n%1").arg(errorMessage));
    errorText->setFont(QFont(font().family(), 12, QFont::Bold));
    errorText->setColor(QColor(255, 0, 0));  // 红色

    // 定位到图表中心
    errorText->position->setType(QCPItemPosition::ptAxisRectRatio);
    errorText->position->setCoords(0.5, 0.5);  // 中心

    // 背景色
    errorText->setPadding(QMargins(10, 10, 10, 10));
    errorText->setBrush(QBrush(QColor(255, 255, 200, 200)));  // 浅黄色半透明

    m_plot->replot();

    // 方案2：修改标题颜色
    QLabel *titleLabel = findChild<QLabel*>("titleLabel");
    if (titleLabel) {
        titleLabel->setStyleSheet("color: red; font-weight: bold;");
    }
}

void MonitorChart::clearErrorState()
{
    if (!m_plot) {
        return;
    }

    // 移除错误文本
    QList<QCPItemText*> textItems = m_plot->findChildren<QCPItemText*>("errorText");
    for (auto *item : textItems) {
        m_plot->removeItem(item);
    }

    m_plot->replot();

    // 恢复标题颜色
    QLabel *titleLabel = findChild<QLabel*>("titleLabel");
    if (titleLabel) {
        titleLabel->setStyleSheet("");  // 清除样式
    }
}
```

### 5. 修改updatePlot()处理错误状态

在`monitorchart.cpp`中：

```cpp
void MonitorChart::updatePlot()
{
    // 如果正在显示错误，不更新图表
    if (m_showingError) {
        return;
    }

    if (m_timestamps.isEmpty()) {
        m_plot->graph(0)->data()->clear();
        m_plot->replot();
        return;
    }

    // 正常绘制逻辑...
}
```

## 验收标准

- [ ] 解析失败时，图表显示红色错误提示
- [ ] 错误提示包含错误信息
- [ ] 标题变为红色
- [ ] 解析恢复后，错误提示消失
- [ ] 标题颜色恢复正常
- [ ] 错误状态下不绘制数据
- [ ] 多个图表同时显示错误
- [ ] 不崩溃

## 测试方法

### 功能测试

**测试场景1：触发解析错误**

1. 添加Roll图表
2. 开始接收正常数据，观察曲线
3. 发送格式错误的数据（模拟解析失败）

**预期结果：**
- 图表显示红色错误文本："解析失败"
- 标题变为红色
- 曲线停止更新

4. 恢复发送正常数据

**预期结果：**
- 错误提示消失
- 标题恢复正常颜色
- 曲线继续绘制

**测试场景2：多图表错误**

1. 添加Roll、Pitch、Yaw三个图表
2. 发送错误数据

**预期结果：**
- 三个图表同时显示错误提示

**测试场景3：错误持续**

1. 添加图表
2. 持续发送错误数据10秒

**预期结果：**
- 错误提示持续显示
- 不重复发送信号（通过日志验证）

### 单元测试

```cpp
void TestMonitorChart::testParseError()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 发送正常数据
    chart.appendData(10.0, 1000);
    QVERIFY(!chart.isShowingError());

    // 模拟解析错误
    MonitorDataManager::instance()->notifyParseError("测试错误");
    QTest::qWait(100);

    // 验证错误状态
    QVERIFY(chart.isShowingError());

    // 模拟恢复
    MonitorDataManager::instance()->onProtocolDataParsed({{"Roll", 20.0}});
    QTest::qWait(100);

    // 验证错误清除
    QVERIFY(!chart.isShowingError());
}
```

### 压力测试

```cpp
void TestErrorHandling::testRepeatedErrors()
{
    MonitorChart chart("Roll", "度", 60, 2);

    // 交替发送错误和正常数据
    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) {
            MonitorDataManager::instance()->notifyParseError("错误" + QString::number(i));
        } else {
            MonitorDataManager::instance()->onProtocolDataParsed({{"Roll", i * 1.0}});
        }
        QTest::qWait(10);
    }

    // 验证无崩溃
    QVERIFY(true);
}
```

## 注意事项

### 1. 避免重复发送错误信号

```cpp
void MonitorDataManager::notifyParseError(const QString &errorMessage)
{
    // 只在状态变化时发送
    if (m_lastParseSuccess) {
        emit parseError(errorMessage);
        m_lastParseSuccess = false;
    }
}
```

**原因：** 解析可能持续失败，避免每次都触发信号。

### 2. 错误恢复检测

```cpp
void MonitorDataManager::onProtocolDataParsed(const QMap<QString, double> &fieldValues)
{
    if (!m_lastParseSuccess) {
        emit parseRecovered();  // 发送恢复信号
        m_lastParseSuccess = true;
    }
    // ...
}
```

### 3. QCPItemText生命周期

**QCustomPlot管理QCPItem：**
- 使用m_plot->removeItem(item)移除
- 或使用findChildren查找并删除

### 4. 线程安全

**使用Qt::QueuedConnection：**
```cpp
connect(manager, &MonitorDataManager::parseError,
        this, &MonitorChart::onParseError,
        Qt::QueuedConnection);
```

### 5. 错误信息来源

**ParseResult结构可能是：**
```cpp
struct ParseResult {
    bool success;
    QString errorMessage;  // 错误描述
    QMap<QString, QVariant> fieldValues;
};
```

**根据实际项目调整。**

## 扩展功能（可选）

### 1. 错误统计

```cpp
int m_errorCount;
qint64 m_lastErrorTime;

void MonitorChart::onParseError(const QString &errorMessage) {
    m_errorCount++;
    m_lastErrorTime = QDateTime::currentMSecsSinceEpoch();
    showErrorState(QString("解析失败 (第%1次)\n%2")
                   .arg(m_errorCount)
                   .arg(errorMessage));
}
```

### 2. 错误日志

```cpp
void MonitorChart::onParseError(const QString &errorMessage) {
    qWarning() << "[MonitorChart]" << m_fieldName << "解析失败:" << errorMessage;
    LogManager::instance()->error("图表{}解析失败: {}", m_fieldName, errorMessage);
    showErrorState(errorMessage);
}
```

### 3. 错误通知音

```cpp
#include <QSound>

void MonitorChart::onParseError(const QString &errorMessage) {
    QSound::play(":/sounds/error.wav");
    showErrorState(errorMessage);
}
```

## 常见问题

### Q1: 错误提示不显示？

**检查：**
- parseError信号是否发送
- onParseError()是否调用
- showErrorState()是否执行
- QCPItemText是否正确添加

### Q2: 错误提示不消失？

**检查：**
- parseRecovered信号是否发送
- onParseRecovered()是否调用
- clearErrorState()是否执行

### Q3: 错误信息为空？

**检查：**
- ParseResult.errorMessage是否有值
- 提供默认错误信息："数据解析失败"

## 后续任务

完成此任务后，P1功能全部完成！继续执行：
- TASK-013: 集成测试（验证所有功能）
- TASK-012: 图表美化（P2，可选）

## 相关文档

- [主技术规范](../monitor-panel-tech-spec.md)
- [TASK-006: 实现图表绘制](./TASK-006-图表绘制.md)
- [QCustomPlot Items文档](https://www.qcustomplot.com/documentation/classQCPItemText.html)
