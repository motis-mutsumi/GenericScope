#ifndef ALGORITHMPROCESSOR_H
#define ALGORITHMPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QVector>

/**
 * @brief 算法处理器类
 *
 * 提供各种数据处理算法，包括：
 * - 信号处理
 * - 数据分析
 * - 图像处理（如果需要）
 * - 统计计算
 */
class AlgorithmProcessor : public QObject
{
    Q_OBJECT

public:
    explicit AlgorithmProcessor(QObject *parent = nullptr);
    ~AlgorithmProcessor();

    /**
     * @brief 数据滤波
     * @param data 输入数据
     * @param filterType 滤波器类型（0=均值，1=中值，2=高斯）
     * @param kernelSize 滤波器大小
     * @return 滤波后的数据
     */
    static QVector<double> filterData(const QVector<double> &data, int filterType, int kernelSize);

    /**
     * @brief 数据平滑
     * @param data 输入数据
     * @param windowSize 窗口大小
     * @return 平滑后的数据
     */
    static QVector<double> smoothData(const QVector<double> &data, int windowSize);

    /**
     * @brief 计算统计信息
     * @param data 输入数据
     * @param mean 输出：均值
     * @param stdDev 输出：标准差
     * @param min 输出：最小值
     * @param max 输出：最大值
     */
    static void calculateStatistics(const QVector<double> &data,
                                    double &mean, double &stdDev,
                                    double &min, double &max);

    /**
     * @brief FFT 变换
     * @param data 输入数据
     * @return FFT 结果（幅度谱）
     */
    static QVector<double> fftTransform(const QVector<double> &data);

    /**
     * @brief 峰值检测
     * @param data 输入数据
     * @param threshold 阈值
     * @return 峰值位置索引
     */
    static QVector<int> detectPeaks(const QVector<double> &data, double threshold);

signals:
    void processingProgress(int percentage);
    void processingFinished();
    void processingError(const QString &error);
};

#endif // ALGORITHMPROCESSOR_H
