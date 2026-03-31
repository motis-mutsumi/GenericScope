TEMPLATE = app
TARGET = GenericScope

QT += core gui widgets printsupport serialport network

include(../global.pri)

# 链接库
LIBS += -L$$PWD/../Bin/$$ARCH/$$BUILD_MODE
LIBS += -lalgorithm -lutil -lprotocol -ltransfer -lplot -llog -lrecord -lcore_plugins_manager

HEADERS += \
    ui/mainwindow.h \
    ui/commandsettingsdialog.h \
    ui/deviceconfigdialog.h \
    ui/newprotocoldialog.h \
    ui/protocoltypeconverter.h \
    ui/protocoltestdialog.h \
    ui/protocolaigenerator.h \
    ui/protocolcligenerator.h \
    ui/aiprotocolinputdialog.h \
    ui/imu3dview.h \
    config/config.h \
    device/devicemanager.h

SOURCES += \
    main.cpp \
    ui/mainwindow.cpp \
    ui/commandsettingsdialog.cpp \
    ui/deviceconfigdialog.cpp \
    ui/newprotocoldialog.cpp \
    ui/protocoltypeconverter.cpp \
    ui/protocoltestdialog.cpp \
    ui/protocolaigenerator.cpp \
    ui/protocolcligenerator.cpp \
    ui/aiprotocolinputdialog.cpp \
    ui/imu3dview.cpp \
    config/config.cpp \
    device/devicemanager.cpp

FORMS += \
    ui/mainwindow.ui \
    ui/deviceconfigdialog.ui \
    ui/commandsettingsdialog.ui \
    ui/aiprotocolinputdialog.ui \
    ui/newprotocoldialog.ui \
    ui/protocoltestdialog.ui

RESOURCES += \
    resources.qrc

# 静态库宏，避免 __declspec(dllimport) 导致链接失败
DEFINES += TRANSFER_LIBRARY

# 包含路径
INCLUDEPATH += \
    $$PWD/../algorithm \
    $$PWD/../util \
    $$PWD/../protocol \
    $$PWD/../transfer \
    $$PWD/../common_component/plot \
    $$PWD/../common_component/log \
    $$PWD/../common_component/record \
    $$PWD/../core_plugin/core_plugins_manager

# Windows 图标
win32:RC_FILE = app.rc
