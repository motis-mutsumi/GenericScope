#ifndef OVENCONNECTION_H
#define OVENCONNECTION_H

#include <QObject>
#include <QByteArray>
#include <QString>

class ScopeUart;

class OvenConnection : public QObject
{
    Q_OBJECT

public:
    explicit OvenConnection(QObject *parent = nullptr);
    ~OvenConnection();

    bool open();
    bool open(const QString &port, int baudRate, int dataBits, int stopBits, const QString &parity, int timeout);
    void close();
    bool isConnected() const;
    bool sendCommand(const QByteArray &command);
    QString lastError() const;

signals:
    void connectionChanged(bool connected);
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);

private:
    void setLastError(const QString &error);

private:
    ScopeUart *m_uart;
    QString m_lastError;
};

#endif // OVENCONNECTION_H
