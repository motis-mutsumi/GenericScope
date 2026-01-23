#include "algorithmprocessor.h"
#include <QtMath>
#include <algorithm>

AlgorithmProcessor::AlgorithmProcessor(QObject *parent)
    : QObject(parent)
{
}

AlgorithmProcessor::~AlgorithmProcessor()
{
}

QVector<double> AlgorithmProcessor::filterData(const QVector<double> &data, int filterType, int kernelSize)
{
    if (data.isEmpty() || kernelSize < 1) {
        return data;
    }

    QVector<double> result;
    result.reserve(data.size());

    int halfKernel = kernelSize / 2;

    for (int i = 0; i < data.size(); ++i) {
        QVector<double> window;
        for (int j = -halfKernel; j <= halfKernel; ++j) {
            int index = i + j;
            if (index >= 0 && index < data.size()) {
                window.append(data[index]);
            }
        }

        double value = 0.0;
        if (filterType == 0) {
            // 均值滤波
            double sum = 0.0;
            for (double v : window) {
                sum += v;
            }
            value = sum / window.size();
        } else if (filterType == 1) {
            // 中值滤波
            std::sort(window.begin(), window.end());
            value = window[window.size() / 2];
        } else {
            // 默认返回原值
            value = data[i];
        }

        result.append(value);
    }

    return result;
}

QVector<double> AlgorithmProcessor::smoothData(const QVector<double> &data, int windowSize)
{
    return filterData(data, 0, windowSize);
}

void AlgorithmProcessor::calculateStatistics(const QVector<double> &data,
                                             double &mean, double &stdDev,
                                             double &min, double &max)
{
    if (data.isEmpty()) {
        mean = stdDev = min = max = 0.0;
        return;
    }

    // 计算均值
    double sum = 0.0;
    min = max = data[0];
    for (double value : data) {
        sum += value;
        if (value < min) min = value;
        if (value > max) max = value;
    }
    mean = sum / data.size();

    // 计算标准差
    double variance = 0.0;
    for (double value : data) {
        double diff = value - mean;
        variance += diff * diff;
    }
    stdDev = qSqrt(variance / data.size());
}

QVector<double> AlgorithmProcessor::fftTransform(const QVector<double> &data)
{
    // TODO: 实现 FFT 算法或集成第三方库（如 FFTW）
    // 这里返回空向量作为占位符
    Q_UNUSED(data);
    return QVector<double>();
}

QVector<int> AlgorithmProcessor::detectPeaks(const QVector<double> &data, double threshold)
{
    QVector<int> peaks;

    if (data.size() < 3) {
        return peaks;
    }

    for (int i = 1; i < data.size() - 1; ++i) {
        if (data[i] > threshold &&
            data[i] > data[i - 1] &&
            data[i] > data[i + 1]) {
            peaks.append(i);
        }
    }

    return peaks;
}
