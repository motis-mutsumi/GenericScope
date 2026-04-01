#ifndef COMMANDSETTINGSDIALOG_H
#define COMMANDSETTINGSDIALOG_H

#include <QDialog>
#include <QVector>
#include <QMap>
#include <QJsonObject>
#include <QFile>

// 前向声明Qt类型
class QTabWidget;
class QTableWidget;
class QLineEdit;
class QTextEdit;
class QPushButton;
class QComboBox;
class QSpinBox;
class QGroupBox;
class QDialogButtonBox;

namespace Ui {
class CommandSettingsDialog;
}

// 前向声明protocol模块的类型
class ProtocolManager;
namespace ProtocolTypes {
    class ProtocolConfig;
}

/**
 * @brief 协议配置对话框（原指令设置对话框）
 *
 * 功能升级为完整的协议配置系统：
 * - 多协议管理（标签页切换）
 * - 帧格式配置（帧头、帧尾、校验方式、字节序等）
 * - 数据字段配置（支持12种数据类型）
 * - JSON导入导出
 * - 协议生成与测试
 *
 * 参考：docs/markdown_all/protocol-config-system-spec.md
 */
class CommandSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 数据类型枚举
     */
    enum class DataType {
        Int8,      // int8_t - 有符号8位整数
        UInt8,     // uint8_t - 无符号8位整数
        Int16,     // int16_t - 有符号16位整数
        UInt16,    // uint16_t - 无符号16位整数
        Int32,     // int32_t - 有符号32位整数
        UInt32,    // uint32_t - 无符号32位整数
        Float,     // float - 32位浮点数
        Double,    // double - 64位浮点数
        MByte,     // 多字节整数（需要缩放因子）
        String     // 字符串
    };

    /**
     * @brief 字节序枚举
     */
    enum class ByteOrder {
        LittleEndian,  // 小端序（低字节在前）
        BigEndian      // 大端序（高字节在前）
    };

    /**
     * @brief 校验方式枚举
     */
    enum class ChecksumType {
        None,           // 无校验
        Sum,            // 累加和
        XOR,            // 异或
        CRC8,           // CRC8
        CRC16_MODBUS,   // CRC16-MODBUS（多项式0xA001，初值0xFFFF）
        CRC16_CCITT,    // CRC16-CCITT（多项式0x1021，初值0xFFFF）
        CRC32           // CRC32（IEEE 802.3标准）
    };

    /**
     * @brief 校验范围枚举
     */
    enum class ChecksumScope {
        FullFrame,      // 从帧起始（包含帧头）
        AfterHeader,    // 从帧头后（不包含帧头）
        DataOnly,       // 仅数据部分（自动排除帧头和长度字段）
        Custom          // 自定义（使用checksumStart字段）
    };

    /**
     * @brief 数据字段配置结构
     */
    struct FieldConfig {
        int index;               // 字段索引
        int elementHead;         // 起始位置（字节偏移）
        QString name;            // 字段名称
        DataType type;           // 数据类型
        int byteLength;          // 字节长度
        double scale;            // 缩放因子
        double offset;           // 偏移量
        QString unit;            // 单位
        double maximum;          // 最大值
        double minimum;          // 最小值
        QString description;     // 描述
        QString tip;             // 提示信息

        FieldConfig() : index(0), elementHead(0), type(DataType::Int16),
                        byteLength(2), scale(1.0), offset(0.0),
                        maximum(0.0), minimum(0.0) {}
    };

    /**
     * @brief 协议配置结构
     */
    struct ProtocolConfig {
        // 协议基本信息
        QString name;            // 协议名称
        QString version;         // 协议版本
        QString description;     // 协议描述

        // 帧格式配置
        QString frameHeader;     // 帧头（16进制字符串，如 "FF AA"）
        QString frameFooter;     // 帧尾（16进制字符串）
        int lengthPosition;      // 长度字段位置（-1表示无长度字段）
        ChecksumType checksumType;  // 校验方式
        ChecksumScope checksumScope; // 校验范围（推荐使用）
        int checksumStart;       // 校验起始位置（当checksumScope=Custom时使用）
        int checksumLength;      // 校验字节数（-1表示到帧尾）
        int checksumPosition;    // 校验码位置（-1表示帧尾前）
        ByteOrder byteOrder;     // 数据字段字节序
        ByteOrder checksumByteOrder; // 校验码字节序（独立于数据字节序）
        int frequency;           // 数据频率（Hz）
        QString separator;       // 分隔符（文本协议）

        // 数据字段配置
        QVector<FieldConfig> fields;

        ProtocolConfig() : lengthPosition(-1), checksumType(ChecksumType::None),
                           checksumScope(ChecksumScope::AfterHeader),
                           checksumStart(0), checksumLength(-1), checksumPosition(-1),
                           byteOrder(ByteOrder::LittleEndian),
                           checksumByteOrder(ByteOrder::LittleEndian),
                           frequency(1000) {}
    };

    explicit CommandSettingsDialog(QWidget *parent = nullptr);
    ~CommandSettingsDialog();

    // 协议管理接口
    QStringList getProtocolNames() const;
    ProtocolConfig getProtocol(const QString &name) const;
    void setProtocols(const QMap<QString, ProtocolConfig> &protocols);
    QMap<QString, ProtocolConfig> getProtocols() const;

