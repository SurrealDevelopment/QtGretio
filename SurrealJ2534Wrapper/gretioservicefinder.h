#ifndef GRETIOSERVICEFINDER_H
#define GRETIOSERVICEFINDER_H

#include <QObject>
#include "qzeroconf.h"


class GretioServiceFinder : public QObject
{
    Q_OBJECT
public:
    explicit GretioServiceFinder(QObject *parent = nullptr);

    void findServices();
    void stopFindingServices();

signals:
    void onFoundService(QZeroConfService zcs);

private:
    QZeroConf zeroConf;
    bool isFinding = false;

private slots:
    void addService(QZeroConfService item);
    void error(QZeroConf::error_t);


};

#endif // GRETIOSERVICEFINDER_H
