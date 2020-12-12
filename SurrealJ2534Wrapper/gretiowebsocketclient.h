#ifndef GRETIOWEBSOCKETCLIENT_H
#define GRETIOWEBSOCKETCLIENT_H

#include <QObject>
#include <QJsonObject>
#include <QtWebSockets/QWebSocket>
#include <QtNetwork/QSslError>
#include <QTimer>

QT_FORWARD_DECLARE_CLASS(QWebSocket)


class GretioWebsocketClient : public QObject
{
    Q_OBJECT
public:
    explicit GretioWebsocketClient(const QUrl &url, bool debug = false, QObject *parent = nullptr);

    bool writeMessage(QJsonObject json);

    void connectNow();
    void connectNow(QUrl &url);
    void close();

    QSslCertificate peerCert();


signals:
    void closed();
    void onJsonMessageReeived(QJsonObject json); // primary what we emit
    void connected();

private Q_SLOTS:
    void onConnected();
    void onMessageReceived(QString message);

    void ping();
    void onSslErrors(const QList<QSslError> &errors);

private:
    QWebSocket m_webSocket;
    QUrl m_url;
    bool m_debug;

    QTimer pingTimer;

};

#endif // GRETIOWEBSOCKETCLIENT_H
