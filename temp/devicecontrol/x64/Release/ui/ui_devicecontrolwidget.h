/********************************************************************************
** Form generated from reading UI file 'devicecontrolwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICECONTROLWIDGET_H
#define UI_DEVICECONTROLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DeviceControlWidget
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *titleLabel;
    QGroupBox *commandsGroupBox;
    QGridLayout *gridLayout;
    QPushButton *calibrateButton;
    QPushButton *resetButton;
    QPushButton *queryButton;
    QPushButton *startButton;
    QPushButton *stopButton;
    QGroupBox *customGroupBox;
    QHBoxLayout *horizontalLayout;
    QLineEdit *customCommandEdit;
    QPushButton *sendCustomButton;
    QGroupBox *historyGroupBox;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *historyEdit;

    void setupUi(QWidget *DeviceControlWidget)
    {
        if (DeviceControlWidget->objectName().isEmpty())
            DeviceControlWidget->setObjectName(QString::fromUtf8("DeviceControlWidget"));
        DeviceControlWidget->resize(600, 500);
        verticalLayout = new QVBoxLayout(DeviceControlWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        titleLabel = new QLabel(DeviceControlWidget);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        titleLabel->setFont(font);
        titleLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(titleLabel);

        commandsGroupBox = new QGroupBox(DeviceControlWidget);
        commandsGroupBox->setObjectName(QString::fromUtf8("commandsGroupBox"));
        gridLayout = new QGridLayout(commandsGroupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        calibrateButton = new QPushButton(commandsGroupBox);
        calibrateButton->setObjectName(QString::fromUtf8("calibrateButton"));
        calibrateButton->setMinimumHeight(40);

        gridLayout->addWidget(calibrateButton, 0, 0, 1, 1);

        resetButton = new QPushButton(commandsGroupBox);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setMinimumHeight(40);

        gridLayout->addWidget(resetButton, 0, 1, 1, 1);

        queryButton = new QPushButton(commandsGroupBox);
        queryButton->setObjectName(QString::fromUtf8("queryButton"));
        queryButton->setMinimumHeight(40);

        gridLayout->addWidget(queryButton, 1, 0, 1, 1);

        startButton = new QPushButton(commandsGroupBox);
        startButton->setObjectName(QString::fromUtf8("startButton"));
        startButton->setMinimumHeight(40);

        gridLayout->addWidget(startButton, 1, 1, 1, 1);

        stopButton = new QPushButton(commandsGroupBox);
        stopButton->setObjectName(QString::fromUtf8("stopButton"));
        stopButton->setMinimumHeight(40);

        gridLayout->addWidget(stopButton, 2, 0, 1, 2);


        verticalLayout->addWidget(commandsGroupBox);

        customGroupBox = new QGroupBox(DeviceControlWidget);
        customGroupBox->setObjectName(QString::fromUtf8("customGroupBox"));
        horizontalLayout = new QHBoxLayout(customGroupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        customCommandEdit = new QLineEdit(customGroupBox);
        customCommandEdit->setObjectName(QString::fromUtf8("customCommandEdit"));

        horizontalLayout->addWidget(customCommandEdit);

        sendCustomButton = new QPushButton(customGroupBox);
        sendCustomButton->setObjectName(QString::fromUtf8("sendCustomButton"));
        sendCustomButton->setMinimumWidth(80);

        horizontalLayout->addWidget(sendCustomButton);


        verticalLayout->addWidget(customGroupBox);

        historyGroupBox = new QGroupBox(DeviceControlWidget);
        historyGroupBox->setObjectName(QString::fromUtf8("historyGroupBox"));
        verticalLayout_2 = new QVBoxLayout(historyGroupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        historyEdit = new QTextEdit(historyGroupBox);
        historyEdit->setObjectName(QString::fromUtf8("historyEdit"));
        historyEdit->setReadOnly(true);

        verticalLayout_2->addWidget(historyEdit);


        verticalLayout->addWidget(historyGroupBox);


        retranslateUi(DeviceControlWidget);

        QMetaObject::connectSlotsByName(DeviceControlWidget);
    } // setupUi

    void retranslateUi(QWidget *DeviceControlWidget)
    {
        DeviceControlWidget->setWindowTitle(QCoreApplication::translate("DeviceControlWidget", "\350\256\276\345\244\207\346\216\247\345\210\266", nullptr));
        titleLabel->setText(QCoreApplication::translate("DeviceControlWidget", "\350\256\276\345\244\207\346\216\247\345\210\266 - \346\214\207\344\273\244\345\217\221\351\200\201", nullptr));
        commandsGroupBox->setTitle(QCoreApplication::translate("DeviceControlWidget", "\345\277\253\346\215\267\346\214\207\344\273\244", nullptr));
        calibrateButton->setText(QCoreApplication::translate("DeviceControlWidget", "\346\240\241\345\207\206", nullptr));
        resetButton->setText(QCoreApplication::translate("DeviceControlWidget", "\345\244\215\344\275\215", nullptr));
        queryButton->setText(QCoreApplication::translate("DeviceControlWidget", "\346\237\245\350\257\242\347\212\266\346\200\201", nullptr));
        startButton->setText(QCoreApplication::translate("DeviceControlWidget", "\345\274\200\345\247\213\351\207\207\351\233\206", nullptr));
        stopButton->setText(QCoreApplication::translate("DeviceControlWidget", "\345\201\234\346\255\242\351\207\207\351\233\206", nullptr));
        customGroupBox->setTitle(QCoreApplication::translate("DeviceControlWidget", "\350\207\252\345\256\232\344\271\211\346\214\207\344\273\244", nullptr));
        customCommandEdit->setPlaceholderText(QCoreApplication::translate("DeviceControlWidget", "\350\276\223\345\205\24516\350\277\233\345\210\266\346\214\207\344\273\244\357\274\210\345\246\202: FF AA 01 02\357\274\211", nullptr));
        sendCustomButton->setText(QCoreApplication::translate("DeviceControlWidget", "\345\217\221\351\200\201", nullptr));
        historyGroupBox->setTitle(QCoreApplication::translate("DeviceControlWidget", "\346\214\207\344\273\244\345\216\206\345\217\262", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeviceControlWidget: public Ui_DeviceControlWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICECONTROLWIDGET_H
