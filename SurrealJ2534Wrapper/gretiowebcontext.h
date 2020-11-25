#ifndef GRETIOWEBCONTEXT_H
#define GRETIOWEBCONTEXT_H

#include <QObject>
#include "gretioauthhandler.h"
#include "gretiowebsocketclient.h"
#include "QNetworkAccessManager"



class GretioWebContext : public QObject
{
    Q_OBJECT
public:
    explicit GretioWebContext(QObject *parent = nullptr);

    ~GretioWebContext();


    bool sendReceiveCseq(QJsonObject toSend, long timeout, QJsonObject * message);

    void waitForOpenOrTimeout();

private:
    GretioWebsocketClient wsc;
    GretioAuthHandler wsah;

    QNetworkAccessManager netAccessManager;


protected:
    int cseq = 0;

private slots:
    void closed();


};

#endif // GRETIOWEBCONTEXT_H
