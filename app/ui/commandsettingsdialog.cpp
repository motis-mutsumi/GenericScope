#include "commandsettingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

CommandSettingsDialog::CommandSettingsDialog(QWidget *parent)
    : QDialog(parent)
    , ui(nullptr)
{
    setupUI();
    setupConnections();
    loadCommands();
    applyStyles();
}

CommandSettingsDialog::~CommandSettingsDialog()
{
}

void CommandSettingsDialog::setupUI()
{
    setWindowTitle("指令设置");
    resize(900, 600);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 创建分割器
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    // ========== 左侧：指令列表 ==========
    QWidget *leftWidget = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox *listGroup = new QGroupBox("指令列表", this);
    QVBoxLayout *listLayout = new QVBoxLayout(listGroup);

    // 创建表格
    QTableWidget *commandTable = new QTableWidget(this);
    commandTable->setObjectName("commandTable");
    commandTable->setColumnCount(3);
    commandTable->setHorizontalHeaderLabels({"序号", "名称", "指令内容(HEX)"});
    commandTable->horizontalHeader()->setStretchLastSection(true);
    commandTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    commandTable->setSelectionMode(QAbstractItemView::SingleSelection);
    commandTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    commandTable->setAlternatingRowColors(true);
    commandTable->verticalHeader()->setVisible(false);

    // 设置列宽
    commandTable->setColumnWidth(0, 60);
    commandTable->setColumnWidth(1, 150);

    listLayout->addWidget(commandTable);

    // 左侧按钮
    QHBoxLayout *leftButtonLayout = new QHBoxLayout();
    QPushButton *addBtn = new QPushButton("添加", this);
    QPushButton *deleteBtn = new QPushButton("删除", this);
    QPushButton *importBtn = new QPushButton("导入", this);

    addBtn->setObjectName("addBtn");
    deleteBtn->setObjectName("deleteBtn");
    importBtn->setObjectName("importBtn");

    leftButtonLayout->addWidget(addBtn);
    leftButtonLayout->addWidget(deleteBtn);
    leftButtonLayout->addWidget(importBtn);
    leftButtonLayout->addStretch();

    listLayout->addLayout(leftButtonLayout);
    leftLayout->addWidget(listGroup);

    // ========== 右侧：指令详情 ==========
    QWidget *rightWidget = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox *detailGroup = new QGroupBox("指令详情", this);
    QVBoxLayout *detailLayout = new QVBoxLayout(detailGroup);
    detailLayout->setSpacing(10);

    // 指令名称
    QLabel *nameLabel = new QLabel("指令名称:", this);
    QLineEdit *nameEdit = new QLineEdit(this);
    nameEdit->setObjectName("nameEdit");
    nameEdit->setPlaceholderText("例如: 复位指令");

    // 指令内容
    QLabel *contentLabel = new QLabel("指令内容(HEX):", this);
    QLineEdit *contentEdit = new QLineEdit(this);
    contentEdit->setObjectName("contentEdit");
    contentEdit->setPlaceholderText("例如: FF AA 01 02 03");

    // 描述
    QLabel *descLabel = new QLabel("描述:", this);
    QTextEdit *descEdit = new QTextEdit(this);
    descEdit->setObjectName("descEdit");
    descEdit->setPlaceholderText("指令的详细说明...");
    descEdit->setMaximumHeight(100);

    detailLayout->addWidget(nameLabel);
    detailLayout->addWidget(nameEdit);
    detailLayout->addWidget(contentLabel);
    detailLayout->addWidget(contentEdit);
    detailLayout->addWidget(descLabel);
    detailLayout->addWidget(descEdit);

    // 右侧按钮
    QHBoxLayout *rightButtonLayout = new QHBoxLayout();
    QPushButton *clearBtn = new QPushButton("清空", this);
    QPushButton *generateBtn = new QPushButton("生成", this);

    clearBtn->setObjectName("clearBtn");
    generateBtn->setObjectName("generateBtn");

    rightButtonLayout->addWidget(clearBtn);
    rightButtonLayout->addWidget(generateBtn);
    rightButtonLayout->addStretch();

    detailLayout->addLayout(rightButtonLayout);
    detailLayout->addStretch();

    rightLayout->addWidget(detailGroup);

    // 添加到分割器
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);

    // ========== 底部按钮 ==========
    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName("buttonBox");
    buttonBox->setStandardButtons(QDialogButtonBox::Ok |
                                   QDialogButtonBox::Cancel |
                                   QDialogButtonBox::Apply);

    QPushButton *okBtn = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelBtn = buttonBox->button(QDialogButtonBox::Cancel);
    QPushButton *applyBtn = buttonBox->button(QDialogButtonBox::Apply);

    okBtn->setText("确定");
    cancelBtn->setText("取消");
    applyBtn->setText("应用");

    mainLayout->addWidget(buttonBox);
}

void CommandSettingsDialog::setupConnections()
{
    // 获取控件
    QTableWidget *commandTable = findChild<QTableWidget*>("commandTable");
    QPushButton *addBtn = findChild<QPushButton*>("addBtn");
    QPushButton *deleteBtn = findChild<QPushButton*>("deleteBtn");
    QPushButton *importBtn = findChild<QPushButton*>("importBtn");
    QPushButton *clearBtn = findChild<QPushButton*>("clearBtn");
    QPushButton *generateBtn = findChild<QPushButton*>("generateBtn");
    QDialogButtonBox *buttonBox = findChild<QDialogButtonBox*>("buttonBox");

    // 连接信号
    connect(addBtn, &QPushButton::clicked, this, &CommandSettingsDialog::onAddCommand);
    connect(deleteBtn, &QPushButton::clicked, this, &CommandSettingsDialog::onDeleteCommand);
    connect(importBtn, &QPushButton::clicked, this, &CommandSettingsDialog::onImportCommands);
    connect(clearBtn, &QPushButton::clicked, this, &CommandSettingsDialog::onClearCommand);
    connect(generateBtn, &QPushButton::clicked, this, &CommandSettingsDialog::onGenerateCommand);

    connect(commandTable, &QTableWidget::itemSelectionChanged,
            this, &CommandSettingsDialog::onTableSelectionChanged);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &CommandSettingsDialog::onAccepted);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &CommandSettingsDialog::onRejected);

    QPushButton *applyBtn = buttonBox->button(QDialogButtonBox::Apply);
    connect(applyBtn, &QPushButton::clicked, this, &CommandSettingsDialog::onApplyClicked);
}

