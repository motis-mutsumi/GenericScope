#ifndef OVENCONTROLDIALOG_H
#define OVENCONTROLDIALOG_H

#include <QByteArray>
#include <QDialog>
#include <QVector>

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QSpinBox;
class QTextEdit;
class QTimer;
class OvenConnection;

class OvenControlDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OvenControlDialog(QWidget *parent = nullptr);

private slots:
    void onConnectClicked();
    void onSaveConfigClicked();
    void onExecuteClicked();
    void onRefreshStatusClicked();
    void onPollTimeout();
    void onReceivedData(const QByteArray &data);
    void onConnectionChanged(bool connected);
    void onConnectionError(const QString &error);

private:
    void setupUi();
    void refreshAvailablePorts();
    void loadConfigToUi();
    void saveUiToConfig();
    bool validateConnectionParams(QString *error) const;
    void updateConnectionUi(bool connected);
    void appendHistory(const QString &message);
    void sendCommand(const QString &name, const QByteArray &command);
    QByteArray buildReadTemperatureCommand(int deviceId) const;
    QByteArray buildSetConstantModeCommand(int deviceId) const;
    QByteArray buildSetTargetTemperatureCommand(int deviceId, double targetTemp) const;
    QByteArray buildPowerOffCommand(int deviceId) const;
    void executeSetTargetTemperature();
    void executeReadCurrentTemperature();
    void executeWaitForStable();
    void executePowerOff();
    void processReceiveBuffer();
    bool tryParseLine();
    bool parseTemperatureResponse(const QByteArray &line, double *temperature) const;
    void updateStatusDisplay(double currentTemp, const QString &statusText);

private:
    OvenConnection *m_connection;
    QTimer *m_pollTimer;
    QByteArray m_rxBuffer;
    QVector<double> m_recentTemperatures;
    bool m_waitingForStable;
    double m_lastTemperature;

    QComboBox *m_portCombo;
    QSpinBox *m_deviceIdSpin;
    QLabel *m_connectionStatusLabel;
    QPushButton *m_connectButton;
    QPushButton *m_saveConfigButton;
    QDoubleSpinBox *m_targetTempSpin;
    QLineEdit *m_currentTempEdit;
    QLabel *m_currentStatusLabel;
    QComboBox *m_actionCombo;
    QPushButton *m_executeButton;
    QPushButton *m_refreshStatusButton;
    QLineEdit *m_lastCommandEdit;
    QTextEdit *m_historyEdit;
};

#endif // OVENCONTROLDIALOG_H
