#include "mainwindow.h"

#include <QApplication>
#include <QLibrary>
#include <QDebug>
#include "J2534.h"
#include <QSettings>
#include <QLibrary>

//void testJ2534() {
//    unsigned long deviceId;
//    char * name = nullptr;
//
//    PassThruOpen(name, &deviceId);
//    qDebug() << "OPEN" << deviceId;
//
//    unsigned long channelId;
//
//
//    PassThruConnect(0L, ISO15765, 0L, 500000L, &channelId);
//
//
//    qDebug() << "J2534 OK";
//}

void testRemote() {
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\PassThruSupport.04.04\\Surreal Dev",QSettings::NativeFormat);
    auto lib = settings.value("FunctionLibrary").toString();

    qDebug() << "Loading lib at " << lib;

    QLibrary myLib(lib);



    if (!myLib.load()) {
        qErrnoWarning("Surreal J2534 DLL Failed to Load");
    }

    PTOPEN open = (PTOPEN) myLib.resolve("PassThruOpen");
    PTCONNECT connect = (PTCONNECT) myLib.resolve("PassThruConnect");


    if (open) {
        qDebug("Surreal J2534 DLL Resolved PTOPEN");
        unsigned long test = 0;
        connect(1,0,0,0,&test);
        qDebug("Surreal J2534 DLL Resolved PTOPEN FINISH");

    } else {
        qErrnoWarning("Surreal J2534 DLL Failed to Resolve PT OPEN");
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

   // testJ2534();
    testRemote();

    return a.exec();
}


