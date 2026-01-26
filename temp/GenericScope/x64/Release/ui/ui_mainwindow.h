/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QWidget *titleBarWidget;
    QHBoxLayout *titleBarLayout;
    QLabel *brandLabel;
    QSpacerItem *titleSpacer1;
    QLabel *appTitleLabel;
    QSpacerItem *titleSpacer2;
    QToolButton *settingsButton;
    QToolButton *darkModeButton;
    QWidget *controlBarWidget;
    QHBoxLayout *controlBarLayout;
    QComboBox *portComboBox;
    QComboBox *baudRateComboBox;
    QPushButton *connectToggleButton;
    QSpacerItem *controlSpacer;
    QCheckBox *recordLogCheckBox;
    QToolButton *menuButton;
    QSplitter *mainSplitter;
    QWidget *leftPanel;
    QVBoxLayout *leftPanelLayout;
    QWidget *filterToolbar;
    QHBoxLayout *filterLayout;
    QToolButton *filterButton;
    QSpacerItem *filterSpacer;
    QTableWidget *dataTableWidget;
    QWidget *centerPanel;
    QVBoxLayout *centerPanelLayout;
    QWidget *attitudeWidget;
    QVBoxLayout *attitudeLayout;
    QLabel *attitudeTitleLabel;
    QWidget *attitudeValuesWidget;
    QHBoxLayout *attitudeValuesLayout;
    QWidget *rollWidget;
    QVBoxLayout *rollLayout;
    QLabel *rollLabel;
    QLabel *rollValueLabel;
    QWidget *pitchWidget;
    QVBoxLayout *pitchLayout;
    QLabel *pitchLabel;
    QLabel *pitchValueLabel;
    QWidget *yawWidget;
    QVBoxLayout *yawLayout;
    QLabel *yawLabel;
    QLabel *yawValueLabel;
    QWidget *visualization3DContainer;
    QGroupBox *imuStatusGroupBox;
    QVBoxLayout *imuStatusLayout;
    QLabel *imuStatusLabel;
    QLabel *imuDataRateLabel;
    QLabel *imuErrorLabel;
    QWidget *rightPanel;
    QVBoxLayout *rightPanelLayout;
    QWidget *chartParamsWidget;
    QHBoxLayout *chartParamsLayout;
    QLabel *chartIconLabel;
    QSpacerItem *chartParamsSpacer;
    QLabel *xRangeLabel;
    QComboBox *xRangeComboBox;
    QLabel *xRangeUnitLabel;
    QLabel *xDotLabel;
    QComboBox *xDotComboBox;
    QToolButton *chartSettingsButton;
    QWidget *chartContainer;
    QWidget *timestampWidget;
    QHBoxLayout *timestampLayout;
    QLabel *startTimeLabel;
    QSpacerItem *timestampSpacer;
    QLabel *timeLabel;
    QSpacerItem *timestampSpacer2;
    QLabel *endTimeLabel;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1512, 982);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        mainLayout->setContentsMargins(0, 0, 0, 0);
        titleBarWidget = new QWidget(centralwidget);
        titleBarWidget->setObjectName(QString::fromUtf8("titleBarWidget"));
        titleBarWidget->setMinimumSize(QSize(0, 50));
        titleBarWidget->setMaximumSize(QSize(16777215, 50));
        titleBarLayout = new QHBoxLayout(titleBarWidget);
        titleBarLayout->setSpacing(10);
        titleBarLayout->setObjectName(QString::fromUtf8("titleBarLayout"));
        titleBarLayout->setContentsMargins(20, 0, 20, 0);
        brandLabel = new QLabel(titleBarWidget);
        brandLabel->setObjectName(QString::fromUtf8("brandLabel"));

        titleBarLayout->addWidget(brandLabel);

        titleSpacer1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        titleBarLayout->addItem(titleSpacer1);

        appTitleLabel = new QLabel(titleBarWidget);
        appTitleLabel->setObjectName(QString::fromUtf8("appTitleLabel"));
        appTitleLabel->setAlignment(Qt::AlignCenter);

        titleBarLayout->addWidget(appTitleLabel);

        titleSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        titleBarLayout->addItem(titleSpacer2);

        settingsButton = new QToolButton(titleBarWidget);
        settingsButton->setObjectName(QString::fromUtf8("settingsButton"));
        settingsButton->setMinimumSize(QSize(40, 40));
        settingsButton->setIconSize(QSize(24, 24));

        titleBarLayout->addWidget(settingsButton);

        darkModeButton = new QToolButton(titleBarWidget);
        darkModeButton->setObjectName(QString::fromUtf8("darkModeButton"));
        darkModeButton->setMinimumSize(QSize(40, 40));
        darkModeButton->setCheckable(true);
        darkModeButton->setIconSize(QSize(24, 24));

        titleBarLayout->addWidget(darkModeButton);


        mainLayout->addWidget(titleBarWidget);

        controlBarWidget = new QWidget(centralwidget);
        controlBarWidget->setObjectName(QString::fromUtf8("controlBarWidget"));
        controlBarWidget->setMinimumSize(QSize(0, 60));
        controlBarWidget->setMaximumSize(QSize(16777215, 60));
        controlBarLayout = new QHBoxLayout(controlBarWidget);
        controlBarLayout->setSpacing(15);
        controlBarLayout->setObjectName(QString::fromUtf8("controlBarLayout"));
        controlBarLayout->setContentsMargins(20, 10, 20, 10);
        portComboBox = new QComboBox(controlBarWidget);
        portComboBox->addItem(QString());
        portComboBox->addItem(QString());
        portComboBox->addItem(QString());
        portComboBox->addItem(QString());
        portComboBox->addItem(QString());
        portComboBox->addItem(QString());
        portComboBox->addItem(QString());
        portComboBox->setObjectName(QString::fromUtf8("portComboBox"));
        portComboBox->setMinimumSize(QSize(150, 35));

        controlBarLayout->addWidget(portComboBox);

        baudRateComboBox = new QComboBox(controlBarWidget);
        baudRateComboBox->addItem(QString());
        baudRateComboBox->addItem(QString());
        baudRateComboBox->addItem(QString());
        baudRateComboBox->addItem(QString());
        baudRateComboBox->addItem(QString());
        baudRateComboBox->setObjectName(QString::fromUtf8("baudRateComboBox"));
        baudRateComboBox->setMinimumSize(QSize(150, 35));

        controlBarLayout->addWidget(baudRateComboBox);

        connectToggleButton = new QPushButton(controlBarWidget);
        connectToggleButton->setObjectName(QString::fromUtf8("connectToggleButton"));
        connectToggleButton->setMinimumSize(QSize(80, 35));
        connectToggleButton->setCheckable(true);

        controlBarLayout->addWidget(connectToggleButton);

        controlSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        controlBarLayout->addItem(controlSpacer);

        recordLogCheckBox = new QCheckBox(controlBarWidget);
        recordLogCheckBox->setObjectName(QString::fromUtf8("recordLogCheckBox"));

        controlBarLayout->addWidget(recordLogCheckBox);

        menuButton = new QToolButton(controlBarWidget);
        menuButton->setObjectName(QString::fromUtf8("menuButton"));
        menuButton->setMinimumSize(QSize(40, 35));
        menuButton->setPopupMode(QToolButton::InstantPopup);

        controlBarLayout->addWidget(menuButton);


        mainLayout->addWidget(controlBarWidget);

        mainSplitter = new QSplitter(centralwidget);
        mainSplitter->setObjectName(QString::fromUtf8("mainSplitter"));
        mainSplitter->setOrientation(Qt::Horizontal);
        leftPanel = new QWidget(mainSplitter);
        leftPanel->setObjectName(QString::fromUtf8("leftPanel"));
        leftPanel->setMinimumSize(QSize(250, 0));
        leftPanelLayout = new QVBoxLayout(leftPanel);
        leftPanelLayout->setSpacing(5);
        leftPanelLayout->setObjectName(QString::fromUtf8("leftPanelLayout"));
        leftPanelLayout->setContentsMargins(5, 5, 5, 5);
        filterToolbar = new QWidget(leftPanel);
        filterToolbar->setObjectName(QString::fromUtf8("filterToolbar"));
        filterToolbar->setMinimumSize(QSize(0, 40));
        filterToolbar->setMaximumSize(QSize(16777215, 40));
        filterLayout = new QHBoxLayout(filterToolbar);
        filterLayout->setObjectName(QString::fromUtf8("filterLayout"));
        filterButton = new QToolButton(filterToolbar);
        filterButton->setObjectName(QString::fromUtf8("filterButton"));

        filterLayout->addWidget(filterButton);

        filterSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        filterLayout->addItem(filterSpacer);


        leftPanelLayout->addWidget(filterToolbar);

        dataTableWidget = new QTableWidget(leftPanel);
        if (dataTableWidget->columnCount() < 3)
            dataTableWidget->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        dataTableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        dataTableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        dataTableWidget->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        dataTableWidget->setObjectName(QString::fromUtf8("dataTableWidget"));
        dataTableWidget->setColumnCount(3);

        leftPanelLayout->addWidget(dataTableWidget);

        mainSplitter->addWidget(leftPanel);
        centerPanel = new QWidget(mainSplitter);
        centerPanel->setObjectName(QString::fromUtf8("centerPanel"));
        centerPanel->setMinimumSize(QSize(400, 0));
        centerPanelLayout = new QVBoxLayout(centerPanel);
        centerPanelLayout->setSpacing(10);
        centerPanelLayout->setObjectName(QString::fromUtf8("centerPanelLayout"));
        centerPanelLayout->setContentsMargins(10, 10, 10, 10);
        attitudeWidget = new QWidget(centerPanel);
        attitudeWidget->setObjectName(QString::fromUtf8("attitudeWidget"));
        attitudeWidget->setMinimumSize(QSize(0, 80));
        attitudeWidget->setMaximumSize(QSize(16777215, 80));
        attitudeLayout = new QVBoxLayout(attitudeWidget);
        attitudeLayout->setObjectName(QString::fromUtf8("attitudeLayout"));
        attitudeTitleLabel = new QLabel(attitudeWidget);
        attitudeTitleLabel->setObjectName(QString::fromUtf8("attitudeTitleLabel"));
        attitudeTitleLabel->setAlignment(Qt::AlignCenter);

        attitudeLayout->addWidget(attitudeTitleLabel);

        attitudeValuesWidget = new QWidget(attitudeWidget);
        attitudeValuesWidget->setObjectName(QString::fromUtf8("attitudeValuesWidget"));
        attitudeValuesLayout = new QHBoxLayout(attitudeValuesWidget);
        attitudeValuesLayout->setSpacing(20);
        attitudeValuesLayout->setObjectName(QString::fromUtf8("attitudeValuesLayout"));
        rollWidget = new QWidget(attitudeValuesWidget);
        rollWidget->setObjectName(QString::fromUtf8("rollWidget"));
        rollLayout = new QVBoxLayout(rollWidget);
        rollLayout->setSpacing(2);
        rollLayout->setObjectName(QString::fromUtf8("rollLayout"));
        rollLabel = new QLabel(rollWidget);
        rollLabel->setObjectName(QString::fromUtf8("rollLabel"));
        rollLabel->setAlignment(Qt::AlignCenter);

        rollLayout->addWidget(rollLabel);

        rollValueLabel = new QLabel(rollWidget);
        rollValueLabel->setObjectName(QString::fromUtf8("rollValueLabel"));
        rollValueLabel->setAlignment(Qt::AlignCenter);

        rollLayout->addWidget(rollValueLabel);


        attitudeValuesLayout->addWidget(rollWidget);

        pitchWidget = new QWidget(attitudeValuesWidget);
        pitchWidget->setObjectName(QString::fromUtf8("pitchWidget"));
        pitchLayout = new QVBoxLayout(pitchWidget);
        pitchLayout->setSpacing(2);
        pitchLayout->setObjectName(QString::fromUtf8("pitchLayout"));
        pitchLabel = new QLabel(pitchWidget);
        pitchLabel->setObjectName(QString::fromUtf8("pitchLabel"));
        pitchLabel->setAlignment(Qt::AlignCenter);

        pitchLayout->addWidget(pitchLabel);

        pitchValueLabel = new QLabel(pitchWidget);
        pitchValueLabel->setObjectName(QString::fromUtf8("pitchValueLabel"));
        pitchValueLabel->setAlignment(Qt::AlignCenter);

        pitchLayout->addWidget(pitchValueLabel);


        attitudeValuesLayout->addWidget(pitchWidget);

        yawWidget = new QWidget(attitudeValuesWidget);
        yawWidget->setObjectName(QString::fromUtf8("yawWidget"));
        yawLayout = new QVBoxLayout(yawWidget);
        yawLayout->setSpacing(2);
        yawLayout->setObjectName(QString::fromUtf8("yawLayout"));
        yawLabel = new QLabel(yawWidget);
        yawLabel->setObjectName(QString::fromUtf8("yawLabel"));
        yawLabel->setAlignment(Qt::AlignCenter);

        yawLayout->addWidget(yawLabel);

        yawValueLabel = new QLabel(yawWidget);
        yawValueLabel->setObjectName(QString::fromUtf8("yawValueLabel"));
        yawValueLabel->setAlignment(Qt::AlignCenter);

        yawLayout->addWidget(yawValueLabel);


        attitudeValuesLayout->addWidget(yawWidget);


        attitudeLayout->addWidget(attitudeValuesWidget);


        centerPanelLayout->addWidget(attitudeWidget);

        visualization3DContainer = new QWidget(centerPanel);
        visualization3DContainer->setObjectName(QString::fromUtf8("visualization3DContainer"));
        visualization3DContainer->setMinimumSize(QSize(0, 400));

        centerPanelLayout->addWidget(visualization3DContainer);

        imuStatusGroupBox = new QGroupBox(centerPanel);
        imuStatusGroupBox->setObjectName(QString::fromUtf8("imuStatusGroupBox"));
        imuStatusGroupBox->setMinimumSize(QSize(0, 100));
        imuStatusLayout = new QVBoxLayout(imuStatusGroupBox);
        imuStatusLayout->setObjectName(QString::fromUtf8("imuStatusLayout"));
        imuStatusLabel = new QLabel(imuStatusGroupBox);
        imuStatusLabel->setObjectName(QString::fromUtf8("imuStatusLabel"));

        imuStatusLayout->addWidget(imuStatusLabel);

        imuDataRateLabel = new QLabel(imuStatusGroupBox);
        imuDataRateLabel->setObjectName(QString::fromUtf8("imuDataRateLabel"));

        imuStatusLayout->addWidget(imuDataRateLabel);

        imuErrorLabel = new QLabel(imuStatusGroupBox);
        imuErrorLabel->setObjectName(QString::fromUtf8("imuErrorLabel"));

        imuStatusLayout->addWidget(imuErrorLabel);


        centerPanelLayout->addWidget(imuStatusGroupBox);

        mainSplitter->addWidget(centerPanel);
        rightPanel = new QWidget(mainSplitter);
        rightPanel->setObjectName(QString::fromUtf8("rightPanel"));
        rightPanel->setMinimumSize(QSize(400, 0));
        rightPanelLayout = new QVBoxLayout(rightPanel);
        rightPanelLayout->setSpacing(10);
        rightPanelLayout->setObjectName(QString::fromUtf8("rightPanelLayout"));
        rightPanelLayout->setContentsMargins(10, 10, 10, 10);
        chartParamsWidget = new QWidget(rightPanel);
        chartParamsWidget->setObjectName(QString::fromUtf8("chartParamsWidget"));
        chartParamsWidget->setMinimumSize(QSize(0, 50));
        chartParamsWidget->setMaximumSize(QSize(16777215, 50));
        chartParamsLayout = new QHBoxLayout(chartParamsWidget);
        chartParamsLayout->setObjectName(QString::fromUtf8("chartParamsLayout"));
        chartIconLabel = new QLabel(chartParamsWidget);
        chartIconLabel->setObjectName(QString::fromUtf8("chartIconLabel"));

        chartParamsLayout->addWidget(chartIconLabel);

        chartParamsSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        chartParamsLayout->addItem(chartParamsSpacer);

        xRangeLabel = new QLabel(chartParamsWidget);
        xRangeLabel->setObjectName(QString::fromUtf8("xRangeLabel"));

        chartParamsLayout->addWidget(xRangeLabel);

        xRangeComboBox = new QComboBox(chartParamsWidget);
        xRangeComboBox->addItem(QString());
        xRangeComboBox->addItem(QString());
        xRangeComboBox->addItem(QString());
        xRangeComboBox->addItem(QString());
        xRangeComboBox->setObjectName(QString::fromUtf8("xRangeComboBox"));
        xRangeComboBox->setMinimumSize(QSize(80, 30));

        chartParamsLayout->addWidget(xRangeComboBox);

        xRangeUnitLabel = new QLabel(chartParamsWidget);
        xRangeUnitLabel->setObjectName(QString::fromUtf8("xRangeUnitLabel"));

        chartParamsLayout->addWidget(xRangeUnitLabel);

        xDotLabel = new QLabel(chartParamsWidget);
        xDotLabel->setObjectName(QString::fromUtf8("xDotLabel"));

        chartParamsLayout->addWidget(xDotLabel);

        xDotComboBox = new QComboBox(chartParamsWidget);
        xDotComboBox->addItem(QString());
        xDotComboBox->addItem(QString());
        xDotComboBox->addItem(QString());
        xDotComboBox->addItem(QString());
        xDotComboBox->setObjectName(QString::fromUtf8("xDotComboBox"));
        xDotComboBox->setMinimumSize(QSize(60, 30));

        chartParamsLayout->addWidget(xDotComboBox);

        chartSettingsButton = new QToolButton(chartParamsWidget);
        chartSettingsButton->setObjectName(QString::fromUtf8("chartSettingsButton"));

        chartParamsLayout->addWidget(chartSettingsButton);


        rightPanelLayout->addWidget(chartParamsWidget);

        chartContainer = new QWidget(rightPanel);
        chartContainer->setObjectName(QString::fromUtf8("chartContainer"));
        chartContainer->setMinimumSize(QSize(0, 300));

        rightPanelLayout->addWidget(chartContainer);

        timestampWidget = new QWidget(rightPanel);
        timestampWidget->setObjectName(QString::fromUtf8("timestampWidget"));
        timestampWidget->setMinimumSize(QSize(0, 40));
        timestampWidget->setMaximumSize(QSize(16777215, 40));
        timestampLayout = new QHBoxLayout(timestampWidget);
        timestampLayout->setObjectName(QString::fromUtf8("timestampLayout"));
        startTimeLabel = new QLabel(timestampWidget);
        startTimeLabel->setObjectName(QString::fromUtf8("startTimeLabel"));

        timestampLayout->addWidget(startTimeLabel);

        timestampSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        timestampLayout->addItem(timestampSpacer);

        timeLabel = new QLabel(timestampWidget);
        timeLabel->setObjectName(QString::fromUtf8("timeLabel"));
        timeLabel->setAlignment(Qt::AlignCenter);

        timestampLayout->addWidget(timeLabel);

        timestampSpacer2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        timestampLayout->addItem(timestampSpacer2);

        endTimeLabel = new QLabel(timestampWidget);
        endTimeLabel->setObjectName(QString::fromUtf8("endTimeLabel"));

        timestampLayout->addWidget(endTimeLabel);


        rightPanelLayout->addWidget(timestampWidget);

        mainSplitter->addWidget(rightPanel);

        mainLayout->addWidget(mainSplitter);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "GenericScope", nullptr));
        brandLabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:18pt; font-weight:600;\">GENERIC</span><span style=\" font-size:18pt; font-weight:600; color:#ff5722;\">SCOPE</span></p></body></html>", nullptr));
        appTitleLabel->setText(QCoreApplication::translate("MainWindow", "<html><head/><body><p><span style=\" font-size:16pt; font-weight:600;\">DS_GenericScope</span></p></body></html>", nullptr));
        settingsButton->setText(QCoreApplication::translate("MainWindow", "\342\232\231", nullptr));
        darkModeButton->setText(QCoreApplication::translate("MainWindow", "\360\237\214\231", nullptr));
        portComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "COM1", nullptr));
        portComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "COM2", nullptr));
        portComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "COM3", nullptr));
        portComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "COM4", nullptr));
        portComboBox->setItemText(4, QCoreApplication::translate("MainWindow", "COM5", nullptr));
        portComboBox->setItemText(5, QCoreApplication::translate("MainWindow", "COM6", nullptr));
        portComboBox->setItemText(6, QCoreApplication::translate("MainWindow", "COM7", nullptr));

        baudRateComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "9600", nullptr));
        baudRateComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "19200", nullptr));
        baudRateComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "38400", nullptr));
        baudRateComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "57600", nullptr));
        baudRateComboBox->setItemText(4, QCoreApplication::translate("MainWindow", "115200", nullptr));

        connectToggleButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        recordLogCheckBox->setText(QCoreApplication::translate("MainWindow", "RecordLog", nullptr));
        menuButton->setText(QCoreApplication::translate("MainWindow", "\342\211\241", nullptr));
        filterButton->setText(QCoreApplication::translate("MainWindow", "\360\237\224\275", nullptr));
        QTableWidgetItem *___qtablewidgetitem = dataTableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "Message", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = dataTableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "Value", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = dataTableWidget->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("MainWindow", "Unit", nullptr));
        attitudeTitleLabel->setText(QCoreApplication::translate("MainWindow", "\360\237\216\257", nullptr));
        rollLabel->setText(QCoreApplication::translate("MainWindow", "Roll(deg)", nullptr));
        rollValueLabel->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        pitchLabel->setText(QCoreApplication::translate("MainWindow", "Pitch(deg)", nullptr));
        pitchValueLabel->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        yawLabel->setText(QCoreApplication::translate("MainWindow", "Yaw(deg)", nullptr));
        yawValueLabel->setText(QCoreApplication::translate("MainWindow", "-", nullptr));
        imuStatusGroupBox->setTitle(QCoreApplication::translate("MainWindow", "IMU Status", nullptr));
        imuStatusLabel->setText(QCoreApplication::translate("MainWindow", "Status: Ready", nullptr));
        imuDataRateLabel->setText(QCoreApplication::translate("MainWindow", "Data Rate: 0 Hz", nullptr));
        imuErrorLabel->setText(QCoreApplication::translate("MainWindow", "Error: None", nullptr));
        chartIconLabel->setText(QCoreApplication::translate("MainWindow", "\360\237\223\212", nullptr));
        xRangeLabel->setText(QCoreApplication::translate("MainWindow", "X_Range", nullptr));
        xRangeComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "10", nullptr));
        xRangeComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "30", nullptr));
        xRangeComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "60", nullptr));
        xRangeComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "120", nullptr));

        xRangeUnitLabel->setText(QCoreApplication::translate("MainWindow", "s", nullptr));
        xDotLabel->setText(QCoreApplication::translate("MainWindow", "X_Dot", nullptr));
        xDotComboBox->setItemText(0, QCoreApplication::translate("MainWindow", "1", nullptr));
        xDotComboBox->setItemText(1, QCoreApplication::translate("MainWindow", "2", nullptr));
        xDotComboBox->setItemText(2, QCoreApplication::translate("MainWindow", "5", nullptr));
        xDotComboBox->setItemText(3, QCoreApplication::translate("MainWindow", "10", nullptr));

        chartSettingsButton->setText(QCoreApplication::translate("MainWindow", "\342\232\231", nullptr));
        startTimeLabel->setText(QCoreApplication::translate("MainWindow", "00:00:00.000", nullptr));
        timeLabel->setText(QCoreApplication::translate("MainWindow", "Time", nullptr));
        endTimeLabel->setText(QCoreApplication::translate("MainWindow", "00:00:00.000", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
