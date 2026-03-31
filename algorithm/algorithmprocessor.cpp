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
    if (data.isEmpty()) {
        return QVector<double>();
    }

    int n = data.size();

    // 填充到2的幂
    int fftSize = 1;
    while (fftSize < n) {
        fftSize <<= 1;
    }

    // 创建复数数组（实部和虚部）
    QVector<double> real(fftSize, 0.0);
    QVector<double> imag(fftSize, 0.0);

    // 复制输入数据
    for (int i = 0; i < n; ++i) {
        real[i] = data[i];
    }

    // 位反转排序
    int bits = 0;
    int temp = fftSize;
    while (temp > 1) {
        bits++;
        temp >>= 1;
    }

    for (int i = 0; i < fftSize; ++i) {
        int j = 0;
        int temp = i;
        for (int b = 0; b < bits; ++b) {
            j = (j << 1) | (temp & 1);
            temp >>= 1;
        }
        if (j > i) {
            qSwap(real[i], real[j]);
            qSwap(imag[i], imag[j]);
        }
    }

    // Cooley-Tukey FFT（迭代实现）
    for (int len = 2; len <= fftSize; len <<= 1) {
        double angle = -2.0 * M_PI / len;
        double wlenReal = qCos(angle);
        double wlenImag = qSin(angle);

        for (int i = 0; i < fftSize; i += len) {
            double wReal = 1.0;
            double wImag = 0.0;

            for (int j = 0; j < len / 2; ++j) {
                int idx1 = i + j;
                int idx2 = i + j + len / 2;

                double tReal = wReal * real[idx2] - wImag * imag[idx2];
                double tImag = wReal * imag[idx2] + wImag * real[idx2];

                real[idx2] = real[idx1] - tReal;
                imag[idx2] = imag[idx1] - tImag;
                real[idx1] = real[idx1] + tReal;
                imag[idx1] = imag[idx1] + tImag;

                double wTempReal = wReal * wlenReal - wImag * wlenImag;
                wImag = wReal * wlenImag + wImag * wlenReal;
                wReal = wTempReal;
            }
        }
    }

    // 计算幅度谱
    QVector<double> magnitude(fftSize / 2);
    for (int i = 0; i < fftSize / 2; ++i) {
        magnitude[i] = qSqrt(real[i] * real[i] + imag[i] * imag[i]);
    }

    return magnitude;
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
