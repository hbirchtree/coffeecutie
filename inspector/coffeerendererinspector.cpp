#include "coffeerendererinspector.h"
#include "ui_coffeerendererinspector.h"

#include <QDateTime>

CoffeeRendererInspector::CoffeeRendererInspector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoffeeRendererInspector)
{
    ui->setupUi(this);
    scene = new GraphPlotter(this);
    ui->graphView->setScene(scene);

    QStringList tableHeader;
    tableHeader << tr("Property") << tr("Data");
    ui->infoView->setHeaderLabels(tableHeader);
}

CoffeeRendererInspector::~CoffeeRendererInspector()
{
    delete scene;
    delete ui;
}

void CoffeeRendererInspector::plotGraph(float frametime)
{
    scene->addPlot(frametime*1000.0);
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
}

void CoffeeRendererInspector::on_plotResizer_valueChanged(int value)
{
    scene->setPlotSize(value);
}

void CoffeeRendererInspector::setSysInfoField(const QString &field, const QString &data)
{
    QTreeWidgetItem* item = nullptr;
    if(m_properties.contains(field)){
        item = m_properties.value(field);
    }else{
        item = new QTreeWidgetItem();
        item->setText(0,field);
        ui->infoView->addTopLevelItem(item);
        m_properties.insert(field,item);
    }
    if(!item)
        return;
    item->setText(1,data);
}
