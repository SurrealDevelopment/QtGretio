#ifndef GRETIODISCOVERYWINDOW_H
#define GRETIODISCOVERYWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include "qzeroconf.h"
#include <QLineEdit>
#include <QUrl>
#include <QStackedWidget>
#include <QLabel>


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

    void showProgressIndicator();

    void showAuthWidget(QString code);

    void addService(QZeroConfService zcs);

    void showError(QString error);

signals:
    void serviceSelected(QUrl url);

private:
    QTableWidget table;
    QList <QZeroConfService> services;
    QLineEdit connectEdit;
    QStackedWidget stack;
    QLabel authLabel;

    void makeDiscoveryWidget();
    void makeLoadingWidget();
    void makeAuthWidget();

    QWidget * discoveryWidget;
    QWidget * loadingWidget;
    QWidget * authWidget;

    void checkUrl();

private slots:
    void rowSelected(int x, int y);
    void rowDoubleClicked(int x, int y);

};

#endif // GRETIODISCOVERYWINDOW_H
