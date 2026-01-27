#include "protocolmanager.h"
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>
#include <QDebug>

ProtocolManager* ProtocolManager::instance()
{
    static ProtocolManager instance;
    return &instance;
}

ProtocolManager::ProtocolManager(QObject *parent)
    : QObject(parent)
{
}

ProtocolManager::~ProtocolManager()
{
}

bool ProtocolManager::loadProtocol(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    ProtocolConfig config = ProtocolConfig::loadFromFile(filePath);
    if (config.name.isEmpty()) {
        qWarning() << "Failed to load protocol from:" << filePath;
        return false;
    }

    m_protocols[config.name] = config;
    emit protocolAdded(config.name);

    qDebug() << "Loaded protocol:" << config.name << "from" << filePath;
    return true;
}

int ProtocolManager::loadProtocols(const QString &dirPath)
{
    QDir dir(dirPath);
    if (!dir.exists()) {
        qWarning() << "Protocol directory does not exist:" << dirPath;
        return 0;
    }

    QStringList filters;
    filters << "*.json";
    QFileInfoList fileList = dir.entryInfoList(filters, QDir::Files);

    int count = 0;
    for (const QFileInfo &fileInfo : fileList) {
        if (loadProtocol(fileInfo.absoluteFilePath())) {
            count++;
        }
    }

    qDebug() << "Loaded" << count << "protocols from" << dirPath;
    return count;
}

bool ProtocolManager::saveProtocol(const QString &name, const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    if (!m_protocols.contains(name)) {
        qWarning() << "Protocol not found:" << name;
        return false;
    }

    bool success = m_protocols[name].saveToFile(filePath);
    if (success) {
        qDebug() << "Saved protocol:" << name << "to" << filePath;
    } else {
        qWarning() << "Failed to save protocol:" << name;
    }

    return success;
}

void ProtocolManager::addProtocol(const ProtocolConfig &config)
{
    QMutexLocker locker(&m_mutex);

    m_protocols[config.name] = config;
    emit protocolAdded(config.name);

    qDebug() << "Added protocol:" << config.name;
}

void ProtocolManager::removeProtocol(const QString &name)
{
    QMutexLocker locker(&m_mutex);

    if (m_protocols.remove(name) > 0) {
        emit protocolRemoved(name);
        qDebug() << "Removed protocol:" << name;

        // 如果删除的是当前协议，清空当前协议
        if (m_currentProtocol == name) {
            m_currentProtocol.clear();
            m_currentParser.reset();
        }
    }
}

bool ProtocolManager::hasProtocol(const QString &name) const
{
    QMutexLocker locker(&m_mutex);
    return m_protocols.contains(name);
}

ProtocolConfig ProtocolManager::getProtocol(const QString &name) const
{
    QMutexLocker locker(&m_mutex);
    return m_protocols.value(name);
}

QStringList ProtocolManager::getProtocolNames() const
{
    QMutexLocker locker(&m_mutex);
    return m_protocols.keys();
}

QSharedPointer<ProtocolParser> ProtocolManager::createParser(const QString &name)
{
    QMutexLocker locker(&m_mutex);

    if (!m_protocols.contains(name)) {
        qWarning() << "Protocol not found:" << name;
        return QSharedPointer<ProtocolParser>();
    }

    return QSharedPointer<ProtocolParser>(new ProtocolParser(m_protocols[name]));
}

void ProtocolManager::setCurrentProtocol(const QString &name)
{
    QMutexLocker locker(&m_mutex);

    if (!m_protocols.contains(name)) {
        qWarning() << "Protocol not found:" << name;
        return;
    }

    m_currentProtocol = name;
    m_currentParser = QSharedPointer<ProtocolParser>(new ProtocolParser(m_protocols[name]));

    emit currentProtocolChanged(name);
    qDebug() << "Current protocol changed to:" << name;
}

QSharedPointer<ProtocolParser> ProtocolManager::getCurrentParser()
{
    QMutexLocker locker(&m_mutex);
    return m_currentParser;
}
