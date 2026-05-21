#include "datarecorder.h"
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDataStream>

DataRecorder::DataRecorder(QObject *parent)
    : QObject(parent)
    , m_recording(false)
    , m_format(CSV)
    , m_file(nullptr)
    , m_stream(nullptr)
    , m_recordedCount(0)
{
}

DataRecorder::~DataRecorder()
{
    stopRecording();
}

bool DataRecorder::startRecording(const QString &fileName, RecordFormat format, const QVariantMap &metadata)
{
    if (m_recording) {
        stopRecording();
    }

    m_format = format;
    m_metadata = metadata;
    m_recordedCount = 0;
    m_dataKeys.clear();

    m_file = new QFile(fileName);
    if (!m_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        delete m_file;
        m_file = nullptr;
        return false;
    }

    if (format == CSV || format == JSON) {
        m_stream = new QTextStream(m_file);
    }

    // 写入文件头
    if (format == CSV) {
        writeCSVHeader();
    } else if (format == JSON) {
        *m_stream << "[\n";
    }

    m_recording = true;
    emit recordingStarted();
    return true;
}

void DataRecorder::stopRecording()
{
    if (!m_recording) {
        return;
    }

    // 写入文件尾
    if (m_format == JSON && m_stream) {
        *m_stream << "\n]\n";
    }

    if (m_stream) {
        delete m_stream;
        m_stream = nullptr;
    }

    if (m_file) {
        m_file->close();
        delete m_file;
        m_file = nullptr;
    }

    m_recording = false;
    emit recordingStopped();
}

void DataRecorder::recordData(qint64 timestamp, const QVariantMap &data)
{
    if (!m_recording) {
        return;
    }

    switch (m_format) {
    case CSV:
        writeCSVData(timestamp, data);
        break;
    case JSON:
        writeJSONData(timestamp, data);
        break;
    case Binary:
        writeBinaryData(timestamp, data);
        break;
    }

    m_recordedCount++;
    emit dataRecorded(m_recordedCount);
}

void DataRecorder::recordData(const QVariantMap &data)
{
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch();
    recordData(timestamp, data);
}

void DataRecorder::writeCSVHeader()
{
    if (!m_stream) return;

    // 写入元数据
    for (auto it = m_metadata.begin(); it != m_metadata.end(); ++it) {
        *m_stream << "# " << it.key() << ": " << it.value().toString() << "\n";
    }

    // 写入列标题
    *m_stream << "Timestamp";
    // 数据键将在第一次记录数据时确定
    *m_stream << "\n";
}

void DataRecorder::writeCSVData(qint64 timestamp, const QVariantMap &data)
{
    if (!m_stream) return;

    // 第一次记录数据时，确定数据键并写入列标题
    if (m_dataKeys.isEmpty()) {
        m_dataKeys = data.keys();
        m_stream->seek(m_stream->pos() - 1);  // 回到上一行
        for (const QString &key : m_dataKeys) {
            *m_stream << "," << key;
        }
        *m_stream << "\n";
    }

    // 写入数据
    *m_stream << timestamp;
    for (const QString &key : m_dataKeys) {
        *m_stream << "," << data.value(key).toString();
    }
    *m_stream << "\n";
    m_stream->flush();
}

void DataRecorder::writeJSONData(qint64 timestamp, const QVariantMap &data)
{
    if (!m_stream) return;

    if (m_recordedCount > 0) {
        *m_stream << ",\n";
    }

    QJsonObject obj;
    obj["timestamp"] = timestamp;
    for (auto it = data.begin(); it != data.end(); ++it) {
        obj[it.key()] = QJsonValue::fromVariant(it.value());
    }

    QJsonDocument doc(obj);
    *m_stream << "  " << doc.toJson(QJsonDocument::Compact);
    m_stream->flush();
}

void DataRecorder::writeBinaryData(qint64 timestamp, const QVariantMap &data)
{
    if (!m_file) return;

    QDataStream stream(m_file);
    stream << timestamp;
    stream << data;
}
