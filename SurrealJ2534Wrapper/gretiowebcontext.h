#ifndef GRETIOWEBCONTEXT_H
#define GRETIOWEBCONTEXT_H

#include <QObject>
#include "gretioauthhandler.h"
#include "gretiowebsocketclient.h"

class GretioWebContext : public QObject
{
    Q_OBJECT
public:
    explicit GretioWebContext(QObject *parent = nullptr);

private:
    GretioWebsocketClient wsc;
    GretioAuthHandler wsah;


};

#endif // GRETIOWEBCONTEXT_H
