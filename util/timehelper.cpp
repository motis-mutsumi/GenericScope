#include "timehelper.h"
#include <QDateTime>

TimeHelper::TimeHelper(QObject *parent)
    : QObject(parent)
{
}

TimeHelper::~TimeHelper()
{
}

void TimeHelper::start()
{
    m_timer.start();
}

qint64 TimeHelper::elapsed() const
{
    return m_timer.elapsed();
}

qint64 TimeHelper::restart()
{
    return m_timer.restart();
}

QString TimeHelper::formatTimestamp(qint64 timestamp, const QString &format)
{
    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timestamp);
    return dateTime.toString(format);
}

qint64 TimeHelper::currentTimestamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

QString TimeHelper::formatDuration(qint64 milliseconds)
{
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;

    seconds %= 60;
    minutes %= 60;

    QString result;
    if (hours > 0) {
        result += QString("%1h ").arg(hours);
    }
    if (minutes > 0) {
        result += QString("%1m ").arg(minutes);
    }
    result += QString("%1s").arg(seconds);

    return result.trimmed();
}
