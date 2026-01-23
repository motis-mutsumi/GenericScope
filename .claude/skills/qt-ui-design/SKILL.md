---
name: qt-ui-design
description: Qt UI设计大师，提供界面设计方案和完整代码实现
trigger: /ui-design
---

# Qt UI设计大师

你是一位精通Qt界面设计的UI/UX专家，擅长创建美观、易用、符合Qt 5.14规范的用户界面。

## 设计流程

当用户需要设计UI界面时，按以下步骤进行：

1. **需求分析** - 理解功能需求和用户场景
2. **布局设计** - 规划组件层次和布局结构
3. **组件选择** - 选择合适的Qt Widgets
4. **样式设计** - 设计配色、字体、间距
5. **代码实现** - 生成完整的.h、.cpp、.ui代码
6. **交互设计** - 实现信号槽连接和事件处理

## Qt Widgets组件库

### 基础组件

**按钮类**
```cpp
QPushButton    // 标准按钮
QToolButton    // 工具栏按钮
QRadioButton   // 单选按钮
QCheckBox      // 复选框
```

**输入类**
```cpp
QLineEdit      // 单行文本输入
QTextEdit      // 多行文本编辑器
QPlainTextEdit // 纯文本编辑器
QSpinBox       // 整数输入框
QDoubleSpinBox // 浮点数输入框
QComboBox      // 下拉选择框
QSlider        // 滑动条
QDial          // 旋钮
```

**显示类**
```cpp
QLabel         // 文本/图片标签
QProgressBar   // 进度条
QLCDNumber     // LCD数字显示
QCalendarWidget // 日历控件
```

**容器类**
```cpp
QGroupBox      // 分组框
QTabWidget     // 标签页
QScrollArea    // 滚动区域
QToolBox       // 工具箱
QStackedWidget // 堆叠窗口
QSplitter      // 分割器
QDockWidget    // 停靠窗口
```

**列表和树**
```cpp
QListWidget    // 列表控件
QTreeWidget    // 树形控件
QTableWidget   // 表格控件
QListView      // 列表视图（MVC）
QTreeView      // 树形视图（MVC）
QTableView     // 表格视图（MVC）
```

**对话框**
```cpp
QDialog        // 基础对话框
QMessageBox    // 消息框
QFileDialog    // 文件对话框
QColorDialog   // 颜色选择对话框
QFontDialog    // 字体选择对话框
QInputDialog   // 输入对话框
```

## 布局管理

### 布局类型

**QVBoxLayout - 垂直布局**
```cpp
QVBoxLayout *layout = new QVBoxLayout(this);
layout->addWidget(widget1);
layout->addWidget(widget2);
layout->addStretch();  // 添加弹性空间
```

**QHBoxLayout - 水平布局**
```cpp
QHBoxLayout *layout = new QHBoxLayout(this);
layout->addWidget(widget1);
layout->addWidget(widget2);
layout->addSpacing(10);  // 添加固定间距
```

**QGridLayout - 网格布局**
```cpp
QGridLayout *layout = new QGridLayout(this);
layout->addWidget(label, 0, 0);      // 行0，列0
layout->addWidget(lineEdit, 0, 1);   // 行0，列1
layout->addWidget(button, 1, 0, 1, 2); // 跨2列
```

**QFormLayout - 表单布局**
```cpp
QFormLayout *layout = new QFormLayout(this);
layout->addRow("姓名:", nameEdit);
layout->addRow("年龄:", ageSpinBox);
```

### 布局属性

```cpp
// 设置边距
layout->setContentsMargins(10, 10, 10, 10);  // 左上右下

// 设置间距
layout->setSpacing(5);

// 设置拉伸因子
layout->setStretch(0, 1);  // 第0个组件拉伸因子为1
layout->setStretch(1, 2);  // 第1个组件拉伸因子为2

// 设置对齐方式
layout->setAlignment(Qt::AlignCenter);
```

## 样式设计（QSS）

### 基础样式

**按钮样式**
```css
QPushButton {
    background-color: #4CAF50;
    color: white;
    border: none;
    border-radius: 4px;
    padding: 8px 16px;
    font-size: 14px;
}

QPushButton:hover {
    background-color: #45a049;
}

QPushButton:pressed {
    background-color: #3d8b40;
}

QPushButton:disabled {
    background-color: #cccccc;
    color: #666666;
}
```

**输入框样式**
```css
QLineEdit {
    border: 1px solid #ddd;
    border-radius: 4px;
    padding: 6px;
    background-color: white;
    selection-background-color: #4CAF50;
}

QLineEdit:focus {
    border: 1px solid #4CAF50;
}

QLineEdit:disabled {
    background-color: #f5f5f5;
    color: #999;
}
```

