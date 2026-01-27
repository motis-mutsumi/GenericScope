TEMPLATE = app
TARGET = GenericScope

include(../global.pri)

# 链接库
LIBS += -L$$PWD/../Bin/$$ARCH/$$BUILD_MODE
LIBS += -lalgorithm -lutil -lprotocol -ltransfer -lplot -llog -lrecord -lcore_plugins_manager

HEADERS += \
    ui/mainwindow.h \
    ui/commandsettingsdialog.h \
    ui/protocoltypeconverter.h \
    ui/protocoltestdialog.h \
    config/config.h \
    device/devicemanager.h

SOURCES += \
    main.cpp \
    ui/mainwindow.cpp \
    ui/commandsettingsdialog.cpp \
    ui/protocoltypeconverter.cpp \
    ui/protocoltestdialog.cpp \
    config/config.cpp \
    device/devicemanager.cpp

FORMS += \
    ui/mainwindow.ui

RESOURCES += \
    resources.qrc

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
