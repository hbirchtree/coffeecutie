#include "coffeeinspector.h"
#include "ui_coffeeinspector.h"

#include "coffeeobjectexplorer.h"

CoffeeInspector::CoffeeInspector(QWidget *parent,
                                 QObjectList engineRoot) :
    QWidget(parent),
    ui(new Ui::CoffeeInspector)
{
    ui->setupUi(this);

    m_explorer = new CoffeeObjectExplorer(this,engineRoot,ui->objectView);

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
    m_explorer->updateInformation();
}
