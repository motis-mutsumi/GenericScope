# GenericScope 全局配置文件

# Qt 模块
QT += core gui widgets network serialport

# C++ 标准
CONFIG += c++17

# 编译器配置
QMAKE_CXXFLAGS += /utf-8
DEFINES += QT_DEPRECATED_WARNINGS

# 架构检测
contains(QT_ARCH, i386) {
    ARCH = x86
} else {
    ARCH = x64
}

# 构建模式
CONFIG(debug, debug|release) {
    BUILD_MODE = Debug
    DEFINES += _DEBUG
} else {
    BUILD_MODE = Release
    DEFINES += NDEBUG
}

# 输出目录配置
DESTDIR = $$PWD/Bin/$$ARCH/$$BUILD_MODE
MOC_DIR = $$PWD/temp/$$TARGET/$$ARCH/$$BUILD_MODE/moc
OBJECTS_DIR = $$PWD/temp/$$TARGET/$$ARCH/$$BUILD_MODE/obj
RCC_DIR = $$PWD/temp/$$TARGET/$$ARCH/$$BUILD_MODE/rcc
UI_DIR = $$PWD/temp/$$TARGET/$$ARCH/$$BUILD_MODE/ui

# 包含路径
INCLUDEPATH += $$PWD

# 库路径
LIBS += -L$$PWD/Bin/$$ARCH/$$BUILD_MODE

# Windows 特定配置
win32 {
    QMAKE_LFLAGS += /INCREMENTAL:NO
    DEFINES += WIN32 _WINDOWS
}

# 禁用警告
DEFINES += _CRT_SECURE_NO_WARNINGS
