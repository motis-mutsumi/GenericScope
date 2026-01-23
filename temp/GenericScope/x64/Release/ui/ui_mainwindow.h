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
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExit;
    QAction *actionAbout;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QWidget *toolbarWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QFrame *line;
    QPushButton *startPollingButton;
    QPushButton *stopPollingButton;
    QFrame *line_2;
    QPushButton *startRecordButton;
    QPushButton *stopRecordButton;
    QSpacerItem *horizontalSpacer;
    QPushButton *settingsButton;
    QSplitter *splitter;
    QWidget *plotWidget;
    QVBoxLayout *verticalLayout_2;
    QWidget *linePlotContainer;
    QWidget *histogramPlotContainer;
    QWidget *logContainer;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuView;
    QMenu *menuHelp;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1200, 800);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        toolbarWidget = new QWidget(centralwidget);
        toolbarWidget->setObjectName(QString::fromUtf8("toolbarWidget"));
        horizontalLayout = new QHBoxLayout(toolbarWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        connectButton = new QPushButton(toolbarWidget);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));

        horizontalLayout->addWidget(connectButton);

        disconnectButton = new QPushButton(toolbarWidget);
        disconnectButton->setObjectName(QString::fromUtf8("disconnectButton"));

        horizontalLayout->addWidget(disconnectButton);

        line = new QFrame(toolbarWidget);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line);

        startPollingButton = new QPushButton(toolbarWidget);
        startPollingButton->setObjectName(QString::fromUtf8("startPollingButton"));

        horizontalLayout->addWidget(startPollingButton);

        stopPollingButton = new QPushButton(toolbarWidget);
        stopPollingButton->setObjectName(QString::fromUtf8("stopPollingButton"));

        horizontalLayout->addWidget(stopPollingButton);

        line_2 = new QFrame(toolbarWidget);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);

        horizontalLayout->addWidget(line_2);

        startRecordButton = new QPushButton(toolbarWidget);
        startRecordButton->setObjectName(QString::fromUtf8("startRecordButton"));

        horizontalLayout->addWidget(startRecordButton);

        stopRecordButton = new QPushButton(toolbarWidget);
        stopRecordButton->setObjectName(QString::fromUtf8("stopRecordButton"));

        horizontalLayout->addWidget(stopRecordButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        settingsButton = new QPushButton(toolbarWidget);
        settingsButton->setObjectName(QString::fromUtf8("settingsButton"));

        horizontalLayout->addWidget(settingsButton);


        verticalLayout->addWidget(toolbarWidget);

        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        plotWidget = new QWidget(splitter);
        plotWidget->setObjectName(QString::fromUtf8("plotWidget"));
        verticalLayout_2 = new QVBoxLayout(plotWidget);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        linePlotContainer = new QWidget(plotWidget);
        linePlotContainer->setObjectName(QString::fromUtf8("linePlotContainer"));
        linePlotContainer->setMinimumSize(QSize(0, 300));

        verticalLayout_2->addWidget(linePlotContainer);

        histogramPlotContainer = new QWidget(plotWidget);
        histogramPlotContainer->setObjectName(QString::fromUtf8("histogramPlotContainer"));
        histogramPlotContainer->setMinimumSize(QSize(0, 200));

        verticalLayout_2->addWidget(histogramPlotContainer);

        splitter->addWidget(plotWidget);
        logContainer = new QWidget(splitter);
        logContainer->setObjectName(QString::fromUtf8("logContainer"));
        logContainer->setMinimumSize(QSize(300, 0));
        splitter->addWidget(logContainer);

        verticalLayout->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1200, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionAbout);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "GenericScope", nullptr));
        actionExit->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        disconnectButton->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        startPollingButton->setText(QCoreApplication::translate("MainWindow", "Start Polling", nullptr));
        stopPollingButton->setText(QCoreApplication::translate("MainWindow", "Stop Polling", nullptr));
        startRecordButton->setText(QCoreApplication::translate("MainWindow", "Start Record", nullptr));
        stopRecordButton->setText(QCoreApplication::translate("MainWindow", "Stop Record", nullptr));
        settingsButton->setText(QCoreApplication::translate("MainWindow", "Settings", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "Help", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
