#include "coffeerendererinspector.h"
#include "ui_coffeerendererinspector.h"

CoffeeRendererInspector::CoffeeRendererInspector(QWidget *parent, CoffeeRenderer *renderer) :
    QWidget(parent),
    ui(new Ui::CoffeeRendererInspector)
{
    ui->setupUi(this);
    scene = new GraphPlotter(this);
    ui->graphView->setScene(scene);

    clearColor.setRedF(0);
    clearColor.setBlueF(0);
    clearColor.setGreenF(0);
    clearColor.setAlphaF(0.40);

    last.setX(0);
    last.setY(0);

    QStringList tableHeader;
    tableHeader << "Value" << "Data";
    ui->infoView->setHeaderLabels(tableHeader);

    connect(renderer,SIGNAL(contextReportFrametime(float)),SLOT(plotGraph(float)),Qt::QueuedConnection);
    connect(renderer,&CoffeeRenderer::winMouseGrabbed,[=](QEvent ev){
        QTreeWidgetItem* it = new QTreeWidgetItem();
        it->setText(0,"Mouse grabbing");
        it->setText(1,ev.type()==QEvent::GrabMouse ? "true" : "false");
        ui->infoView->addTopLevelItem(it);
    });
}

CoffeeRendererInspector::~CoffeeRendererInspector()
{
    delete scene;
    delete ui;
}

void CoffeeRendererInspector::plotGraph(float frametime)
{
    if(!this->isVisible()||QDateTime::currentMSecsSinceEpoch()<measureTime)
        return;
    measureTime=QDateTime::currentMSecsSinceEpoch()+100;

    scene->addPlot(frametime*1000.0);
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
}

void CoffeeRendererInspector::on_plotResizer_valueChanged(int value)
{
    scene->setPlotSize(value);
}
