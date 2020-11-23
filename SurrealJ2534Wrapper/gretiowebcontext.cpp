#include "gretiowebcontext.h"

GretioWebContext::GretioWebContext(QObject *parent)
    : QObject(parent),
      wsc(QUrl(QStringLiteral("ws://localhost:57039/ws")), true, this),
      wsah(&wsc, this)

{
    connect(&wsc, &GretioWebsocketClient::connected, &wsah, &GretioAuthHandler::onConnected);
    connect(&wsc, &GretioWebsocketClient::onJsonMessageReeived, &wsah, &GretioAuthHandler::inputMessage);


    wsc.connectNow();

}
