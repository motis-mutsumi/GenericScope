TEMPLATE = lib
TARGET = plot
CONFIG += staticlib

QT += core gui widgets printsupport

include(../../global.pri)

HEADERS += \
    basicplot.h \
    lineplot.h \
    histogramplot.h \
    monitorpanel.h \
    monitorchart.h \
    monitorconfigdialog.h \
    monitordatamanager.h \
    qcustomplot.h

SOURCES += \
    basicplot.cpp \
    lineplot.cpp \
    histogramplot.cpp \
    monitorpanel.cpp \
    monitorchart.cpp \
    monitorconfigdialog.cpp \
    monitordatamanager.cpp \
    qcustomplot.cpp
