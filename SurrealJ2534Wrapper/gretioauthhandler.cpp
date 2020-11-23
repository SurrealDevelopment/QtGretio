#include "gretioauthhandler.h"

GretioAuthHandler::GretioAuthHandler(GretioWebsocketClient * wsc, QObject *parent): QObject(parent)
{
    this->wsc = wsc;
}

void GretioAuthHandler::inputMessage(QJsonObject json)
{
    if (this->authComplete) {
        // just emit
        emit onMessageToNext(json);
        return;
    }

    if (json["t"].toInt() == 2 && json["id"].toInt() == 80
            && json["authResult"].toBool() == true) {
        qDebug("Auth  was good");
        this->authComplete = true;
    }


}

void GretioAuthHandler::onConnected()
{
    QJsonObject object
    {
        {"t", 1},
        {"id", 80},
        {"token", this->ourToken}
    };

    wsc->writeMessage(object);


}

