# P2-03: 功能增强

**优先级**: P2（低优先级）
**预计工作量**: 5-7天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

添加高级功能，提升用户体验和产品竞争力。

### 任务目标
1. 协议模板库
2. 批量协议生成
3. 数据回放功能
4. 多设备并发支持
5. 性能优化

---

## 🎯 验收标准

### 功能验收
- [ ] 协议模板库至少包含10个常用协议
- [ ] 支持批量导入协议
- [ ] 数据回放支持CSV格式
- [ ] 支持同时连接3个以上设备
- [ ] 性能优化后CPU占用降低20%

---

## 🔧 功能详细设计

### 1. 协议模板库（1-2天）

#### 1.1 内置协议模板

创建常用协议模板：

```
protocols/templates/
├── imu/
│   ├── imu_standard_v1.json      # 标准IMU协议
│   ├── imu_mpu6050.json          # MPU6050
│   └── imu_bmi160.json           # BMI160
├── modbus/
│   ├── modbus_rtu.json           # MODBUS RTU
│   └── modbus_tcp.json           # MODBUS TCP
├── sensors/
│   ├── temperature_ds18b20.json  # DS18B20温度传感器
│   ├── humidity_dht22.json       # DHT22温湿度
│   └── pressure_bmp280.json      # BMP280气压传感器
├── industrial/
│   ├── plc_siemens_s7.json       # 西门子PLC
│   └── scada_opc_ua.json         # OPC UA
└── custom/
    └── user_protocols.json       # 用户自定义
```

#### 1.2 模板管理器

```cpp
class ProtocolTemplateManager : public QObject
{
    Q_OBJECT
public:
    static ProtocolTemplateManager* instance();

    // 加载所有模板
    void loadTemplates();

    // 获取模板列表
    QStringList getCategories() const;
    QStringList getTemplates(const QString &category) const;

    // 应用模板
    ProtocolConfig applyTemplate(const QString &templateName);

    // 保存为模板
    void saveAsTemplate(const ProtocolConfig &config, const QString &name);

signals:
    void templatesLoaded();
    void templateAdded(const QString &name);
};
```

#### 1.3 UI集成

在CommandSettingsDialog中添加"从模板创建"按钮：

```cpp
void CommandSettingsDialog::onCreateFromTemplate()
{
    TemplateSelectionDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString templateName = dialog.selectedTemplate();
        ProtocolConfig config =
            ProtocolTemplateManager::instance()->applyTemplate(templateName);

        // 填充到UI
        loadProtocolConfig(config);
    }
}
```

---

### 2. 批量协议生成（1天）

#### 2.1 批量导入界面

```cpp
class BatchProtocolImportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BatchProtocolImportDialog(QWidget *parent = nullptr);

    // 添加文件
    void addFiles(const QStringList &files);

    // 获取导入结果
    QList<ProtocolConfig> importedProtocols() const;

private slots:
    void onSelectFiles();
    void onImportAll();
    void onValidateAll();
};
```

#### 2.2 支持格式

- JSON格式（单个或批量）
- CSV格式（字段配置表）
- Excel格式（通过第三方库）

#### 2.3 使用示例

```cpp
// 批量导入JSON文件
BatchProtocolImportDialog dialog;
dialog.addFiles({
    "protocols/imu_v1.json",
    "protocols/imu_v2.json",
    "protocols/modbus.json"
});

if (dialog.exec() == QDialog::Accepted) {
    for (const auto &config : dialog.importedProtocols()) {
        ProtocolManager::instance()->addProtocol(config);
    }
}
```

---

### 3. 数据回放功能（1-2天）

#### 3.1 回放管理器

```cpp
class DataReplayManager : public QObject
{
    Q_OBJECT
public:
    explicit DataReplayManager(QObject *parent = nullptr);

    // 加载回放文件
    bool loadFile(const QString &filePath);

    // 回放控制
    void play();
    void pause();
    void stop();
    void seek(int position);

    // 回放速度
    void setSpeed(double speed); // 1.0=正常，2.0=2倍速

signals:
    void dataFrame(const ParseResult &result);
    void progressChanged(int current, int total);
    void replayFinished();

private:
    QFile m_file;
    QTimer m_timer;
    double m_speed;
    int m_currentLine;
};
```

#### 3.2 回放控制UI

```
┌─────────────────────────────────────────┐
│ 数据回放                        [_][□][×]│
├─────────────────────────────────────────┤
│ 文件: imu_data_20260129.csv             │
│ [打开文件]                              │
│                                         │
│ ╔════════════════════════════╗          │
│ ║ [▶] [⏸] [⏹] [⏮] [⏭]      ║          │
│ ║                            ║          │
│ ║ 速度: [1x▼]                ║          │
│ ║                            ║          │
│ ║ ━━━━━━━━━●━━━━━━━━━━       ║          │
│ ║ 00:05 / 01:30              ║          │
│ ╚════════════════════════════╝          │
│                                         │
│ 实时数据显示（与正常模式相同）           │
└─────────────────────────────────────────┘
```

