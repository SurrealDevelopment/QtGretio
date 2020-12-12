#include "gretioauthhandler.h"
#include <QFile>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDir>


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

    if (json["t"].toInt() == 2 && json["id"].toInt() == 80 && json["challengeCode"].isString()) {
        auto code = json["challengeCode"].toString();
        qDebug() << "Challenge Code: " << code;
        emit onAuthPending(code);
    }
    else if (json["t"].toInt() == 2 && json["id"].toInt() == 80 && json["token"].isString()
             && json["authResult"].toBool() == true && token.isNull()) {
        qDebug("Auth  was good. New Token.");
        this->token = json["token"].toString();
        this->addToken(this->pubkey, this->token);

        this->authComplete = true;

        emit onAuthSuccess();
    }
    else if (json["t"].toInt() == 2 && json["id"].toInt() == 80
            && json["authResult"].toBool() == true) {
        qDebug("Auth  was good");
        this->authComplete = true;

        emit onAuthSuccess();
    } else if(json["t"].toInt() == 2 && json["id"].toInt() == 80
              && json["authResult"].toBool() == false) {
        qDebug("Auth  was bad");
        this->authComplete = false;

        emit onAuthFail("Bad Auth");
    }


}

void GretioAuthHandler::onConnected()
{

    this->token = QString();
    // look at certificate of server
    auto cert = wsc->peerCert();

    if (cert.isNull() ) {
        qDebug() << "No cert";
        emit onAuthFail("No Certificate");
        return;
    }

    this->pubkey = cert.publicKey();

    auto token = getTokenForKey(cert.publicKey());

    if (token.isNull()) {
        qDebug() << "No token. Requesting auth";
        QJsonObject object
        {
            {"t", 1},
            {"id", 80},
            {"requestNewToken", true}
        };

        wsc->writeMessage(object);
    } else {
        this->token = token;
        QJsonObject object
        {
            {"t", 1},
            {"id", 80},
            {"token", token}
        };

        wsc->writeMessage(object);

    }

}

QJsonDocument readServersFile() {
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile file(path + "/servertoken.json");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Missing or could not open " << path;
        return QJsonDocument();
    }
    QByteArray bytes = file.readAll();
    file.close();
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson( bytes, &jsonError );

    if( jsonError.error != QJsonParseError::NoError )
    {
         qDebug() << "Reading servers json failed" << jsonError.errorString();
         return QJsonDocument();
    }
    return document;
}


void writeServerFiles(QJsonDocument doc) {
    auto path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    auto dir = QDir(path);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    QFile file(path + "/servertoken.json");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate| QIODevice::Text)) {
        qDebug() << "could not open to write " << path;
        return;
    }
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);
    QTextStream iStream( &file );
    iStream.setCodec( "utf-8" );
    iStream << bytes;
    file.close();
}




QString GretioAuthHandler::getTokenForKey(QSslKey key)
{
    auto json = readServersFile().object();
    QJsonValue lookup = json[QString::fromStdString(key.toPem().toStdString())];

    if (lookup.isNull() || !lookup.isString()) {
        return QString();
    } else {
        return lookup.toString();
    }

}

void GretioAuthHandler::addToken(QSslKey key, QString token)
{
    auto json = readServersFile().object();

    json[QString::fromStdString(key.toPem().toStdString())] = token;

    writeServerFiles(QJsonDocument(json));
}




bool GretioAuthHandler::isAuth()
{
    return this->authComplete;
}
