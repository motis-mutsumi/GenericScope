#ifndef TURNTABLECONTROLDIALOG_H
#define TURNTABLECONTROLDIALOG_H

#include <QByteArray>
#include <QDialog>
#include <QVector>

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QSpinBox;
class QTableWidget;
class QTextEdit;
class TurntableConnection;

class TurntableControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TurntableControlDialog(QWidget *parent = nullptr);

private slots:
    void onConnectClicked();
    void onSaveConfigClicked();
    void onAxisCountChanged(int axisCount);
    void onModeChanged();
    void onSendMotionClicked();
    void onRefreshStatusClicked();
    void onReceivedData(const QByteArray &data);
    void onConnectionChanged(bool connected);
    void onConnectionError(const QString &error);

private:
    struct AxisCommand
    {
        quint8 control = 0x00;
        float param1 = 0.0f;
        float param2 = 0.0f;
        float param3 = 0.0f;
    };

    void setupUi();
    void loadConfigToUi();
    void saveUiToConfig();
    bool validateConnectionParams(QString *error) const;
    void updateConnectionUi(bool connected);
    void setupCommands();
    void sendQuickCommand(quint8 control, const QString &name, bool useStopAcceleration = false);
    void sendFrame(const QString &name, const QByteArray &frame);
    QByteArray buildCommandFrame(const QVector<AxisCommand> &commands) const;
    QVector<AxisCommand> buildSelectedAxisCommands(quint8 control, float p1, float p2, float p3) const;
    void appendFloatLE(QByteArray *frame, float value) const;
    quint8 checksum(const QByteArray &frame, int start, int count) const;
    QString hexText(const QByteArray &frame) const;
    void appendHistory(const QString &message);
    void updateParamLabels();
    void updateStatusRows();
    void processReceiveBuffer();
    bool tryParseFrame(int length);
    bool verifyFrame(const QByteArray &frame) const;
    int statusFrameLength() const;
    int paddedAckFrameLength() const;
    int compactAckFrameLength() const;
    void handleStatusFrame(const QByteArray &frame);
    void handleAckFrame(const QByteArray &frame);
    QString describeDeviceStatus(quint8 status) const;
    QString describeAxisState(quint8 state) const;
    QString describeAckStatus(quint8 status) const;
    float readFloatLE(const QByteArray &data, int offset) const;

private:
    TurntableConnection *m_connection;
    QByteArray m_rxBuffer;
    bool m_waitingStatusRefresh;

    QLineEdit *m_remoteIpEdit;
    QSpinBox *m_remotePortSpin;
    QSpinBox *m_localPortSpin;
    QLabel *m_connectionStatusLabel;
    QPushButton *m_connectButton;
    QPushButton *m_saveConfigButton;
    QSpinBox *m_axisCountSpin;
    QSpinBox *m_axisSpin;
    QCheckBox *m_allAxesCheck;
    QComboBox *m_modeCombo;
    QLabel *m_param1Label;
    QLabel *m_param2Label;
    QLabel *m_param3Label;
    QDoubleSpinBox *m_param1Spin;
    QDoubleSpinBox *m_param2Spin;
    QDoubleSpinBox *m_param3Spin;
    QLineEdit *m_framePreviewEdit;
    QTextEdit *m_historyEdit;
    QTableWidget *m_statusTable;
};

#endif // TURNTABLECONTROLDIALOG_H
