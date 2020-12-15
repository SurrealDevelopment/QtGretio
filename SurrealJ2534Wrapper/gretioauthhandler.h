#ifndef GRETIOAUTHHANDLER_H
#define GRETIOAUTHHANDLER_H

#include <QObject>
#include <QJsonObject>
#include "gretiowebsocketclient.h"
#include <QSslKey>

/**
 * @brief The GretioAuthHandler class is used as a pipeline member (connected).
 * It provides auth management before allowing messages to be emitted further
 *
 */
class GretioAuthHandler : public QObject
{
    Q_OBJECT
public:
    explicit GretioAuthHandler(GretioWebsocketClient * wsc, QObject *parent = nullptr);

    bool isAuth();


signals:

    void onAuthSuccess();
    void onAuthFail(QString reason);
    void onAuthPending(QString code);
    void onMessageToNext(QJsonObject json);

public slots:
    void inputMessage(QJsonObject json);
    void onConnected();

private:
    bool authComplete = false;
    GretioWebsocketClient * wsc;

    QString token;
    QSslKey pubkey;

    QString getTokenForKey(QSslKey key);
    void addToken(QSslKey key, QString token);


};

#endif // GRETIOAUTHHANDLER_H
