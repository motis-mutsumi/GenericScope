#include "imu3dview.h"
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DRender/QPointLight>
#include <QVBoxLayout>

IMU3DView::IMU3DView(QWidget *parent)
    : QWidget(parent)
    , m_view(nullptr)
    , m_container(nullptr)
    , m_rootEntity(nullptr)
    , m_imuEntity(nullptr)
    , m_imuTransform(nullptr)
    , m_camera(nullptr)
    , m_roll(0.0)
    , m_pitch(0.0)
    , m_yaw(0.0)
{
    // 创建Qt3D窗口（QWindow对象，无法设置父对象）
    m_view = new Qt3DExtras::Qt3DWindow();
    m_view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x1e1e1e)));

    // 将Qt3D窗口嵌入到QWidget容器
    m_container = QWidget::createWindowContainer(m_view, this);

    // 设置布局
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_container);

    setupScene();
}

IMU3DView::~IMU3DView()
{
    // 显式删除Qt3D窗口（QWindow对象需要手动管理）
    if (m_view) {
        delete m_view;
        m_view = nullptr;
    }
    // m_container 作为 QWidget 子对象会自动删除
    // m_rootEntity 等 Qt3D 对象通过父子关系自动管理
}

void IMU3DView::setupScene()
{
    // 创建根实体
    m_rootEntity = new Qt3DCore::QEntity();

    // 设置相机
    m_camera = m_view->camera();
    m_camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    m_camera->setPosition(QVector3D(0, 5, 10));
    m_camera->setViewCenter(QVector3D(0, 0, 0));

    // 添加相机控制器（支持鼠标交互）
    Qt3DExtras::QOrbitCameraController *camController =
        new Qt3DExtras::QOrbitCameraController(m_rootEntity);
    camController->setLinearSpeed(50.0f);
    camController->setLookSpeed(180.0f);
    camController->setCamera(m_camera);

    // 添加光源
    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(m_rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1.0f);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(QVector3D(0, 10, 10));
    lightEntity->addComponent(light);
    lightEntity->addComponent(lightTransform);

    // 创建IMU模型
    createIMUModel();

    // 创建坐标轴
    createCoordinateAxes();

    // 创建参考网格
    createGrid();

    // 设置场景
    m_view->setRootEntity(m_rootEntity);
}

void IMU3DView::createIMUModel()
{
    // 创建IMU实体（使用长方体代表IMU）
    m_imuEntity = new Qt3DCore::QEntity(m_rootEntity);

    // 创建长方体网格（模拟IMU传感器）
    Qt3DExtras::QCuboidMesh *mesh = new Qt3DExtras::QCuboidMesh();
    mesh->setXExtent(2.0f);  // 长
    mesh->setYExtent(0.5f);  // 高
    mesh->setZExtent(1.0f);  // 宽

    // 创建材质（蓝色金属材质）
    Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
    material->setDiffuse(QColor(QRgb(0x3498db)));
    material->setSpecular(QColor(QRgb(0xffffff)));
    material->setShininess(50.0f);

    // 创建变换组件
    m_imuTransform = new Qt3DCore::QTransform();

    // 添加组件到实体
    m_imuEntity->addComponent(mesh);
    m_imuEntity->addComponent(material);
    m_imuEntity->addComponent(m_imuTransform);
}

void IMU3DView::createCoordinateAxes()
{
    // 辅助函数：创建单个坐标轴
    auto createAxis = [this](const QVector3D &direction, const QColor &color,
                            float rotationAngle, const QVector3D &rotationAxis) {
        // 创建轴实体
        Qt3DCore::QEntity *axisEntity = new Qt3DCore::QEntity(m_rootEntity);

        // 轴线（圆柱体）
        Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
        cylinder->setRadius(0.05f);
        cylinder->setLength(3.0f);

        // 材质
        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(color);

        // 变换
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
        if (rotationAngle != 0.0f) {
            transform->setRotation(QQuaternion::fromAxisAndAngle(rotationAxis, rotationAngle));
        }
        transform->setTranslation(direction * 1.5f);

        axisEntity->addComponent(cylinder);
        axisEntity->addComponent(material);
        axisEntity->addComponent(transform);

        // 创建箭头（圆锥）
        Qt3DCore::QEntity *arrowEntity = new Qt3DCore::QEntity(m_rootEntity);
        Qt3DExtras::QConeMesh *cone = new Qt3DExtras::QConeMesh();
        cone->setBottomRadius(0.1f);
        cone->setLength(0.3f);

        Qt3DExtras::QPhongMaterial *arrowMaterial = new Qt3DExtras::QPhongMaterial();
        arrowMaterial->setDiffuse(color);

        Qt3DCore::QTransform *arrowTransform = new Qt3DCore::QTransform();
        if (rotationAngle != 0.0f) {
            arrowTransform->setRotation(QQuaternion::fromAxisAndAngle(rotationAxis, rotationAngle));
        }
        arrowTransform->setTranslation(direction * 3.15f);

        arrowEntity->addComponent(cone);
        arrowEntity->addComponent(arrowMaterial);
        arrowEntity->addComponent(arrowTransform);
    };

    // X轴 - 红色（沿X轴方向，需要绕Z轴旋转90度）
    createAxis(QVector3D(1, 0, 0), Qt::red, 90.0f, QVector3D(0, 0, 1));

    // Y轴 - 绿色（沿Y轴方向，默认方向）
    createAxis(QVector3D(0, 1, 0), Qt::green, 0.0f, QVector3D(0, 0, 0));

    // Z轴 - 蓝色（沿Z轴方向，需要绕X轴旋转90度）
    createAxis(QVector3D(0, 0, 1), Qt::blue, 90.0f, QVector3D(1, 0, 0));
}

void IMU3DView::createGrid()
{
    // 网格实现（简化版：暂不实现，后续可添加）
    // 可以使用QGeometry和QGeometryRenderer创建网格线
}

void IMU3DView::setAttitude(double roll, double pitch, double yaw)
{
    m_roll = roll;
    m_pitch = pitch;
    m_yaw = yaw;

    if (!m_imuTransform) {
        return;
    }

    // 欧拉角转四元数
    // 注意：Qt3D使用的是Z-Y-X顺序（Yaw-Pitch-Roll）
    // Roll: 绕X轴旋转
    // Pitch: 绕Y轴旋转
    // Yaw: 绕Z轴旋转
    QQuaternion qRoll = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), static_cast<float>(roll));
    QQuaternion qPitch = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), static_cast<float>(pitch));
    QQuaternion qYaw = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), static_cast<float>(yaw));

    // 组合旋转：Yaw * Pitch * Roll
    QQuaternion rotation = qYaw * qPitch * qRoll;
    m_imuTransform->setRotation(rotation);

    emit angleChanged(roll, pitch, yaw);
}

void IMU3DView::resetCamera()
{
    if (m_camera) {
        m_camera->setPosition(QVector3D(0, 5, 10));
        m_camera->setViewCenter(QVector3D(0, 0, 0));
    }
}
