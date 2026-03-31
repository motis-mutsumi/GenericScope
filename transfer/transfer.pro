QT += serialport serialbus network core

TEMPLATE = lib
CONFIG += staticlib
DEFINES += TRANSFER_LIBRARY

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    scopecontroltransfer.cpp \
    scopetransferbasic.cpp \
    scopetransfermanager.cpp

HEADERS += \
    scopecontroltransfer.h \
    scopetransferbasic.h \
    transfer_global.h \
    scopetransfermanager.h

include(../global.pri)

include(uart/uart.pri)
include(tcp/tcp.pri)
include(modbus/modbus.pri)
include(udp/udp.pri)

INCLUDEPATH += $$PWD/../util
INCLUDEPATH += $$PWD/../libs/spdlog
INCLUDEPATH += $$PWD/../libs
INCLUDEPATH += $$PWD/../common_component/log
INCLUDEPATH += $$PWD/../protocol

# 库依赖配置
# 注意：由于使用了 ordered 构建，log 库会在 transfer 之前编译
# 在 shadow build 中，库文件位于构建目录的对应位置
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../common_component/log/release/ -llog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../common_component/log/debug/ -llog
else:unix: LIBS += -L$$OUT_PWD/../common_component/log/ -llog

# protocol 模块依赖 (新增)
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../protocol/release/ -lprotocol
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../protocol/debug/ -lprotocol
else:unix: LIBS += -L$$OUT_PWD/../protocol/ -lprotocol

# 同时保留原有的库路径配置（用于非 shadow build）
LIBS += -L$$PWD/../Bin/$$ARCH/$$BUILD_MODE

TARGET = transfer

DESTDIR = $$PWD/../Bin/$$ARCH/$$BUILD_MODE

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target


