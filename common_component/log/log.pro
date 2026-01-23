TEMPLATE = lib
TARGET = log
CONFIG += staticlib

QT += core gui widgets

include(../../global.pri)

HEADERS += \
    logmanager.h \
    logwidget.h

SOURCES += \
    logmanager.cpp \
    logwidget.cpp

# spdlog 支持（如果使用）
# INCLUDEPATH += ../../libs/spdlog/include
