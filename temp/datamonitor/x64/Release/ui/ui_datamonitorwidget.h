/********************************************************************************
** Form generated from reading UI file 'datamonitorwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAMONITORWIDGET_H
#define UI_DATAMONITORWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DataMonitorWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QGroupBox *thresholdGroupBox;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *thresholdTable;
    QGroupBox *alertGroupBox;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *alertLogEdit;

    void setupUi(QWidget *DataMonitorWidget)
    {
        if (DataMonitorWidget->objectName().isEmpty())
            DataMonitorWidget->setObjectName(QString::fromUtf8("DataMonitorWidget"));
        DataMonitorWidget->resize(600, 500);
        verticalLayout = new QVBoxLayout(DataMonitorWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(DataMonitorWidget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        thresholdGroupBox = new QGroupBox(DataMonitorWidget);
        thresholdGroupBox->setObjectName(QString::fromUtf8("thresholdGroupBox"));
        verticalLayout_2 = new QVBoxLayout(thresholdGroupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        thresholdTable = new QTableWidget(thresholdGroupBox);
        thresholdTable->setObjectName(QString::fromUtf8("thresholdTable"));
        thresholdTable->setAlternatingRowColors(true);
        thresholdTable->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_2->addWidget(thresholdTable);


        verticalLayout->addWidget(thresholdGroupBox);

        alertGroupBox = new QGroupBox(DataMonitorWidget);
        alertGroupBox->setObjectName(QString::fromUtf8("alertGroupBox"));
        verticalLayout_3 = new QVBoxLayout(alertGroupBox);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        alertLogEdit = new QTextEdit(alertGroupBox);
        alertLogEdit->setObjectName(QString::fromUtf8("alertLogEdit"));
        alertLogEdit->setReadOnly(true);
        alertLogEdit->setMaximumHeight(150);

        verticalLayout_3->addWidget(alertLogEdit);


        verticalLayout->addWidget(alertGroupBox);


        retranslateUi(DataMonitorWidget);

        QMetaObject::connectSlotsByName(DataMonitorWidget);
    } // setupUi

    void retranslateUi(QWidget *DataMonitorWidget)
    {
        DataMonitorWidget->setWindowTitle(QCoreApplication::translate("DataMonitorWidget", "\346\225\260\346\215\256\347\233\221\346\216\247", nullptr));
        titleLabel->setText(QCoreApplication::translate("DataMonitorWidget", "\346\225\260\346\215\256\347\233\221\346\216\247 - \345\256\236\346\227\266\351\230\210\345\200\274\346\243\200\346\265\213", nullptr));
        thresholdGroupBox->setTitle(QCoreApplication::translate("DataMonitorWidget", "\351\230\210\345\200\274\347\233\221\346\216\247\350\241\250", nullptr));
        alertGroupBox->setTitle(QCoreApplication::translate("DataMonitorWidget", "\350\255\246\346\212\245\346\227\245\345\277\227", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DataMonitorWidget: public Ui_DataMonitorWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAMONITORWIDGET_H
