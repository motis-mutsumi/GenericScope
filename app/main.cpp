#include "ui/mainwindow.h"
#include "config/config.h"
#include "common_component/log/logmanager.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("GenericScope");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("GenericScope");

    // 单实例检查
    QSharedMemory sharedMemory("GenericScope_SingleInstance");
    if (!sharedMemory.create(1)) {
        QMessageBox::warning(nullptr, "Warning",
                           "GenericScope is already running!");
        return 0;
    }

    // 启用高 DPI 缩放
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // 加载配置
    Config::instance()->load();

    // 初始化日志系统
    LogManager::instance()->initialize(Config::instance()->dirs.logDir, true, true);
    LogManager::instance()->setLogLevel(LogManager::Info);

    // 创建主窗口
    MainWindow mainWindow;
    mainWindow.show();

    LOG_INFO("GenericScope started");

    int result = app.exec();

    // 保存配置
    Config::instance()->save();

    LOG_INFO("GenericScope stopped");

    return result;
}
