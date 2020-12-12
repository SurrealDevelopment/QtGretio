#include "gretiomainwindow.h"

#include <QGuiApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QLineEdit>
#include <QFormLayout>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>



GretioMainWindow::GretioMainWindow(QWidget *parent) : QMainWindow(parent)
{
    connect(&table, &QTableWidget::cellClicked, this, &GretioMainWindow::rowSelected);
    connect(&table, &QTableWidget::cellDoubleClicked, this, &GretioMainWindow::rowDoubleClicked);

    makeAuthWidget();
    makeDiscoveryWidget();
    makeLoadingWidget();
    setCentralWidget(&stack);

    setWindowTitle("Gretio Web Client");

}


void GretioMainWindow::showDiscovery()
{




    stack.setCurrentWidget(this->discoveryWidget);

    setWindowTitle("Device Selection");
    show();
}

void GretioMainWindow::showProgressIndicator()
{
    stack.setCurrentWidget(this->loadingWidget);

    show();
}


void GretioMainWindow::showAuthWidget(QString code)
{
    this->authLabel.setText(code);
    stack.setCurrentWidget(this->authWidget);

    show();
}

void GretioMainWindow::addService(QZeroConfService zcs)
{
    qint32 row;
    QTableWidgetItem *cell;

    row = table.rowCount();
    table.insertRow(row);
    cell = new QTableWidgetItem(zcs->name());
    table.setItem(row, 0, cell);
    cell = new QTableWidgetItem(zcs->ip().toString());
    table.setItem(row, 1, cell);
    table.resizeColumnsToContents();

    services.push_back(zcs);

}

void GretioMainWindow::makeDiscoveryWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;


    // Top row
    QFormLayout *formLayout = new QFormLayout;

    // connect button
    QPushButton *button = new QPushButton(tr("Connect"));
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    connect(button, &QPushButton::clicked, this, [=]() {
        this->checkUrl();
    });



    connectEdit.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    formLayout->addRow(button, &connectEdit);
    formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    mainLayout->addLayout(formLayout);
    mainLayout->setAlignment(formLayout, Qt::AlignTop);


    // table
    table.verticalHeader()->hide();
    table.horizontalHeader()->hide();
    table.setColumnCount(2);
    table.setEditTriggers(QAbstractItemView::NoEditTriggers);
    table.setSelectionBehavior(QAbstractItemView::SelectRows);
    table.setSelectionMode(QAbstractItemView::SingleSelection);
    table.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);



    mainLayout->addWidget(&table);



    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);
    widget->setMinimumSize(350,200);

    this->discoveryWidget = widget;

    stack.addWidget(widget);
}

void GretioMainWindow::makeLoadingWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    auto label = new QLabel(tr("Please wait..."));
    mainLayout->addWidget(label);
    mainLayout->setAlignment(label, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);

    this->loadingWidget = widget;

    stack.addWidget(widget);
}

void GretioMainWindow::makeAuthWidget()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    auto label = new QLabel(tr("Enter code below on device"));
    mainLayout->addWidget(label);
    mainLayout->setAlignment(label, Qt::AlignCenter);
    QGroupBox *group = new QGroupBox();

    QVBoxLayout *boxGroup = new QVBoxLayout();
    boxGroup->addWidget(&(this->authLabel));
    boxGroup->setAlignment(&(this->authLabel), Qt::AlignCenter);

    auto font = this->authLabel.font();
    font.setBold(true);
    font.setPointSize(24);
    this->authLabel.setFont(font);

    group->setLayout(boxGroup);
    mainLayout->addWidget(group);
    mainLayout->setAlignment(boxGroup, Qt::AlignCenter);


    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);

    this->authWidget = widget;

    stack.addWidget(widget);



}

void GretioMainWindow::checkUrl()
{
    QUrl url(connectEdit.text());

    if (!url.isValid()) {
        QMessageBox msgBox;
        msgBox.setText("URL is not valid.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    emit serviceSelected(url);
}

void GretioMainWindow::rowSelected(int x, int y)
{

    auto service = services[x];
    auto url = "wss://" + service->ip().toString() + ":" + QString::number(service->port()) + "/ws";

    this->connectEdit.setText(url);


}

void GretioMainWindow::rowDoubleClicked(int x, int y)
{
    this->rowSelected(x,y);
    this->checkUrl();

}
