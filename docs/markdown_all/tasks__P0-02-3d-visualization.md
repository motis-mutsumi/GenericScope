# P0-02: 3D姿态可视化实现

**优先级**: P0（高优先级）
**预计工作量**: 2-3天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

在主窗口中间面板实现IMU姿态的3D可视化，实时显示Roll/Pitch/Yaw姿态角度。

### 当前状态
- ✅ 主窗口UI已预留3D可视化区域
- ✅ Roll/Pitch/Yaw角度数据已解析并显示
- ❌ **3D可视化未实现**（当前区域为空白）

### 任务目标
1. 选择并集成3D渲染库（Qt3D 或 OpenGL）
2. 创建3D模型（飞机或立方体代表IMU）
3. 绑定姿态数据到3D模型旋转
4. 添加坐标轴和参考网格
5. 支持鼠标交互（旋转、缩放视角）

---

## 🎯 验收标准

### 功能验收
- [ ] 3D场景正常渲染，无闪烁
- [ ] 3D模型根据Roll/Pitch/Yaw实时旋转
- [ ] 坐标轴标注清晰（X/Y/Z轴，不同颜色）
- [ ] 支持鼠标拖动旋转视角
- [ ] 支持滚轮缩放
- [ ] 支持重置视角按钮
- [ ] 性能流畅（60fps @ 1000Hz数据更新）

### 视觉验收
- [ ] 3D模型美观（推荐飞机模型）
- [ ] 坐标轴箭头明显
- [ ] 背景色与主题配色一致
- [ ] 光照效果合理

### 代码质量
- [ ] 符合Qt 5.14规范
- [ ] 3D组件独立封装，可复用
- [ ] 无内存泄漏
- [ ] 帧率稳定

---

## 🔧 技术方案

### 方案选择

#### 方案A：Qt3D（推荐）✅
**优点**:
- Qt官方支持，集成简单
- QML或C++两种方式可选
- 自带相机、光照、材质系统
- 性能优秀

**缺点**:
- Qt 5.14的Qt3D相对较老
- 功能不如专业3D引擎丰富

#### 方案B：原生OpenGL
**优点**:
- 完全控制渲染流程
- 性能最优

**缺点**:
- 开发工作量大
- 需要手动管理着色器、缓冲区
- 学习曲线陡峭

**结论**: 选择**方案A（Qt3D）**，开发效率高且满足需求。

---

## 📝 实施步骤

### Step 1: 添加Qt3D依赖（30分钟）

#### 1.1 修改 `app/app.pro`

```pro
# 添加Qt3D模块
QT += 3dcore 3drender 3dextras 3dinput

# 如果使用QML方式
QT += qml quick 3dquick
```

#### 1.2 修改 `global.pri`（如果需要全局配置）

```pro
# Qt 5.14 3D模块
contains(QT, 3dcore) {
    QT += 3drender 3dextras 3dinput
}
```

### Step 2: 创建3D组件类（2小时）

#### 2.1 创建 `app/ui/imu3dview.h`

```cpp
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
}

class IMU3DView : public QWidget
{
    Q_OBJECT
public:
    explicit IMU3DView(QWidget *parent = nullptr);
    ~IMU3DView();

    // 设置姿态角度（度）
    void setAttitude(double roll, double pitch, double yaw);

    // 重置视角
    void resetCamera();

signals:
    void angleChanged(double roll, double pitch, double yaw);

private:
    void setupScene();
    void createIMUModel();
    void createCoordinateAxes();
    void createGrid();

private:
    Qt3DExtras::Qt3DWindow *m_view;
    Qt3DCore::QEntity *m_rootEntity;
    Qt3DCore::QEntity *m_imuEntity;
    Qt3DCore::QTransform *m_imuTransform;
    Qt3DRender::QCamera *m_camera;

    // 当前姿态
    double m_roll;
    double m_pitch;
    double m_yaw;
};

#endif // IMU3DVIEW_H
```

