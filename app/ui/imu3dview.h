#ifndef IMU3DVIEW_H
#define IMU3DVIEW_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QCamera>

namespace Qt3DExtras {
    class QPhongMaterial;
    class QCuboidMesh;
    class QConeMesh;
    class QCylinderMesh;
}

/**
 * @brief IMU姿态3D可视化组件
 *
 * 使用Qt3D渲染IMU传感器的实时姿态，支持Roll/Pitch/Yaw角度显示。
 * 包含3D模型、坐标轴和鼠标交互功能。
 */
class IMU3DView : public QWidget
{
    Q_OBJECT
public:
    explicit IMU3DView(QWidget *parent = nullptr);
    ~IMU3DView();

    /**
     * @brief 设置姿态角度
     * @param roll 横滚角（度，-180 ~ 180）
     * @param pitch 俯仰角（度，-90 ~ 90）
     * @param yaw 偏航角（度，0 ~ 360）
     */
    void setAttitude(double roll, double pitch, double yaw);

    /**
     * @brief 重置相机视角到默认位置
     */
    void resetCamera();

    /**
     * @brief 设置场景背景色
     * @param color 背景颜色
     */
    void setBackgroundColor(const QColor &color);

signals:
    /**
     * @brief 姿态角度变化信号
     */
    void angleChanged(double roll, double pitch, double yaw);

private:
    void setupScene();
    void createIMUModel();
    void createCoordinateAxes();
    void createGrid();

private:
    Qt3DExtras::Qt3DWindow *m_view;
    QWidget *m_container;           // 容器窗口指针
    Qt3DCore::QEntity *m_rootEntity;
    Qt3DCore::QEntity *m_imuEntity;
    Qt3DCore::QTransform *m_imuTransform;
    Qt3DRender::QCamera *m_camera;

    // 当前姿态角度
    double m_roll;
    double m_pitch;
    double m_yaw;
};

#endif // IMU3DVIEW_H
