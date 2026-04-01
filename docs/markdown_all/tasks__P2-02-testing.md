# P2-02: 测试覆盖

**优先级**: P2（低优先级）
**预计工作量**: 3-5天
**状态**: ⏳ 待开始
**负责人**: 待分配

---

## 📋 任务概述

为项目核心模块添加单元测试和集成测试，提升代码质量和稳定性。

### 当前状态
- ❌ **无单元测试**
- ❌ **无集成测试**
- ❌ **无性能测试**

### 任务目标
1. 搭建测试框架（Qt Test）
2. 为Protocol模块添加单元测试
3. 为Transfer模块添加单元测试
4. 添加端到端集成测试
5. 添加性能测试

---

## 🎯 验收标准

### 测试覆盖率
- [ ] Protocol模块覆盖率 ≥ 80%
- [ ] Transfer模块覆盖率 ≥ 70%
- [ ] 关键路径100%覆盖

### 测试类型
- [ ] 单元测试（各模块独立测试）
- [ ] 集成测试（模块间交互测试）
- [ ] 性能测试（1000Hz数据流）

### CI/CD
- [ ] 配置自动化测试
- [ ] 每次提交自动运行测试
- [ ] 测试报告生成

---

## 🔧 技术方案

### 1. 测试框架选择

使用**Qt Test框架**（Qt自带）

**优点**：
- Qt原生支持
- 与Qt API无缝集成
- 支持GUI测试
- 内置Benchmark工具

### 2. 项目结构

```
GenericScope/
├── tests/                          # 测试根目录
│   ├── tests.pro                   # 测试主项目
│   ├── protocol_test/              # Protocol模块测试
│   │   ├── protocol_test.pro
│   │   ├── tst_protocolconfig.cpp
│   │   ├── tst_protocolparser.cpp
│   │   ├── tst_checksumcalculator.cpp
│   │   └── tst_datatypeconverter.cpp
│   ├── transfer_test/              # Transfer模块测试
│   │   ├── transfer_test.pro
│   │   ├── tst_scopeuart.cpp
│   │   └── tst_scopetcp.cpp
│   ├── integration_test/           # 集成测试
│   │   ├── integration_test.pro
│   │   └── tst_protocol_uart.cpp
│   └── benchmark/                  # 性能测试
│       ├── benchmark.pro
│       └── bench_protocolparser.cpp
```

### 3. Protocol模块单元测试

#### 3.1 测试ChecksumCalculator

**tst_checksumcalculator.cpp**:

```cpp
#include <QtTest>
#include "protocol/checksumcalculator.h"

class TestChecksumCalculator : public QObject
{
    Q_OBJECT

private slots:
    void testSum();
    void testXOR();
    void testCRC8();
    void testCRC16_MODBUS();
    void testCRC16_CCITT();
    void testCRC32();
};

void TestChecksumCalculator::testSum()
{
    QByteArray data = QByteArray::fromHex("0102030405");

    uint16_t sum = ChecksumCalculator::calculateSum(data);

    QCOMPARE(sum, static_cast<uint16_t>(0x0F)); // 1+2+3+4+5=15
}

void TestChecksumCalculator::testCRC16_MODBUS()
{
    // 标准测试向量
    QByteArray data = QByteArray::fromHex("0102");

    uint16_t crc = ChecksumCalculator::calculateCRC16_MODBUS(data);

    // 预期值需要通过独立工具验证
    QCOMPARE(crc, static_cast<uint16_t>(0x8141));
}

void TestChecksumCalculator::testCRC32()
{
    QByteArray data = "123456789";

    uint32_t crc = ChecksumCalculator::calculateCRC32(
        reinterpret_cast<const uint8_t*>(data.constData()),
        data.size()
    );

    // IEEE 802.3标准测试向量
    QCOMPARE(crc, static_cast<uint32_t>(0xCBF43926));
}

QTEST_MAIN(TestChecksumCalculator)
#include "tst_checksumcalculator.moc"
```

