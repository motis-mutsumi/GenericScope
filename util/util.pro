TEMPLATE = lib
TARGET = util
CONFIG += staticlib

include(../global.pri)

HEADERS += \
    async_queue.h \
    timehelper.h

SOURCES += \
    timehelper.cpp
