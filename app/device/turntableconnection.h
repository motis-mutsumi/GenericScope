#ifndef TURNTABLECONNECTION_H
#define TURNTABLECONNECTION_H

#include <QObject>
#include <QByteArray>
#include <QString>

class ScopeUdp;

class TurntableConnection : public QObject
{
    Q_OBJECT

public:
    explicit TurntableConnection(QObject *parent = nullptr);
    ~TurntableConnection();

    bool open();
    bool open(const QString &remoteIp, int remotePort, int localPort);
    void close();
    bool isConnected() const;
    bool sendFrame(const QByteArray &frame);
    QString lastError() const;

signals:
    void connectionChanged(bool connected);
    void dataReceived(const QByteArray &data);
    void errorOccurred(const QString &error);

private:
    void setLastError(const QString &error);

private:
    ScopeUdp *m_udp;
    QString m_lastError;
};

#endif // TURNTABLECONNECTION_H