#### 3.2 测试ProtocolParser

**tst_protocolparser.cpp**:

```cpp
class TestProtocolParser : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();    // 测试套件初始化
    void cleanupTestCase(); // 测试套件清理
    void init();            // 每个测试前执行
    void cleanup();         // 每个测试后执行

    // 测试用例
    void testParseValidFrame();
    void testParseInvalidChecksum();
    void testParseIncompleteData();
    void testParseMultipleFrames();
    void testByteOrder();
    void testFieldExtraction();
    void testDataTypeConversion();

private:
    ProtocolConfig m_config;
    QSharedPointer<ProtocolParser> m_parser;
};

void TestProtocolParser::initTestCase()
{
    // 创建测试协议配置
    m_config.name = "Test_Protocol";
    m_config.frameHeader = QByteArray::fromHex("AA55");
    m_config.frameFooter = QByteArray::fromHex("DC7E");
    m_config.checksumType = ChecksumType::CRC16_MODBUS;
    m_config.byteOrder = ByteOrder::LittleEndian;

    // 添加测试字段
    FieldConfig field;
    field.name = "Temperature";
    field.type = DataType::Int32;
    field.elementHead = 6;
    field.byteLength = 4;
    field.scale = 0.00390625; // 1/256
    m_config.fields.append(field);

    m_parser = QSharedPointer<ProtocolParser>::create(m_config);
}

void TestProtocolParser::testParseValidFrame()
{
    // 构造测试数据
    QByteArray testFrame = QByteArray::fromHex(
        "AA55"         // 帧头
        "0101"         // 其他数据
        "2E00"         // 其他数据
        "C7F62901"     // Temperature字段（小端int32）
        // ... 更多数据 ...
        "ABCD"         // CRC16（假设）
        "DC7E"         // 帧尾
    );

    ParseResult result = m_parser->parse(testFrame);

    QVERIFY(result.success);
    QVERIFY(result.fieldValues.contains("Temperature"));

    // 验证温度值计算
    // 原始值: 0x012962C7 = 19456711
    // 实际值: 19456711 * 0.00390625 ≈ 76000
    double temp = result.fieldValues["Temperature"].toDouble();
    QCOMPARE(qRound(temp), 76000);
}

void TestProtocolParser::testParseInvalidChecksum()
{
    QByteArray testFrame = QByteArray::fromHex(
        "AA55 0000 FFFF DC7E"  // 错误的CRC
    );

    ParseResult result = m_parser->parse(testFrame);

    QVERIFY(!result.success);
    QVERIFY(result.errorMessage.contains("校验"));
}

QTEST_MAIN(TestProtocolParser)
#include "tst_protocolparser.moc"
```

### 4. Transfer模块单元测试

#### 4.1 测试ScopeUart

```cpp
class TestScopeUart : public QObject
{
    Q_OBJECT

private slots:
    void testOpen();
    void testClose();
    void testSendData();
    void testReceiveData();
    void testProtocolIntegration();
};

void TestScopeUart::testProtocolIntegration()
{
    ScopeUart uart;

    // 设置虚拟串口（用于测试）
    QVariantMap config;
    config["port"] = "COM_VIRTUAL";
    uart.setConfig(config);

    // 设置协议
    uart.setProtocol("Test_Protocol");

    // 模拟接收数据
    QByteArray testData = QByteArray::fromHex("AA55...");

    QSignalSpy spy(&uart, &ScopeUart::parseResultReady);

    // 触发数据接收
    // uart.simulateReceive(testData);  // 需要添加测试接口

    QCOMPARE(spy.count(), 1);
}
```

### 5. 集成测试

#### 5.1 端到端测试

**tst_protocol_uart.cpp**:

