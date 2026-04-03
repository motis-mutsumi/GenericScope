#ifndef COMMANDSENDDIALOG_H
#define COMMANDSENDDIALOG_H

#include <QDialog>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QSet>
#include <QByteArray>

class DeviceManager;
class QComboBox;
class QLineEdit;
class QLabel;
class QPushButton;
class QCheckBox;
class QPlainTextEdit;

class CommandSendDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CommandSendDialog(DeviceManager *deviceManager, QWidget *parent = nullptr);

private slots:
    void onProtocolChanged(int index);
    void onCommandChanged(int index);
    void onRefreshClicked();
    void onSendClicked();
    void onResendClicked();
    void onToggleFavoriteClicked();
    void onOnlyFavoriteToggled(bool checked);
    void onDeviceDataReceived(const QByteArray &data);
    void onClearRxLogClicked();

private:
    struct CommandItem {
        QString protocolName;
        QString commandName;
        QString payloadHex;
        QString description;
    };

    void setupUI();
    void setupConnections();
    void loadCommandsFromSettings();
    void rebuildProtocolList(const QString &preferredProtocol = QString());
    void rebuildCommandList(const QString &preferredCommand = QString());
    void applyCommandToEditor();
    QString commandKey(const QString &protocolName, const QString &commandName) const;
    QString toSpacedHex(const QByteArray &data) const;
    void loadFavoriteKeys();
    void saveFavoriteKeys() const;
    void loadSendHistory();
    void appendSendHistory(const QString &protocolName, const QString &commandName, const QString &payloadHex);
    bool sendHexPayload(const QString &payloadHex, QString *errorMsg = nullptr, int *byteCount = nullptr);
    QString normalizeHex(const QString &hex) const;
    bool isValidHex(const QString &hex) const;

private:
    DeviceManager *m_deviceManager;
    QVector<CommandItem> m_commands;
    QSet<QString> m_favoriteKeys;
    QStringList m_sendHistory; // format: "protocol|command|hex"

    QComboBox *m_protocolCombo;
    QComboBox *m_commandCombo;
    QCheckBox *m_onlyFavoriteCheck;
    QLineEdit *m_hexEdit;
    QLabel *m_descLabel;
    QLabel *m_statusLabel;
    QComboBox *m_historyCombo;
    QPushButton *m_refreshBtn;
    QPushButton *m_toggleFavoriteBtn;
    QPushButton *m_resendBtn;
    QPushButton *m_sendBtn;
    QPlainTextEdit *m_rxLogEdit;
    QPushButton *m_clearRxLogBtn;
};

#endif // COMMANDSENDDIALOG_H