private slots:
    // 协议管理
    void onNewProtocol();
    void onDeleteProtocol();
    void onTabChanged(int index);

    // 帧格式配置
    void onFrameHeaderChanged();
    void onFrameFooterChanged();
    void onChecksumTypeChanged(int index);
    void onByteOrderChanged(int index);

    // 字段配置
    void onAddField();
    void onDeleteField();
    void onMoveFieldUp();
    void onMoveFieldDown();
    void onFieldCellChanged(int row, int column);

    // 导入导出
    void onImportProtocol();
    void onExportProtocol();
    void onImportFields();

    // 按钮操作
    void onGenerateProtocol();
    void onTestProtocol();
    void onApply();
    void onOk();
    void onCancel();

    // 帮助按钮
    void showHelp();

private:
    void setupUI();
    void setupConnections();
    void setupTableColumns();
    void applyStyles();

    void updateProtocolTabs();
    void updateFrameFormatUI();
    void updateFieldTable();

    ProtocolConfig getCurrentConfig() const;
    void setCurrentConfig(const ProtocolConfig &config);

    bool validateConfig(QString *errorMsg = nullptr);
    bool validateHexString(const QString &hexStr) const;

    // 数据类型转换
    QString dataTypeToString(DataType type) const;
    DataType stringToDataType(const QString &str) const;
    QString byteOrderToString(ByteOrder order) const;
    ByteOrder stringToByteOrder(const QString &str) const;
    QString checksumTypeToString(ChecksumType type) const;
    ChecksumType stringToChecksumType(const QString &str) const;

    // JSON序列化
    QJsonObject fieldToJson(const FieldConfig &field) const;
    FieldConfig jsonToField(const QJsonObject &json) const;
    QJsonObject configToJson(const ProtocolConfig &config) const;
    ProtocolConfig jsonToConfig(const QJsonObject &json) const;

    void loadProtocols();
    void saveProtocols();

    // 字段导入辅助函数
    bool parseFieldsFromCSV(QFile *file, QVector<FieldConfig> &fields, QString *errorMsg);
    bool parseFieldsFromJSON(QFile *file, QVector<FieldConfig> &fields, QString *errorMsg);

    // 与ProtocolManager交互
    void syncToProtocolManager();
    void syncFromProtocolManager();

private:
    Ui::CommandSettingsDialog *ui;

    // 数据成员
    QMap<QString, ProtocolConfig> m_protocols;
    QString m_currentProtocolName;
    bool m_isModified;

    // 表格列索引常量
    static const int kFieldTableIndexColumn = 0;
    static const int kFieldTableElementHeadColumn = 1;
    static const int kFieldTableNameColumn = 2;
    static const int kFieldTableTypeColumn = 3;
    static const int kFieldTableByteLengthColumn = 4;
    static const int kFieldTableScaleColumn = 5;
    static const int kFieldTableOffsetColumn = 6;
    static const int kFieldTableUnitColumn = 7;
    static const int kFieldTableMaxColumn = 8;
    static const int kFieldTableMinColumn = 9;
    static const int kFieldTableDescColumn = 10;
    static const int kFieldTableTipColumn = 11;
};

#endif // COMMANDSETTINGSDIALOG_H