void CommandSettingsDialog::onAddCommand()
{
    QTableWidget *commandTable = findChild<QTableWidget*>("commandTable");
    QLineEdit *nameEdit = findChild<QLineEdit*>("nameEdit");
    QLineEdit *contentEdit = findChild<QLineEdit*>("contentEdit");

    QString name = nameEdit->text().trimmed();
    QString content = contentEdit->text().trimmed();

    if (name.isEmpty() || content.isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写指令名称和内容！");
        return;
    }

    // 添加到表格
    int row = commandTable->rowCount();
    commandTable->insertRow(row);

    commandTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    commandTable->setItem(row, 1, new QTableWidgetItem(name));
    commandTable->setItem(row, 2, new QTableWidgetItem(content));

    // 清空输入框
    clearCommandDetails();

    QMessageBox::information(this, "成功", "指令添加成功！");
}

void CommandSettingsDialog::onDeleteCommand()
{
    QTableWidget *commandTable = findChild<QTableWidget*>("commandTable");

    int currentRow = commandTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, "警告", "请先选择要删除的指令！");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "确认删除", "确定要删除选中的指令吗？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        commandTable->removeRow(currentRow);

        // 更新序号
        for (int i = 0; i < commandTable->rowCount(); ++i) {
            commandTable->item(i, 0)->setText(QString::number(i + 1));
        }

        clearCommandDetails();
    }
}

void CommandSettingsDialog::onImportCommands()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, "导入指令文件", "",
        "JSON文件 (*.json);;文本文件 (*.txt);;所有文件 (*.*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法打开文件！");
        return;
    }

    // TODO: 实现文件解析逻辑
    QMessageBox::information(this, "提示",
        QString("导入功能待实现\n文件: %1").arg(fileName));

    file.close();
}

void CommandSettingsDialog::onClearCommand()
{
    clearCommandDetails();
}

void CommandSettingsDialog::onGenerateCommand()
{
    // TODO: 实现自动生成指令逻辑
    QMessageBox::information(this, "提示", "自动生成功能待实现");
}

void CommandSettingsDialog::onTableSelectionChanged()
{
    QTableWidget *commandTable = findChild<QTableWidget*>("commandTable");
    int currentRow = commandTable->currentRow();

    if (currentRow >= 0) {
        updateCommandDetails(currentRow);
    }
}

void CommandSettingsDialog::onAccepted()
{
    saveCommands();
    accept();
}

void CommandSettingsDialog::onRejected()
{
    reject();
}

void CommandSettingsDialog::onApplyClicked()
{
    saveCommands();
    QMessageBox::information(this, "成功", "设置已应用！");
}

void CommandSettingsDialog::loadCommands()
{
    // TODO: 从配置文件加载指令
    // 这里可以添加一些示例数据
    QTableWidget *commandTable = findChild<QTableWidget*>("commandTable");

    // 示例数据
    QStringList examples = {
        "复位指令|FF AA 01",
        "查询状态|FF AA 02",
        "设置参数|FF AA 03"
    };

    for (int i = 0; i < examples.size(); ++i) {
        QStringList parts = examples[i].split("|");
        if (parts.size() == 2) {
            commandTable->insertRow(i);
            commandTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            commandTable->setItem(i, 1, new QTableWidgetItem(parts[0]));
            commandTable->setItem(i, 2, new QTableWidgetItem(parts[1]));
        }
    }
}

void CommandSettingsDialog::saveCommands()
{
    // TODO: 保存指令到配置文件
    QTableWidget *commandTable = findChild<QTableWidget*>("commandTable");

    qDebug() << "保存" << commandTable->rowCount() << "条指令";
}

void CommandSettingsDialog::updateCommandDetails(int row)
{
    QTableWidget *commandTable = findChild<QTableWidget*>("commandTable");
    QLineEdit *nameEdit = findChild<QLineEdit*>("nameEdit");
    QLineEdit *contentEdit = findChild<QLineEdit*>("contentEdit");

    if (row >= 0 && row < commandTable->rowCount()) {
        nameEdit->setText(commandTable->item(row, 1)->text());
        contentEdit->setText(commandTable->item(row, 2)->text());
    }
}

void CommandSettingsDialog::clearCommandDetails()
{
    QLineEdit *nameEdit = findChild<QLineEdit*>("nameEdit");
    QLineEdit *contentEdit = findChild<QLineEdit*>("contentEdit");
    QTextEdit *descEdit = findChild<QTextEdit*>("descEdit");

    nameEdit->clear();
    contentEdit->clear();
    descEdit->clear();
}

void CommandSettingsDialog::applyStyles()
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

        QLineEdit, QTextEdit {
            border: 1px solid #ddd;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }

        QLineEdit:focus, QTextEdit:focus {
            border: 1px solid #4CAF50;
        }

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

        QLabel {
            font-weight: bold;
            color: #333;
        }
    )";

    setStyleSheet(styleSheet);
}
