/********************************************************************************
** Form generated from reading UI file 'deviceconfigdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEVICECONFIGDIALOG_H
#define UI_DEVICECONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DeviceConfigDialog
{
public:
    QVBoxLayout *mainLayout;
    QHBoxLayout *statusLayout;
    QLabel *connectionStatusLabel;
    QSpacerItem *statusSpacer;
    QHBoxLayout *protocolLayout;
    QLabel *protocolLabel;
    QSpacerItem *protocolSpacer1;
    QRadioButton *binaryProtocolRadio;
    QRadioButton *quaternionProtocolRadio;
    QSpacerItem *protocolSpacer2;
    QHBoxLayout *toolbarLayout;
    QPushButton *refreshBtn;
    QPushButton *calibrateBtn;
    QPushButton *resetBtn;
    QPushButton *restartBtn;
    QSpacerItem *toolbarSpacer;
    QTabWidget *tabWidget;
    QWidget *deviceInfoTab;
    QVBoxLayout *deviceInfoLayout;
    QGroupBox *basicInfoGroup;
    QFormLayout *basicInfoLayout;
    QLabel *snLabel;
    QLineEdit *snEdit;
    QLabel *firmwareVersionLabel;
    QLineEdit *firmwareVersionEdit;
    QLabel *algorithmVersionLabel;
    QLineEdit *algorithmVersionEdit;
    QGroupBox *outputGroup;
    QGridLayout *outputLayout;
    QLabel *baudRateLabel;
    QComboBox *baudRateCombo;
    QLabel *baudRateUnitLabel;
    QLabel *frequencyLabel;
    QComboBox *frequencyCombo;
    QLabel *frequencyUnitLabel;
    QPushButton *writeOutputBtn;
    QGroupBox *gyroGroup;
    QGridLayout *gyroLayout;
    QLabel *gyroRangeLabel;
    QComboBox *gyroRangeCombo;
    QLabel *gyroRangeUnitLabel;
    QLabel *gyroFilterLabel;
    QComboBox *gyroFilterCombo;
    QLabel *gyroFilterUnitLabel;
    QPushButton *writeGyroBtn;
    QGroupBox *accelGroup;
    QGridLayout *accelLayout;
    QLabel *accelRangeLabel;
    QComboBox *accelRangeCombo;
    QLabel *accelRangeUnitLabel;
    QLabel *accelFilterLabel;
    QComboBox *accelFilterCombo;
    QLabel *accelFilterUnitLabel;
    QPushButton *writeAccelBtn;
    QSpacerItem *deviceInfoSpacer;
    QWidget *firmwareTab;
    QVBoxLayout *firmwareLayout;
    QLabel *firmwarePlaceholder;

    void setupUi(QDialog *DeviceConfigDialog)
    {
        if (DeviceConfigDialog->objectName().isEmpty())
            DeviceConfigDialog->setObjectName(QString::fromUtf8("DeviceConfigDialog"));
        DeviceConfigDialog->resize(900, 650);
        mainLayout = new QVBoxLayout(DeviceConfigDialog);
        mainLayout->setSpacing(10);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(15, 15, 15, 15);
        statusLayout = new QHBoxLayout();
        statusLayout->setObjectName(QString::fromUtf8("statusLayout"));
        connectionStatusLabel = new QLabel(DeviceConfigDialog);
        connectionStatusLabel->setObjectName(QString::fromUtf8("connectionStatusLabel"));
        connectionStatusLabel->setStyleSheet(QString::fromUtf8("QLabel { color: #E74C3C; font-weight: bold; font-size: 14px; padding: 5px 10px; }"));

        statusLayout->addWidget(connectionStatusLabel);

        statusSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        statusLayout->addItem(statusSpacer);


        mainLayout->addLayout(statusLayout);

        protocolLayout = new QHBoxLayout();
        protocolLayout->setObjectName(QString::fromUtf8("protocolLayout"));
        protocolLabel = new QLabel(DeviceConfigDialog);
        protocolLabel->setObjectName(QString::fromUtf8("protocolLabel"));
        protocolLabel->setStyleSheet(QString::fromUtf8("font-weight: bold;"));

        protocolLayout->addWidget(protocolLabel);

        protocolSpacer1 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        protocolLayout->addItem(protocolSpacer1);

        binaryProtocolRadio = new QRadioButton(DeviceConfigDialog);
        binaryProtocolRadio->setObjectName(QString::fromUtf8("binaryProtocolRadio"));
        binaryProtocolRadio->setChecked(true);

        protocolLayout->addWidget(binaryProtocolRadio);

        quaternionProtocolRadio = new QRadioButton(DeviceConfigDialog);
        quaternionProtocolRadio->setObjectName(QString::fromUtf8("quaternionProtocolRadio"));

        protocolLayout->addWidget(quaternionProtocolRadio);

        protocolSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        protocolLayout->addItem(protocolSpacer2);


        mainLayout->addLayout(protocolLayout);

        toolbarLayout = new QHBoxLayout();
        toolbarLayout->setObjectName(QString::fromUtf8("toolbarLayout"));
        refreshBtn = new QPushButton(DeviceConfigDialog);
        refreshBtn->setObjectName(QString::fromUtf8("refreshBtn"));
        refreshBtn->setMinimumSize(QSize(80, 0));

        toolbarLayout->addWidget(refreshBtn);

        calibrateBtn = new QPushButton(DeviceConfigDialog);
        calibrateBtn->setObjectName(QString::fromUtf8("calibrateBtn"));
        calibrateBtn->setMinimumSize(QSize(100, 0));

        toolbarLayout->addWidget(calibrateBtn);

        resetBtn = new QPushButton(DeviceConfigDialog);
        resetBtn->setObjectName(QString::fromUtf8("resetBtn"));
        resetBtn->setMinimumSize(QSize(80, 0));

        toolbarLayout->addWidget(resetBtn);

        restartBtn = new QPushButton(DeviceConfigDialog);
        restartBtn->setObjectName(QString::fromUtf8("restartBtn"));
        restartBtn->setMinimumSize(QSize(80, 0));

        toolbarLayout->addWidget(restartBtn);

        toolbarSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        toolbarLayout->addItem(toolbarSpacer);


        mainLayout->addLayout(toolbarLayout);

        tabWidget = new QTabWidget(DeviceConfigDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        deviceInfoTab = new QWidget();
        deviceInfoTab->setObjectName(QString::fromUtf8("deviceInfoTab"));
        deviceInfoLayout = new QVBoxLayout(deviceInfoTab);
        deviceInfoLayout->setSpacing(15);
        deviceInfoLayout->setObjectName(QString::fromUtf8("deviceInfoLayout"));
        basicInfoGroup = new QGroupBox(deviceInfoTab);
        basicInfoGroup->setObjectName(QString::fromUtf8("basicInfoGroup"));
        basicInfoLayout = new QFormLayout(basicInfoGroup);
        basicInfoLayout->setObjectName(QString::fromUtf8("basicInfoLayout"));
        snLabel = new QLabel(basicInfoGroup);
        snLabel->setObjectName(QString::fromUtf8("snLabel"));

        basicInfoLayout->setWidget(0, QFormLayout::LabelRole, snLabel);

        snEdit = new QLineEdit(basicInfoGroup);
        snEdit->setObjectName(QString::fromUtf8("snEdit"));
        snEdit->setReadOnly(true);

        basicInfoLayout->setWidget(0, QFormLayout::FieldRole, snEdit);

        firmwareVersionLabel = new QLabel(basicInfoGroup);
        firmwareVersionLabel->setObjectName(QString::fromUtf8("firmwareVersionLabel"));

        basicInfoLayout->setWidget(1, QFormLayout::LabelRole, firmwareVersionLabel);

        firmwareVersionEdit = new QLineEdit(basicInfoGroup);
        firmwareVersionEdit->setObjectName(QString::fromUtf8("firmwareVersionEdit"));
        firmwareVersionEdit->setReadOnly(true);

        basicInfoLayout->setWidget(1, QFormLayout::FieldRole, firmwareVersionEdit);

        algorithmVersionLabel = new QLabel(basicInfoGroup);
        algorithmVersionLabel->setObjectName(QString::fromUtf8("algorithmVersionLabel"));

        basicInfoLayout->setWidget(2, QFormLayout::LabelRole, algorithmVersionLabel);

        algorithmVersionEdit = new QLineEdit(basicInfoGroup);
        algorithmVersionEdit->setObjectName(QString::fromUtf8("algorithmVersionEdit"));
        algorithmVersionEdit->setReadOnly(true);

        basicInfoLayout->setWidget(2, QFormLayout::FieldRole, algorithmVersionEdit);


        deviceInfoLayout->addWidget(basicInfoGroup);

        outputGroup = new QGroupBox(deviceInfoTab);
        outputGroup->setObjectName(QString::fromUtf8("outputGroup"));
        outputLayout = new QGridLayout(outputGroup);
        outputLayout->setObjectName(QString::fromUtf8("outputLayout"));
        baudRateLabel = new QLabel(outputGroup);
        baudRateLabel->setObjectName(QString::fromUtf8("baudRateLabel"));

        outputLayout->addWidget(baudRateLabel, 0, 0, 1, 1);

        baudRateCombo = new QComboBox(outputGroup);
        baudRateCombo->addItem(QString());
        baudRateCombo->addItem(QString());
        baudRateCombo->addItem(QString());
        baudRateCombo->addItem(QString());
        baudRateCombo->addItem(QString());
        baudRateCombo->addItem(QString());
        baudRateCombo->addItem(QString());
        baudRateCombo->addItem(QString());
        baudRateCombo->setObjectName(QString::fromUtf8("baudRateCombo"));

        outputLayout->addWidget(baudRateCombo, 0, 1, 1, 1);

        baudRateUnitLabel = new QLabel(outputGroup);
        baudRateUnitLabel->setObjectName(QString::fromUtf8("baudRateUnitLabel"));

        outputLayout->addWidget(baudRateUnitLabel, 0, 2, 1, 1);

        frequencyLabel = new QLabel(outputGroup);
        frequencyLabel->setObjectName(QString::fromUtf8("frequencyLabel"));

        outputLayout->addWidget(frequencyLabel, 1, 0, 1, 1);

        frequencyCombo = new QComboBox(outputGroup);
        frequencyCombo->addItem(QString());
        frequencyCombo->addItem(QString());
        frequencyCombo->addItem(QString());
        frequencyCombo->addItem(QString());
        frequencyCombo->addItem(QString());
        frequencyCombo->addItem(QString());
        frequencyCombo->addItem(QString());
        frequencyCombo->addItem(QString());
        frequencyCombo->setObjectName(QString::fromUtf8("frequencyCombo"));

        outputLayout->addWidget(frequencyCombo, 1, 1, 1, 1);

        frequencyUnitLabel = new QLabel(outputGroup);
        frequencyUnitLabel->setObjectName(QString::fromUtf8("frequencyUnitLabel"));

        outputLayout->addWidget(frequencyUnitLabel, 1, 2, 1, 1);

        writeOutputBtn = new QPushButton(outputGroup);
        writeOutputBtn->setObjectName(QString::fromUtf8("writeOutputBtn"));
        writeOutputBtn->setMinimumSize(QSize(100, 0));

        outputLayout->addWidget(writeOutputBtn, 0, 3, 2, 1);


        deviceInfoLayout->addWidget(outputGroup);

        gyroGroup = new QGroupBox(deviceInfoTab);
        gyroGroup->setObjectName(QString::fromUtf8("gyroGroup"));
        gyroLayout = new QGridLayout(gyroGroup);
        gyroLayout->setObjectName(QString::fromUtf8("gyroLayout"));
        gyroRangeLabel = new QLabel(gyroGroup);
        gyroRangeLabel->setObjectName(QString::fromUtf8("gyroRangeLabel"));

        gyroLayout->addWidget(gyroRangeLabel, 0, 0, 1, 1);

        gyroRangeCombo = new QComboBox(gyroGroup);
        gyroRangeCombo->addItem(QString());
        gyroRangeCombo->addItem(QString());
        gyroRangeCombo->addItem(QString());
        gyroRangeCombo->addItem(QString());
        gyroRangeCombo->setObjectName(QString::fromUtf8("gyroRangeCombo"));

        gyroLayout->addWidget(gyroRangeCombo, 0, 1, 1, 1);

        gyroRangeUnitLabel = new QLabel(gyroGroup);
        gyroRangeUnitLabel->setObjectName(QString::fromUtf8("gyroRangeUnitLabel"));

        gyroLayout->addWidget(gyroRangeUnitLabel, 0, 2, 1, 1);

        gyroFilterLabel = new QLabel(gyroGroup);
        gyroFilterLabel->setObjectName(QString::fromUtf8("gyroFilterLabel"));

        gyroLayout->addWidget(gyroFilterLabel, 1, 0, 1, 1);

        gyroFilterCombo = new QComboBox(gyroGroup);
        gyroFilterCombo->addItem(QString());
        gyroFilterCombo->addItem(QString());
        gyroFilterCombo->addItem(QString());
        gyroFilterCombo->addItem(QString());
        gyroFilterCombo->addItem(QString());
        gyroFilterCombo->addItem(QString());
        gyroFilterCombo->setObjectName(QString::fromUtf8("gyroFilterCombo"));

        gyroLayout->addWidget(gyroFilterCombo, 1, 1, 1, 1);

        gyroFilterUnitLabel = new QLabel(gyroGroup);
        gyroFilterUnitLabel->setObjectName(QString::fromUtf8("gyroFilterUnitLabel"));

        gyroLayout->addWidget(gyroFilterUnitLabel, 1, 2, 1, 1);

        writeGyroBtn = new QPushButton(gyroGroup);
        writeGyroBtn->setObjectName(QString::fromUtf8("writeGyroBtn"));
        writeGyroBtn->setMinimumSize(QSize(100, 0));

        gyroLayout->addWidget(writeGyroBtn, 0, 3, 2, 1);


        deviceInfoLayout->addWidget(gyroGroup);

        accelGroup = new QGroupBox(deviceInfoTab);
        accelGroup->setObjectName(QString::fromUtf8("accelGroup"));
        accelLayout = new QGridLayout(accelGroup);
        accelLayout->setObjectName(QString::fromUtf8("accelLayout"));
        accelRangeLabel = new QLabel(accelGroup);
        accelRangeLabel->setObjectName(QString::fromUtf8("accelRangeLabel"));

        accelLayout->addWidget(accelRangeLabel, 0, 0, 1, 1);

        accelRangeCombo = new QComboBox(accelGroup);
        accelRangeCombo->addItem(QString());
        accelRangeCombo->addItem(QString());
        accelRangeCombo->addItem(QString());
        accelRangeCombo->addItem(QString());
        accelRangeCombo->setObjectName(QString::fromUtf8("accelRangeCombo"));

        accelLayout->addWidget(accelRangeCombo, 0, 1, 1, 1);

        accelRangeUnitLabel = new QLabel(accelGroup);
        accelRangeUnitLabel->setObjectName(QString::fromUtf8("accelRangeUnitLabel"));

        accelLayout->addWidget(accelRangeUnitLabel, 0, 2, 1, 1);

        accelFilterLabel = new QLabel(accelGroup);
        accelFilterLabel->setObjectName(QString::fromUtf8("accelFilterLabel"));

        accelLayout->addWidget(accelFilterLabel, 1, 0, 1, 1);

        accelFilterCombo = new QComboBox(accelGroup);
        accelFilterCombo->addItem(QString());
        accelFilterCombo->addItem(QString());
        accelFilterCombo->addItem(QString());
        accelFilterCombo->addItem(QString());
        accelFilterCombo->addItem(QString());
        accelFilterCombo->addItem(QString());
        accelFilterCombo->setObjectName(QString::fromUtf8("accelFilterCombo"));

        accelLayout->addWidget(accelFilterCombo, 1, 1, 1, 1);

        accelFilterUnitLabel = new QLabel(accelGroup);
        accelFilterUnitLabel->setObjectName(QString::fromUtf8("accelFilterUnitLabel"));

        accelLayout->addWidget(accelFilterUnitLabel, 1, 2, 1, 1);

        writeAccelBtn = new QPushButton(accelGroup);
        writeAccelBtn->setObjectName(QString::fromUtf8("writeAccelBtn"));
        writeAccelBtn->setMinimumSize(QSize(100, 0));

        accelLayout->addWidget(writeAccelBtn, 0, 3, 2, 1);


        deviceInfoLayout->addWidget(accelGroup);

        deviceInfoSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        deviceInfoLayout->addItem(deviceInfoSpacer);

        tabWidget->addTab(deviceInfoTab, QString());
        firmwareTab = new QWidget();
        firmwareTab->setObjectName(QString::fromUtf8("firmwareTab"));
        firmwareLayout = new QVBoxLayout(firmwareTab);
        firmwareLayout->setObjectName(QString::fromUtf8("firmwareLayout"));
        firmwarePlaceholder = new QLabel(firmwareTab);
        firmwarePlaceholder->setObjectName(QString::fromUtf8("firmwarePlaceholder"));
        firmwarePlaceholder->setAlignment(Qt::AlignCenter);
        firmwarePlaceholder->setStyleSheet(QString::fromUtf8("color: gray; font-size: 14px;"));

        firmwareLayout->addWidget(firmwarePlaceholder);

        tabWidget->addTab(firmwareTab, QString());

        mainLayout->addWidget(tabWidget);


        retranslateUi(DeviceConfigDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(DeviceConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *DeviceConfigDialog)
    {
        DeviceConfigDialog->setWindowTitle(QCoreApplication::translate("DeviceConfigDialog", "\350\256\276\345\244\207\351\205\215\347\275\256", nullptr));
        connectionStatusLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\344\270\262\345\217\243\346\234\252\350\277\236\346\216\245", nullptr));
        protocolLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\344\270\262\345\217\243\351\200\232\344\277\241\345\215\217\350\256\256", nullptr));
        binaryProtocolRadio->setText(QCoreApplication::translate("DeviceConfigDialog", "\344\272\214\350\277\233\345\210\266", nullptr));
        quaternionProtocolRadio->setText(QCoreApplication::translate("DeviceConfigDialog", "\344\272\214\350\277\233\345\210\266\357\274\210\345\233\233\345\205\203\346\225\260\357\274\211", nullptr));
        refreshBtn->setText(QCoreApplication::translate("DeviceConfigDialog", "\345\210\267\346\226\260", nullptr));
        calibrateBtn->setText(QCoreApplication::translate("DeviceConfigDialog", "\346\260\264\345\271\263\346\240\241\345\207\206", nullptr));
        resetBtn->setText(QCoreApplication::translate("DeviceConfigDialog", "\351\207\215\347\275\256", nullptr));
        restartBtn->setText(QCoreApplication::translate("DeviceConfigDialog", "\351\207\215\345\220\257", nullptr));
        basicInfoGroup->setTitle(QCoreApplication::translate("DeviceConfigDialog", "\345\237\272\346\234\254\344\277\241\346\201\257", nullptr));
        snLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "SN", nullptr));
        snEdit->setText(QCoreApplication::translate("DeviceConfigDialog", "NACK", nullptr));
        firmwareVersionLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\345\233\272\344\273\266\347\211\210\346\234\254", nullptr));
        firmwareVersionEdit->setText(QCoreApplication::translate("DeviceConfigDialog", "NACK", nullptr));
        algorithmVersionLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\347\256\227\346\263\225\347\211\210\346\234\254", nullptr));
        algorithmVersionEdit->setText(QCoreApplication::translate("DeviceConfigDialog", "NACK", nullptr));
        outputGroup->setTitle(QCoreApplication::translate("DeviceConfigDialog", "\350\256\276\345\244\207\350\276\223\345\207\272", nullptr));
        baudRateLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\346\263\242\347\211\271\347\216\207", nullptr));
        baudRateCombo->setItemText(0, QCoreApplication::translate("DeviceConfigDialog", "9600", nullptr));
        baudRateCombo->setItemText(1, QCoreApplication::translate("DeviceConfigDialog", "19200", nullptr));
        baudRateCombo->setItemText(2, QCoreApplication::translate("DeviceConfigDialog", "38400", nullptr));
        baudRateCombo->setItemText(3, QCoreApplication::translate("DeviceConfigDialog", "57600", nullptr));
        baudRateCombo->setItemText(4, QCoreApplication::translate("DeviceConfigDialog", "115200", nullptr));
        baudRateCombo->setItemText(5, QCoreApplication::translate("DeviceConfigDialog", "230400", nullptr));
        baudRateCombo->setItemText(6, QCoreApplication::translate("DeviceConfigDialog", "460800", nullptr));
        baudRateCombo->setItemText(7, QCoreApplication::translate("DeviceConfigDialog", "921600", nullptr));

        baudRateUnitLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "Bps", nullptr));
        frequencyLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\351\242\221\347\216\207", nullptr));
        frequencyCombo->setItemText(0, QCoreApplication::translate("DeviceConfigDialog", "1", nullptr));
        frequencyCombo->setItemText(1, QCoreApplication::translate("DeviceConfigDialog", "10", nullptr));
        frequencyCombo->setItemText(2, QCoreApplication::translate("DeviceConfigDialog", "20", nullptr));
        frequencyCombo->setItemText(3, QCoreApplication::translate("DeviceConfigDialog", "50", nullptr));
        frequencyCombo->setItemText(4, QCoreApplication::translate("DeviceConfigDialog", "100", nullptr));
        frequencyCombo->setItemText(5, QCoreApplication::translate("DeviceConfigDialog", "200", nullptr));
        frequencyCombo->setItemText(6, QCoreApplication::translate("DeviceConfigDialog", "500", nullptr));
        frequencyCombo->setItemText(7, QCoreApplication::translate("DeviceConfigDialog", "1000", nullptr));

        frequencyUnitLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "Hz", nullptr));
        writeOutputBtn->setText(QCoreApplication::translate("DeviceConfigDialog", "\345\206\231\345\205\245", nullptr));
        gyroGroup->setTitle(QCoreApplication::translate("DeviceConfigDialog", "\351\231\200\350\236\272\344\273\252", nullptr));
        gyroRangeLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\351\207\217\347\250\213", nullptr));
        gyroRangeCombo->setItemText(0, QCoreApplication::translate("DeviceConfigDialog", "250", nullptr));
        gyroRangeCombo->setItemText(1, QCoreApplication::translate("DeviceConfigDialog", "500", nullptr));
        gyroRangeCombo->setItemText(2, QCoreApplication::translate("DeviceConfigDialog", "1000", nullptr));
        gyroRangeCombo->setItemText(3, QCoreApplication::translate("DeviceConfigDialog", "2000", nullptr));

        gyroRangeUnitLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\302\260/s", nullptr));
        gyroFilterLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\346\273\244\346\263\242\345\231\250", nullptr));
        gyroFilterCombo->setItemText(0, QCoreApplication::translate("DeviceConfigDialog", "5", nullptr));
        gyroFilterCombo->setItemText(1, QCoreApplication::translate("DeviceConfigDialog", "10", nullptr));
        gyroFilterCombo->setItemText(2, QCoreApplication::translate("DeviceConfigDialog", "20", nullptr));
        gyroFilterCombo->setItemText(3, QCoreApplication::translate("DeviceConfigDialog", "42", nullptr));
        gyroFilterCombo->setItemText(4, QCoreApplication::translate("DeviceConfigDialog", "98", nullptr));
        gyroFilterCombo->setItemText(5, QCoreApplication::translate("DeviceConfigDialog", "188", nullptr));

        gyroFilterUnitLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "Hz", nullptr));
        writeGyroBtn->setText(QCoreApplication::translate("DeviceConfigDialog", "\345\206\231\345\205\245", nullptr));
        accelGroup->setTitle(QCoreApplication::translate("DeviceConfigDialog", "\345\212\240\351\200\237\345\272\246\350\256\241", nullptr));
        accelRangeLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\351\207\217\347\250\213", nullptr));
        accelRangeCombo->setItemText(0, QCoreApplication::translate("DeviceConfigDialog", "2", nullptr));
        accelRangeCombo->setItemText(1, QCoreApplication::translate("DeviceConfigDialog", "4", nullptr));
        accelRangeCombo->setItemText(2, QCoreApplication::translate("DeviceConfigDialog", "8", nullptr));
        accelRangeCombo->setItemText(3, QCoreApplication::translate("DeviceConfigDialog", "16", nullptr));

        accelRangeUnitLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "g", nullptr));
        accelFilterLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "\346\273\244\346\263\242\345\231\250", nullptr));
        accelFilterCombo->setItemText(0, QCoreApplication::translate("DeviceConfigDialog", "5", nullptr));
        accelFilterCombo->setItemText(1, QCoreApplication::translate("DeviceConfigDialog", "10", nullptr));
        accelFilterCombo->setItemText(2, QCoreApplication::translate("DeviceConfigDialog", "20", nullptr));
        accelFilterCombo->setItemText(3, QCoreApplication::translate("DeviceConfigDialog", "42", nullptr));
        accelFilterCombo->setItemText(4, QCoreApplication::translate("DeviceConfigDialog", "98", nullptr));
        accelFilterCombo->setItemText(5, QCoreApplication::translate("DeviceConfigDialog", "188", nullptr));

        accelFilterUnitLabel->setText(QCoreApplication::translate("DeviceConfigDialog", "Hz", nullptr));
        writeAccelBtn->setText(QCoreApplication::translate("DeviceConfigDialog", "\345\206\231\345\205\245", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(deviceInfoTab), QCoreApplication::translate("DeviceConfigDialog", "\350\256\276\345\244\207\344\277\241\346\201\257", nullptr));
        firmwarePlaceholder->setText(QCoreApplication::translate("DeviceConfigDialog", "\345\233\272\344\273\266\345\215\207\347\272\247\345\212\237\350\203\275\345\276\205\345\256\236\347\216\260", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(firmwareTab), QCoreApplication::translate("DeviceConfigDialog", "\345\233\272\344\273\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DeviceConfigDialog: public Ui_DeviceConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEVICECONFIGDIALOG_H
