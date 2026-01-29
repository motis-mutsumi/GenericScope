TEMPLATE = lib
CONFIG += plugin
TARGET = datamonitor

include(../../../global.pri)

# 输出目录
DESTDIR = $$PWD/../../../Bin/$$ARCH/$$BUILD_MODE/core_plugins

# 包含路径
INCLUDEPATH += $$PWD/../../core_plugins_manager

# 头文件
HEADERS += \
    datamonitorplugin.h \
    datamonitorwidget.h

# 源文件
SOURCES += \
    datamonitorplugin.cpp \
    datamonitorwidget.cpp

# UI文件
FORMS += \
    datamonitorwidget.ui

# 元数据
OTHER_FILES += \
    metadata.json
