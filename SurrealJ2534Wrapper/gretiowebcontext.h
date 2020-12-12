#ifndef GRETIOWEBCONTEXT_H
#define GRETIOWEBCONTEXT_H

#include <QObject>
#include "gretioauthhandler.h"
#include "gretiowebsocketclient.h"
#include "QNetworkAccessManager"
#include "gretioservicefinder.h"
#include "gretiomainwindow.h"

#define NOT_CONNECTED 0
#define WAIT_AUTH 1
#define CONNECTED 2
#define CONNECTING 3

class GretioWebContext : public QObject
{
    Q_OBJECT
public:
    explicit GretioWebContext(QObject *parent = nullptr);

    ~GretioWebContext();


    bool sendReceiveCseq(QJsonObject toSend, long timeout, QJsonObject * message);

    bool waitForOpen();

private:
    GretioMainWindow window;
    GretioServiceFinder gsf;

    GretioWebsocketClient wsc;
    GretioAuthHandler wsah;

    QNetworkAccessManager netAccessManager;

    int connectionStatus = NOT_CONNECTED;

signals:
    void onConnectionStatusChange(int status);


protected:
    int cseq = 0;

private slots:
    void closed();
    void onFoundService(QZeroConfService zcs);


};

#endif // GRETIOWEBCONTEXT_H
