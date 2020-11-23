#include "mainwindow.h"

#include <QApplication>
#include <QLibrary>
#include <QDebug>
#include "J2534.h"

void testJ2534() {
    unsigned long deviceId;
    char * name = nullptr;

    PassThruOpen(name, &deviceId);

    qDebug() << "J2534 OK";
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    testJ2534();

    return a.exec();
}


