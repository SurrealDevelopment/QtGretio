#ifndef GRETIODISCOVERYWINDOW_H
#define GRETIODISCOVERYWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "qzeroconf.h"


/**
 * @brief The GretioDiscoveryWindow aids the user
 * in discovering a gretio server on its network using
 * dns service discovery or by  specifiyng manually
 */
class GretioMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GretioMainWindow(QWidget *parent = nullptr);

    void showDiscovery();

    void addService(QZeroConfService zcs);

signals:

    void serviceSelected(QZeroConfService zcs);

private:
    QTableWidget table;

private slots:
    void rowSelected(int x, int y);

};

#endif // GRETIODISCOVERYWINDOW_H
