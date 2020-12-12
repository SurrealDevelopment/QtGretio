#include "gretiowebcontext.h"
#include "QEventLoop"
#include "QTimer"
#include "QDebug"

GretioWebContext::GretioWebContext(QObject *parent)
    : QObject(parent),
      gsf(this)      ,
      wsc(QUrl(QStringLiteral("ws://localhost:57039/ws")), true, this),
      wsah(&wsc, this)

{
    connect(&gsf, &GretioServiceFinder::onFoundService, this, &GretioWebContext::onFoundService);

    connect(&wsc, &GretioWebsocketClient::connected, &wsah, &GretioAuthHandler::onConnected);
    connect(&wsc, &GretioWebsocketClient::connected, this, [=]() {
        this->connectionStatus = WAIT_AUTH;
        emit onConnectionStatusChange(WAIT_AUTH);
    });

    connect(&wsc, &GretioWebsocketClient::closed, this, &GretioWebContext::closed);
    connect(&wsc, &GretioWebsocketClient::onJsonMessageReeived, &wsah, &GretioAuthHandler::inputMessage);

    connect(&wsah, &GretioAuthHandler::onAuthFail, this, [=]() {
        wsc.close();
    });

    connect(&wsah, &GretioAuthHandler::onAuthPending, this, [=](QString code) {
        window.showAuthWidget(code);
    });

    connect(&wsah, &GretioAuthHandler::onAuthSuccess, this, [=]() {
        this->connectionStatus = CONNECTED;
        emit onConnectionStatusChange(CONNECTED);
        window.hide();

    });


    // begin discovery immediately
    gsf.findServices();

    window.showDiscovery();

    connect(&window, &GretioMainWindow::serviceSelected, this, [=](QUrl url) {
        //window.showProgressIndicator();
        wsc.connectNow(url);
        this->connectionStatus = CONNECTING;
        emit onConnectionStatusChange(CONNECTING);


    });


}

GretioWebContext::~GretioWebContext()
{
    this->wsc.close();
}

bool GretioWebContext::sendReceiveCseq(QJsonObject toSend, long timeout, QJsonObject * message)
{
    // increment cseq
    auto cseq = this->cseq++;
    toSend["cseq"] = QJsonValue(cseq);

    QTimer timer;
    QEventLoop loop;
    bool result = false;


    connect(&wsah, &GretioAuthHandler::onMessageToNext, &loop, [&loop, &result, cseq, &message]( QJsonObject json) {

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

bool GretioWebContext::waitForOpen()
{
    QTimer timer;
    QEventLoop loop;

    // wait forever for auth to finish or some failure
    connect(&wsc, &GretioWebsocketClient::closed, &loop, &QEventLoop::quit);
    connect(&wsah, &GretioAuthHandler::onAuthSuccess, &loop, &QEventLoop::quit);
    connect(&wsah, &GretioAuthHandler::onAuthFail, &loop, &QEventLoop::quit);

    loop.exec();
    disconnect(&loop);

    return connectionStatus == CONNECTED;

}

void GretioWebContext::closed()
{
    qDebug() << "WS Closed";
    this->connectionStatus = NOT_CONNECTED;
    emit onConnectionStatusChange(NOT_CONNECTED);
}

void GretioWebContext::onFoundService(QZeroConfService zcs)
{
    window.addService(zcs);
}
