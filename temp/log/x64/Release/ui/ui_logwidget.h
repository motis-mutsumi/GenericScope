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
    QPushButton *clearFilesButton;
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

        clearFilesButton = new QPushButton(LogWidget);
        clearFilesButton->setObjectName(QString::fromUtf8("clearFilesButton"));

        toolLayout->addWidget(clearFilesButton);

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
        levelLabel->setText(QCoreApplication::translate("LogWidget", "\347\272\247\345\210\253:", nullptr));
        levelFilter->setItemText(0, QCoreApplication::translate("LogWidget", "\345\205\250\351\203\250", nullptr));
        levelFilter->setItemText(1, QCoreApplication::translate("LogWidget", "\350\260\203\350\257\225", nullptr));
        levelFilter->setItemText(2, QCoreApplication::translate("LogWidget", "\344\277\241\346\201\257", nullptr));
        levelFilter->setItemText(3, QCoreApplication::translate("LogWidget", "\350\255\246\345\221\212", nullptr));
        levelFilter->setItemText(4, QCoreApplication::translate("LogWidget", "\351\224\231\350\257\257", nullptr));
        levelFilter->setItemText(5, QCoreApplication::translate("LogWidget", "\344\270\245\351\207\215", nullptr));

        searchLabel->setText(QCoreApplication::translate("LogWidget", "\346\220\234\347\264\242:", nullptr));
        searchEdit->setPlaceholderText(QCoreApplication::translate("LogWidget", "\346\214\211\345\205\263\351\224\256\350\257\215\350\277\207\346\273\244...", nullptr));
        caseSensitiveCheck->setText(QCoreApplication::translate("LogWidget", "\345\214\272\345\210\206\345\244\247\345\260\217\345\206\231", nullptr));
        clearButton->setText(QCoreApplication::translate("LogWidget", "\346\270\205\347\251\272", nullptr));
#if QT_CONFIG(tooltip)
        clearButton->setToolTip(QCoreApplication::translate("LogWidget", "\344\273\205\346\270\205\347\251\272\346\230\276\347\244\272", nullptr));
#endif // QT_CONFIG(tooltip)
        clearFilesButton->setText(QCoreApplication::translate("LogWidget", "\346\270\205\351\231\244\346\226\207\344\273\266", nullptr));
#if QT_CONFIG(tooltip)
        clearFilesButton->setToolTip(QCoreApplication::translate("LogWidget", "\345\210\240\351\231\244\347\243\201\347\233\230\344\270\212\347\232\204\346\211\200\346\234\211\346\227\245\345\277\227\346\226\207\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
        saveButton->setText(QCoreApplication::translate("LogWidget", "\344\277\235\345\255\230", nullptr));
        (void)LogWidget;
    } // retranslateUi

};

namespace Ui {
    class LogWidget: public Ui_LogWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGWIDGET_H
