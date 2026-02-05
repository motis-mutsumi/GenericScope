#include "monitordatamanager.h"
#include <QDateTime>
#include <QMutexLocker>

// 静态成员初始化
MonitorDataManager* MonitorDataManager::s_instance = nullptr;
QMutex MonitorDataManager::s_mutex;

MonitorDataManager::MonitorDataManager(QObject *parent)
    : QObject(parent)
{
}

MonitorDataManager::~MonitorDataManager()
{
}

MonitorDataManager* MonitorDataManager::instance()
{
    if (s_instance == nullptr) {
        QMutexLocker locker(&s_mutex);
        if (s_instance == nullptr) {
            s_instance = new MonitorDataManager();
        }
    }
    return s_instance;
}

void MonitorDataManager::onProtocolDataParsed(const QMap<QString, double> &fieldValues)
{
    if (fieldValues.isEmpty()) {
        return;
    }

    // 生成时间戳（所有字段使用同一时间戳，表示同一批数据）
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();

    // 遍历所有字段，分发数据
    for (auto it = fieldValues.begin(); it != fieldValues.end(); ++it) {
        const QString &fieldName = it.key();
        double value = it.value();

        // 发送数据更新信号
        emit dataUpdated(fieldName, value, timestamp);
    }
}
