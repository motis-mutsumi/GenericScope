/********************************************************************************
** Form generated from reading UI file 'commandsettingsdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMANDSETTINGSDIALOG_H
#define UI_COMMANDSETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommandSettingsDialog
{
public:
    QVBoxLayout *mainLayout;
    QWidget *tabContainer;
    QHBoxLayout *tabContainerLayout;
    QLabel *protocolListLabel;
    QTabWidget *tabWidget;
    QPushButton *newProtocolBtn;
    QPushButton *deleteProtocolBtn;
    QSplitter *splitter;
    QGroupBox *frameFormatGroup;
    QVBoxLayout *frameFormatLayout;
    QGroupBox *infoGroup;
    QFormLayout *infoLayout;
    QLabel *nameLabel;
    QLineEdit *protocolNameEdit;
    QLabel *versionLabel;
    QLineEdit *protocolVersionEdit;
    QLabel *descLabel;
    QTextEdit *protocolDescEdit;
    QGroupBox *frameGroup;
    QFormLayout *frameLayout;
    QLabel *headerLabel;
    QLineEdit *frameHeaderEdit;
    QLabel *footerLabel;
    QLineEdit *frameFooterEdit;
    QLabel *lengthPosLabel;
    QSpinBox *lengthPositionSpin;
    QGroupBox *checksumGroup;
    QFormLayout *checksumLayout;
    QLabel *checksumTypeLabel;
    QComboBox *checksumTypeCombo;
    QWidget *checksumParamWidget;
    QHBoxLayout *checksumParamLayout;
    QLabel *checksumStartLabel;
    QSpinBox *checksumStartSpin;
    QLabel *checksumLengthLabel;
    QSpinBox *checksumLengthSpin;
    QLabel *checksumPosLabel;
    QSpinBox *checksumPositionSpin;
    QGroupBox *otherGroup;
    QFormLayout *otherLayout;
    QLabel *byteOrderLabel;
    QComboBox *byteOrderCombo;
    QLabel *frequencyLabel;
    QSpinBox *frequencySpin;
    QLabel *separatorLabel;
    QLineEdit *separatorEdit;
    QSpacerItem *verticalSpacer;
    QGroupBox *fieldConfigGroup;
    QVBoxLayout *fieldConfigLayout;
    QWidget *toolbarWidget;
    QHBoxLayout *toolbarLayout;
    QPushButton *addFieldBtn;
    QPushButton *deleteFieldBtn;
    QPushButton *moveUpBtn;
    QPushButton *moveDownBtn;
    QPushButton *importFieldBtn;
    QSpacerItem *horizontalSpacer;
    QTableWidget *fieldTable;
    QWidget *buttonWidget;
    QHBoxLayout *buttonLayout;
    QPushButton *importProtocolBtn;
    QPushButton *exportProtocolBtn;
    QPushButton *generateProtocolBtn;
    QPushButton *testProtocolBtn;
    QSpacerItem *horizontalSpacer_2;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *CommandSettingsDialog)
    {
        if (CommandSettingsDialog->objectName().isEmpty())
            CommandSettingsDialog->setObjectName(QString::fromUtf8("CommandSettingsDialog"));
        CommandSettingsDialog->resize(1600, 800);
        mainLayout = new QVBoxLayout(CommandSettingsDialog);
        mainLayout->setSpacing(10);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(10, 10, 10, 10);
        tabContainer = new QWidget(CommandSettingsDialog);
        tabContainer->setObjectName(QString::fromUtf8("tabContainer"));
        tabContainerLayout = new QHBoxLayout(tabContainer);
        tabContainerLayout->setSpacing(5);
        tabContainerLayout->setObjectName(QString::fromUtf8("tabContainerLayout"));
        tabContainerLayout->setContentsMargins(0, 0, 0, 0);
        protocolListLabel = new QLabel(tabContainer);
        protocolListLabel->setObjectName(QString::fromUtf8("protocolListLabel"));

        tabContainerLayout->addWidget(protocolListLabel);

        tabWidget = new QTabWidget(tabContainer);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setTabsClosable(false);
        tabWidget->setMovable(true);

        tabContainerLayout->addWidget(tabWidget);

        newProtocolBtn = new QPushButton(tabContainer);
        newProtocolBtn->setObjectName(QString::fromUtf8("newProtocolBtn"));

        tabContainerLayout->addWidget(newProtocolBtn);

        deleteProtocolBtn = new QPushButton(tabContainer);
        deleteProtocolBtn->setObjectName(QString::fromUtf8("deleteProtocolBtn"));

        tabContainerLayout->addWidget(deleteProtocolBtn);


        mainLayout->addWidget(tabContainer);

        splitter = new QSplitter(CommandSettingsDialog);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        frameFormatGroup = new QGroupBox(splitter);
        frameFormatGroup->setObjectName(QString::fromUtf8("frameFormatGroup"));
        frameFormatLayout = new QVBoxLayout(frameFormatGroup);
        frameFormatLayout->setSpacing(8);
        frameFormatLayout->setObjectName(QString::fromUtf8("frameFormatLayout"));
        infoGroup = new QGroupBox(frameFormatGroup);
        infoGroup->setObjectName(QString::fromUtf8("infoGroup"));
        infoLayout = new QFormLayout(infoGroup);
        infoLayout->setObjectName(QString::fromUtf8("infoLayout"));
        nameLabel = new QLabel(infoGroup);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        infoLayout->setWidget(0, QFormLayout::LabelRole, nameLabel);

        protocolNameEdit = new QLineEdit(infoGroup);
        protocolNameEdit->setObjectName(QString::fromUtf8("protocolNameEdit"));

        infoLayout->setWidget(0, QFormLayout::FieldRole, protocolNameEdit);

        versionLabel = new QLabel(infoGroup);
        versionLabel->setObjectName(QString::fromUtf8("versionLabel"));

        infoLayout->setWidget(1, QFormLayout::LabelRole, versionLabel);

        protocolVersionEdit = new QLineEdit(infoGroup);
        protocolVersionEdit->setObjectName(QString::fromUtf8("protocolVersionEdit"));

        infoLayout->setWidget(1, QFormLayout::FieldRole, protocolVersionEdit);

        descLabel = new QLabel(infoGroup);
        descLabel->setObjectName(QString::fromUtf8("descLabel"));

        infoLayout->setWidget(2, QFormLayout::LabelRole, descLabel);

        protocolDescEdit = new QTextEdit(infoGroup);
        protocolDescEdit->setObjectName(QString::fromUtf8("protocolDescEdit"));
        protocolDescEdit->setMaximumSize(QSize(16777215, 60));

        infoLayout->setWidget(2, QFormLayout::FieldRole, protocolDescEdit);


        frameFormatLayout->addWidget(infoGroup);

        frameGroup = new QGroupBox(frameFormatGroup);
        frameGroup->setObjectName(QString::fromUtf8("frameGroup"));
        frameLayout = new QFormLayout(frameGroup);
        frameLayout->setObjectName(QString::fromUtf8("frameLayout"));
        headerLabel = new QLabel(frameGroup);
        headerLabel->setObjectName(QString::fromUtf8("headerLabel"));

        frameLayout->setWidget(0, QFormLayout::LabelRole, headerLabel);

        frameHeaderEdit = new QLineEdit(frameGroup);
        frameHeaderEdit->setObjectName(QString::fromUtf8("frameHeaderEdit"));

        frameLayout->setWidget(0, QFormLayout::FieldRole, frameHeaderEdit);

        footerLabel = new QLabel(frameGroup);
        footerLabel->setObjectName(QString::fromUtf8("footerLabel"));

        frameLayout->setWidget(1, QFormLayout::LabelRole, footerLabel);

        frameFooterEdit = new QLineEdit(frameGroup);
        frameFooterEdit->setObjectName(QString::fromUtf8("frameFooterEdit"));

        frameLayout->setWidget(1, QFormLayout::FieldRole, frameFooterEdit);

        lengthPosLabel = new QLabel(frameGroup);
        lengthPosLabel->setObjectName(QString::fromUtf8("lengthPosLabel"));

        frameLayout->setWidget(2, QFormLayout::LabelRole, lengthPosLabel);

        lengthPositionSpin = new QSpinBox(frameGroup);
        lengthPositionSpin->setObjectName(QString::fromUtf8("lengthPositionSpin"));
        lengthPositionSpin->setMinimum(-1);
        lengthPositionSpin->setMaximum(255);
        lengthPositionSpin->setValue(-1);

        frameLayout->setWidget(2, QFormLayout::FieldRole, lengthPositionSpin);


        frameFormatLayout->addWidget(frameGroup);

        checksumGroup = new QGroupBox(frameFormatGroup);
        checksumGroup->setObjectName(QString::fromUtf8("checksumGroup"));
        checksumLayout = new QFormLayout(checksumGroup);
        checksumLayout->setObjectName(QString::fromUtf8("checksumLayout"));
        checksumTypeLabel = new QLabel(checksumGroup);
        checksumTypeLabel->setObjectName(QString::fromUtf8("checksumTypeLabel"));

        checksumLayout->setWidget(0, QFormLayout::LabelRole, checksumTypeLabel);

        checksumTypeCombo = new QComboBox(checksumGroup);
        checksumTypeCombo->addItem(QString());
        checksumTypeCombo->addItem(QString());
        checksumTypeCombo->addItem(QString());
        checksumTypeCombo->addItem(QString());
        checksumTypeCombo->addItem(QString());
        checksumTypeCombo->addItem(QString());
        checksumTypeCombo->setObjectName(QString::fromUtf8("checksumTypeCombo"));

        checksumLayout->setWidget(0, QFormLayout::FieldRole, checksumTypeCombo);

        checksumParamWidget = new QWidget(checksumGroup);
        checksumParamWidget->setObjectName(QString::fromUtf8("checksumParamWidget"));
        checksumParamLayout = new QHBoxLayout(checksumParamWidget);
        checksumParamLayout->setObjectName(QString::fromUtf8("checksumParamLayout"));
        checksumParamLayout->setContentsMargins(0, 0, 0, 0);
        checksumStartLabel = new QLabel(checksumParamWidget);
        checksumStartLabel->setObjectName(QString::fromUtf8("checksumStartLabel"));

        checksumParamLayout->addWidget(checksumStartLabel);

        checksumStartSpin = new QSpinBox(checksumParamWidget);
        checksumStartSpin->setObjectName(QString::fromUtf8("checksumStartSpin"));
        checksumStartSpin->setMaximum(255);

        checksumParamLayout->addWidget(checksumStartSpin);

        checksumLengthLabel = new QLabel(checksumParamWidget);
        checksumLengthLabel->setObjectName(QString::fromUtf8("checksumLengthLabel"));

        checksumParamLayout->addWidget(checksumLengthLabel);

        checksumLengthSpin = new QSpinBox(checksumParamWidget);
        checksumLengthSpin->setObjectName(QString::fromUtf8("checksumLengthSpin"));
        checksumLengthSpin->setMinimum(-1);
        checksumLengthSpin->setMaximum(255);
        checksumLengthSpin->setValue(-1);

        checksumParamLayout->addWidget(checksumLengthSpin);


        checksumLayout->setWidget(1, QFormLayout::FieldRole, checksumParamWidget);

        checksumPosLabel = new QLabel(checksumGroup);
        checksumPosLabel->setObjectName(QString::fromUtf8("checksumPosLabel"));

        checksumLayout->setWidget(2, QFormLayout::LabelRole, checksumPosLabel);

        checksumPositionSpin = new QSpinBox(checksumGroup);
        checksumPositionSpin->setObjectName(QString::fromUtf8("checksumPositionSpin"));
        checksumPositionSpin->setMinimum(-1);
        checksumPositionSpin->setMaximum(255);
        checksumPositionSpin->setValue(-1);

        checksumLayout->setWidget(2, QFormLayout::FieldRole, checksumPositionSpin);


        frameFormatLayout->addWidget(checksumGroup);

        otherGroup = new QGroupBox(frameFormatGroup);
        otherGroup->setObjectName(QString::fromUtf8("otherGroup"));
        otherLayout = new QFormLayout(otherGroup);
        otherLayout->setObjectName(QString::fromUtf8("otherLayout"));
        byteOrderLabel = new QLabel(otherGroup);
        byteOrderLabel->setObjectName(QString::fromUtf8("byteOrderLabel"));

        otherLayout->setWidget(0, QFormLayout::LabelRole, byteOrderLabel);

        byteOrderCombo = new QComboBox(otherGroup);
        byteOrderCombo->addItem(QString());
        byteOrderCombo->addItem(QString());
        byteOrderCombo->setObjectName(QString::fromUtf8("byteOrderCombo"));

        otherLayout->setWidget(0, QFormLayout::FieldRole, byteOrderCombo);

        frequencyLabel = new QLabel(otherGroup);
        frequencyLabel->setObjectName(QString::fromUtf8("frequencyLabel"));

        otherLayout->setWidget(1, QFormLayout::LabelRole, frequencyLabel);

        frequencySpin = new QSpinBox(otherGroup);
        frequencySpin->setObjectName(QString::fromUtf8("frequencySpin"));
        frequencySpin->setMinimum(1);
        frequencySpin->setMaximum(10000);
        frequencySpin->setValue(1000);

        otherLayout->setWidget(1, QFormLayout::FieldRole, frequencySpin);

        separatorLabel = new QLabel(otherGroup);
        separatorLabel->setObjectName(QString::fromUtf8("separatorLabel"));

        otherLayout->setWidget(2, QFormLayout::LabelRole, separatorLabel);

        separatorEdit = new QLineEdit(otherGroup);
        separatorEdit->setObjectName(QString::fromUtf8("separatorEdit"));

        otherLayout->setWidget(2, QFormLayout::FieldRole, separatorEdit);


        frameFormatLayout->addWidget(otherGroup);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        frameFormatLayout->addItem(verticalSpacer);

        splitter->addWidget(frameFormatGroup);
        fieldConfigGroup = new QGroupBox(splitter);
        fieldConfigGroup->setObjectName(QString::fromUtf8("fieldConfigGroup"));
        fieldConfigLayout = new QVBoxLayout(fieldConfigGroup);
        fieldConfigLayout->setSpacing(8);
        fieldConfigLayout->setObjectName(QString::fromUtf8("fieldConfigLayout"));
        toolbarWidget = new QWidget(fieldConfigGroup);
        toolbarWidget->setObjectName(QString::fromUtf8("toolbarWidget"));
        toolbarLayout = new QHBoxLayout(toolbarWidget);
        toolbarLayout->setObjectName(QString::fromUtf8("toolbarLayout"));
        toolbarLayout->setContentsMargins(0, 0, 0, 0);
        addFieldBtn = new QPushButton(toolbarWidget);
        addFieldBtn->setObjectName(QString::fromUtf8("addFieldBtn"));

        toolbarLayout->addWidget(addFieldBtn);

        deleteFieldBtn = new QPushButton(toolbarWidget);
        deleteFieldBtn->setObjectName(QString::fromUtf8("deleteFieldBtn"));

        toolbarLayout->addWidget(deleteFieldBtn);

        moveUpBtn = new QPushButton(toolbarWidget);
        moveUpBtn->setObjectName(QString::fromUtf8("moveUpBtn"));
        moveUpBtn->setMaximumSize(QSize(40, 16777215));

        toolbarLayout->addWidget(moveUpBtn);

        moveDownBtn = new QPushButton(toolbarWidget);
        moveDownBtn->setObjectName(QString::fromUtf8("moveDownBtn"));
        moveDownBtn->setMaximumSize(QSize(40, 16777215));

        toolbarLayout->addWidget(moveDownBtn);

        importFieldBtn = new QPushButton(toolbarWidget);
        importFieldBtn->setObjectName(QString::fromUtf8("importFieldBtn"));

        toolbarLayout->addWidget(importFieldBtn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbarLayout->addItem(horizontalSpacer);


        fieldConfigLayout->addWidget(toolbarWidget);

        fieldTable = new QTableWidget(fieldConfigGroup);
        if (fieldTable->columnCount() < 12)
            fieldTable->setColumnCount(12);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        fieldTable->setHorizontalHeaderItem(11, __qtablewidgetitem11);
        fieldTable->setObjectName(QString::fromUtf8("fieldTable"));
        fieldTable->setAlternatingRowColors(true);
        fieldTable->setSelectionMode(QAbstractItemView::SingleSelection);
        fieldTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        fieldTable->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        fieldTable->horizontalHeader()->setStretchLastSection(true);
        fieldTable->verticalHeader()->setVisible(false);

        fieldConfigLayout->addWidget(fieldTable);

        splitter->addWidget(fieldConfigGroup);

        mainLayout->addWidget(splitter);

        buttonWidget = new QWidget(CommandSettingsDialog);
        buttonWidget->setObjectName(QString::fromUtf8("buttonWidget"));
        buttonLayout = new QHBoxLayout(buttonWidget);
        buttonLayout->setObjectName(QString::fromUtf8("buttonLayout"));
        buttonLayout->setContentsMargins(0, 0, 0, 0);
        importProtocolBtn = new QPushButton(buttonWidget);
        importProtocolBtn->setObjectName(QString::fromUtf8("importProtocolBtn"));

        buttonLayout->addWidget(importProtocolBtn);

        exportProtocolBtn = new QPushButton(buttonWidget);
        exportProtocolBtn->setObjectName(QString::fromUtf8("exportProtocolBtn"));

        buttonLayout->addWidget(exportProtocolBtn);

        generateProtocolBtn = new QPushButton(buttonWidget);
        generateProtocolBtn->setObjectName(QString::fromUtf8("generateProtocolBtn"));

        buttonLayout->addWidget(generateProtocolBtn);

        testProtocolBtn = new QPushButton(buttonWidget);
        testProtocolBtn->setObjectName(QString::fromUtf8("testProtocolBtn"));

        buttonLayout->addWidget(testProtocolBtn);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        buttonLayout->addItem(horizontalSpacer_2);

        buttonBox = new QDialogButtonBox(buttonWidget);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Apply|QDialogButtonBox::Cancel|QDialogButtonBox::Help|QDialogButtonBox::Ok);

        buttonLayout->addWidget(buttonBox);


        mainLayout->addWidget(buttonWidget);


        retranslateUi(CommandSettingsDialog);

        QMetaObject::connectSlotsByName(CommandSettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *CommandSettingsDialog)
    {
        CommandSettingsDialog->setWindowTitle(QCoreApplication::translate("CommandSettingsDialog", "\345\215\217\350\256\256\351\205\215\347\275\256", nullptr));
        protocolListLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\215\217\350\256\256\345\210\227\350\241\250:", nullptr));
        newProtocolBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\226\260\345\273\272\345\215\217\350\256\256", nullptr));
        deleteProtocolBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\210\240\351\231\244\345\215\217\350\256\256", nullptr));
        frameFormatGroup->setTitle(QCoreApplication::translate("CommandSettingsDialog", "\345\270\247\346\240\274\345\274\217\351\205\215\347\275\256", nullptr));
        infoGroup->setTitle(QCoreApplication::translate("CommandSettingsDialog", "\345\215\217\350\256\256\344\277\241\346\201\257", nullptr));
        nameLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\220\215\347\247\260:", nullptr));
        protocolNameEdit->setPlaceholderText(QCoreApplication::translate("CommandSettingsDialog", "\344\276\213\345\246\202: IMU_Protocol_V1", nullptr));
        versionLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\347\211\210\346\234\254:", nullptr));
        protocolVersionEdit->setPlaceholderText(QCoreApplication::translate("CommandSettingsDialog", "\344\276\213\345\246\202: 1.0.0", nullptr));
        descLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\217\217\350\277\260:", nullptr));
        protocolDescEdit->setPlaceholderText(QCoreApplication::translate("CommandSettingsDialog", "\345\215\217\350\256\256\346\217\217\350\277\260...", nullptr));
        frameGroup->setTitle(QCoreApplication::translate("CommandSettingsDialog", "\345\270\247\347\273\223\346\236\204", nullptr));
        headerLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\270\247\345\244\264(HEX):", nullptr));
        frameHeaderEdit->setPlaceholderText(QCoreApplication::translate("CommandSettingsDialog", "\344\276\213\345\246\202: FF AA", nullptr));
        footerLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\270\247\345\260\276(HEX):", nullptr));
        frameFooterEdit->setPlaceholderText(QCoreApplication::translate("CommandSettingsDialog", "\345\217\257\351\200\211\357\274\214\344\276\213\345\246\202: 0D 0A", nullptr));
        lengthPosLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\351\225\277\345\272\246\344\275\215\347\275\256:", nullptr));
        lengthPositionSpin->setSpecialValueText(QCoreApplication::translate("CommandSettingsDialog", "\346\227\240", nullptr));
        checksumGroup->setTitle(QCoreApplication::translate("CommandSettingsDialog", "\346\240\241\351\252\214\351\205\215\347\275\256", nullptr));
        checksumTypeLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\240\241\351\252\214\346\226\271\345\274\217:", nullptr));
        checksumTypeCombo->setItemText(0, QCoreApplication::translate("CommandSettingsDialog", "\346\227\240\346\240\241\351\252\214", nullptr));
        checksumTypeCombo->setItemText(1, QCoreApplication::translate("CommandSettingsDialog", "Sum", nullptr));
        checksumTypeCombo->setItemText(2, QCoreApplication::translate("CommandSettingsDialog", "XOR", nullptr));
        checksumTypeCombo->setItemText(3, QCoreApplication::translate("CommandSettingsDialog", "CRC8", nullptr));
        checksumTypeCombo->setItemText(4, QCoreApplication::translate("CommandSettingsDialog", "CRC16", nullptr));
        checksumTypeCombo->setItemText(5, QCoreApplication::translate("CommandSettingsDialog", "CRC32", nullptr));

        checksumStartLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\350\265\267\345\247\213:", nullptr));
        checksumLengthLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\351\225\277\345\272\246:", nullptr));
        checksumLengthSpin->setSpecialValueText(QCoreApplication::translate("CommandSettingsDialog", "\345\210\260\345\270\247\345\260\276", nullptr));
        checksumPosLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\240\241\351\252\214\344\275\215\347\275\256:", nullptr));
        checksumPositionSpin->setSpecialValueText(QCoreApplication::translate("CommandSettingsDialog", "\345\270\247\345\260\276\345\211\215", nullptr));
        otherGroup->setTitle(QCoreApplication::translate("CommandSettingsDialog", "\345\205\266\344\273\226\351\205\215\347\275\256", nullptr));
        byteOrderLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\255\227\350\212\202\345\272\217:", nullptr));
        byteOrderCombo->setItemText(0, QCoreApplication::translate("CommandSettingsDialog", "LittleEndian", nullptr));
        byteOrderCombo->setItemText(1, QCoreApplication::translate("CommandSettingsDialog", "BigEndian", nullptr));

        frequencyLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\351\242\221\347\216\207:", nullptr));
        frequencySpin->setSuffix(QCoreApplication::translate("CommandSettingsDialog", " Hz", nullptr));
        separatorLabel->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\210\206\351\232\224\347\254\246:", nullptr));
        separatorEdit->setPlaceholderText(QCoreApplication::translate("CommandSettingsDialog", "\346\226\207\346\234\254\345\215\217\350\256\256\345\210\206\351\232\224\347\254\246\357\274\214\345\246\202\351\200\227\345\217\267", nullptr));
        fieldConfigGroup->setTitle(QCoreApplication::translate("CommandSettingsDialog", "\345\255\227\346\256\265\351\205\215\347\275\256", nullptr));
        addFieldBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\267\273\345\212\240\345\255\227\346\256\265", nullptr));
        deleteFieldBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\210\240\351\231\244\345\255\227\346\256\265", nullptr));
        moveUpBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\342\206\221", nullptr));
        moveDownBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\342\206\223", nullptr));
        importFieldBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\257\274\345\205\245\345\255\227\346\256\265...", nullptr));
        QTableWidgetItem *___qtablewidgetitem = fieldTable->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\272\217\345\217\267", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = fieldTable->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("CommandSettingsDialog", "\350\265\267\345\247\213", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = fieldTable->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\220\215\347\247\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = fieldTable->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("CommandSettingsDialog", "\347\261\273\345\236\213", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = fieldTable->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("CommandSettingsDialog", "\351\225\277\345\272\246", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = fieldTable->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("CommandSettingsDialog", "\347\274\251\346\224\276\345\233\240\345\255\220", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = fieldTable->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\201\217\347\247\273\351\207\217", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = fieldTable->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\215\225\344\275\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = fieldTable->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\234\200\345\244\247\345\200\274", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = fieldTable->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\234\200\345\260\217\345\200\274", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = fieldTable->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\217\217\350\277\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = fieldTable->horizontalHeaderItem(11);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\217\220\347\244\272", nullptr));
        importProtocolBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\257\274\345\205\245\345\215\217\350\256\256...", nullptr));
        exportProtocolBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\345\257\274\345\207\272\345\215\217\350\256\256...", nullptr));
        generateProtocolBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\347\224\237\346\210\220\345\215\217\350\256\256", nullptr));
        testProtocolBtn->setText(QCoreApplication::translate("CommandSettingsDialog", "\346\265\213\350\257\225\345\215\217\350\256\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CommandSettingsDialog: public Ui_CommandSettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMANDSETTINGSDIALOG_H
