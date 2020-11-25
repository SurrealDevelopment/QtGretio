#include "gretiowebcontext.h"
#include "QEventLoop"
#include "QTimer"
#include "QDebug"

GretioWebContext::GretioWebContext(QObject *parent)
    : QObject(parent),
      wsc(QUrl(QStringLiteral("ws://localhost:57039/ws")), true, this),
      wsah(&wsc, this)

{
    connect(&wsc, &GretioWebsocketClient::connected, &wsah, &GretioAuthHandler::onConnected);

    connect(&wsc, &GretioWebsocketClient::closed, this, &GretioWebContext::closed);
    connect(&wsc, &GretioWebsocketClient::onJsonMessageReeived, &wsah, &GretioAuthHandler::inputMessage);


    wsc.connectNow();

}

GretioWebContext::~GretioWebContext()
{
    this->wsc.close();
}

bool GretioWebContext::sendReceiveCseq(QJsonObject toSend, long timeout, QJsonObject * message)
{
    // increment cseq
    auto cseq = this->cseq++;
    toSend["cseq"] = QJsonValue(this->cseq++);

    QTimer timer;
    QEventLoop loop;
    bool result = false;


    auto lambda =


    connect(&wsah, &GretioAuthHandler::onMessageToNext, &loop, [&loop, &result, cseq, message]( QJsonObject json) {

        if (json["cseq"].toInt(-1) == cseq) {
            // this is our message!
            result = true;
            *message = json; // copy message

            loop.quit(); // stop the loop
        }
        // else just keep going

    });
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));


    if (!this->wsc.writeMessage(toSend)) {
        qDebug() << "MESSAGE FAILED TO SEND";

    } else {
        timer.start(timeout + 1000L); // add one second to timeout for processing

        loop.exec();

    }


    disconnect(&timer, &QTimer::timeout, &loop, nullptr); // disconnect anyhting in loop

    return result;



}

void GretioWebContext::waitForOpenOrTimeout()
{
    QTimer timer;
    QEventLoop loop;

    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    connect(&wsah, &GretioAuthHandler::onAuthSuccess, &loop, &QEventLoop::quit);

    timer.start(5000L); // add one second to timeout for processing

    loop.exec();


}

void GretioWebContext::closed()
{
    qDebug() << "WS Closed";
}
