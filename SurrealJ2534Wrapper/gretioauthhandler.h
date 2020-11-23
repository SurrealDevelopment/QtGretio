#ifndef GRETIOAUTHHANDLER_H
#define GRETIOAUTHHANDLER_H

#include <QObject>
#include <QJsonObject>
#include "gretiowebsocketclient.h"

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


signals:
    void onAuthSuccess();
    void onAuthFail();
    void onAuthPending();
    void onMessageToNext(QJsonObject json);

public slots:
    void inputMessage(QJsonObject json);
    void onConnected();

private:
    QString ourToken = "PTLPAPI";
    bool authComplete = false;
    GretioWebsocketClient * wsc;


};

#endif // GRETIOAUTHHANDLER_H
