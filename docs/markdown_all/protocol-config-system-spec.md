# IMU协议配置与动态解析系统 技术规范

> **文档版本**: v1.0
> **创建日期**: 2026-01-26
> **作者**: GenericScope需求分析专家
> **状态**: 需求确认完成，待开发

---

## 目录

1. [需求概述](#1-需求概述)
2. [功能描述](#2-功能描述)
3. [技术方案](#3-技术方案)
4. [实现细节](#4-实现细节)
5. [测试方案](#5-测试方案)
6. [任务分解](#6-任务分解)
7. [风险与注意事项](#7-风险与注意事项)
8. [参考资料](#8-参考资料)

---

## 1. 需求概述

### 1.1 需求背景

不同厂商的IMU设备使用不同的通信协议，包括：
- **帧格式差异**：帧头、帧尾、长度字段位置不同
- **校验方式差异**：Sum、XOR、CRC8/16/32等
- **数据格式差异**：字段顺序、数据类型、字节序不同
- **数据解析差异**：缩放因子、偏移量、单位不同

**当前痛点**：
- 每次测试新厂商设备都需要修改C++代码
- 需要重新编译整个项目
- 维护成本高，容易出错
- 无法快速切换不同协议

### 1.2 业务价值

- ✅ **提升测试效率**：无需修改代码即可测试不同厂商设备
- ✅ **降低维护成本**：协议配置化，易于维护和扩展
- ✅ **提高灵活性**：运行时动态切换协议
- ✅ **减少错误**：配置化避免代码修改引入的bug
- ✅ **便于协作**：非开发人员也可以配置协议

### 1.3 目标用户

- IMU设备测试工程师
- 硬件集成工程师
- 系统集成人员

### 1.4 用户故事

**作为** IMU设备测试工程师
**我想要** 通过配置界面定义不同厂商的协议格式
**以便** 无需修改代码就能测试不同厂商的IMU设备

### 1.5 验收标准

- [ ] 可以配置帧格式（帧头、帧尾、校验方式、字节序等）
- [ ] 可以配置数据字段（支持所有数据类型）
- [ ] 可以保存/加载JSON配置文件
- [ ] 可以在运行时切换协议
- [ ] 可以正确解析高频数据（100-1000Hz）
- [ ] 可以在主界面显示解析结果
- [ ] 支持所有校验方式（Sum、XOR、CRC8/16/32）
- [ ] 支持大小端配置

---

## 2. 功能描述

### 2.1 核心功能（MVP）

#### 功能1：协议管理
- 多协议支持（标签页切换）
- 新建/删除/重命名协议
- 协议列表管理
- 协议导入导出

#### 功能2：帧格式配置

| 配置项 | 说明 | 示例 |
|--------|------|------|
| 帧头 | 16进制字符串 | `FF AA` |
| 帧尾 | 16进制字符串（可选） | `0D 0A` |
| 长度位置 | 长度字段在帧中的位置（字节索引） | `2` |
| 校验方式 | 无校验、Sum、XOR、CRC8、CRC16、CRC32 | `Sum` |
| 校验起始位置 | 校验计算的起始字节 | `0` |
| 校验字节数 | 参与校验计算的字节数 | `4` |
| 校验码位置 | 校验码在帧中的位置 | `帧尾前` |
| 字节序 | Little-Endian、Big-Endian | `Little-Endian` |
| 频率 | 数据帧频率（Hz） | `2000` |
| 分隔符 | 文本协议的分隔符（可选） | `,` |

#### 功能3：数据字段配置

表格配置，每个字段包含：

| 列名 | 说明 | 示例 |
|------|------|------|
| index | 字段索引（自动生成） | `1` |
| elementhead | 字段在帧中的起始位置（字节偏移） | `0` |
| name | 字段名称 | `gx` |
| type | 数据类型 | `mbyte_t` |
| bytelength | 字节长度 | `3` |
| scale | 缩放因子（用于mbyte_t类型） | `6.10352e-05` |
| offset | 偏移量（用于数值转换） | `0` |
| value | 当前值（运行时显示） | `0.123` |
| unit | 单位 | `°/s` |
| maximum | 最大值（用于范围检查） | `500` |
| minimum | 最小值（用于范围检查） | `-500` |
| description | 字段描述 | `X轴角速度` |
| tip | 提示信息 | `陀螺仪X轴` |

**支持的数据类型**：
- **基础类型**：`int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `float`, `double`
- **特殊类型**：
  - `mbyte_t`：多字节整数，需要缩放因子转换为浮点数
  - `string`：字符串（固定长度/变长）
  - `array`：数组（如：`float[3]`表示三轴数据）
  - `bitfield`：位域（一个字节中的某几位）

#### 功能4：JSON导入导出
- 保存协议配置到JSON文件
- 从JSON文件加载协议配置
- 支持批量导入多个协议
- JSON格式规范（见4.5节）

#### 功能5：协议生成与应用
- 点击"生成协议"按钮，生成JSON配置文件
- 运行时动态加载协议
- 在主界面选择当前使用的协议
- 协议切换无需重启程序

#### 功能6：数据预览
- 在主界面实时显示解析结果
- 显示原始数据（16进制）
- 显示解析后的字段值
- 显示数据速率和统计信息

### 2.2 后续功能（V2）
- 指令配置与发送
- 指令测试功能
- 协议模板库
- 协议验证与调试工具
- 数据录制与回放
- 协议文档自动生成

---

## 3. 技术方案

### 3.1 架构设计

```
┌─────────────────────────────────────────────────────────┐
│                    UI Layer                              │
│  ┌──────────────────────────────────────────────────┐  │
│  │  ProtocolConfigDialog (协议配置界面)              │  │
│  │  - 多标签页管理协议                               │  │
│  │  - 帧格式配置区                                   │  │
│  │  - 字段配置表格                                   │  │
│  │  - JSON导入导出                                   │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │  MainWindow (主界面)                              │  │
│  │  - 协议选择下拉框                                 │  │
│  │  - 数据显示区（通用化）                           │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│                  Business Layer                          │
│  ┌──────────────────────────────────────────────────┐  │
│  │  ProtocolManager (协议管理器)                     │  │
│  │  - 加载/保存协议配置                              │  │
│  │  - 管理协议列表                                   │  │
│  │  - 创建协议解析器                                 │  │
│  └──────────────────────────────────────────────────┘  │
│  ┌──────────────────────────────────────────────────┐  │
│  │  ProtocolParser (协议解析器)                      │  │
│  │  - 根据配置解析数据帧                             │  │
│  │  - 校验码验证                                     │  │
│  │  - 数据类型转换                                   │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
                          ↓
┌─────────────────────────────────────────────────────────┐
│                  Transfer Layer                          │
│  ┌──────────────────────────────────────────────────┐  │
│  │  ScopeUart (串口传输)                             │  │
│  │  - 接收原始数据                                   │  │
│  │  - 调用ProtocolParser解析                         │  │
│  │  - 发送解析结果信号                               │  │
│  └──────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### 3.2 模块划分

#### 模块1：protocol（新建模块）
- `protocolconfig.h/cpp` - 协议配置数据结构
- `protocolparser.h/cpp` - 协议解析器
- `protocolmanager.h/cpp` - 协议管理器（单例）
- `checksumcalculator.h/cpp` - 校验码计算器
- `datatypeconverter.h/cpp` - 数据类型转换器

#### 模块2：app/ui（修改现有模块）
- `protocolconfigdialog.h/cpp/ui` - 协议配置界面（改造CommandSettingsDialog）
- `mainwindow.h/cpp` - 主界面（添加协议选择和通用化显示）

#### 模块3：transfer（修改现有模块）
- `scopeuart.h/cpp` - 集成ProtocolParser

### 3.3 数据流设计

```
1. 配置阶段：
   用户 → ProtocolConfigDialog → ProtocolConfig → JSON文件

2. 加载阶段：
   JSON文件 → ProtocolManager → ProtocolParser

3. 运行阶段：
   串口数据 → ScopeUart → ProtocolParser → ParseResult → MainWindow
```

### 3.4 核心类设计

#### 3.4.1 ProtocolConfig（协议配置数据结构）

```cpp
// protocol/protocolconfig.h

#ifndef PROTOCOLCONFIG_H
#define PROTOCOLCONFIG_H

#include <QString>
#include <QVector>
#include <QVariant>
#include <QJsonObject>

namespace GenericScope {

// 数据类型枚举
enum class DataType {
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Float,
    Double,
    MByte,      // 多字节整数（需要缩放因子）
    String,     // 字符串
    Array,      // 数组
    BitField    // 位域
};

// 字节序枚举
enum class ByteOrder {
    LittleEndian,
    BigEndian
};

// 校验方式枚举
enum class ChecksumType {
    None,
    Sum,        // 累加和
    XOR,        // 异或
    CRC8,       // CRC8
    CRC16,      // CRC16
    CRC32       // CRC32
};

// 数据字段配置
struct FieldConfig {
    int index;                  // 字段索引
    int elementHead;            // 起始位置（字节偏移）
    QString name;               // 字段名称
    DataType type;              // 数据类型
    int byteLength;             // 字节长度
    double scale;               // 缩放因子
    double offset;              // 偏移量
    QString unit;               // 单位
    double maximum;             // 最大值
    double minimum;             // 最小值
    QString description;        // 描述
    QString tip;                // 提示信息

    // 运行时数据
    QVariant value;             // 当前值

    // JSON序列化
    QJsonObject toJson() const;
    static FieldConfig fromJson(const QJsonObject &json);
};

// 协议配置
class ProtocolConfig {
public:
    ProtocolConfig();

    // 协议基本信息
    QString name;               // 协议名称
    QString version;            // 协议版本
    QString description;        // 协议描述

    // 帧格式配置
    QByteArray frameHeader;     // 帧头（16进制）
    QByteArray frameFooter;     // 帧尾（16进制，可选）
    int lengthPosition;         // 长度字段位置（-1表示无长度字段）
    ChecksumType checksumType;  // 校验方式
    int checksumStart;          // 校验起始位置
    int checksumLength;         // 校验字节数（-1表示到帧尾）
    int checksumPosition;       // 校验码位置（-1表示帧尾前）
    ByteOrder byteOrder;        // 字节序
    int frequency;              // 数据频率（Hz）
    QString separator;          // 分隔符（文本协议）

    // 数据字段配置
    QVector<FieldConfig> fields;

    // JSON序列化
    QJsonObject toJson() const;
    static ProtocolConfig fromJson(const QJsonObject &json);

    // 保存/加载
    bool saveToFile(const QString &filePath) const;
    static ProtocolConfig loadFromFile(const QString &filePath);

    // 验证配置有效性
    bool validate(QString *errorMsg = nullptr) const;
};

} // namespace GenericScope

#endif // PROTOCOLCONFIG_H
```

#### 3.4.2 ProtocolParser（协议解析器）

```cpp
// protocol/protocolparser.h

#ifndef PROTOCOLPARSER_H
#define PROTOCOLPARSER_H

#include "protocolconfig.h"
#include <QByteArray>
#include <QMap>
#include <QVariant>

namespace GenericScope {

// 解析结果
struct ParseResult {
    bool success;                       // 是否解析成功
    QString errorMsg;                   // 错误信息
    QMap<QString, QVariant> fieldValues; // 字段名 -> 值
    QByteArray rawData;                 // 原始数据
    quint64 timestamp;                  // 时间戳（毫秒）

    ParseResult() : success(false), timestamp(0) {}
};

// 协议解析器
class ProtocolParser {
public:
    explicit ProtocolParser(const ProtocolConfig &config);

    // 解析数据帧
    ParseResult parse(const QByteArray &data);

    // 查找帧头
    int findFrameHeader(const QByteArray &data, int startPos = 0) const;

    // 提取完整帧
    QByteArray extractFrame(const QByteArray &data, int headerPos) const;

    // 验证校验码
    bool verifyChecksum(const QByteArray &frame) const;

    // 解析字段
    QVariant parseField(const QByteArray &frame, const FieldConfig &field) const;

    // 获取配置
    const ProtocolConfig& config() const { return m_config; }

private:
    ProtocolConfig m_config;

    // 数据类型转换
    QVariant convertInt8(const QByteArray &data, int offset) const;
    QVariant convertUInt8(const QByteArray &data, int offset) const;
    QVariant convertInt16(const QByteArray &data, int offset) const;
    QVariant convertUInt16(const QByteArray &data, int offset) const;
    QVariant convertInt32(const QByteArray &data, int offset) const;
    QVariant convertUInt32(const QByteArray &data, int offset) const;
    QVariant convertFloat(const QByteArray &data, int offset) const;
    QVariant convertDouble(const QByteArray &data, int offset) const;
    QVariant convertMByte(const QByteArray &data, int offset, int length, double scale) const;
    QVariant convertString(const QByteArray &data, int offset, int length) const;

    // 校验码计算
    quint8 calculateSum(const QByteArray &data, int start, int length) const;
    quint8 calculateXOR(const QByteArray &data, int start, int length) const;
    quint8 calculateCRC8(const QByteArray &data, int start, int length) const;
    quint16 calculateCRC16(const QByteArray &data, int start, int length) const;
    quint32 calculateCRC32(const QByteArray &data, int start, int length) const;
};

} // namespace GenericScope

#endif // PROTOCOLPARSER_H
```

#### 3.4.3 ProtocolManager（协议管理器）

```cpp
// protocol/protocolmanager.h

#ifndef PROTOCOLMANAGER_H
#define PROTOCOLMANAGER_H

#include "protocolconfig.h"
#include "protocolparser.h"
#include <QObject>
#include <QMap>
#include <QSharedPointer>

namespace GenericScope {

// 协议管理器（单例）
class ProtocolManager : public QObject {
    Q_OBJECT

public:
    // 获取单例
    static ProtocolManager* instance();

    // 加载协议
    bool loadProtocol(const QString &filePath);
    bool loadProtocols(const QString &dirPath);

    // 保存协议
    bool saveProtocol(const QString &name, const QString &filePath);

    // 协议管理
    void addProtocol(const ProtocolConfig &config);
    void removeProtocol(const QString &name);
    bool hasProtocol(const QString &name) const;
    ProtocolConfig getProtocol(const QString &name) const;
    QStringList getProtocolNames() const;

    // 创建解析器
    QSharedPointer<ProtocolParser> createParser(const QString &name);

    // 当前协议
    void setCurrentProtocol(const QString &name);
    QString getCurrentProtocol() const { return m_currentProtocol; }
    QSharedPointer<ProtocolParser> getCurrentParser();

signals:
    void protocolAdded(const QString &name);
    void protocolRemoved(const QString &name);
    void currentProtocolChanged(const QString &name);

private:
    ProtocolManager(QObject *parent = nullptr);
    ~ProtocolManager();
    ProtocolManager(const ProtocolManager&) = delete;
    ProtocolManager& operator=(const ProtocolManager&) = delete;

    QMap<QString, ProtocolConfig> m_protocols;
    QString m_currentProtocol;
    QSharedPointer<ProtocolParser> m_currentParser;
};

} // namespace GenericScope

#endif // PROTOCOLMANAGER_H
```

#### 3.4.4 ProtocolConfigDialog（协议配置界面）

```cpp
// app/ui/protocolconfigdialog.h

#ifndef PROTOCOLCONFIGDIALOG_H
#define PROTOCOLCONFIGDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QTextEdit>
#include "protocol/protocolconfig.h"

namespace GenericScope {

class ProtocolConfigDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProtocolConfigDialog(QWidget *parent = nullptr);
    ~ProtocolConfigDialog();

    // 加载/保存协议
    void loadProtocol(const QString &name);
    void saveCurrentProtocol();

private slots:
    // 协议管理
    void onNewProtocol();
    void onDeleteProtocol();
    void onRenameProtocol();
    void onImportProtocol();
    void onExportProtocol();
    void onTabChanged(int index);

    // 帧格式配置
    void onFrameHeaderChanged();
    void onFrameFooterChanged();
    void onChecksumTypeChanged(int index);
    void onByteOrderChanged(int index);

    // 字段配置
    void onAddField();
    void onDeleteField();
    void onFieldSelectionChanged();
    void onFieldDataChanged(int row, int column);
    void onMoveFieldUp();
    void onMoveFieldDown();

    // 按钮操作
    void onGenerateProtocol();
    void onTestProtocol();
    void onApply();
    void onOk();
    void onCancel();

private:
    void setupUI();
    void setupFrameFormatUI();
    void setupFieldTableUI();
    void setupButtons();

    void updateProtocolTabs();
    void updateFrameFormatUI();
    void updateFieldTable();

    ProtocolConfig getCurrentConfig() const;
    void setCurrentConfig(const ProtocolConfig &config);

    bool validateConfig(QString *errorMsg = nullptr);

    // UI组件
    QTabWidget *m_tabWidget;

    // 帧格式配置
    QLineEdit *m_frameHeaderEdit;
    QLineEdit *m_frameFooterEdit;
    QSpinBox *m_lengthPositionSpin;
    QComboBox *m_checksumTypeCombo;
    QSpinBox *m_checksumStartSpin;
    QSpinBox *m_checksumLengthSpin;
    QSpinBox *m_checksumPositionSpin;
    QComboBox *m_byteOrderCombo;
    QSpinBox *m_frequencySpin;
    QLineEdit *m_separatorEdit;

    // 字段配置表格
    QTableWidget *m_fieldTable;

    // 协议信息
    QLineEdit *m_protocolNameEdit;
    QLineEdit *m_protocolVersionEdit;
    QTextEdit *m_protocolDescEdit;

    // 当前协议列表
    QMap<QString, ProtocolConfig> m_protocols;
    QString m_currentProtocolName;
};

} // namespace GenericScope

#endif // PROTOCOLCONFIGDIALOG_H
```

---

## 4. 实现细节

### 4.1 mbyte_t类型的处理

`mbyte_t`是多字节整数类型，需要通过缩放因子转换为浮点数。

**实现示例**：

```cpp
// protocol/datatypeconverter.cpp

QVariant ProtocolParser::convertMByte(const QByteArray &data, int offset,
                                       int length, double scale) const {
    if (offset + length > data.size()) {
        return QVariant();
    }

    // 读取多字节整数（支持1-8字节）
    qint64 rawValue = 0;

    if (m_config.byteOrder == ByteOrder::LittleEndian) {
        // 小端序：低字节在前
        for (int i = 0; i < length; ++i) {
            rawValue |= (static_cast<qint64>(static_cast<quint8>(data[offset + i])) << (i * 8));
        }
    } else {
        // 大端序：高字节在前
        for (int i = 0; i < length; ++i) {
            rawValue |= (static_cast<qint64>(static_cast<quint8>(data[offset + i])) << ((length - 1 - i) * 8));
        }
    }

    // 处理符号位（如果是有符号数）
    int bitCount = length * 8;
    if (rawValue & (1LL << (bitCount - 1))) {
        // 负数：符号扩展
        rawValue |= (~0LL << bitCount);
    }

    // 应用缩放因子
    double result = rawValue * scale;

    return QVariant(result);
}
```

**使用示例**：

```cpp
// 3字节陀螺仪数据，缩放因子 6.10352e-05
FieldConfig gyroX;
gyroX.name = "gx";
gyroX.type = DataType::MByte;
gyroX.byteLength = 3;
gyroX.scale = 6.10352e-05;
gyroX.elementHead = 0;

// 原始数据：0x12 0x34 0x56 (小端序)
// rawValue = 0x563412 = 5649426
// result = 5649426 * 6.10352e-05 = 344.8°/s
```

### 4.2 校验码计算

#### 4.2.1 Sum校验（累加和）

```cpp
quint8 ProtocolParser::calculateSum(const QByteArray &data, int start, int length) const {
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint8 sum = 0;
    for (int i = 0; i < length; ++i) {
        sum += static_cast<quint8>(data[start + i]);
    }

    return sum;
}
```

**示例**：
```
数据：0xFF 0xAA 0x01 0x02 0x03
Sum = (0xFF + 0xAA + 0x01 + 0x02 + 0x03) & 0xFF = 0xA9
```

#### 4.2.2 XOR校验（异或）

```cpp
quint8 ProtocolParser::calculateXOR(const QByteArray &data, int start, int length) const {
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint8 xorValue = 0;
    for (int i = 0; i < length; ++i) {
        xorValue ^= static_cast<quint8>(data[start + i]);
    }

    return xorValue;
}
```

**示例**：
```
数据：0xFF 0xAA 0x01 0x02 0x03
XOR = 0xFF ^ 0xAA ^ 0x01 ^ 0x02 ^ 0x03 = 0xA9
```

#### 4.2.3 CRC16校验（MODBUS标准）

```cpp
quint16 ProtocolParser::calculateCRC16(const QByteArray &data, int start, int length) const {
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    quint16 crc = 0xFFFF;

    for (int i = 0; i < length; ++i) {
        crc ^= static_cast<quint8>(data[start + i]);

        for (int j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}
```

**示例**：
```
数据：0x01 0x03 0x00 0x00 0x00 0x02
CRC16 = 0xC40B (小端序：0x0B 0xC4)
```

#### 4.2.4 CRC32校验（IEEE 802.3标准）

```cpp
quint32 ProtocolParser::calculateCRC32(const QByteArray &data, int start, int length) const {
    if (start < 0 || start + length > data.size()) {
        return 0;
    }

    // CRC32查找表
    static const quint32 crc32Table[256] = {
        0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
        // ... (完整的256项查找表)
    };

    quint32 crc = 0xFFFFFFFF;

    for (int i = 0; i < length; ++i) {
        quint8 byte = static_cast<quint8>(data[start + i]);
        crc = (crc >> 8) ^ crc32Table[(crc ^ byte) & 0xFF];
    }

    return ~crc;
}
```

### 4.3 字节序处理

```cpp
// protocol/datatypeconverter.cpp

QVariant ProtocolParser::convertInt16(const QByteArray &data, int offset) const {
    if (offset + 2 > data.size()) {
        return QVariant();
    }

    qint16 value;

    if (m_config.byteOrder == ByteOrder::LittleEndian) {
        // 小端序：低字节在前
        value = static_cast<qint16>(
            (static_cast<quint8>(data[offset + 0]) << 0) |
            (static_cast<quint8>(data[offset + 1]) << 8)
        );
    } else {
        // 大端序：高字节在前
        value = static_cast<qint16>(
            (static_cast<quint8>(data[offset + 0]) << 8) |
            (static_cast<quint8>(data[offset + 1]) << 0)
        );
    }

    return QVariant(value);
}

QVariant ProtocolParser::convertFloat(const QByteArray &data, int offset) const {
    if (offset + 4 > data.size()) {
        return QVariant();
    }

    union {
        quint32 i;
        float f;
    } converter;

    if (m_config.byteOrder == ByteOrder::LittleEndian) {
        converter.i =
            (static_cast<quint32>(static_cast<quint8>(data[offset + 0])) << 0) |
            (static_cast<quint32>(static_cast<quint8>(data[offset + 1])) << 8) |
            (static_cast<quint32>(static_cast<quint8>(data[offset + 2])) << 16) |
            (static_cast<quint32>(static_cast<quint8>(data[offset + 3])) << 24);
    } else {
        converter.i =
            (static_cast<quint32>(static_cast<quint8>(data[offset + 0])) << 24) |
            (static_cast<quint32>(static_cast<quint8>(data[offset + 1])) << 16) |
            (static_cast<quint32>(static_cast<quint8>(data[offset + 2])) << 8) |
            (static_cast<quint32>(static_cast<quint8>(data[offset + 3])) << 0);
    }

    return QVariant(converter.f);
}
```

### 4.4 分包处理

```cpp
// protocol/protocolparser.cpp

ParseResult ProtocolParser::parse(const QByteArray &data) {
    ParseResult result;

    // 1. 查找帧头
    int headerPos = findFrameHeader(data);
    if (headerPos < 0) {
        result.errorMsg = "Frame header not found";
        return result;
    }

    // 2. 提取完整帧
    QByteArray frame = extractFrame(data, headerPos);
    if (frame.isEmpty()) {
        result.errorMsg = "Incomplete frame";
        return result;
    }

    // 3. 验证校验码
    if (!verifyChecksum(frame)) {
        result.errorMsg = "Checksum verification failed";
        return result;
    }

    // 4. 解析字段
    for (const FieldConfig &field : m_config.fields) {
        QVariant value = parseField(frame, field);
        if (value.isValid()) {
            result.fieldValues[field.name] = value;
        }
    }

    result.success = true;
    result.rawData = frame;
    result.timestamp = QDateTime::currentMSecsSinceEpoch();

    return result;
}

int ProtocolParser::findFrameHeader(const QByteArray &data, int startPos) const {
    if (m_config.frameHeader.isEmpty()) {
        return 0;  // 无帧头，从头开始
    }

    return data.indexOf(m_config.frameHeader, startPos);
}

QByteArray ProtocolParser::extractFrame(const QByteArray &data, int headerPos) const {
    int frameStart = headerPos;
    int frameLength = 0;

    // 方法1：根据长度字段确定帧长度
    if (m_config.lengthPosition >= 0) {
        int lengthPos = frameStart + m_config.lengthPosition;
        if (lengthPos + 1 < data.size()) {
            quint8 length = static_cast<quint8>(data[lengthPos]);
            frameLength = m_config.frameHeader.size() + length;
        }
    }
    // 方法2：根据帧尾确定帧长度
    else if (!m_config.frameFooter.isEmpty()) {
        int footerPos = data.indexOf(m_config.frameFooter, frameStart + m_config.frameHeader.size());
        if (footerPos >= 0) {
            frameLength = footerPos - frameStart + m_config.frameFooter.size();
        }
    }
    // 方法3：根据字段配置计算帧长度
    else {
        int maxOffset = 0;
        for (const FieldConfig &field : m_config.fields) {
            int endPos = field.elementHead + field.byteLength;
            if (endPos > maxOffset) {
                maxOffset = endPos;
            }
        }
        frameLength = m_config.frameHeader.size() + maxOffset;
        if (m_config.checksumType != ChecksumType::None) {
            frameLength += (m_config.checksumType == ChecksumType::CRC32) ? 4 :
                          (m_config.checksumType == ChecksumType::CRC16) ? 2 : 1;
        }
    }

    // 检查数据是否足够
    if (frameStart + frameLength > data.size()) {
        return QByteArray();  // 数据不完整
    }

    return data.mid(frameStart, frameLength);
}

bool ProtocolParser::verifyChecksum(const QByteArray &frame) const {
    if (m_config.checksumType == ChecksumType::None) {
        return true;  // 无校验
    }

    // 确定校验范围
    int checksumStart = m_config.checksumStart;
    int checksumLength = m_config.checksumLength;
    if (checksumLength < 0) {
        checksumLength = frame.size() - checksumStart -
                        ((m_config.checksumType == ChecksumType::CRC32) ? 4 :
                         (m_config.checksumType == ChecksumType::CRC16) ? 2 : 1);
    }

    // 计算校验码
    quint32 calculated = 0;
    switch (m_config.checksumType) {
        case ChecksumType::Sum:
            calculated = calculateSum(frame, checksumStart, checksumLength);
            break;
        case ChecksumType::XOR:
            calculated = calculateXOR(frame, checksumStart, checksumLength);
            break;
        case ChecksumType::CRC16:
            calculated = calculateCRC16(frame, checksumStart, checksumLength);
            break;
        case ChecksumType::CRC32:
            calculated = calculateCRC32(frame, checksumStart, checksumLength);
            break;
        default:
            return false;
    }

    // 提取帧中的校验码
    int checksumPos = m_config.checksumPosition;
    if (checksumPos < 0) {
        // 校验码在帧尾前
        checksumPos = frame.size() -
                     ((m_config.checksumType == ChecksumType::CRC32) ? 4 :
                      (m_config.checksumType == ChecksumType::CRC16) ? 2 : 1);
    }

    quint32 received = 0;
    if (m_config.checksumType == ChecksumType::CRC32) {
        received = convertUInt32(frame, checksumPos).toUInt();
    } else if (m_config.checksumType == ChecksumType::CRC16) {
        received = convertUInt16(frame, checksumPos).toUInt();
    } else {
        received = convertUInt8(frame, checksumPos).toUInt();
    }

    return calculated == received;
}
```

### 4.5 JSON配置文件格式

**完整示例**：

```json
{
    "name": "IMU_Protocol_V1",
    "version": "1.0.0",
    "description": "某厂商IMU协议配置",
    "frameFormat": {
        "header": "FF AA",
        "footer": "",
        "lengthPosition": 2,
        "checksumType": "Sum",
        "checksumStart": 0,
        "checksumLength": -1,
        "checksumPosition": -1,
        "byteOrder": "LittleEndian",
        "frequency": 2000,
        "separator": ""
    },
    "fields": [
        {
            "index": 1,
            "elementHead": 0,
            "name": "gx",
            "type": "mbyte_t",
            "byteLength": 3,
            "scale": 6.10352e-05,
            "offset": 0.0,
            "unit": "°/s",
            "maximum": 500.0,
            "minimum": -500.0,
            "description": "X轴角速度",
            "tip": "陀螺仪X轴"
        },
        {
            "index": 2,
            "elementHead": 3,
            "name": "gy",
            "type": "mbyte_t",
            "byteLength": 3,
            "scale": 6.10352e-05,
            "offset": 0.0,
            "unit": "°/s",
            "maximum": 500.0,
            "minimum": -500.0,
            "description": "Y轴角速度",
            "tip": "陀螺仪Y轴"
        },
        {
            "index": 3,
            "elementHead": 6,
            "name": "gz",
            "type": "mbyte_t",
            "byteLength": 3,
            "scale": 6.10352e-05,
            "offset": 0.0,
            "unit": "°/s",
            "maximum": 500.0,
            "minimum": -500.0,
            "description": "Z轴角速度",
            "tip": "陀螺仪Z轴"
        },
        {
            "index": 4,
            "elementHead": 9,
            "name": "ax",
            "type": "int16_t",
            "byteLength": 2,
            "scale": 0.001,
            "offset": 0.0,
            "unit": "m/s²",
            "maximum": 16.0,
            "minimum": -16.0,
            "description": "X轴加速度",
            "tip": "加速度计X轴"
        },
        {
            "index": 5,
            "elementHead": 11,
            "name": "ay",
            "type": "int16_t",
            "byteLength": 2,
            "scale": 0.001,
            "offset": 0.0,
            "unit": "m/s²",
            "maximum": 16.0,
            "minimum": -16.0,
            "description": "Y轴加速度",
            "tip": "加速度计Y轴"
        },
        {
            "index": 6,
            "elementHead": 13,
            "name": "az",
            "type": "int16_t",
            "byteLength": 2,
            "scale": 0.001,
            "offset": 0.0,
            "unit": "m/s²",
            "maximum": 16.0,
            "minimum": -16.0,
            "description": "Z轴加速度",
            "tip": "加速度计Z轴"
        },
        {
            "index": 7,
            "elementHead": 15,
            "name": "temperature",
            "type": "int16_t",
            "byteLength": 2,
            "scale": 0.01,
            "offset": 0.0,
            "unit": "°C",
            "maximum": 85.0,
            "minimum": -40.0,
            "description": "温度",
            "tip": "芯片温度"
        }
    ]
}
```

**数据类型映射**：

| JSON字符串 | C++枚举 | 说明 |
|-----------|---------|------|
| `"int8_t"` | `DataType::Int8` | 有符号8位整数 |
| `"uint8_t"` | `DataType::UInt8` | 无符号8位整数 |
| `"int16_t"` | `DataType::Int16` | 有符号16位整数 |
| `"uint16_t"` | `DataType::UInt16` | 无符号16位整数 |
| `"int32_t"` | `DataType::Int32` | 有符号32位整数 |
| `"uint32_t"` | `DataType::UInt32` | 无符号32位整数 |
| `"float"` | `DataType::Float` | 32位浮点数 |
| `"double"` | `DataType::Double` | 64位浮点数 |
| `"mbyte_t"` | `DataType::MByte` | 多字节整数 |
| `"string"` | `DataType::String` | 字符串 |

---

## 5. 测试方案

### 5.1 单元测试

#### 5.1.1 数据类型转换测试

```cpp
// tests/test_datatypeconverter.cpp

#include <QtTest>
#include "protocol/protocolparser.h"

class TestDataTypeConverter : public QObject {
    Q_OBJECT

private slots:
    void testInt8Conversion();
    void testUInt8Conversion();
    void testInt16LittleEndian();
    void testInt16BigEndian();
    void testFloatConversion();
    void testMByteConversion();
    void testStringConversion();
};

void TestDataTypeConverter::testInt8Conversion() {
    ProtocolConfig config;
    config.byteOrder = ByteOrder::LittleEndian;
    ProtocolParser parser(config);

    QByteArray data;
    data.append(static_cast<char>(0x7F));  // 127
    data.append(static_cast<char>(0x80));  // -128
    data.append(static_cast<char>(0xFF));  // -1

    QCOMPARE(parser.convertInt8(data, 0).toInt(), 127);
    QCOMPARE(parser.convertInt8(data, 1).toInt(), -128);
    QCOMPARE(parser.convertInt8(data, 2).toInt(), -1);
}

void TestDataTypeConverter::testInt16LittleEndian() {
    ProtocolConfig config;
    config.byteOrder = ByteOrder::LittleEndian;
    ProtocolParser parser(config);

    QByteArray data;
    data.append(static_cast<char>(0x34));  // 低字节
    data.append(static_cast<char>(0x12));  // 高字节
    // 0x1234 = 4660

    QCOMPARE(parser.convertInt16(data, 0).toInt(), 4660);
}

void TestDataTypeConverter::testInt16BigEndian() {
    ProtocolConfig config;
    config.byteOrder = ByteOrder::BigEndian;
    ProtocolParser parser(config);

    QByteArray data;
    data.append(static_cast<char>(0x12));  // 高字节
    data.append(static_cast<char>(0x34));  // 低字节
    // 0x1234 = 4660

    QCOMPARE(parser.convertInt16(data, 0).toInt(), 4660);
}

void TestDataTypeConverter::testMByteConversion() {
    ProtocolConfig config;
    config.byteOrder = ByteOrder::LittleEndian;
    ProtocolParser parser(config);

    QByteArray data;
    data.append(static_cast<char>(0x12));
    data.append(static_cast<char>(0x34));
    data.append(static_cast<char>(0x56));
    // 0x563412 = 5649426

    double scale = 6.10352e-05;
    QVariant result = parser.convertMByte(data, 0, 3, scale);

    QVERIFY(result.isValid());
    QVERIFY(qAbs(result.toDouble() - 344.8) < 0.1);
}

QTEST_MAIN(TestDataTypeConverter)
#include "test_datatypeconverter.moc"
```

#### 5.1.2 校验码计算测试

```cpp
// tests/test_checksum.cpp

#include <QtTest>
#include "protocol/protocolparser.h"

class TestChecksum : public QObject {
    Q_OBJECT

private slots:
    void testSumChecksum();
    void testXORChecksum();
    void testCRC16Checksum();
    void testCRC32Checksum();
};

void TestChecksum::testSumChecksum() {
    ProtocolConfig config;
    config.checksumType = ChecksumType::Sum;
    ProtocolParser parser(config);

    QByteArray data;
    data.append(static_cast<char>(0xFF));
    data.append(static_cast<char>(0xAA));
    data.append(static_cast<char>(0x01));
    data.append(static_cast<char>(0x02));
    data.append(static_cast<char>(0x03));

    quint8 sum = parser.calculateSum(data, 0, 5);
    // 0xFF + 0xAA + 0x01 + 0x02 + 0x03 = 0x2A9 & 0xFF = 0xA9
    QCOMPARE(sum, static_cast<quint8>(0xA9));
}

void TestChecksum::testXORChecksum() {
    ProtocolConfig config;
    config.checksumType = ChecksumType::XOR;
    ProtocolParser parser(config);

    QByteArray data;
    data.append(static_cast<char>(0xFF));
    data.append(static_cast<char>(0xAA));
    data.append(static_cast<char>(0x01));
    data.append(static_cast<char>(0x02));
    data.append(static_cast<char>(0x03));

    quint8 xorValue = parser.calculateXOR(data, 0, 5);
    // 0xFF ^ 0xAA ^ 0x01 ^ 0x02 ^ 0x03 = 0xA9
    QCOMPARE(xorValue, static_cast<quint8>(0xA9));
}

void TestChecksum::testCRC16Checksum() {
    ProtocolConfig config;
    config.checksumType = ChecksumType::CRC16;
    ProtocolParser parser(config);

    QByteArray data;
    data.append(static_cast<char>(0x01));
    data.append(static_cast<char>(0x03));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x00));
    data.append(static_cast<char>(0x02));

    quint16 crc = parser.calculateCRC16(data, 0, 6);
    // MODBUS CRC16: 0xC40B
    QCOMPARE(crc, static_cast<quint16>(0xC40B));
}

QTEST_MAIN(TestChecksum)
#include "test_checksum.moc"
```

#### 5.1.3 帧解析测试

```cpp
// tests/test_frameparser.cpp

#include <QtTest>
#include "protocol/protocolparser.h"

class TestFrameParser : public QObject {
    Q_OBJECT

private slots:
    void testFindFrameHeader();
    void testExtractFrameWithLength();
    void testExtractFrameWithFooter();
    void testVerifyChecksum();
    void testParseCompleteFrame();
};

void TestFrameParser::testFindFrameHeader() {
    ProtocolConfig config;
    config.frameHeader = QByteArray::fromHex("FFAA");
    ProtocolParser parser(config);

    QByteArray data = QByteArray::fromHex("0102FFAA0304");
    int pos = parser.findFrameHeader(data);

    QCOMPARE(pos, 2);
}

void TestFrameParser::testExtractFrameWithLength() {
    ProtocolConfig config;
    config.frameHeader = QByteArray::fromHex("FFAA");
    config.lengthPosition = 2;
    ProtocolParser parser(config);

    QByteArray data = QByteArray::fromHex("FFAA0501020304");
    // 帧头(2) + 长度字段(1) + 数据(5)
    QByteArray frame = parser.extractFrame(data, 0);

    QCOMPARE(frame.size(), 7);
}

void TestFrameParser::testParseCompleteFrame() {
    ProtocolConfig config;
    config.frameHeader = QByteArray::fromHex("FFAA");
    config.checksumType = ChecksumType::Sum;
    config.checksumStart = 0;
    config.checksumLength = 5;
    config.checksumPosition = 5;
    config.byteOrder = ByteOrder::LittleEndian;

    FieldConfig field;
    field.index = 1;
    field.elementHead = 2;
    field.name = "value";
    field.type = DataType::Int16;
    field.byteLength = 2;
    field.scale = 1.0;
    field.offset = 0.0;

    config.fields.append(field);

    ProtocolParser parser(config);

    // 构造测试帧：FF AA 34 12 00 A9
    // 帧头(2) + 数据(2) + 填充(1) + 校验(1)
    QByteArray data = QByteArray::fromHex("FFAA341200A9");

    ParseResult result = parser.parse(data);

    QVERIFY(result.success);
    QVERIFY(result.fieldValues.contains("value"));
    QCOMPARE(result.fieldValues["value"].toInt(), 0x1234);
}

QTEST_MAIN(TestFrameParser)
#include "test_frameparser.moc"
```

### 5.2 集成测试

#### 5.2.1 配置保存加载测试

```cpp
// tests/test_protocolconfig.cpp

#include <QtTest>
#include "protocol/protocolconfig.h"
#include "protocol/protocolmanager.h"

class TestProtocolConfig : public QObject {
    Q_OBJECT

private slots:
    void testSaveAndLoad();
    void testJSONSerialization();
    void testProtocolManager();
};

void TestProtocolConfig::testSaveAndLoad() {
    // 创建配置
    ProtocolConfig config;
    config.name = "TestProtocol";
    config.version = "1.0.0";
    config.frameHeader = QByteArray::fromHex("FFAA");
    config.checksumType = ChecksumType::Sum;
    config.byteOrder = ByteOrder::LittleEndian;

    FieldConfig field;
    field.name = "test";
    field.type = DataType::Int16;
    field.byteLength = 2;
    config.fields.append(field);

    // 保存到文件
    QString filePath = QDir::temp().filePath("test_protocol.json");
    QVERIFY(config.saveToFile(filePath));

    // 从文件加载
    ProtocolConfig loaded = ProtocolConfig::loadFromFile(filePath);

    QCOMPARE(loaded.name, config.name);
    QCOMPARE(loaded.version, config.version);
    QCOMPARE(loaded.frameHeader, config.frameHeader);
    QCOMPARE(loaded.fields.size(), 1);
    QCOMPARE(loaded.fields[0].name, QString("test"));

    // 清理
    QFile::remove(filePath);
}

void TestProtocolConfig::testProtocolManager() {
    ProtocolManager *manager = ProtocolManager::instance();

    // 创建协议
    ProtocolConfig config;
    config.name = "TestProtocol";
    config.frameHeader = QByteArray::fromHex("FFAA");

    // 添加协议
    manager->addProtocol(config);
    QVERIFY(manager->hasProtocol("TestProtocol"));

    // 获取协议
    ProtocolConfig retrieved = manager->getProtocol("TestProtocol");
    QCOMPARE(retrieved.name, QString("TestProtocol"));

    // 创建解析器
    auto parser = manager->createParser("TestProtocol");
    QVERIFY(parser != nullptr);

    // 删除协议
    manager->removeProtocol("TestProtocol");
    QVERIFY(!manager->hasProtocol("TestProtocol"));
}

QTEST_MAIN(TestProtocolConfig)
#include "test_protocolconfig.moc"
```

#### 5.2.2 实时解析测试

```cpp
// tests/test_realtime_parsing.cpp

#include <QtTest>
#include "protocol/protocolparser.h"
#include "transfer/scopeuart.h"

class TestRealtimeParsing : public QObject {
    Q_OBJECT

private slots:
    void testHighFrequencyData();
    void testPartialFrames();
    void testMultipleFrames();
};

void TestRealtimeParsing::testHighFrequencyData() {
    // 模拟2000Hz数据流
    ProtocolConfig config;
    config.frameHeader = QByteArray::fromHex("FFAA");
    config.checksumType = ChecksumType::None;
    config.byteOrder = ByteOrder::LittleEndian;

    FieldConfig field;
    field.elementHead = 0;
    field.name = "value";
    field.type = DataType::Int16;
    field.byteLength = 2;
    config.fields.append(field);

    ProtocolParser parser(config);

    // 生成1000个测试帧
    int successCount = 0;
    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 1000; ++i) {
        QByteArray frame = QByteArray::fromHex("FFAA");
        frame.append(static_cast<char>(i & 0xFF));
        frame.append(static_cast<char>((i >> 8) & 0xFF));

        ParseResult result = parser.parse(frame);
        if (result.success) {
            successCount++;
        }
    }

    qint64 elapsed = timer.elapsed();

    QCOMPARE(successCount, 1000);
    QVERIFY(elapsed < 1000);  // 应该在1秒内完成
    qDebug() << "Parsed 1000 frames in" << elapsed << "ms";
}

void TestRealtimeParsing::testPartialFrames() {
    ProtocolConfig config;
    config.frameHeader = QByteArray::fromHex("FFAA");
    config.lengthPosition = 2;
    ProtocolParser parser(config);

    // 模拟分包接收
    QByteArray part1 = QByteArray::fromHex("FFAA");
    QByteArray part2 = QByteArray::fromHex("050102");
    QByteArray part3 = QByteArray::fromHex("0304");

    // 第一部分：不完整
    ParseResult result1 = parser.parse(part1);
    QVERIFY(!result1.success);

    // 拼接后：完整
    QByteArray complete = part1 + part2 + part3;
    ParseResult result2 = parser.parse(complete);
    QVERIFY(result2.success);
}

QTEST_MAIN(TestRealtimeParsing)
#include "test_realtime_parsing.moc"
```

#### 5.2.3 UI集成测试

**手动测试清单**：

1. **协议配置界面测试**
   - [ ] 打开协议配置对话框
   - [ ] 创建新协议
   - [ ] 配置帧格式（帧头、校验方式、字节序）
   - [ ] 添加数据字段
   - [ ] 编辑字段属性
   - [ ] 删除字段
   - [ ] 保存协议到JSON文件
   - [ ] 从JSON文件加载协议
   - [ ] 切换协议标签页
   - [ ] 删除协议

2. **主界面集成测试**
   - [ ] 在主界面选择协议
   - [ ] 连接串口设备
   - [ ] 实时显示解析数据
   - [ ] 验证数据表格更新
   - [ ] 验证图表更新
   - [ ] 切换协议（运行时）
   - [ ] 断开连接

3. **性能测试**
   - [ ] 测试100Hz数据流
   - [ ] 测试1000Hz数据流
   - [ ] 测试2000Hz数据流
   - [ ] 监控CPU使用率
   - [ ] 监控内存使用
   - [ ] 检查UI响应性

4. **错误处理测试**
   - [ ] 发送错误校验码的帧
   - [ ] 发送不完整的帧
   - [ ] 发送错误格式的帧
   - [ ] 加载损坏的JSON文件
   - [ ] 配置无效的协议参数
   - [ ] 验证错误提示信息

---

## 6. 任务分解

### 阶段1：基础架构（预计3天）

#### TASK-001: 创建protocol模块基础结构
- **优先级**: P0（最高）
- **预估工作量**: 0.5天
- **依赖任务**: 无
- **涉及文件**:
  - `protocol/protocol.pro`（新建）
  - `protocol/protocolconfig.h`（新建）
  - `protocol/protocolconfig.cpp`（新建）
  - `GenericScope.pro`（修改）

**详细描述**:
1. 在项目根目录创建`protocol`子目录
2. 创建`protocol.pro`文件，配置为静态库
3. 定义基础数据结构：
   - `DataType`枚举
   - `ByteOrder`枚举
   - `ChecksumType`枚举
   - `FieldConfig`结构体
   - `ProtocolConfig`类
4. 在`GenericScope.pro`的SUBDIRS中添加protocol模块
5. 配置依赖关系：app依赖protocol

**验收标准**:
- [ ] protocol模块可以独立编译
- [ ] 生成libprotocol.a静态库
- [ ] 其他模块可以包含protocol头文件

#### TASK-002: 实现ProtocolConfig的JSON序列化
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-001
- **涉及文件**:
  - `protocol/protocolconfig.cpp`（修改）

**详细描述**:
1. 实现`FieldConfig::toJson()`方法
2. 实现`FieldConfig::fromJson()`静态方法
3. 实现`ProtocolConfig::toJson()`方法
4. 实现`ProtocolConfig::fromJson()`静态方法
5. 实现`ProtocolConfig::saveToFile()`方法
6. 实现`ProtocolConfig::loadFromFile()`静态方法
7. 实现`ProtocolConfig::validate()`方法
8. 处理JSON解析错误和异常

**验收标准**:
- [ ] 可以将ProtocolConfig保存为JSON文件
- [ ] 可以从JSON文件加载ProtocolConfig
- [ ] JSON格式符合4.5节规范
- [ ] 错误处理完善，有详细错误信息

#### TASK-003: 实现校验码计算器
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-001
- **涉及文件**:
  - `protocol/checksumcalculator.h`（新建）
  - `protocol/checksumcalculator.cpp`（新建）

**详细描述**:
1. 创建`ChecksumCalculator`类
2. 实现`calculateSum()`方法（累加和）
3. 实现`calculateXOR()`方法（异或）
4. 实现`calculateCRC8()`方法
5. 实现`calculateCRC16()`方法（MODBUS标准）
6. 实现`calculateCRC32()`方法（IEEE 802.3标准）
7. 编写单元测试（test_checksum.cpp）

**验收标准**:
- [ ] 所有校验算法实现正确
- [ ] 单元测试全部通过
- [ ] 性能满足要求（2000Hz数据流）

#### TASK-004: 实现数据类型转换器
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-001
- **涉及文件**:
  - `protocol/datatypeconverter.h`（新建）
  - `protocol/datatypeconverter.cpp`（新建）

**详细描述**:
1. 创建`DataTypeConverter`类
2. 实现基础类型转换：
   - `convertInt8()`, `convertUInt8()`
   - `convertInt16()`, `convertUInt16()`
   - `convertInt32()`, `convertUInt32()`
   - `convertFloat()`, `convertDouble()`
3. 实现特殊类型转换：
   - `convertMByte()`（多字节整数+缩放因子）
   - `convertString()`（字符串）
4. 支持大小端配置
5. 编写单元测试（test_datatypeconverter.cpp）

**验收标准**:
- [ ] 所有数据类型转换正确
- [ ] 大小端处理正确
- [ ] mbyte_t类型转换精度满足要求
- [ ] 单元测试全部通过

#### TASK-005: 实现ProtocolManager单例
- **优先级**: P1
- **预估工作量**: 0.5天
- **依赖任务**: TASK-002
- **涉及文件**:
  - `protocol/protocolmanager.h`（新建）
  - `protocol/protocolmanager.cpp`（新建）

**详细描述**:
1. 创建`ProtocolManager`单例类
2. 实现协议管理功能：
   - `loadProtocol()` - 加载单个协议
   - `loadProtocols()` - 批量加载协议
   - `saveProtocol()` - 保存协议
   - `addProtocol()` - 添加协议
   - `removeProtocol()` - 删除协议
   - `getProtocol()` - 获取协议
   - `getProtocolNames()` - 获取协议列表
3. 实现当前协议管理：
   - `setCurrentProtocol()` - 设置当前协议
   - `getCurrentProtocol()` - 获取当前协议名称
   - `getCurrentParser()` - 获取当前解析器
4. 定义信号：
   - `protocolAdded()`
   - `protocolRemoved()`
   - `currentProtocolChanged()`

**验收标准**:
- [ ] 单例模式实现正确
- [ ] 协议管理功能完整
- [ ] 信号发射正确
- [ ] 线程安全

### 阶段2：协议解析器（预计3天）

#### TASK-006: 实现ProtocolParser基础框架
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-003, TASK-004
- **涉及文件**:
  - `protocol/protocolparser.h`（新建）
  - `protocol/protocolparser.cpp`（新建）

**详细描述**:
1. 创建`ProtocolParser`类
2. 定义`ParseResult`结构体
3. 实现构造函数（接收ProtocolConfig）
4. 实现`parse()`主方法框架
5. 集成ChecksumCalculator
6. 集成DataTypeConverter

**验收标准**:
- [ ] 类结构清晰
- [ ] 接口定义完整
- [ ] 可以编译通过

#### TASK-007: 实现帧查找和提取
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-006
- **涉及文件**:
  - `protocol/protocolparser.cpp`（修改）

**详细描述**:
1. 实现`findFrameHeader()`方法
   - 在数据流中查找帧头
   - 支持从指定位置开始查找
2. 实现`extractFrame()`方法
   - 根据长度字段提取帧
   - 根据帧尾提取帧
   - 根据字段配置计算帧长度
3. 处理分包情况
4. 编写单元测试（test_frameparser.cpp）

**验收标准**:
- [ ] 可以正确查找帧头
- [ ] 可以提取完整帧
- [ ] 处理分包情况
- [ ] 单元测试通过

#### TASK-008: 实现校验码验证
- **优先级**: P0
- **预估工作量**: 0.5天
- **依赖任务**: TASK-007
- **涉及文件**:
  - `protocol/protocolparser.cpp`（修改）

**详细描述**:
1. 实现`verifyChecksum()`方法
2. 根据配置确定校验范围
3. 调用ChecksumCalculator计算校验码
4. 从帧中提取校验码
5. 比较计算值和接收值
6. 编写单元测试

**验收标准**:
- [ ] 所有校验方式验证正确
- [ ] 校验失败返回错误信息
- [ ] 单元测试通过

#### TASK-009: 实现字段解析
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-008
- **涉及文件**:
  - `protocol/protocolparser.cpp`（修改）

**详细描述**:
1. 实现`parseField()`方法
2. 根据字段类型调用相应的转换函数
3. 应用缩放因子和偏移量
4. 进行范围检查（maximum/minimum）
5. 处理解析错误
6. 编写单元测试

**验收标准**:
- [ ] 所有字段类型解析正确
- [ ] 缩放因子和偏移量应用正确
- [ ] 范围检查有效
- [ ] 单元测试通过

#### TASK-010: 完善parse()主方法
- **优先级**: P0
- **预估工作量**: 0.5天
- **依赖任务**: TASK-009
- **涉及文件**:
  - `protocol/protocolparser.cpp`（修改）

**详细描述**:
1. 整合所有子方法
2. 实现完整的解析流程：
   - 查找帧头
   - 提取完整帧
   - 验证校验码
   - 解析所有字段
3. 填充ParseResult
4. 错误处理和日志记录
5. 性能优化
6. 编写集成测试

**验收标准**:
- [ ] 完整解析流程正确
- [ ] 错误处理完善
- [ ] 性能满足2000Hz要求
- [ ] 集成测试通过

### 阶段3：UI界面（预计4天）

#### TASK-011: 设计ProtocolConfigDialog界面
- **优先级**: P1
- **预估工作量**: 1天
- **依赖任务**: TASK-005
- **涉及文件**:
  - `app/ui/protocolconfigdialog.ui`（新建）
  - `app/ui/protocolconfigdialog.h`（新建）
  - `app/ui/protocolconfigdialog.cpp`（新建）

**详细描述**:
1. 使用Qt Designer设计界面
2. 布局结构：
   - 顶部：协议标签页（QTabWidget）
   - 左侧：帧格式配置区
   - 右侧：字段配置表格
   - 底部：按钮区
3. 帧格式配置控件：
   - 帧头/帧尾输入框（16进制）
   - 长度位置、校验方式、字节序下拉框
   - 频率、分隔符输入框
4. 字段配置表格：
   - 12列：index, elementHead, name, type, byteLength, scale, offset, unit, maximum, minimum, description, tip
   - 支持添加/删除/编辑行
   - 支持上移/下移行
5. 按钮：
   - 新建协议、删除协议、重命名协议
   - 导入协议、导出协议
   - 生成协议、测试协议
   - 确定、取消、应用

**验收标准**:
- [ ] 界面布局合理美观
- [ ] 所有控件正确放置
- [ ] 样式统一（参考CommandSettingsDialog）

#### TASK-012: 实现协议管理功能
- **优先级**: P1
- **预估工作量**: 1天
- **依赖任务**: TASK-011
- **涉及文件**:
  - `app/ui/protocolconfigdialog.cpp`（修改）

**详细描述**:
1. 实现`onNewProtocol()`槽函数
   - 弹出输入对话框获取协议名称
   - 创建新的空白协议
   - 添加新标签页
2. 实现`onDeleteProtocol()`槽函数
   - 确认对话框
   - 删除当前协议
   - 移除标签页
3. 实现`onRenameProtocol()`槽函数
   - 弹出输入对话框
   - 重命名协议
   - 更新标签页标题
4. 实现`onTabChanged()`槽函数
   - 保存当前协议配置
   - 加载新协议配置
   - 更新UI显示

**验收标准**:
- [ ] 可以创建新协议
- [ ] 可以删除协议（有确认）
- [ ] 可以重命名协议
- [ ] 标签页切换正确

#### TASK-013: 实现帧格式配置功能
- **优先级**: P1
- **预估工作量**: 0.5天
- **依赖任务**: TASK-012
- **涉及文件**:
  - `app/ui/protocolconfigdialog.cpp`（修改）

**详细描述**:
1. 实现`updateFrameFormatUI()`方法
   - 从ProtocolConfig加载配置到UI控件
2. 实现`onFrameHeaderChanged()`等槽函数
   - 验证16进制输入格式
   - 更新ProtocolConfig
3. 实现下拉框变化处理
   - 校验方式变化
   - 字节序变化
4. 实时验证配置有效性

**验收标准**:
- [ ] 帧格式配置正确加载
- [ ] 配置修改实时保存
- [ ] 16进制输入验证有效
- [ ] 配置验证提示清晰

#### TASK-014: 实现字段配置功能
- **优先级**: P1
- **预估工作量**: 1.5天
- **依赖任务**: TASK-013
- **涉及文件**:
  - `app/ui/protocolconfigdialog.cpp`（修改）

**详细描述**:
1. 实现`updateFieldTable()`方法
   - 从ProtocolConfig加载字段到表格
   - 设置表格列宽和样式
2. 实现`onAddField()`槽函数
   - 在表格末尾添加新行
   - 设置默认值
   - 自动分配index
3. 实现`onDeleteField()`槽函数
   - 删除选中行
   - 重新计算index
4. 实现`onFieldDataChanged()`槽函数
   - 验证输入数据
   - 更新ProtocolConfig
5. 实现`onMoveFieldUp()`和`onMoveFieldDown()`
   - 交换字段顺序
   - 更新index
6. 实现字段选择变化处理

**验收标准**:
- [ ] 字段表格正确显示
- [ ] 可以添加/删除字段
- [ ] 可以编辑字段属性
- [ ] 可以调整字段顺序
- [ ] 数据验证有效

#### TASK-015: 实现导入导出功能
- **优先级**: P1
- **预估工作量**: 0.5天
- **依赖任务**: TASK-014
- **涉及文件**:
  - `app/ui/protocolconfigdialog.cpp`（修改）

**详细描述**:
1. 实现`onImportProtocol()`槽函数
   - 打开文件对话框（JSON文件）
   - 调用ProtocolConfig::loadFromFile()
   - 添加到协议列表
   - 创建新标签页
2. 实现`onExportProtocol()`槽函数
   - 打开保存文件对话框
   - 调用ProtocolConfig::saveToFile()
3. 实现`onGenerateProtocol()`槽函数
   - 验证当前配置
   - 保存到默认位置
   - 提示成功信息
4. 错误处理和用户提示

**验收标准**:
- [ ] 可以导入JSON协议文件
- [ ] 可以导出协议到JSON文件
- [ ] 可以生成协议到默认位置
- [ ] 错误提示清晰

### 阶段4：集成与测试（预计2天）

#### TASK-016: 集成ProtocolParser到ScopeUart
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-010
- **涉及文件**:
  - `transfer/scopeuart.h`（修改）
  - `transfer/scopeuart.cpp`（修改）

**详细描述**:
1. 在ScopeUart中添加ProtocolParser成员
2. 修改数据接收处理：
   - 接收原始数据
   - 调用parser->parse()
   - 发射解析结果信号
3. 添加协议切换接口：
   - `setProtocol(const QString &name)`
   - 从ProtocolManager获取解析器
4. 处理解析错误
5. 性能优化（避免阻塞）

**验收标准**:
- [ ] 串口数据正确解析
- [ ] 解析结果正确发射
- [ ] 可以动态切换协议
- [ ] 性能满足要求

#### TASK-017: 修改MainWindow支持动态协议
- **优先级**: P0
- **预估工作量**: 1天
- **依赖任务**: TASK-016
- **涉及文件**:
  - `app/ui/mainwindow.h`（修改）
  - `app/ui/mainwindow.cpp`（修改）
  - `app/ui/mainwindow.ui`（修改）

**详细描述**:
1. 添加协议选择下拉框到UI
   - 位置：设备控制栏
   - 从ProtocolManager获取协议列表
2. 实现`onProtocolChanged()`槽函数
   - 通知ScopeUart切换协议
   - 清空当前数据显示
   - 重新配置数据表格列
3. 修改数据显示逻辑：
   - 通用化数据表格（根据字段配置动态创建列）
   - 通用化图表（根据字段配置动态添加曲线）
4. 连接解析结果信号到显示槽函数
5. 添加"协议配置"菜单项
   - 打开ProtocolConfigDialog

**验收标准**:
- [ ] 可以选择协议
- [ ] 协议切换正确
- [ ] 数据表格动态更新
- [ ] 图表动态更新
- [ ] 可以打开协议配置对话框

#### TASK-018: 编写单元测试
- **优先级**: P1
- **预估工作量**: 1天
- **依赖任务**: TASK-010
- **涉及文件**:
  - `tests/test_datatypeconverter.cpp`（新建）
  - `tests/test_checksum.cpp`（新建）
  - `tests/test_frameparser.cpp`（新建）
  - `tests/test_protocolconfig.cpp`（新建）
  - `tests/tests.pro`（修改）

**详细描述**:
1. 编写数据类型转换测试（见5.1.1节）
2. 编写校验码计算测试（见5.1.2节）
3. 编写帧解析测试（见5.1.3节）
4. 编写配置保存加载测试（见5.2.1节）
5. 配置测试项目
6. 运行所有测试并修复问题

**验收标准**:
- [ ] 所有单元测试通过
- [ ] 代码覆盖率>80%
- [ ] 测试报告生成

#### TASK-019: 集成测试和性能测试
- **优先级**: P1
- **预估工作量**: 1天
- **依赖任务**: TASK-017, TASK-018
- **涉及文件**:
  - `tests/test_realtime_parsing.cpp`（新建）

**详细描述**:
1. 编写实时解析测试（见5.2.2节）
2. 进行性能测试：
   - 100Hz数据流
   - 1000Hz数据流
   - 2000Hz数据流
3. 监控资源使用：
   - CPU使用率
   - 内存使用
   - UI响应时间
4. 性能优化
5. 压力测试

**验收标准**:
- [ ] 2000Hz数据流稳定解析
- [ ] CPU使用率<30%
- [ ] 内存使用稳定
- [ ] UI响应流畅

#### TASK-020: 文档和代码审查
- **优先级**: P2
- **预估工作量**: 0.5天
- **依赖任务**: TASK-019
- **涉及文件**:
  - `docs/protocol-config-system-spec.md`（修改）
  - `CLAUDE.md`（修改）
  - 所有源代码文件

**详细描述**:
1. 更新技术规范文档
2. 更新CLAUDE.md
3. 添加代码注释
4. 代码审查：
   - 代码规范检查
   - Qt 5.14兼容性检查
   - 内存泄漏检查
   - 线程安全检查
5. 生成API文档（Doxygen）

**验收标准**:
- [ ] 文档完整准确
- [ ] 代码注释充分
- [ ] 代码审查通过
- [ ] API文档生成

### 任务依赖关系图

```
TASK-001 (基础结构)
  ├─> TASK-002 (JSON序列化)
  │     └─> TASK-005 (ProtocolManager)
  │           └─> TASK-011 (UI设计)
  │                 └─> TASK-012 (协议管理)
  │                       └─> TASK-013 (帧格式配置)
  │                             └─> TASK-014 (字段配置)
  │                                   └─> TASK-015 (导入导出)
  ├─> TASK-003 (校验码)
  │     └─> TASK-006 (Parser框架)
  └─> TASK-004 (类型转换)
        └─> TASK-006 (Parser框架)
              └─> TASK-007 (帧查找提取)
                    └─> TASK-008 (校验验证)
                          └─> TASK-009 (字段解析)
                                └─> TASK-010 (完善parse)
                                      ├─> TASK-016 (集成ScopeUart)
                                      │     └─> TASK-017 (修改MainWindow)
                                      │           └─> TASK-019 (集成测试)
                                      └─> TASK-018 (单元测试)
                                            └─> TASK-019 (集成测试)
                                                  └─> TASK-020 (文档审查)
```

### 总体时间估算

- **阶段1（基础架构）**: 3天
- **阶段2（协议解析器）**: 3天
- **阶段3（UI界面）**: 4天
- **阶段4（集成与测试）**: 2天

**总计**: 12个工作日（约2.5周）

**建议排期**:
- 第1周：完成阶段1和阶段2（基础架构+解析器）
- 第2周：完成阶段3（UI界面）
- 第3周前半：完成阶段4（集成与测试）

---

## 7. 风险与注意事项

### 7.1 技术风险

#### 风险1：高频数据解析性能不足
**描述**: 2000Hz数据流可能导致CPU占用过高，UI卡顿

**影响**: 高
**概率**: 中

**缓解措施**:
1. 使用独立线程进行数据解析
2. 使用AsyncQueue进行异步处理
3. 优化数据类型转换算法（避免重复计算）
4. 使用查找表优化CRC计算
5. 减少内存分配（对象池、预分配）
6. UI更新频率限制（100Hz显示，2000Hz处理）

**应急方案**:
- 降低UI更新频率到50Hz
- 简化数据显示（只显示关键字段）
- 使用更高效的数据结构（QVector预分配）

#### 风险2：mbyte_t类型精度损失
**描述**: 多字节整数转换为浮点数可能存在精度问题

**影响**: 中
**概率**: 低

**缓解措施**:
1. 使用double而非float存储结果
2. 使用qint64存储原始整数值
3. 充分测试边界值和极值
4. 提供精度配置选项

**应急方案**:
- 提供原始值显示选项
- 允许用户自定义精度

#### 风险3：分包处理复杂度
**描述**: 串口数据可能分包接收，帧边界识别困难

**影响**: 高
**概率**: 高

**缓解措施**:
1. 维护接收缓冲区
2. 实现滑动窗口查找帧头
3. 支持多种帧长度确定方式（长度字段、帧尾、固定长度）
4. 充分测试分包场景
5. 添加超时机制清理不完整帧

**应急方案**:
- 增大接收缓冲区
- 降低波特率
- 使用更可靠的帧格式（带长度字段和帧尾）

#### 风险4：Qt 5.14 API兼容性
**描述**: 使用了Qt 6特性导致编译失败

**影响**: 中
**概率**: 低

**缓解措施**:
1. 严格遵守Qt 5.14 API规范
2. 避免使用Qt 6新增特性
3. 代码审查检查兼容性
4. 在Qt 5.14环境中编译测试

**应急方案**:
- 使用Qt 5.14兼容的替代API
- 条件编译处理版本差异

### 7.2 性能风险

#### 风险5：JSON解析性能
**描述**: 频繁加载大型JSON配置文件可能影响启动速度

**影响**: 低
**概率**: 低

**缓解措施**:
1. 启动时只加载当前协议
2. 延迟加载其他协议
3. 缓存解析结果
4. 优化JSON文件大小

**应急方案**:
- 使用二进制格式存储配置
- 压缩JSON文件

#### 风险6：内存占用
**描述**: 大量协议配置和历史数据占用内存过多

**影响**: 中
**概率**: 中

**缓解措施**:
1. 限制历史数据缓存大小
2. 及时释放不用的协议配置
3. 使用智能指针管理内存
4. 定期进行内存监控

**应急方案**:
- 减少缓存大小
- 增加内存清理频率
- 提供内存使用统计

### 7.3 兼容性风险

#### 风险7：不同厂商协议差异大
**描述**: 某些特殊协议无法用当前配置系统表达

**影响**: 中
**概率**: 中

**缓解措施**:
1. 设计可扩展的数据类型系统
2. 支持自定义解析脚本（V2功能）
3. 提供插件机制（V2功能）
4. 充分调研常见协议格式

**应急方案**:
- 为特殊协议编写专用解析器
- 提供协议转换工具

#### 风险8：配置文件版本兼容
**描述**: 配置文件格式升级导致旧配置无法加载

**影响**: 中
**概率**: 中

**缓解措施**:
1. 在JSON中添加版本号字段
2. 实现版本迁移逻辑
3. 向后兼容旧版本格式
4. 提供配置升级工具

**应急方案**:
- 手动编辑JSON文件
- 重新创建配置

### 7.4 开发风险

#### 风险9：需求变更
**描述**: 开发过程中需求发生变化

**影响**: 高
**概率**: 中

**缓解措施**:
1. 采用模块化设计，易于修改
2. 充分的需求确认
3. 预留扩展接口
4. 敏捷开发，快速迭代

**应急方案**:
- 调整开发计划
- 分阶段交付

#### 风险10：测试不充分
**描述**: 测试覆盖不全面，遗漏bug

**影响**: 高
**概率**: 中

**缓解措施**:
1. 编写完善的单元测试
2. 进行充分的集成测试
3. 真实设备测试
4. 代码审查
5. 使用静态分析工具

**应急方案**:
- 增加测试时间
- 引入自动化测试

### 7.5 注意事项

#### 开发注意事项

1. **线程安全**
   - ProtocolParser在独立线程运行，注意数据竞争
   - 使用互斥锁保护共享数据
   - UI更新必须在主线程

2. **内存管理**
   - 使用智能指针（QSharedPointer）管理ProtocolParser
   - 及时释放大型缓冲区
   - 避免循环引用

3. **错误处理**
   - 所有解析错误必须有详细错误信息
   - 不能因为解析错误导致程序崩溃
   - 记录错误日志便于调试

4. **性能优化**
   - 避免在解析循环中进行字符串操作
   - 预分配容器大小
   - 使用const引用传递参数
   - 避免不必要的拷贝

5. **代码规范**
   - 遵循GenericScope代码规范
   - 使用C++17特性
   - 添加充分的注释
   - 使用有意义的变量名

#### 测试注意事项

1. **边界测试**
   - 测试最小/最大值
   - 测试空数据
   - 测试超长数据

2. **异常测试**
   - 测试错误校验码
   - 测试不完整帧
   - 测试错误格式

3. **性能测试**
   - 测试不同频率数据流
   - 测试长时间运行稳定性
   - 监控资源使用

4. **兼容性测试**
   - 在Qt 5.14环境测试
   - 在不同Windows版本测试
   - 测试不同分辨率显示

#### 部署注意事项

1. **配置文件管理**
   - 提供默认协议配置
   - 配置文件放在可写目录
   - 支持配置备份和恢复

2. **用户文档**
   - 提供协议配置指南
   - 提供常见问题解答
   - 提供示例配置文件

3. **版本管理**
   - 记录配置文件版本
   - 提供版本升级工具
   - 保持向后兼容

---

## 8. 参考资料

### 8.1 内部文档

1. **GenericScope架构文档**
   - 文件：`d:\app\App_GeneriScope\GenericScope\CLAUDE.md`
   - 内容：项目架构、模块说明、开发规范

2. **主界面设计文档**
   - 位置：`CLAUDE.md` 第3.6节
   - 内容：主界面布局、数据格式、功能说明

3. **传输层设计文档**
   - 位置：`CLAUDE.md` 第3.3节
   - 内容：传输抽象层、ScopeUart实现

4. **代码审查记录**
   - 位置：`CLAUDE.md` 第4.1节
   - 内容：Qt 5.14规范、代码优化建议

### 8.2 Protocol0界面参考

**Protocol0软件**是某IMU厂商提供的配置工具，提供了协议配置的参考界面：

**主要特性**:
1. **协议列表管理**
   - 多协议标签页切换
   - 新建/删除/重命名协议

2. **帧格式配置**
   - 帧头/帧尾（16进制输入）
   - 长度字段位置
   - 校验方式（Sum、XOR、CRC）
   - 字节序（大端/小端）

3. **字段配置表格**
   - 字段名称、类型、位置、长度
   - 缩放因子、偏移量
   - 单位、范围

4. **数据预览**
   - 实时显示解析结果
   - 16进制原始数据显示
   - 数据速率统计

**界面截图位置**:
- （如果有截图，放在`docs/images/protocol0_reference.png`）

**借鉴要点**:
- 清晰的分区布局
- 直观的表格配置
- 实时预览功能
- 16进制输入验证

### 8.3 Qt 5.14 API文档

1. **Qt Core**
   - QByteArray: https://doc.qt.io/qt-5.14/qbytearray.html
   - QVector: https://doc.qt.io/qt-5.14/qvector.html
   - QVariant: https://doc.qt.io/qt-5.14/qvariant.html
   - QJsonDocument: https://doc.qt.io/qt-5.14/qjsondocument.html

2. **Qt Widgets**
   - QTableWidget: https://doc.qt.io/qt-5.14/qtablewidget.html
   - QTabWidget: https://doc.qt.io/qt-5.14/qtabwidget.html
   - QDialog: https://doc.qt.io/qt-5.14/qdialog.html

3. **Qt Serial Port**
   - QSerialPort: https://doc.qt.io/qt-5.14/qserialport.html

**重要API差异**:
- Qt 5.14的`QVector::resize()`只支持单参数
- 使用`QStringLiteral`优化字符串常量
- 信号槽连接使用新语法（函数指针）

### 8.4 CRC算法参考

1. **CRC16 (MODBUS)**
   - 标准：MODBUS RTU
   - 多项式：0xA001（反向）
   - 初始值：0xFFFF
   - 参考：http://www.modbustools.com/modbus.html

2. **CRC32 (IEEE 802.3)**
   - 标准：IEEE 802.3
   - 多项式：0x04C11DB7
   - 初始值：0xFFFFFFFF
   - 参考：https://en.wikipedia.org/wiki/Cyclic_redundancy_check

3. **在线CRC计算器**
   - https://crccalc.com/
   - 用于验证CRC算法实现

### 8.5 数据类型参考

1. **C++基础类型**
   - int8_t, uint8_t: 1字节
   - int16_t, uint16_t: 2字节
   - int32_t, uint32_t: 4字节
   - float: 4字节（IEEE 754）
   - double: 8字节（IEEE 754）

2. **字节序**
   - Little-Endian（小端）：低字节在前（x86架构）
   - Big-Endian（大端）：高字节在前（网络字节序）

3. **IEEE 754浮点数格式**
   - float: 1位符号 + 8位指数 + 23位尾数
   - double: 1位符号 + 11位指数 + 52位尾数

### 8.6 相关开源项目

1. **QCustomPlot**
   - 网址：https://www.qcustomplot.com/
   - 用途：实时数据绘图（GenericScope已集成）

2. **spdlog**
   - 网址：https://github.com/gabime/spdlog
   - 用途：高性能日志库（GenericScope已集成）

3. **Protocol Buffers**
   - 网址：https://developers.google.com/protocol-buffers
   - 用途：二进制协议序列化（可选参考）

### 8.7 测试工具

1. **串口调试助手**
   - SSCOM（Windows）
   - CoolTerm（跨平台）
   - 用途：发送测试数据

2. **16进制编辑器**
   - HxD（Windows）
   - 010 Editor（跨平台）
   - 用途：查看二进制数据

3. **性能分析工具**
   - Qt Creator Profiler
   - Visual Studio Profiler
   - 用途：性能分析和优化

### 8.8 学习资源

1. **Qt官方教程**
   - https://doc.qt.io/qt-5.14/gettingstarted.html

2. **C++17特性**
   - https://en.cppreference.com/w/cpp/17

3. **串口通信协议设计**
   - 《Serial Port Complete》by Jan Axelson

4. **数据解析最佳实践**
   - 《Effective C++》by Scott Meyers
   - 《C++ Concurrency in Action》by Anthony Williams

---

## 附录

### A. 术语表

| 术语 | 英文 | 说明 |
|------|------|------|
| 帧头 | Frame Header | 数据帧的起始标识 |
| 帧尾 | Frame Footer | 数据帧的结束标识 |
| 校验码 | Checksum | 用于验证数据完整性的值 |
| 字节序 | Byte Order / Endianness | 多字节数据的存储顺序 |
| 小端序 | Little-Endian | 低字节在前的存储方式 |
| 大端序 | Big-Endian | 高字节在前的存储方式 |
| 缩放因子 | Scale Factor | 将整数转换为浮点数的乘数 |
| 偏移量 | Offset | 数据转换时的加法偏移 |
| 分包 | Fragmentation | 数据帧被分成多个包接收 |
| 协议解析 | Protocol Parsing | 将原始数据转换为结构化数据 |

### B. 配置示例

**示例1：简单协议（无校验）**

```json
{
    "name": "SimpleProtocol",
    "version": "1.0.0",
    "description": "简单的IMU协议，无校验",
    "frameFormat": {
        "header": "AA 55",
        "footer": "",
        "lengthPosition": -1,
        "checksumType": "None",
        "byteOrder": "LittleEndian",
        "frequency": 100
    },
    "fields": [
        {
            "index": 1,
            "elementHead": 0,
            "name": "ax",
            "type": "int16_t",
            "byteLength": 2,
            "scale": 0.001,
            "offset": 0.0,
            "unit": "m/s²"
        }
    ]
}
```

**示例2：复杂协议（CRC16校验）**

```json
{
    "name": "ComplexProtocol",
    "version": "1.0.0",
    "description": "复杂的IMU协议，带CRC16校验",
    "frameFormat": {
        "header": "FF AA",
        "footer": "0D 0A",
        "lengthPosition": 2,
        "checksumType": "CRC16",
        "checksumStart": 0,
        "checksumLength": -1,
        "checksumPosition": -1,
        "byteOrder": "BigEndian",
        "frequency": 2000
    },
    "fields": [
        {
            "index": 1,
            "elementHead": 0,
            "name": "gx",
            "type": "mbyte_t",
            "byteLength": 3,
            "scale": 6.10352e-05,
            "offset": 0.0,
            "unit": "°/s",
            "maximum": 500.0,
            "minimum": -500.0
        }
    ]
}
```

### C. 常见问题

**Q1: 如何确定帧长度？**

A: 有三种方式：
1. 长度字段：帧中包含长度字段，指示数据长度
2. 帧尾：通过查找帧尾确定帧结束位置
3. 固定长度：根据字段配置计算固定帧长度

**Q2: mbyte_t类型如何使用？**

A: mbyte_t用于多字节整数（1-8字节），需要配置：
- byteLength：字节数
- scale：缩放因子
- 计算公式：`result = rawValue * scale + offset`

**Q3: 如何处理分包？**

A: 解析器会维护接收缓冲区，自动处理分包：
1. 查找帧头
2. 检查数据是否完整
3. 如果不完整，等待更多数据
4. 如果完整，提取并解析

**Q4: 如何切换协议？**

A: 在主界面选择协议下拉框，选择目标协议即可。切换会：
1. 停止当前数据接收
2. 加载新协议配置
3. 重新配置显示界面
4. 恢复数据接收

**Q5: 如何调试协议配置？**

A: 建议步骤：
1. 使用串口调试助手发送测试数据
2. 在协议配置对话框查看原始数据（16进制）
3. 检查帧头、帧尾是否正确识别
4. 检查校验码是否通过
5. 检查字段值是否正确解析
6. 查看日志文件获取详细错误信息

---

**文档结束**

> **版本历史**:
> - v1.0 (2026-01-26): 初始版本，完整技术规范



