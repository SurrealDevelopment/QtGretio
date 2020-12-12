#include "gretiowebsocketclient.h"
#include <QtCore/QDebug>
#include <QJsonDocument>

QT_USE_NAMESPACE

//! [constructor]
GretioWebsocketClient::GretioWebsocketClient(const QUrl &url, bool debug, QObject *parent) : QObject(parent),
    m_url(url),
    m_debug(debug)
{


    connect(&m_webSocket, &QWebSocket::connected, this, &GretioWebsocketClient::onConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &GretioWebsocketClient::closed);
    connect(&m_webSocket, QOverload<const QList<QSslError>&>::of(&QWebSocket::sslErrors),
                this, &GretioWebsocketClient::onSslErrors);

    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [=](QAbstractSocket::SocketError error) {
        qDebug() << " Socket Error " << error;
    });

}

bool GretioWebsocketClient::writeMessage(QJsonObject json)
{
    //if (!m_webSocket.isValid()) return false;

    auto doc = QJsonDocument(json);
    QString msg;

    if (m_debug) {
        // Utf 8 encoded
        msg = doc.toJson(QJsonDocument::Indented).toStdString().data();
        //qDebug() << "Tx:" << msg;
    }
    else {
        msg = doc.toJson(QJsonDocument::Compact).toStdString().data();
    }

    m_webSocket.sendTextMessage(msg);

    return true;
}

void GretioWebsocketClient::connectNow()
{
    if (m_debug)
            qDebug() << "WebSocket client:" << m_url;
    m_webSocket.open(QUrl(this->m_url));
}

void GretioWebsocketClient::connectNow(QUrl &url)
{
    this->m_url = url;
    return this->connectNow();
}

void GretioWebsocketClient::close()
{
    qDebug() << "WebSocket closed";

    disconnect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &GretioWebsocketClient::onMessageReceived);

    disconnect(&pingTimer, &QTimer::timeout,
            this, &GretioWebsocketClient::ping);

    m_webSocket.close();
}

QSslCertificate GretioWebsocketClient::peerCert()
{
    return m_webSocket.sslConfiguration().peerCertificate();
}

//! [onConnected]
void GretioWebsocketClient::onConnected()
{
    qDebug() << "WebSocket connected";

    connect(&m_webSocket, &QWebSocket::textMessageReceived,
            this, &GretioWebsocketClient::onMessageReceived);

    connect(&pingTimer, &QTimer::timeout,
            this, &GretioWebsocketClient::ping);

    pingTimer.setInterval(5000);
    pingTimer.start();


    m_webSocket.sslConfiguration().peerCertificate();

    emit connected();
}
//! [onConnected]


//! [onTextMessageReceived]
void GretioWebsocketClient::onMessageReceived(QString message)
{
    // if (m_debug)
    //     qDebug() << "Message received:" << message;
    QJsonParseError e;
    auto doc = QJsonDocument::fromJson(message.toUtf8(), &e);

    if (e.error != QJsonParseError::NoError) {
        qDebug("Json parse error: %s", e.errorString().toStdString().data());
        m_webSocket.close(QWebSocketProtocol::CloseCodeProtocolError, "Not JSON"); // close now
        return;
    }

    // check if required "id" and "t" types exist.
    auto jsonObj = doc.object();

    if (!jsonObj.contains("id") || !jsonObj.contains("t")) {
        qDebug("Json parse error: Missing required id and t fields");
        m_webSocket.close(QWebSocketProtocol::CloseCodeProtocolError, "Missing required t and/or id field"); // close now

    }

    emit onJsonMessageReeived(jsonObj);

}

void GretioWebsocketClient::ping()
{
    // we need to send periodic messages so our connection doesnt timeout
    // some j2534 clients will have long perioids of inactivity
    // so this is required

    if (m_webSocket.isValid()) {
        m_webSocket.ping();
    }
}

void GretioWebsocketClient::onSslErrors(const QList<QSslError> &errors)
{
    /*
     * There is no way to really to authenicate the certs so all self signed certs need to be
     * accepted. There might be a better way of doing this, but this is secure enough for now.
     */

    bool okError = true;

    foreach (QSslError e, errors)
   {
       if (    e.error() == QSslError::HostNameMismatch ||
               e.error() == QSslError::UnableToGetLocalIssuerCertificate ||
               e.error() == QSslError::UnableToVerifyFirstCertificate) {

       } else {
           qDebug() << "ssl error: " << e;
           okError = false;
       }
   }

    if (okError) {
        m_webSocket.ignoreSslErrors();
    }
}




