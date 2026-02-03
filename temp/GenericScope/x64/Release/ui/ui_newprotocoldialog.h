/********************************************************************************
** Form generated from reading UI file 'newprotocoldialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEWPROTOCOLDIALOG_H
#define UI_NEWPROTOCOLDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewProtocolDialog
{
public:
    QVBoxLayout *mainLayout;
    QGroupBox *infoGroup;
    QFormLayout *infoLayout;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QLabel *versionLabel;
    QLineEdit *versionEdit;
    QLabel *descriptionLabel;
    QTextEdit *descriptionEdit;
    QGroupBox *templateGroup;
    QVBoxLayout *templateLayout;
    QHBoxLayout *templateSelectLayout;
    QLabel *templateLabel;
    QComboBox *templateCombo;
    QTextEdit *templateDescEdit;
    QCheckBox *addDefaultFieldsCheck;
    QLabel *noteLabel;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewProtocolDialog)
    {
        if (NewProtocolDialog->objectName().isEmpty())
            NewProtocolDialog->setObjectName(QString::fromUtf8("NewProtocolDialog"));
        NewProtocolDialog->resize(600, 500);
        mainLayout = new QVBoxLayout(NewProtocolDialog);
        mainLayout->setSpacing(10);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(15, 15, 15, 15);
        infoGroup = new QGroupBox(NewProtocolDialog);
        infoGroup->setObjectName(QString::fromUtf8("infoGroup"));
        infoLayout = new QFormLayout(infoGroup);
        infoLayout->setObjectName(QString::fromUtf8("infoLayout"));
        nameLabel = new QLabel(infoGroup);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        infoLayout->setWidget(0, QFormLayout::LabelRole, nameLabel);

        nameEdit = new QLineEdit(infoGroup);
        nameEdit->setObjectName(QString::fromUtf8("nameEdit"));

        infoLayout->setWidget(0, QFormLayout::FieldRole, nameEdit);

        versionLabel = new QLabel(infoGroup);
        versionLabel->setObjectName(QString::fromUtf8("versionLabel"));

        infoLayout->setWidget(1, QFormLayout::LabelRole, versionLabel);

        versionEdit = new QLineEdit(infoGroup);
        versionEdit->setObjectName(QString::fromUtf8("versionEdit"));

        infoLayout->setWidget(1, QFormLayout::FieldRole, versionEdit);

        descriptionLabel = new QLabel(infoGroup);
        descriptionLabel->setObjectName(QString::fromUtf8("descriptionLabel"));

        infoLayout->setWidget(2, QFormLayout::LabelRole, descriptionLabel);

        descriptionEdit = new QTextEdit(infoGroup);
        descriptionEdit->setObjectName(QString::fromUtf8("descriptionEdit"));
        descriptionEdit->setMaximumSize(QSize(16777215, 80));

        infoLayout->setWidget(2, QFormLayout::FieldRole, descriptionEdit);


        mainLayout->addWidget(infoGroup);

        templateGroup = new QGroupBox(NewProtocolDialog);
        templateGroup->setObjectName(QString::fromUtf8("templateGroup"));
        templateLayout = new QVBoxLayout(templateGroup);
        templateLayout->setObjectName(QString::fromUtf8("templateLayout"));
        templateSelectLayout = new QHBoxLayout();
        templateSelectLayout->setObjectName(QString::fromUtf8("templateSelectLayout"));
        templateLabel = new QLabel(templateGroup);
        templateLabel->setObjectName(QString::fromUtf8("templateLabel"));

        templateSelectLayout->addWidget(templateLabel);

        templateCombo = new QComboBox(templateGroup);
        templateCombo->addItem(QString());
        templateCombo->addItem(QString());
        templateCombo->addItem(QString());
        templateCombo->addItem(QString());
        templateCombo->addItem(QString());
        templateCombo->addItem(QString());
        templateCombo->setObjectName(QString::fromUtf8("templateCombo"));

        templateSelectLayout->addWidget(templateCombo);


        templateLayout->addLayout(templateSelectLayout);

        templateDescEdit = new QTextEdit(templateGroup);
        templateDescEdit->setObjectName(QString::fromUtf8("templateDescEdit"));
        templateDescEdit->setMaximumSize(QSize(16777215, 120));
        templateDescEdit->setReadOnly(true);

        templateLayout->addWidget(templateDescEdit);

        addDefaultFieldsCheck = new QCheckBox(templateGroup);
        addDefaultFieldsCheck->setObjectName(QString::fromUtf8("addDefaultFieldsCheck"));
        addDefaultFieldsCheck->setChecked(true);

        templateLayout->addWidget(addDefaultFieldsCheck);


        mainLayout->addWidget(templateGroup);

        noteLabel = new QLabel(NewProtocolDialog);
        noteLabel->setObjectName(QString::fromUtf8("noteLabel"));
        noteLabel->setStyleSheet(QString::fromUtf8("color: #666; font-style: italic;"));

        mainLayout->addWidget(noteLabel);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        mainLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(NewProtocolDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        mainLayout->addWidget(buttonBox);


        retranslateUi(NewProtocolDialog);

        QMetaObject::connectSlotsByName(NewProtocolDialog);
    } // setupUi

    void retranslateUi(QDialog *NewProtocolDialog)
    {
        NewProtocolDialog->setWindowTitle(QCoreApplication::translate("NewProtocolDialog", "\346\226\260\345\273\272\345\215\217\350\256\256\345\220\221\345\257\274", nullptr));
        infoGroup->setTitle(QCoreApplication::translate("NewProtocolDialog", "\345\215\217\350\256\256\345\237\272\346\234\254\344\277\241\346\201\257", nullptr));
        nameLabel->setText(QCoreApplication::translate("NewProtocolDialog", "\345\215\217\350\256\256\345\220\215\347\247\260*:", nullptr));
        nameEdit->setPlaceholderText(QCoreApplication::translate("NewProtocolDialog", "\344\276\213\345\246\202: IMU_Protocol_V1", nullptr));
        versionLabel->setText(QCoreApplication::translate("NewProtocolDialog", "\345\215\217\350\256\256\347\211\210\346\234\254:", nullptr));
        versionEdit->setText(QCoreApplication::translate("NewProtocolDialog", "1.0.0", nullptr));
        versionEdit->setPlaceholderText(QCoreApplication::translate("NewProtocolDialog", "\344\276\213\345\246\202: 1.0.0", nullptr));
        descriptionLabel->setText(QCoreApplication::translate("NewProtocolDialog", "\345\215\217\350\256\256\346\217\217\350\277\260:", nullptr));
        descriptionEdit->setPlaceholderText(QCoreApplication::translate("NewProtocolDialog", "\350\276\223\345\205\245\345\215\217\350\256\256\346\217\217\350\277\260...", nullptr));
        templateGroup->setTitle(QCoreApplication::translate("NewProtocolDialog", "\345\215\217\350\256\256\346\250\241\346\235\277", nullptr));
        templateLabel->setText(QCoreApplication::translate("NewProtocolDialog", "\351\200\211\346\213\251\346\250\241\346\235\277:", nullptr));
        templateCombo->setItemText(0, QCoreApplication::translate("NewProtocolDialog", "\347\251\272\345\215\217\350\256\256", nullptr));
        templateCombo->setItemText(1, QCoreApplication::translate("NewProtocolDialog", "\345\237\272\347\241\200\344\270\262\345\217\243\345\215\217\350\256\256", nullptr));
        templateCombo->setItemText(2, QCoreApplication::translate("NewProtocolDialog", "MODBUS RTU\345\215\217\350\256\256", nullptr));
        templateCombo->setItemText(3, QCoreApplication::translate("NewProtocolDialog", "\350\207\252\345\256\232\344\271\211IMU\345\215\217\350\256\256", nullptr));
        templateCombo->setItemText(4, QCoreApplication::translate("NewProtocolDialog", "\346\226\207\346\234\254CSV\345\215\217\350\256\256", nullptr));
        templateCombo->setItemText(5, QCoreApplication::translate("NewProtocolDialog", "\360\237\244\226 AI\346\231\272\350\203\275\347\224\237\346\210\220", nullptr));

        addDefaultFieldsCheck->setText(QCoreApplication::translate("NewProtocolDialog", "\346\267\273\345\212\240\351\273\230\350\256\244\346\225\260\346\215\256\345\255\227\346\256\265\357\274\210\351\200\202\347\224\250\344\272\216\351\200\211\345\256\232\346\250\241\346\235\277\357\274\211", nullptr));
        noteLabel->setText(QCoreApplication::translate("NewProtocolDialog", "\346\217\220\347\244\272: \345\210\233\345\273\272\345\220\216\345\217\257\344\273\245\345\234\250\345\215\217\350\256\256\351\205\215\347\275\256\347\225\214\351\235\242\344\270\255\344\277\256\346\224\271\346\211\200\346\234\211\350\256\276\347\275\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NewProtocolDialog: public Ui_NewProtocolDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEWPROTOCOLDIALOG_H
