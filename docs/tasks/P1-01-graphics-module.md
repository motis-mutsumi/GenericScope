# P1-01: Graphics图形标注模块实现

**优先级**: P1（中优先级）
**预计工作量**: 2-3天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

实现Graphics图形标注模块，提供图像显示和交互式测量工具。

### 当前状态
- ✅ Plot/Log/Record模块已实现
- ❌ **Graphics模块缺失**

### 任务目标
1. 实现GraphicsView图形显示组件
2. 实现GraphicsItem交互式测量工具
3. 支持点、线、矩形、圆形标注
4. 支持缩放、平移、测量

---

## 🎯 验收标准

### 功能验收
- [ ] 图像正常显示
- [ ] 支持缩放（滚轮、按钮）
- [ ] 支持平移（拖动）
- [ ] 支持点标注（坐标显示）
- [ ] 支持线段测量（长度显示）
- [ ] 支持矩形区域（面积显示）
- [ ] 支持圆形标注（半径/直径）
- [ ] 标注可编辑、删除

### 代码质量
- [ ] 符合Qt规范
- [ ] 独立模块，可复用
- [ ] 详细注释

---

## 🔧 技术方案

### 模块结构

```
common_component/graphics/
├── graphics.pro
├── graphicsview.h/cpp          # 主视图组件
├── graphicsscene.h/cpp         # 场景管理
└── items/
    ├── graphicsitem.h/cpp      # 标注基类
    ├── pointitem.h/cpp         # 点标注
    ├── lineitem.h/cpp          # 线段标注
    ├── rectitem.h/cpp          # 矩形标注
    └── circleitem.h/cpp        # 圆形标注
```

### 核心类设计

#### GraphicsView类

```cpp
class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(QWidget *parent = nullptr);

    // 加载图像
    void setImage(const QImage &image);

    // 标注工具
    void setToolMode(ToolMode mode);

    // 缩放控制
    void zoomIn();
    void zoomOut();
    void fitInView();

signals:
    void itemAdded(GraphicsItem *item);
    void itemSelected(GraphicsItem *item);
    void measurementChanged(double value, const QString &unit);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};
```

### 实施步骤（详见文档）

---

## 📚 参考资料

- [Qt Graphics View Framework](https://doc.qt.io/qt-5/graphicsview.html)
- [QGraphicsItem](https://doc.qt.io/qt-5/qgraphicsitem.html)

---

**创建日期**: 2026-01-29