```cpp
class TestProtocolUartIntegration : public QObject
{
    Q_OBJECT

private slots:
    void testEndToEnd();
};

void TestProtocolUartIntegration::testEndToEnd()
{
    // 1. 创建协议配置
    ProtocolConfig config = createTestConfig();
    ProtocolManager::instance()->addProtocol(config);

    // 2. 创建串口
    ScopeUart uart;
    uart.setProtocol(config.name);

    // 3. 发送测试数据
    QByteArray testData = generateTestFrame();

    // 4. 验证解析结果
    QSignalSpy spy(&uart, &ScopeUart::parseResultReady);

    uart.simulateReceive(testData);

    QCOMPARE(spy.count(), 1);

    ParseResult result = spy.at(0).at(0).value<ParseResult>();
    QVERIFY(result.success);
    QVERIFY(result.fieldValues.size() > 0);
}
```

### 6. 性能测试（Benchmark）

**bench_protocolparser.cpp**:

```cpp
class BenchProtocolParser : public QObject
{
    Q_OBJECT

private slots:
    void benchParseSingleFrame();
    void benchParseHighFrequency();
};

void BenchProtocolParser::benchParseSingleFrame()
{
    ProtocolParser parser(createTestConfig());
    QByteArray testFrame = generateTestFrame();

    QBENCHMARK {
        parser.parse(testFrame);
    }
}

void BenchProtocolParser::benchParseHighFrequency()
{
    // 模拟1000Hz数据流
    ProtocolParser parser(createTestConfig());
    QByteArray testFrame = generateTestFrame();

    int iterations = 1000;

    QBENCHMARK {
        for (int i = 0; i < iterations; ++i) {
            parser.parse(testFrame);
        }
    }
}

QTEST_MAIN(BenchProtocolParser)
#include "bench_protocolparser.moc"
```

### 7. 测试配置文件

**tests/tests.pro**:

```pro
TEMPLATE = subdirs

SUBDIRS = \
    protocol_test \
    transfer_test \
    integration_test \
    benchmark

# 测试依赖关系
integration_test.depends = protocol_test transfer_test
```

**protocol_test/protocol_test.pro**:

```pro
QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

# 包含被测试模块
INCLUDEPATH += $$PWD/../../protocol
LIBS += -L$$PWD/../../Bin/x64/Release -lprotocol

# 测试源文件
SOURCES += \
    tst_checksumcalculator.cpp \
    tst_protocolparser.cpp \
    tst_datatypeconverter.cpp
```

---

## 📝 实施步骤

### Step 1: 搭建测试框架（0.5天）

1. 创建tests目录
2. 配置tests.pro
3. 编写第一个简单测试验证环境

### Step 2: Protocol模块测试（1.5天）

1. ChecksumCalculator测试（0.5天）
2. DataTypeConverter测试（0.5天）
3. ProtocolParser测试（0.5天）

### Step 3: Transfer模块测试（1天）

1. ScopeUart测试
2. ScopeTcp测试

### Step 4: 集成测试（1天）

1. Protocol+Uart集成
2. 端到端流程测试

### Step 5: 性能测试（0.5天）

1. 解析性能测试
2. 高频数据流测试

### Step 6: CI/CD配置（0.5天）

配置GitHub Actions或Jenkins

---

## 🧪 运行测试

### 编译测试

```bash
cd tests
qmake tests.pro
jom
```

### 运行所有测试

```bash
cd Bin/x64/Release
./protocol_test.exe
./transfer_test.exe
./integration_test.exe
```

### 运行性能测试

```bash
./benchmark.exe -o results.xml
```

### 查看覆盖率

```bash
# 需要安装gcov或lcov
lcov --directory . --capture --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

---

## ✅ 完成检查清单

- [ ] 测试框架搭建完成
- [ ] Protocol模块测试覆盖率≥80%
- [ ] Transfer模块测试覆盖率≥70%
- [ ] 集成测试通过
- [ ] 性能测试达标（解析速度≥1000fps）
- [ ] CI/CD配置完成
- [ ] 测试文档完成

---

**创建日期**: 2026-01-29
