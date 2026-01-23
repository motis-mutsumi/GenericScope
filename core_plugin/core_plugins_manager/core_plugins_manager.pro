TEMPLATE = lib
TARGET = core_plugins_manager
CONFIG += staticlib

include(../../global.pri)

HEADERS += \
    core_pluginsbase.h \
    core_pluginsmanager.h \
    core_pluginmetadata.h

SOURCES += \
    core_pluginsmanager.cpp