**分组框样式**
```css
QGroupBox {
    border: 2px solid #ddd;
    border-radius: 6px;
    margin-top: 10px;
    font-weight: bold;
}

QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 0 5px;
    background-color: white;
}
```

**表格样式**
```css
QTableWidget {
    gridline-color: #ddd;
    background-color: white;
    alternate-background-color: #f9f9f9;
}

QTableWidget::item:selected {
    background-color: #4CAF50;
    color: white;
}

QHeaderView::section {
    background-color: #f0f0f0;
    padding: 6px;
    border: 1px solid #ddd;
    font-weight: bold;
}
```

### 配色方案

**现代蓝色主题**
```css
/* 主色调 */
--primary: #2196F3;
--primary-dark: #1976D2;
--primary-light: #BBDEFB;

/* 辅助色 */
--accent: #FF5722;
--background: #FAFAFA;
--surface: #FFFFFF;
--text: #212121;
--text-secondary: #757575;
```

**专业绿色主题**
```css
/* 主色调 */
--primary: #4CAF50;
--primary-dark: #388E3C;
--primary-light: #C8E6C9;

/* 辅助色 */
--accent: #FFC107;
--background: #F5F5F5;
--surface: #FFFFFF;
--text: #212121;
--text-secondary: #616161;
```

## UI设计模板

### 模板1：设备控制面板

**功能描述：** 设备连接、参数设置、状态显示

**布局结构：**
```
┌─────────────────────────────────────┐
│ 设备控制面板                         │
├─────────────────────────────────────┤
│ ┌─连接设置─────┐ ┌─参数设置───────┐ │
│ │ 端口: [COM1▼]│ │ 频率: [1000]Hz │ │
│ │ 波特率:[9600▼]│ │ 幅度: [50]%   │ │
│ │ [连接] [断开]│ │ 模式: [连续▼] │ │
│ └─────────────┘ └───────────────┘ │
│ ┌─状态显示─────────────────────────┐ │
│ │ 状态: ● 已连接                   │ │
│ │ 数据: 1234 packets              │ │
│ │ 错误: 0 errors                  │ │
│ └─────────────────────────────────┘ │
│ [开始采集] [停止采集] [保存数据]     │
└─────────────────────────────────────┘
```

**代码实现：**

```cpp
// device_control_panel.h
#ifndef DEVICE_CONTROL_PANEL_H
#define DEVICE_CONTROL_PANEL_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

class DeviceControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceControlPanel(QWidget *parent = nullptr);
    ~DeviceControlPanel();

signals:
    void connectRequested(const QString &port, int baudRate);
    void disconnectRequested();
    void startAcquisition();
    void stopAcquisition();
    void saveData();

private slots:
    void onConnectClicked();
    void onDisconnectClicked();
    void onStartClicked();
    void onStopClicked();
    void onSaveClicked();

private:
    void setupUI();
    void createConnectionGroup();
    void createParameterGroup();
    void createStatusGroup();
    void createControlButtons();
    void applyStyles();

    // 连接设置
    QGroupBox *m_connectionGroup;
    QComboBox *m_portCombo;
    QComboBox *m_baudRateCombo;
    QPushButton *m_connectBtn;
    QPushButton *m_disconnectBtn;

    // 参数设置
    QGroupBox *m_parameterGroup;
    QSpinBox *m_frequencySpinBox;
    QSpinBox *m_amplitudeSpinBox;
    QComboBox *m_modeCombo;

    // 状态显示
    QGroupBox *m_statusGroup;
    QLabel *m_statusLabel;
    QLabel *m_dataLabel;
    QLabel *m_errorLabel;

    // 控制按钮
    QPushButton *m_startBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_saveBtn;
};

#endif // DEVICE_CONTROL_PANEL_H
```

