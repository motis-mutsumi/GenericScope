#include "core_pluginsmanager.h"
#include <QDir>
#include <QDebug>

Core_PluginsManager* Core_PluginsManager::s_instance = nullptr;

Core_PluginsManager::Core_PluginsManager(QObject *parent)
    : QObject(parent)
{
}

Core_PluginsManager::~Core_PluginsManager()
{
    unloadAllPlugins();
}

Core_PluginsManager* Core_PluginsManager::instance()
{
    if (!s_instance) {
        s_instance = new Core_PluginsManager();
    }
    return s_instance;
}

int Core_PluginsManager::loadAllPlugins(const QString &pluginDir)
{
    QDir dir(pluginDir);
    if (!dir.exists()) {
        qWarning() << "Plugin directory does not exist:" << pluginDir;
        return 0;
    }

    int loadedCount = 0;
    QStringList filters;
    filters << "*.dll" << "*.so" << "*.dylib";
    dir.setNameFilters(filters);

    for (const QString &fileName : dir.entryList(QDir::Files)) {
        QString pluginPath = dir.absoluteFilePath(fileName);
        if (loadPlugin(pluginPath)) {
            loadedCount++;
        }
    }

    return loadedCount;
}

bool Core_PluginsManager::loadPlugin(const QString &pluginPath)
{
    QPluginLoader *loader = new QPluginLoader(pluginPath);
    QObject *pluginObject = loader->instance();

    if (!pluginObject) {
        qWarning() << "Failed to load plugin:" << pluginPath << loader->errorString();
        delete loader;
        return false;
    }

    CorePluginsBase *plugin = qobject_cast<CorePluginsBase*>(pluginObject);
    if (!plugin) {
        qWarning() << "Plugin does not implement CorePluginsBase interface:" << pluginPath;
        loader->unload();
        delete loader;
        return false;
    }

    QString pluginName = plugin->pluginName();
    if (m_plugins.contains(pluginName)) {
        qWarning() << "Plugin already loaded:" << pluginName;
        loader->unload();
        delete loader;
        return false;
    }

    // 初始化插件
    if (!plugin->initialize()) {
        qWarning() << "Failed to initialize plugin:" << pluginName;
        loader->unload();
        delete loader;
        return false;
    }

    // 连接信号
    connect(plugin, &CorePluginsBase::sendMessageToManager,
            this, &Core_PluginsManager::onPluginMessage);
    connect(plugin, &CorePluginsBase::sendMessageToMain,
            this, &Core_PluginsManager::pluginMessage);

    m_plugins[pluginName] = plugin;
    m_loaders[pluginName] = loader;

    emit pluginLoaded(pluginName);
    qDebug() << "Plugin loaded:" << pluginName << plugin->pluginVersion();

    return true;
}

void Core_PluginsManager::unloadAllPlugins()
{
    for (const QString &pluginName : m_plugins.keys()) {
        unloadPlugin(pluginName);
    }
}

void Core_PluginsManager::unloadPlugin(const QString &pluginName)
{
    if (!m_plugins.contains(pluginName)) {
        return;
    }

    CorePluginsBase *plugin = m_plugins.take(pluginName);
    QPluginLoader *loader = m_loaders.take(pluginName);

    plugin->uninitialize();
    loader->unload();
    delete loader;

    emit pluginUnloaded(pluginName);
    qDebug() << "Plugin unloaded:" << pluginName;
}

CorePluginsBase* Core_PluginsManager::getPlugin(const QString &pluginName)
{
    return m_plugins.value(pluginName, nullptr);
}

QStringList Core_PluginsManager::getPluginNames() const
{
    return m_plugins.keys();
}

void Core_PluginsManager::sendMessageToPlugin(const QString &pluginName, const CorePluginMetaData &data)
{
    CorePluginsBase *plugin = getPlugin(pluginName);
    if (plugin) {
        plugin->handleManagerMessage(data);
    }
}

void Core_PluginsManager::onPluginMessage(const CorePluginMetaData &data)
{
    // 转发插件消息
    emit pluginMessage(data);
}