#### 2.2 创建 `app/ui/imu3dview.cpp`

```cpp
#include "imu3dview.h"
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DRender/QPointLight>
#include <QVBoxLayout>

IMU3DView::IMU3DView(QWidget *parent)
    : QWidget(parent)
    , m_view(nullptr)
    , m_rootEntity(nullptr)
    , m_imuEntity(nullptr)
    , m_imuTransform(nullptr)
    , m_camera(nullptr)
    , m_roll(0.0)
    , m_pitch(0.0)
    , m_yaw(0.0)
{
    // 创建Qt3D窗口
    m_view = new Qt3DExtras::Qt3DWindow();
    m_view->defaultFrameGraph()->setClearColor(QColor(QRgb(0x1e1e1e)));

    // 将Qt3D窗口嵌入到QWidget
    QWidget *container = QWidget::createWindowContainer(m_view, this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(container);

    setupScene();
}

IMU3DView::~IMU3DView()
{
    // Qt3D对象通过父子关系自动管理
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
    // X轴（红色）
    auto createAxis = [this](const QVector3D &direction, const QColor &color) {
        Qt3DCore::QEntity *axisEntity = new Qt3DCore::QEntity(m_rootEntity);

        // 轴线（圆柱体）
        Qt3DExtras::QCylinderMesh *cylinder = new Qt3DExtras::QCylinderMesh();
        cylinder->setRadius(0.05f);
        cylinder->setLength(3.0f);

        // 箭头（圆锥）
        Qt3DExtras::QConeMesh *cone = new Qt3DExtras::QConeMesh();
        cone->setBottomRadius(0.1f);
        cone->setLength(0.3f);

        // 材质
        Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial();
        material->setDiffuse(color);

        // 变换
        Qt3DCore::QTransform *transform = new Qt3DCore::QTransform();
        transform->setRotation(
            QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1),
                                         direction == QVector3D(1,0,0) ? -90 :
                                         direction == QVector3D(0,0,1) ? 0 : 0));
        transform->setTranslation(direction * 1.5f);

        axisEntity->addComponent(cylinder);
        axisEntity->addComponent(material);
        axisEntity->addComponent(transform);

        return axisEntity;
    };

    // X轴 - 红色
    createAxis(QVector3D(1, 0, 0), Qt::red);

    // Y轴 - 绿色
    createAxis(QVector3D(0, 1, 0), Qt::green);

    // Z轴 - 蓝色
    createAxis(QVector3D(0, 0, 1), Qt::blue);
}

void IMU3DView::createGrid()
{
    // 网格实现（可选，使用线框）
    // 简化版：暂不实现，后续可添加
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
    QQuaternion qRoll = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), roll);
    QQuaternion qPitch = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), pitch);
    QQuaternion qYaw = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), yaw);

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
```

### Step 3: 集成到MainWindow（1小时）

#### 3.1 修改 `app/ui/mainwindow.h`

```cpp
#include "imu3dview.h"

class MainWindow : public QMainWindow {
    // ... 现有代码 ...

private:
    IMU3DView *m_3dView;  // 新增
};
```

#### 3.2 修改 `app/ui/mainwindow.cpp`

```cpp
void MainWindow::setupUI()
{
    // ... 现有代码 ...

    // 创建3D视图（替换原来的占位符）
    m_3dView = new IMU3DView(this);

    // 将3D视图添加到中间面板
    // 假设中间面板布局为 ui->centerLayout
    ui->centerLayout->addWidget(m_3dView);
}

void MainWindow::onParseResultReady(const ParseResult &result)
{
    // 更新3D视图
    if (result.fieldValues.contains("Roll") &&
        result.fieldValues.contains("Pitch") &&
        result.fieldValues.contains("Yaw")) {

        double roll = result.fieldValues["Roll"].toDouble();
        double pitch = result.fieldValues["Pitch"].toDouble();
        double yaw = result.fieldValues["Yaw"].toDouble();

        m_3dView->setAttitude(roll, pitch, yaw);
    }

    // ... 现有更新逻辑 ...
}
```

