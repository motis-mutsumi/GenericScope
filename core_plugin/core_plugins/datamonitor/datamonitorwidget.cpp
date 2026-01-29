#include "datamonitorwidget.h"
#include "ui_datamonitorwidget.h"
#include <QDateTime>

DataMonitorWidget::DataMonitorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataMonitorWidget)
{
    ui->setupUi(this);
    initTable();
}

DataMonitorWidget::~DataMonitorWidget()
{
    delete ui;
}

void DataMonitorWidget::setThresholds(const QMap<QString, double> &thresholds)
{
    m_thresholds = thresholds;

    // 初始化表格行
    int row = 0;
    for (auto it = thresholds.begin(); it != thresholds.end(); ++it) {
        m_fieldRowMap[it.key()] = row;
        ui->thresholdTable->insertRow(row);

        // 字段名
        ui->thresholdTable->setItem(row, 0, new QTableWidgetItem(it.key()));
        // 当前值（初始为0）
        ui->thresholdTable->setItem(row, 1, new QTableWidgetItem("0.00"));
        // 阈值
        ui->thresholdTable->setItem(row, 2, new QTableWidgetItem(QString::number(it.value(), 'f', 2)));
        // 状态（初始为正常）
        QTableWidgetItem *statusItem = new QTableWidgetItem("正常");
        statusItem->setForeground(Qt::green);
        ui->thresholdTable->setItem(row, 3, statusItem);

        row++;
    }
}

void DataMonitorWidget::updateData(const QVariantMap &fieldData)
{
    for (auto it = fieldData.begin(); it != fieldData.end(); ++it) {
        QString fieldName = it.key();
        double value = it.value().toDouble();

        if (m_fieldRowMap.contains(fieldName)) {
            int row = m_fieldRowMap[fieldName];

            // 更新当前值
            ui->thresholdTable->item(row, 1)->setText(QString::number(value, 'f', 2));

            // 检查是否超过阈值
            if (m_thresholds.contains(fieldName)) {
                bool exceeded = qAbs(value) > m_thresholds[fieldName];
                QTableWidgetItem *statusItem = ui->thresholdTable->item(row, 3);

                if (exceeded) {
                    statusItem->setText("警报");
                    statusItem->setForeground(Qt::red);

                    // 添加警报日志
                    QString logMessage = QString("[%1] %2: %3 > %4")
                                             .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                                             .arg(fieldName)
                                             .arg(value, 0, 'f', 2)
                                             .arg(m_thresholds[fieldName], 0, 'f', 2);
                    addAlertLog(logMessage);

                    emit thresholdExceeded(fieldName, value);
                } else {
                    statusItem->setText("正常");
                    statusItem->setForeground(Qt::green);
                }
            }
        }
    }
}

void DataMonitorWidget::initTable()
{
    ui->thresholdTable->setColumnCount(4);
    ui->thresholdTable->setHorizontalHeaderLabels({"字段名", "当前值", "阈值", "状态"});
    ui->thresholdTable->horizontalHeader()->setStretchLastSection(true);
    ui->thresholdTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->thresholdTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->thresholdTable->setAlternatingRowColors(true);
}

void DataMonitorWidget::addAlertLog(const QString &message)
{
    ui->alertLogEdit->append(message);

    // 限制日志行数（最多1000行）
    if (ui->alertLogEdit->document()->lineCount() > 1000) {
        QTextCursor cursor = ui->alertLogEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 100);
        cursor.removeSelectedText();
    }
}
