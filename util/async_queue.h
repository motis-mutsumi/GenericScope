#ifndef ASYNC_QUEUE_H
#define ASYNC_QUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QAtomicInt>
#include <functional>

/**
 * @brief 异步队列模板类
 *
 * 提供线程安全的异步数据处理队列，支持：
 * - 多工作线程并行处理
 * - 容量管理和溢出处理
 * - 统计信息跟踪
 * - 优先级队列（可选）
 */
template<typename T>
class AsyncQueue : public QObject
{
public:
    using ProcessorFunc = std::function<void(const T&)>;

    /**
     * @brief 构造函数
     * @param workerCount 工作线程数量
     * @param capacity 队列容量（0表示无限制）
     * @param parent 父对象
     */
    explicit AsyncQueue(int workerCount = 1, int capacity = 0, QObject *parent = nullptr)
        : QObject(parent)
        , m_workerCount(workerCount)
        , m_capacity(capacity)
        , m_running(false)
        , m_totalProcessed(0)
        , m_totalDropped(0)
    {
    }

    ~AsyncQueue()
    {
        stop();
    }

    /**
     * @brief 设置处理函数
     * @param processor 处理函数
     */
    void setProcessor(ProcessorFunc processor)
    {
        m_processor = processor;
    }

    /**
     * @brief 启动队列
     */
    void start()
    {
        if (m_running) {
            return;
        }

        m_running = true;

        // 创建工作线程
        for (int i = 0; i < m_workerCount; ++i) {
            QThread *thread = QThread::create([this]() {
                workerThread();
            });
            m_workers.append(thread);
            thread->start();
        }
    }

    /**
     * @brief 停止队列
     */
    void stop()
    {
        if (!m_running) {
            return;
        }

        m_running = false;
        m_condition.wakeAll();

        // 等待所有工作线程结束
        for (QThread *thread : m_workers) {
            thread->wait();
            delete thread;
        }
        m_workers.clear();

        // 清空队列
        QMutexLocker locker(&m_mutex);
        m_queue.clear();
    }

    /**
     * @brief 添加数据到队列
     * @param data 数据
     * @return 是否成功添加
     */
    bool push(const T &data)
    {
        QMutexLocker locker(&m_mutex);

        // 检查容量
        if (m_capacity > 0 && m_queue.size() >= m_capacity) {
            m_totalDropped++;
            return false;
        }

        m_queue.enqueue(data);
        m_condition.wakeOne();
        return true;
    }

    /**
     * @brief 获取队列大小
     * @return 队列中的元素数量
     */
    int size() const
    {
        QMutexLocker locker(&m_mutex);
        return m_queue.size();
    }

    /**
     * @brief 清空队列
     */
    void clear()
    {
        QMutexLocker locker(&m_mutex);
        m_queue.clear();
    }

    /**
     * @brief 获取统计信息
     */
    qint64 totalProcessed() const { return m_totalProcessed; }
    qint64 totalDropped() const { return m_totalDropped; }

private:
    void workerThread()
    {
        while (m_running) {
            T data;
            bool hasData = false;

            {
                QMutexLocker locker(&m_mutex);
                if (m_queue.isEmpty()) {
                    m_condition.wait(&m_mutex, 100);
                }

                if (!m_queue.isEmpty()) {
                    data = m_queue.dequeue();
                    hasData = true;
                }
            }

            if (hasData && m_processor) {
                m_processor(data);
                m_totalProcessed++;
            }
        }
    }

private:
    int m_workerCount;
    int m_capacity;
    QAtomicInt m_running;
    QQueue<T> m_queue;
    mutable QMutex m_mutex;
    QWaitCondition m_condition;
    QList<QThread*> m_workers;
    ProcessorFunc m_processor;
    QAtomicInteger<qint64> m_totalProcessed;
    QAtomicInteger<qint64> m_totalDropped;
};

#endif // ASYNC_QUEUE_H
