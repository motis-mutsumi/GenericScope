/********************************************************************************
** Form generated from reading UI file 'protocoltestdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROTOCOLTESTDIALOG_H
#define UI_PROTOCOLTESTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ProtocolTestDialog
{
public:
    QVBoxLayout *mainLayout;
    QGroupBox *inputGroup;
    QVBoxLayout *inputLayout;
    QTextEdit *inputEdit;
    QHBoxLayout *btnLayout;
    QPushButton *parseBtn;
    QPushButton *clearBtn;
    QPushButton *sampleBtn;
    QSpacerItem *horizontalSpacer;
    QLabel *statusLabel;
    QGroupBox *resultGroup;
    QVBoxLayout *resultLayout;
    QLabel *checksumLabel;
    QLabel *rawDataLabel;
    QTableWidget *resultTable;
    QHBoxLayout *closeBtnLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *closeBtn;

    void setupUi(QDialog *ProtocolTestDialog)
    {
        if (ProtocolTestDialog->objectName().isEmpty())
            ProtocolTestDialog->setObjectName(QString::fromUtf8("ProtocolTestDialog"));
        ProtocolTestDialog->resize(800, 600);
        mainLayout = new QVBoxLayout(ProtocolTestDialog);
        mainLayout->setSpacing(10);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(10, 10, 10, 10);
        inputGroup = new QGroupBox(ProtocolTestDialog);
        inputGroup->setObjectName(QString::fromUtf8("inputGroup"));
        inputLayout = new QVBoxLayout(inputGroup);
        inputLayout->setObjectName(QString::fromUtf8("inputLayout"));
        inputEdit = new QTextEdit(inputGroup);
        inputEdit->setObjectName(QString::fromUtf8("inputEdit"));
        inputEdit->setMaximumSize(QSize(16777215, 100));

        inputLayout->addWidget(inputEdit);

        btnLayout = new QHBoxLayout();
        btnLayout->setObjectName(QString::fromUtf8("btnLayout"));
        parseBtn = new QPushButton(inputGroup);
        parseBtn->setObjectName(QString::fromUtf8("parseBtn"));

        btnLayout->addWidget(parseBtn);

        clearBtn = new QPushButton(inputGroup);
        clearBtn->setObjectName(QString::fromUtf8("clearBtn"));

        btnLayout->addWidget(clearBtn);

        sampleBtn = new QPushButton(inputGroup);
        sampleBtn->setObjectName(QString::fromUtf8("sampleBtn"));

        btnLayout->addWidget(sampleBtn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        btnLayout->addItem(horizontalSpacer);


        inputLayout->addLayout(btnLayout);


        mainLayout->addWidget(inputGroup);

        statusLabel = new QLabel(ProtocolTestDialog);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setStyleSheet(QString::fromUtf8("QLabel { color: blue; font-weight: bold; }"));

        mainLayout->addWidget(statusLabel);

        resultGroup = new QGroupBox(ProtocolTestDialog);
        resultGroup->setObjectName(QString::fromUtf8("resultGroup"));
        resultLayout = new QVBoxLayout(resultGroup);
        resultLayout->setObjectName(QString::fromUtf8("resultLayout"));
        checksumLabel = new QLabel(resultGroup);
        checksumLabel->setObjectName(QString::fromUtf8("checksumLabel"));
        checksumLabel->setStyleSheet(QString::fromUtf8("QLabel { padding: 5px; }"));

        resultLayout->addWidget(checksumLabel);

        rawDataLabel = new QLabel(resultGroup);
        rawDataLabel->setObjectName(QString::fromUtf8("rawDataLabel"));
        rawDataLabel->setWordWrap(true);
        rawDataLabel->setStyleSheet(QString::fromUtf8("QLabel { padding: 5px; background-color: #f0f0f0; }"));

        resultLayout->addWidget(rawDataLabel);

        resultTable = new QTableWidget(resultGroup);
        if (resultTable->columnCount() < 4)
            resultTable->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        resultTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        resultTable->setObjectName(QString::fromUtf8("resultTable"));
        resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        resultTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        resultTable->horizontalHeader()->setStretchLastSection(true);

        resultLayout->addWidget(resultTable);


        mainLayout->addWidget(resultGroup);

        closeBtnLayout = new QHBoxLayout();
        closeBtnLayout->setObjectName(QString::fromUtf8("closeBtnLayout"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        closeBtnLayout->addItem(horizontalSpacer_2);

        closeBtn = new QPushButton(ProtocolTestDialog);
        closeBtn->setObjectName(QString::fromUtf8("closeBtn"));

        closeBtnLayout->addWidget(closeBtn);


        mainLayout->addLayout(closeBtnLayout);


        retranslateUi(ProtocolTestDialog);

        QMetaObject::connectSlotsByName(ProtocolTestDialog);
    } // setupUi

    void retranslateUi(QDialog *ProtocolTestDialog)
    {
        ProtocolTestDialog->setWindowTitle(QCoreApplication::translate("ProtocolTestDialog", "\345\215\217\350\256\256\346\265\213\350\257\225", nullptr));
        inputGroup->setTitle(QCoreApplication::translate("ProtocolTestDialog", "\346\265\213\350\257\225\346\225\260\346\215\256\350\276\223\345\205\245\357\274\21016\350\277\233\345\210\266\357\274\211", nullptr));
        inputEdit->setPlaceholderText(QCoreApplication::translate("ProtocolTestDialog", "\350\257\267\350\276\223\345\205\24516\350\277\233\345\210\266\346\225\260\346\215\256\357\274\214\344\276\213\345\246\202\357\274\232FF AA 01 02 03 04\n"
"\346\224\257\346\214\201\347\251\272\346\240\274\343\200\201\346\215\242\350\241\214\345\210\206\351\232\224", nullptr));
        parseBtn->setText(QCoreApplication::translate("ProtocolTestDialog", "\350\247\243\346\236\220\346\225\260\346\215\256", nullptr));
        clearBtn->setText(QCoreApplication::translate("ProtocolTestDialog", "\346\270\205\347\251\272", nullptr));
        sampleBtn->setText(QCoreApplication::translate("ProtocolTestDialog", "\345\212\240\350\275\275\347\244\272\344\276\213", nullptr));
        statusLabel->setText(QCoreApplication::translate("ProtocolTestDialog", "\345\260\261\347\273\252", nullptr));
        resultGroup->setTitle(QCoreApplication::translate("ProtocolTestDialog", "\350\247\243\346\236\220\347\273\223\346\236\234", nullptr));
        checksumLabel->setText(QCoreApplication::translate("ProtocolTestDialog", "\346\240\241\351\252\214\347\240\201\357\274\232\346\234\252\351\252\214\350\257\201", nullptr));
        rawDataLabel->setText(QCoreApplication::translate("ProtocolTestDialog", "\345\216\237\345\247\213\346\225\260\346\215\256\357\274\232", nullptr));
        QTableWidgetItem *___qtablewidgetitem = resultTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("ProtocolTestDialog", "\345\255\227\346\256\265\345\220\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = resultTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("ProtocolTestDialog", "\346\225\260\346\215\256\347\261\273\345\236\213", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = resultTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("ProtocolTestDialog", "\350\247\243\346\236\220\345\200\274", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = resultTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("ProtocolTestDialog", "\345\215\225\344\275\215", nullptr));
        closeBtn->setText(QCoreApplication::translate("ProtocolTestDialog", "\345\205\263\351\227\255", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ProtocolTestDialog: public Ui_ProtocolTestDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROTOCOLTESTDIALOG_H
