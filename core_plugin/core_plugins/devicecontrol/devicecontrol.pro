TEMPLATE = lib
CONFIG += plugin
TARGET = devicecontrol

include(../../../global.pri)

# 输出目录
DESTDIR = $$PWD/../../../Bin/$$ARCH/$$BUILD_MODE/core_plugins

# 包含路径
INCLUDEPATH += $$PWD/../../core_plugins_manager

# 头文件
HEADERS += \
    devicecontrolplugin.h \
    devicecontrolwidget.h

# 源文件
SOURCES += \
    devicecontrolplugin.cpp \
    devicecontrolwidget.cpp

# UI文件
FORMS += \
    devicecontrolwidget.ui

# 元数据
OTHER_FILES += \
    metadata.json
