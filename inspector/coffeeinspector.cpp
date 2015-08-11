#include "coffeeinspector.h"
#include "ui_coffeeinspector.h"

#include "coffeeobjectexplorer.h"

CoffeeInspector::CoffeeInspector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoffeeInspector)
{
    ui->setupUi(this);

    QStringList headers;
    headers << "Property" << "Value";
    ui->objectView->setHeaderLabels(headers);

    refreshTimer = new QTimer();
    connect(refreshTimer,SIGNAL(timeout()),SLOT(updateInformation()));
    refreshTimer->setInterval(1000);
    refreshTimer->start();
}

CoffeeInspector::~CoffeeInspector()
{
    delete refreshTimer;
    delete ui;
}

void CoffeeInspector::updateInformation()
{
    if(m_explorer)
        m_explorer->updateInformation();
}

void CoffeeInspector::setExplorer(CoffeeObjectExplorer *explorer)
{
    explorer->setTarget(ui->objectView);
    m_explorer = explorer;
}

