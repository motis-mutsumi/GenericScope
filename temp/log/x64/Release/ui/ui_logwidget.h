/********************************************************************************
** Form generated from reading UI file 'logwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGWIDGET_H
#define UI_LOGWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LogWidget
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *toolLayout;
    QLabel *levelLabel;
    QComboBox *levelFilter;
    QLabel *searchLabel;
    QLineEdit *searchEdit;
    QCheckBox *caseSensitiveCheck;
    QSpacerItem *toolSpacer;
    QPushButton *clearButton;
    QPushButton *saveButton;
    QTextEdit *logTextEdit;

    void setupUi(QWidget *LogWidget)
    {
        if (LogWidget->objectName().isEmpty())
            LogWidget->setObjectName(QString::fromUtf8("LogWidget"));
        LogWidget->resize(800, 200);
        mainLayout = new QVBoxLayout(LogWidget);
        mainLayout->setSpacing(5);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(5, 5, 5, 5);
        toolLayout = new QHBoxLayout();
        toolLayout->setSpacing(10);
        toolLayout->setObjectName(QString::fromUtf8("toolLayout"));
        levelLabel = new QLabel(LogWidget);
        levelLabel->setObjectName(QString::fromUtf8("levelLabel"));

        toolLayout->addWidget(levelLabel);

        levelFilter = new QComboBox(LogWidget);
        levelFilter->addItem(QString());
        levelFilter->addItem(QString());
        levelFilter->addItem(QString());
        levelFilter->addItem(QString());
        levelFilter->addItem(QString());
        levelFilter->addItem(QString());
        levelFilter->setObjectName(QString::fromUtf8("levelFilter"));
        levelFilter->setMinimumSize(QSize(100, 0));

        toolLayout->addWidget(levelFilter);

        searchLabel = new QLabel(LogWidget);
        searchLabel->setObjectName(QString::fromUtf8("searchLabel"));

        toolLayout->addWidget(searchLabel);

        searchEdit = new QLineEdit(LogWidget);
        searchEdit->setObjectName(QString::fromUtf8("searchEdit"));
        searchEdit->setMinimumSize(QSize(200, 0));
        searchEdit->setClearButtonEnabled(true);

        toolLayout->addWidget(searchEdit);

        caseSensitiveCheck = new QCheckBox(LogWidget);
        caseSensitiveCheck->setObjectName(QString::fromUtf8("caseSensitiveCheck"));

        toolLayout->addWidget(caseSensitiveCheck);

        toolSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolLayout->addItem(toolSpacer);

        clearButton = new QPushButton(LogWidget);
        clearButton->setObjectName(QString::fromUtf8("clearButton"));

        toolLayout->addWidget(clearButton);

        saveButton = new QPushButton(LogWidget);
        saveButton->setObjectName(QString::fromUtf8("saveButton"));

        toolLayout->addWidget(saveButton);


        mainLayout->addLayout(toolLayout);

        logTextEdit = new QTextEdit(LogWidget);
        logTextEdit->setObjectName(QString::fromUtf8("logTextEdit"));
        logTextEdit->setReadOnly(true);
        QFont font;
        font.setFamily(QString::fromUtf8("Consolas"));
        font.setPointSize(9);
        logTextEdit->setFont(font);

        mainLayout->addWidget(logTextEdit);


        retranslateUi(LogWidget);

        QMetaObject::connectSlotsByName(LogWidget);
    } // setupUi

    void retranslateUi(QWidget *LogWidget)
    {
        levelLabel->setText(QCoreApplication::translate("LogWidget", "Level:", nullptr));
        levelFilter->setItemText(0, QCoreApplication::translate("LogWidget", "All", nullptr));
        levelFilter->setItemText(1, QCoreApplication::translate("LogWidget", "Debug", nullptr));
        levelFilter->setItemText(2, QCoreApplication::translate("LogWidget", "Info", nullptr));
        levelFilter->setItemText(3, QCoreApplication::translate("LogWidget", "Warning", nullptr));
        levelFilter->setItemText(4, QCoreApplication::translate("LogWidget", "Error", nullptr));
        levelFilter->setItemText(5, QCoreApplication::translate("LogWidget", "Critical", nullptr));

        searchLabel->setText(QCoreApplication::translate("LogWidget", "Search:", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("LogWidget", "Filter by keyword...", nullptr));
        caseSensitiveCheck->setText(QCoreApplication::translate("LogWidget", "Case Sensitive", nullptr));
        clearButton->setText(QCoreApplication::translate("LogWidget", "Clear", nullptr));
        saveButton->setText(QCoreApplication::translate("LogWidget", "Save", nullptr));
        (void)LogWidget;
    } // retranslateUi

};

namespace Ui {
    class LogWidget: public Ui_LogWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGWIDGET_H
