/********************************************************************************
** Form generated from reading UI file 'aiprotocolinputdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AIPROTOCOLINPUTDIALOG_H
#define UI_AIPROTOCOLINPUTDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_AIProtocolInputDialog
{
public:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QGroupBox *dataGroup;
    QVBoxLayout *dataLayout;
    QLabel *dataHint;
    QTextEdit *rawDataEdit;
    QGroupBox *rulesGroup;
    QVBoxLayout *rulesLayout;
    QLabel *rulesHint;
    QTextEdit *rulesEdit;
    QGroupBox *methodGroup;
    QVBoxLayout *methodLayout;
    QRadioButton *useAPIRadio;
    QRadioButton *useCLIRadio;
    QLabel *methodHintLabel;
    QLabel *statusLabel;
    QProgressBar *progressBar;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *buttonLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pasteExampleBtn;
    QPushButton *clearBtn;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *generateBtn;
    QPushButton *cancelBtn;

    void setupUi(QDialog *AIProtocolInputDialog)
    {
        if (AIProtocolInputDialog->objectName().isEmpty())
            AIProtocolInputDialog->setObjectName(QString::fromUtf8("AIProtocolInputDialog"));
        AIProtocolInputDialog->resize(700, 600);
        mainLayout = new QVBoxLayout(AIProtocolInputDialog);
        mainLayout->setSpacing(15);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(20, 20, 20, 20);
        titleLabel = new QLabel(AIProtocolInputDialog);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        titleLabel->setStyleSheet(QString::fromUtf8("font-size: 14px; font-weight: bold; color: #2c3e50;"));

        mainLayout->addWidget(titleLabel);

        dataGroup = new QGroupBox(AIProtocolInputDialog);
        dataGroup->setObjectName(QString::fromUtf8("dataGroup"));
        dataLayout = new QVBoxLayout(dataGroup);
        dataLayout->setObjectName(QString::fromUtf8("dataLayout"));
        dataHint = new QLabel(dataGroup);
        dataHint->setObjectName(QString::fromUtf8("dataHint"));
        dataHint->setStyleSheet(QString::fromUtf8("color: #7f8c8d; font-size: 12px;"));

        dataLayout->addWidget(dataHint);

        rawDataEdit = new QTextEdit(dataGroup);
        rawDataEdit->setObjectName(QString::fromUtf8("rawDataEdit"));
        rawDataEdit->setMinimumSize(QSize(0, 100));
        rawDataEdit->setMaximumSize(QSize(16777215, 150));

        dataLayout->addWidget(rawDataEdit);


        mainLayout->addWidget(dataGroup);

        rulesGroup = new QGroupBox(AIProtocolInputDialog);
        rulesGroup->setObjectName(QString::fromUtf8("rulesGroup"));
        rulesLayout = new QVBoxLayout(rulesGroup);
        rulesLayout->setObjectName(QString::fromUtf8("rulesLayout"));
        rulesHint = new QLabel(rulesGroup);
        rulesHint->setObjectName(QString::fromUtf8("rulesHint"));
        rulesHint->setStyleSheet(QString::fromUtf8("color: #7f8c8d; font-size: 12px;"));

        rulesLayout->addWidget(rulesHint);

        rulesEdit = new QTextEdit(rulesGroup);
        rulesEdit->setObjectName(QString::fromUtf8("rulesEdit"));
        rulesEdit->setMinimumSize(QSize(0, 150));

        rulesLayout->addWidget(rulesEdit);


        mainLayout->addWidget(rulesGroup);

        methodGroup = new QGroupBox(AIProtocolInputDialog);
        methodGroup->setObjectName(QString::fromUtf8("methodGroup"));
        methodLayout = new QVBoxLayout(methodGroup);
        methodLayout->setObjectName(QString::fromUtf8("methodLayout"));
        useAPIRadio = new QRadioButton(methodGroup);
        useAPIRadio->setObjectName(QString::fromUtf8("useAPIRadio"));

        methodLayout->addWidget(useAPIRadio);

        useCLIRadio = new QRadioButton(methodGroup);
        useCLIRadio->setObjectName(QString::fromUtf8("useCLIRadio"));
        useCLIRadio->setChecked(true);

        methodLayout->addWidget(useCLIRadio);

        methodHintLabel = new QLabel(methodGroup);
        methodHintLabel->setObjectName(QString::fromUtf8("methodHintLabel"));
        methodHintLabel->setStyleSheet(QString::fromUtf8("color: #3498db; font-size: 12px; padding: 5px;"));
        methodHintLabel->setWordWrap(true);

        methodLayout->addWidget(methodHintLabel);


        mainLayout->addWidget(methodGroup);

        statusLabel = new QLabel(AIProtocolInputDialog);
        statusLabel->setObjectName(QString::fromUtf8("statusLabel"));
        statusLabel->setStyleSheet(QString::fromUtf8("color: #27ae60; font-weight: bold;"));
        statusLabel->setWordWrap(true);

        mainLayout->addWidget(statusLabel);

        progressBar = new QProgressBar(AIProtocolInputDialog);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setVisible(false);
        progressBar->setMinimum(0);
        progressBar->setMaximum(0);

        mainLayout->addWidget(progressBar);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        mainLayout->addItem(verticalSpacer);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(horizontalSpacer);

        pasteExampleBtn = new QPushButton(AIProtocolInputDialog);
        pasteExampleBtn->setObjectName(QString::fromUtf8("pasteExampleBtn"));

        buttonLayout->addWidget(pasteExampleBtn);

        clearBtn = new QPushButton(AIProtocolInputDialog);
        clearBtn->setObjectName(QString::fromUtf8("clearBtn"));

        buttonLayout->addWidget(clearBtn);

        horizontalSpacer_2 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        buttonLayout->addItem(horizontalSpacer_2);

        generateBtn = new QPushButton(AIProtocolInputDialog);
        generateBtn->setObjectName(QString::fromUtf8("generateBtn"));

        buttonLayout->addWidget(generateBtn);

        cancelBtn = new QPushButton(AIProtocolInputDialog);
        cancelBtn->setObjectName(QString::fromUtf8("cancelBtn"));

        buttonLayout->addWidget(cancelBtn);


        mainLayout->addLayout(buttonLayout);


        retranslateUi(AIProtocolInputDialog);

        generateBtn->setDefault(true);


        QMetaObject::connectSlotsByName(AIProtocolInputDialog);
    } // setupUi

    void retranslateUi(QDialog *AIProtocolInputDialog)
    {
        AIProtocolInputDialog->setWindowTitle(QCoreApplication::translate("AIProtocolInputDialog", "AI\346\231\272\350\203\275\345\215\217\350\256\256\347\224\237\346\210\220", nullptr));
        titleLabel->setText(QCoreApplication::translate("AIProtocolInputDialog", "\351\200\232\350\277\207AI\345\210\206\346\236\220\345\216\237\345\247\213\346\225\260\346\215\256\345\222\214\350\247\243\346\236\220\350\247\204\345\210\231\357\274\214\350\207\252\345\212\250\347\224\237\346\210\220\345\215\217\350\256\256\351\205\215\347\275\256", nullptr));
        dataGroup->setTitle(QCoreApplication::translate("AIProtocolInputDialog", "\345\216\237\345\247\21316\350\277\233\345\210\266\346\225\260\346\215\256\346\240\267\346\234\254", nullptr));
        dataHint->setText(QCoreApplication::translate("AIProtocolInputDialog", "\350\257\267\350\276\223\345\205\245\344\270\200\346\235\241\345\256\214\346\225\264\347\232\20416\350\277\233\345\210\266\346\225\260\346\215\256\346\240\267\346\234\254\357\274\210\347\251\272\346\240\274\345\210\206\351\232\224\357\274\214\345\246\202\357\274\232AA 55 01 02 03\357\274\211", nullptr));
        rawDataEdit->setPlaceholderText(QCoreApplication::translate("AIProtocolInputDialog", "\347\244\272\344\276\213\357\274\232\n"
"AA55 0101 2E00 C7F6 2901 601C 0000 0266 F7FF 38DA FFFF 959B 0100 C9C9 0E00 6104 EDFF BF46 1810 D4FF 0000 E1FF 0000 40FF 0000 0000 DC7E", nullptr));
        rulesGroup->setTitle(QCoreApplication::translate("AIProtocolInputDialog", "\350\247\243\346\236\220\350\247\204\345\210\231\346\217\217\350\277\260", nullptr));
        rulesHint->setText(QCoreApplication::translate("AIProtocolInputDialog", "\350\257\267\350\257\246\347\273\206\346\217\217\350\277\260\346\225\260\346\215\256\347\232\204\350\247\243\346\236\220\350\247\204\345\210\231\357\274\210\350\265\267\345\247\213\344\275\215\347\275\256\343\200\201\346\225\260\346\215\256\347\261\273\345\236\213\343\200\201\347\274\251\346\224\276\345\233\240\345\255\220\347\255\211\357\274\211", nullptr));
        rulesEdit->setPlaceholderText(QCoreApplication::translate("AIProtocolInputDialog", "\347\244\272\344\276\213\357\274\232\n"
"\345\216\237\345\247\213\346\225\260\346\215\256\344\273\216\347\254\2547\345\255\227\350\212\202\345\274\200\345\247\213\357\274\214\345\217\22644\345\255\227\350\212\202\357\274\214\346\214\211\347\205\247\345\260\217\347\253\257int32\346\250\241\345\274\217\350\247\243\346\236\220\344\270\27211\344\270\252\346\225\260\346\215\256\n"
"\346\225\260\346\215\256\346\216\222\345\210\227\351\241\272\345\272\217\357\274\232\346\227\266\351\227\264\346\210\263\343\200\201\346\270\251\345\272\246\343\200\201\350\247\222\351\200\237\345\272\246(3\344\270\252)\343\200\201\345\212\240\351\200\237\345\272\246(3\344\270\252)\343\200\201\346\254\247\346\213\211\350\247\222(3\344\270\252)\n"
"\n"
"\350\275\254\346\215\242\345\205\254\345\274\217\357\274\232\n"
"- \346\270\251\345\272\246/256 = \345\256\236\351\231\205\346\270\251\345\272\246\n"
"- \350\247\222\351\200\237\345\272\246/65536/1000000*\351\207\217\347\250\213 = \345\256\236\351\231\205\350\247\222\351\200\237\345\272\246"
                        "\n"
"- \345\212\240\351\200\237\345\272\246/65536/32768*\351\207\217\347\250\213 = \345\256\236\351\231\205\345\212\240\351\200\237\345\272\246\n"
"- \346\254\247\346\213\211\350\247\222\350\275\254\346\210\220I16\345\220\216*180/32768 = \345\256\236\351\231\205\346\254\247\346\213\211\350\247\222", nullptr));
        methodGroup->setTitle(QCoreApplication::translate("AIProtocolInputDialog", "\347\224\237\346\210\220\346\226\271\345\274\217", nullptr));
        useAPIRadio->setText(QCoreApplication::translate("AIProtocolInputDialog", "\344\275\277\347\224\250API\350\260\203\347\224\250\357\274\210\351\234\200\350\246\201API\345\257\206\351\222\245\357\274\211", nullptr));
        useCLIRadio->setText(QCoreApplication::translate("AIProtocolInputDialog", "\344\275\277\347\224\250CLI\345\221\275\344\273\244\350\241\214\357\274\210\351\234\200\350\246\201\345\256\211\350\243\205claude\345\221\275\344\273\244\357\274\211", nullptr));
        pasteExampleBtn->setText(QCoreApplication::translate("AIProtocolInputDialog", "\347\262\230\350\264\264\347\244\272\344\276\213", nullptr));
        clearBtn->setText(QCoreApplication::translate("AIProtocolInputDialog", "\346\270\205\347\251\272", nullptr));
        generateBtn->setText(QCoreApplication::translate("AIProtocolInputDialog", "\347\224\237\346\210\220\345\215\217\350\256\256", nullptr));
        cancelBtn->setText(QCoreApplication::translate("AIProtocolInputDialog", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AIProtocolInputDialog: public Ui_AIProtocolInputDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AIPROTOCOLINPUTDIALOG_H
