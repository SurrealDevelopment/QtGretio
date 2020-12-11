#include "gretioservicefinder.h"

GretioServiceFinder::GretioServiceFinder(QObject *parent) : QObject(parent)
{
    connect(&zeroConf, &QZeroConf::serviceAdded, this, &GretioServiceFinder::addService);
}

void GretioServiceFinder::findServices()
{
    if (!isFinding) {
        qDebug("Starting to find services...");
        isFinding = true;
        zeroConf.startBrowser("_gretiowebservice._tcp", QAbstractSocket::AnyIPProtocol);
    }
}

void GretioServiceFinder::stopFindingServices()
{
    if (isFinding) {
        isFinding = false;
        zeroConf.stopBrowser();
    }
}

void GretioServiceFinder::addService(QZeroConfService item)
{
    qDebug() << "Found Service " << item->name();
    emit onFoundService(item);
}

void GretioServiceFinder::error(QZeroConf::error_t)
{

}
