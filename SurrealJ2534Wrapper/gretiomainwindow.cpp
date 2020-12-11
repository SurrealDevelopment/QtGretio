#include "gretiomainwindow.h"

#include <QGuiApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QLineEdit>
#include <QFormLayout>


GretioMainWindow::GretioMainWindow(QWidget *parent) : QMainWindow(parent)
{

}

void GretioMainWindow::showDiscovery()
{


    QVBoxLayout *mainLayout = new QVBoxLayout;


    // Top row
    QFormLayout *formLayout = new QFormLayout;

    // connect button
    QPushButton *button = new QPushButton(tr("Connect"));
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    QLineEdit *txt = new QLineEdit();
    txt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    formLayout->addRow(button, txt);
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


    connect(&table, &QTableWidget::cellDoubleClicked, this, &GretioMainWindow::rowSelected);

    mainLayout->addWidget(&table);

    QWidget *widget = new QWidget;
    widget->setLayout(mainLayout);
    widget->setMinimumSize(350,200);


    setCentralWidget(widget);
    setWindowTitle("Gretio Device Selection");
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

}

void GretioMainWindow::rowSelected(int x, int y)
{

}
