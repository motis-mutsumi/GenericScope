# P1-02: 图像采集功能实现

**优先级**: P1（中优先级）
**预计工作量**: 2-3天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

实现图像采集传输功能（AcqTransfer），支持相机SDK集成。

### 当前状态
- ✅ ImageTransfer接口已定义
- ❌ **AcqTransfer实现缺失**

### 任务目标
1. 实现AcqTransfer图像采集类
2. 集成相机SDK（如海康威视、大恒等）
3. 支持基于回调的帧传递
4. 支持触发模式和连续模式

---

## 🎯 验收标准

### 功能验收
- [ ] 相机正常连接和断开
- [ ] 图像正常采集
- [ ] 回调机制正常工作
- [ ] 支持触发和连续两种模式
- [ ] 支持设置曝光、增益等参数

### 性能要求
- [ ] 帧率达到30fps
- [ ] 无丢帧
- [ ] 内存无泄漏

---

## 🔧 技术方案

### AcqTransfer类设计

```cpp
class AcqTransfer : public ImageTransfer {
    Q_OBJECT
public:
    explicit AcqTransfer(QObject *parent = nullptr);

    // 相机控制
    bool open() override;
    bool close() override;

    // 采集控制
    bool startAcquisition() override;
    bool stopAcquisition() override;

    // 参数设置
    void setExposure(double exposure);
    void setGain(double gain);
    void setTriggerMode(TriggerMode mode);

    // 回调设置
    void setImageCallback(ImageCallback callback) override;

private:
    void* m_cameraHandle;  // SDK句柄
    ImageCallback m_callback;
    std::atomic<bool> m_isRunning;
};
```

### SDK集成方案

推荐SDK：
1. **海康威视 MVS SDK**（常用）
2. **大恒水星 GxIAPI**
3. **OpenCV VideoCapture**（通用方案）

---

## 📚 参考资料

- 相机SDK文档
- ImageTransfer接口定义

---

**创建日期**: 2026-01-29
