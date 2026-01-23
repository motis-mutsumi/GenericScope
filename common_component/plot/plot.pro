TEMPLATE = lib
TARGET = plot
CONFIG += staticlib

QT += core gui widgets

include(../../global.pri)

HEADERS += \
    basicplot.h \
    lineplot.h \
    histogramplot.h

SOURCES += \
    basicplot.cpp \
    lineplot.cpp \
    histogramplot.cpp

# QCustomPlot 支持（如果使用）
# HEADERS += qcustomplot.h
# SOURCES += qcustomplot.cpp