### Step 4: 更新app.pro（5分钟）

```pro
# 头文件
HEADERS += \
    ui/mainwindow.h \
    ui/imu3dview.h \     # 新增
    # ... 其他头文件 ...

# 源文件
SOURCES += \
    ui/mainwindow.cpp \
    ui/imu3dview.cpp \   # 新增
    # ... 其他源文件 ...

# Qt模块
QT += 3dcore 3drender 3dextras 3dinput
```

### Step 5: 测试和优化（4-8小时）

#### 5.1 基本功能测试
- 编译运行
- 验证3D场景渲染
- 测试姿态数据绑定

#### 5.2 性能优化
- 检查帧率（目标60fps）
- 优化数据更新频率（限制为30Hz即可）
- 减少不必要的重绘

#### 5.3 视觉优化
- 调整光照参数
- 优化材质颜色
- 添加阴影（可选）

---

## 🧪 测试方案

### 测试用例1：静态姿态显示

**输入**:
```cpp
m_3dView->setAttitude(30.0, 45.0, 60.0);
```

**预期结果**:
- 3D模型按指定角度旋转
- 视觉上符合Roll/Pitch/Yaw定义

### 测试用例2：动态姿态更新

**输入**: 以100Hz频率更新姿态数据

**预期结果**:
- 3D模型平滑旋转
- 帧率稳定在60fps
- 无卡顿或闪烁

### 测试用例3：鼠标交互

**操作**:
- 拖动鼠标旋转视角
- 滚轮缩放
- 点击重置按钮

**预期结果**:
- 交互响应流畅
- 视角变化符合预期

---

## 📚 参考资料

### Qt3D官方文档
- [Qt3D Overview](https://doc.qt.io/qt-5/qt3d-overview.html)
- [Qt3D C++ Classes](https://doc.qt.io/qt-5/qt3d-cpp.html)
- [Qt3D Examples](https://doc.qt.io/qt-5/qt3d-examples.html)

### 欧拉角与四元数
- 欧拉角顺序：通常使用Z-Y-X（Yaw-Pitch-Roll）
- 四元数避免万向锁问题
- Qt使用`QQuaternion`类

### 相关代码
- `app/ui/mainwindow.h/cpp` - 主窗口
- OpenCV示例（`libs/opencv/`）- 参考3D可视化

---

## ⚠️ 注意事项

### Qt3D版本兼容性
- Qt 5.14的Qt3D相对稳定
- 避免使用Qt 6特性
- 注意QML与C++混合使用的问题

### 性能考虑
- 限制姿态更新频率（30Hz足够，人眼识别极限）
- 使用信号节流（throttle）避免过度更新
- 禁用不必要的特效

### 坐标系统
- 明确定义Roll/Pitch/Yaw的旋转方向
- 与IMU传感器坐标系保持一致
- 添加文档说明

---

## 🎨 扩展功能（可选）

### 高级特性
- [ ] 加载真实的飞机3D模型（.obj/.stl格式）
- [ ] 添加姿态轨迹记录（路径可视化）
- [ ] 陀螺仪数据可视化（旋转速度矢量）
- [ ] 加速度计数据可视化（力矢量）
- [ ] 录制3D动画并导出

---

## ✅ 完成检查清单

- [ ] Qt3D模块正确集成
- [ ] 3D场景正常渲染
- [ ] 姿态数据正确绑定
- [ ] 鼠标交互流畅
- [ ] 性能达标（60fps）
- [ ] 代码添加注释
- [ ] 编写使用文档
- [ ] 通过代码审查

---

**创建日期**: 2026-01-29
**最后更新**: 2026-01-29
**预计完成**: 2026-02-01
