#ifndef DATARECORDER_H
#define DATARECORDER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QVariantMap>
#include <QVector>

/**
 * @brief 数据录制器
 *
 * 提供传感器数据的录制和保存功能
 */
class DataRecorder : public QObject
{
    Q_OBJECT

public:
    enum RecordFormat {
        CSV,        // CSV 格式
        JSON,       // JSON 格式
        Binary      // 二进制格式
    };
    Q_ENUM(RecordFormat)

    explicit DataRecorder(QObject *parent = nullptr);
    ~DataRecorder();

    /**
     * @brief 开始录制
     * @param fileName 文件名
     * @param format 文件格式
     * @param metadata 元数据
     * @return 是否成功
     */
    bool startRecording(const QString &fileName, RecordFormat format, const QVariantMap &metadata = QVariantMap());

    /**
     * @brief 停止录制
     */
    void stopRecording();

    /**
     * @brief 检查是否正在录制
     * @return 录制状态
     */
    bool isRecording() const { return m_recording; }

    /**
     * @brief 记录数据点
     * @param timestamp 时间戳
     * @param data 数据
     */
    void recordData(qint64 timestamp, const QVariantMap &data);

    /**
     * @brief 记录数据点（自动时间戳）
     * @param data 数据
     */
    void recordData(const QVariantMap &data);

    /**
     * @brief 获取录制的数据点数量
     * @return 数据点数量
     */
    int recordedCount() const { return m_recordedCount; }

signals:
    /**
     * @brief 录制开始信号
     */
    void recordingStarted();

    /**
     * @brief 录制停止信号
     */
    void recordingStopped();

    /**
     * @brief 数据记录信号
     * @param count 已记录的数据点数量
     */
    void dataRecorded(int count);

private:
    void writeCSVHeader();
    void writeCSVData(qint64 timestamp, const QVariantMap &data);
    void writeJSONData(qint64 timestamp, const QVariantMap &data);
    void writeBinaryData(qint64 timestamp, const QVariantMap &data);

private:
    bool m_recording;
    RecordFormat m_format;
    QFile *m_file;
    QTextStream *m_stream;
    QVariantMap m_metadata;
    int m_recordedCount;
    QStringList m_dataKeys;
};

#endif // DATARECORDER_H