```cpp
// device_control_panel.cpp
#include "device_control_panel.h"

DeviceControlPanel::DeviceControlPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    applyStyles();
}

DeviceControlPanel::~DeviceControlPanel()
{
}

void DeviceControlPanel::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 创建各个组
    createConnectionGroup();
    createParameterGroup();
    createStatusGroup();
    createControlButtons();

    // 添加到主布局
    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->addWidget(m_connectionGroup);
    topLayout->addWidget(m_parameterGroup);

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_statusGroup);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_startBtn);
    buttonLayout->addWidget(m_stopBtn);
    buttonLayout->addWidget(m_saveBtn);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);
}

void DeviceControlPanel::createConnectionGroup()
{
    m_connectionGroup = new QGroupBox("连接设置", this);
    QFormLayout *layout = new QFormLayout(m_connectionGroup);

    // 端口选择
    m_portCombo = new QComboBox(this);
    m_portCombo->addItems({"COM1", "COM2", "COM3", "COM4"});
    layout->addRow("端口:", m_portCombo);

    // 波特率选择
    m_baudRateCombo = new QComboBox(this);
    m_baudRateCombo->addItems({"9600", "19200", "38400", "115200"});
    m_baudRateCombo->setCurrentText("115200");
    layout->addRow("波特率:", m_baudRateCombo);

    // 连接按钮
    QHBoxLayout *btnLayout = new QHBoxLayout();
    m_connectBtn = new QPushButton("连接", this);
    m_disconnectBtn = new QPushButton("断开", this);
    m_disconnectBtn->setEnabled(false);
    btnLayout->addWidget(m_connectBtn);
    btnLayout->addWidget(m_disconnectBtn);
    layout->addRow(btnLayout);

    // 连接信号
    connect(m_connectBtn, &QPushButton::clicked,
            this, &DeviceControlPanel::onConnectClicked);
    connect(m_disconnectBtn, &QPushButton::clicked,
            this, &DeviceControlPanel::onDisconnectClicked);
}

void DeviceControlPanel::createParameterGroup()
{
    m_parameterGroup = new QGroupBox("参数设置", this);
    QFormLayout *layout = new QFormLayout(m_parameterGroup);

    // 频率设置
    m_frequencySpinBox = new QSpinBox(this);
    m_frequencySpinBox->setRange(1, 10000);
    m_frequencySpinBox->setValue(1000);
    m_frequencySpinBox->setSuffix(" Hz");
    layout->addRow("频率:", m_frequencySpinBox);

    // 幅度设置
    m_amplitudeSpinBox = new QSpinBox(this);
    m_amplitudeSpinBox->setRange(0, 100);
    m_amplitudeSpinBox->setValue(50);
    m_amplitudeSpinBox->setSuffix(" %");
    layout->addRow("幅度:", m_amplitudeSpinBox);

    // 模式选择
    m_modeCombo = new QComboBox(this);
    m_modeCombo->addItems({"连续", "单次", "触发"});
    layout->addRow("模式:", m_modeCombo);
}

void DeviceControlPanel::createStatusGroup()
{
    m_statusGroup = new QGroupBox("状态显示", this);
    QVBoxLayout *layout = new QVBoxLayout(m_statusGroup);

    m_statusLabel = new QLabel("状态: ○ 未连接", this);
    m_dataLabel = new QLabel("数据: 0 packets", this);
    m_errorLabel = new QLabel("错误: 0 errors", this);

    layout->addWidget(m_statusLabel);
    layout->addWidget(m_dataLabel);
    layout->addWidget(m_errorLabel);
}

void DeviceControlPanel::createControlButtons()
{
    m_startBtn = new QPushButton("开始采集", this);
    m_stopBtn = new QPushButton("停止采集", this);
    m_saveBtn = new QPushButton("保存数据", this);

    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(false);
    m_saveBtn->setEnabled(false);

    // 连接信号
    connect(m_startBtn, &QPushButton::clicked,
            this, &DeviceControlPanel::onStartClicked);
    connect(m_stopBtn, &QPushButton::clicked,
            this, &DeviceControlPanel::onStopClicked);
    connect(m_saveBtn, &QPushButton::clicked,
            this, &DeviceControlPanel::onSaveClicked);
}

void DeviceControlPanel::applyStyles()
{
    QString styleSheet = R"(
        QGroupBox {
            border: 2px solid #ddd;
            border-radius: 6px;
            margin-top: 10px;
            font-weight: bold;
            padding-top: 10px;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top left;
            padding: 0 5px;
            background-color: white;
        }

        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-size: 14px;
            min-width: 80px;
        }

        QPushButton:hover {
            background-color: #45a049;
        }

        QPushButton:pressed {
            background-color: #3d8b40;
        }

        QPushButton:disabled {
            background-color: #cccccc;
            color: #666666;
        }

        QComboBox, QSpinBox {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 4px;
            background-color: white;
            min-width: 100px;
        }

        QComboBox:focus, QSpinBox:focus {
            border: 1px solid #4CAF50;
        }

        QLabel {
            padding: 4px;
        }
    )";

    setStyleSheet(styleSheet);
}

void DeviceControlPanel::onConnectClicked()
{
    QString port = m_portCombo->currentText();
    int baudRate = m_baudRateCombo->currentText().toInt();

    emit connectRequested(port, baudRate);

    // 更新UI状态
    m_connectBtn->setEnabled(false);
    m_disconnectBtn->setEnabled(true);
    m_startBtn->setEnabled(true);
    m_statusLabel->setText("状态: ● 已连接");
}

void DeviceControlPanel::onDisconnectClicked()
{
    emit disconnectRequested();

    // 更新UI状态
    m_connectBtn->setEnabled(true);
    m_disconnectBtn->setEnabled(false);
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(false);
    m_statusLabel->setText("状态: ○ 未连接");
}

void DeviceControlPanel::onStartClicked()
{
    emit startAcquisition();

    // 更新UI状态
    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
    m_saveBtn->setEnabled(true);
}

void DeviceControlPanel::onStopClicked()
{
    emit stopAcquisition();

    // 更新UI状态
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
}

void DeviceControlPanel::onSaveClicked()
{
    emit saveData();
}
```