#### 3.3 CSV格式

```csv
Timestamp,Roll,Pitch,Yaw,AccX,AccY,AccZ,GyroX,GyroY,GyroZ
1000,1.23,4.56,7.89,0.1,0.2,9.8,0.01,0.02,0.03
1100,1.25,4.58,7.91,0.11,0.21,9.79,0.011,0.021,0.031
...
```

---

### 4. 多设备并发支持（2天）

#### 4.1 设备管理器增强

```cpp
class MultiDeviceManager : public QObject
{
    Q_OBJECT
public:
    static MultiDeviceManager* instance();

    // 设备管理
    QString addDevice(const QVariantMap &config);
    void removeDevice(const QString &deviceId);

    // 获取设备列表
    QStringList getDeviceIds() const;
    DeviceInfo getDeviceInfo(const QString &deviceId) const;

    // 数据订阅
    void subscribeData(const QString &deviceId, QObject *subscriber);

signals:
    void deviceAdded(const QString &deviceId);
    void deviceRemoved(const QString &deviceId);
    void deviceDataReady(const QString &deviceId, const ParseResult &data);

private:
    QMap<QString, ScopeTransferBasic*> m_devices;
};
```

#### 4.2 多设备UI

```
┌─────────────────────────────────────────┐
│ 设备管理                        [_][□][×]│
├─────────────────────────────────────────┤
│ ┌─设备列表───────────┬─设备详情────────┐│
│ │☑ 设备1 (COM7)      │ 名称: IMU-001   ││
│ │  ├ 状态: 已连接    │ 类型: UART      ││
│ │  └ 速率: 1000Hz    │ 协议: IMU_V1    ││
│ │                    │                 ││
│ │☑ 设备2 (192.168.1.100)             ││
│ │  ├ 状态: 已连接    │ 数据率: 500Hz   ││
│ │  └ 速率: 500Hz     │ 延迟: 5ms       ││
│ │                    │                 ││
│ │☐ 设备3 (COM9)      │ 状态: 未连接    ││
│ │  └ 状态: 已断开    │                 ││
│ │                    │                 ││
│ │ [+添加] [-删除]    │                 ││
│ └───────────────────┴────────────────┘│
└─────────────────────────────────────────┘
```

#### 4.3 数据同步

支持多设备数据时间戳同步：

```cpp
class DataSynchronizer : public QObject
{
    Q_OBJECT
public:
    void addDataFrame(const QString &deviceId,
                     const ParseResult &data);

signals:
    void synchronizedData(const QMap<QString, ParseResult> &data);

private:
    // 缓存最近的数据帧
    QMap<QString, QQueue<ParseResult>> m_dataBuffer;

    // 基于时间戳同步
    void synchronize();
};
```

---

### 5. 性能优化（1天）

#### 5.1 优化点

1. **减少UI更新频率**
   ```cpp
   // 限制UI更新为30Hz
   QTimer *uiUpdateTimer = new QTimer(this);
   uiUpdateTimer->setInterval(33); // 33ms ≈ 30Hz
   connect(uiUpdateTimer, &QTimer::timeout, this, &MainWindow::updateUI);
   ```

2. **数据批处理**
   ```cpp
   // 累积100个数据点后一次性更新图表
   void MainWindow::onDataReceived(const ParseResult &data) {
       m_dataBuffer.append(data);

       if (m_dataBuffer.size() >= 100) {
           m_plot->addDataBatch(m_dataBuffer);
           m_dataBuffer.clear();
       }
   }
   ```

3. **异步解析**
   ```cpp
   // 在工作线程解析数据
   AsyncQueue<QByteArray> parseQueue(4, 1000);
   parseQueue.setProcessor([](const QByteArray &data) {
       ParseResult result = parser->parse(data);
       emit resultReady(result);
   });
   ```

4. **内存池**
   ```cpp
   // ParseResult对象池
   class ParseResultPool {
   public:
       ParseResult* acquire();
       void release(ParseResult *result);

   private:
       QQueue<ParseResult*> m_pool;
   };
   ```

---

## 📝 实施步骤

### Week 1: 模板库和批量导入
- Day 1-2: 协议模板库
- Day 3: 批量导入功能

### Week 2: 回放和多设备
- Day 4-5: 数据回放
- Day 6-7: 多设备支持

### Week 3: 性能优化
- Day 8: 性能分析
- Day 9: 优化实施
- Day 10: 测试验证

---

## ✅ 完成检查清单

- [ ] 协议模板库（≥10个模板）
- [ ] 批量导入功能正常
- [ ] 数据回放流畅
- [ ] 多设备并发无冲突
- [ ] 性能提升达标
- [ ] 所有功能测试通过
- [ ] 用户文档更新

---

**创建日期**: 2026-01-29
**最后更新**: 2026-01-29
**预计完成**: 2026-02-10