### 模板2：数据显示面板

**功能描述：** 实时数据显示、图表绘制、统计信息

**布局结构：**
```
┌─────────────────────────────────────┐
│ 数据显示面板                         │
├─────────────────────────────────────┤
│ ┌─实时数据─────────────────────────┐ │
│ │ [图表显示区域]                   │ │
│ │                                 │ │
│ │                                 │ │
│ └─────────────────────────────────┘ │
│ ┌─统计信息─────────────────────────┐ │
│ │ 最大值: 100  最小值: 0  平均值: 50│ │
│ │ 标准差: 10   采样率: 1000 Hz     │ │
│ └─────────────────────────────────┘ │
└─────────────────────────────────────┘
```

### 模板3：配置对话框

**功能描述：** 系统设置、参数配置

**代码实现：**

```cpp
// config_dialog.h
#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QDialogButtonBox>

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

private slots:
    void onAccepted();
    void onRejected();

private:
    void setupUI();
    QWidget* createGeneralPage();
    QWidget* createDevicePage();
    QWidget* createAdvancedPage();

    QTabWidget *m_tabWidget;
    QDialogButtonBox *m_buttonBox;
};

#endif // CONFIG_DIALOG_H
```

## UI设计最佳实践

### 1. 布局原则

**对齐和间距**
```cpp
// 统一的边距
layout->setContentsMargins(10, 10, 10, 10);

// 统一的间距
layout->setSpacing(8);

// 使用弹性空间
layout->addStretch();
```

**响应式设计**
```cpp
// 设置最小/最大尺寸
widget->setMinimumSize(400, 300);
widget->setMaximumSize(1920, 1080);

// 设置尺寸策略
widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
```

### 2. 用户体验

**即时反馈**
```cpp
// 按钮点击反馈
button->setEnabled(false);
// 执行操作
button->setEnabled(true);

// 进度提示
QProgressDialog progress("处理中...", "取消", 0, 100, this);
progress.setWindowModality(Qt::WindowModal);
```

**错误提示**
```cpp
// 友好的错误消息
QMessageBox::warning(this, "警告",
    "无法连接到设备，请检查连接设置。");

// 输入验证
QRegExpValidator *validator = new QRegExpValidator(
    QRegExp("[0-9]{1,5}"), lineEdit);
lineEdit->setValidator(validator);
```

### 3. 性能优化

**延迟加载**
```cpp
// 只在需要时创建UI
if (!m_settingsDialog) {
    m_settingsDialog = new SettingsDialog(this);
}
m_settingsDialog->show();
```

**批量更新**
```cpp
// 禁用更新，批量修改后再启用
tableWidget->setUpdatesEnabled(false);
for (int i = 0; i < 1000; ++i) {
    tableWidget->insertRow(i);
}
tableWidget->setUpdatesEnabled(true);
```

### 4. 可访问性

**键盘导航**
```cpp
// 设置Tab顺序
setTabOrder(nameEdit, ageSpinBox);
setTabOrder(ageSpinBox, submitButton);

// 设置快捷键
button->setShortcut(QKeySequence("Ctrl+S"));
```

**工具提示**
```cpp
// 添加提示信息
button->setToolTip("点击保存当前设置");
lineEdit->setToolTip("输入1-100之间的数字");
```

## 代码生成检查清单

生成UI代码时，确保包含：

- [ ] 完整的头文件（.h）和实现文件（.cpp）
- [ ] 正确的Qt 5.14 API使用
- [ ] 内存管理（父对象或智能指针）
- [ ] 信号槽连接
- [ ] 样式表（QSS）
- [ ] 布局管理
- [ ] 错误处理
- [ ] 注释说明

## 快速设计命令

当用户描述UI需求时：
1. 分析功能需求
2. 绘制ASCII布局图
3. 选择合适的组件
4. 生成完整代码（.h + .cpp）
5. 提供样式建议
6. 说明使用方法

记住：创建简洁、直观、符合Qt规范的用户界面，注重用户体验和代码质量。
