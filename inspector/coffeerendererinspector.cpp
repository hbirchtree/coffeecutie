#include "coffeerendererinspector.h"
#include "ui_coffeerendererinspector.h"

#include "general/filehandler.h"

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

    fpsItem = new QTreeWidgetItem();
    fpsItem->setText(0,"Framerate (FPS)");
    frameTimeItem = new QTreeWidgetItem();
    frameTimeItem->setText(0,"Frametime");
    memoryUsageItem = new QTreeWidgetItem();
    memoryUsageItem->setText(0,"Memory usage");

    ui->infoView->addTopLevelItem(fpsItem);
    ui->infoView->addTopLevelItem(frameTimeItem);
    ui->infoView->addTopLevelItem(memoryUsageItem);

    connect(renderer,SIGNAL(contextReportFrametime(float)),SLOT(plotGraph(float)),Qt::QueuedConnection);
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
    measureTime=QDateTime::currentMSecsSinceEpoch()+checkInterval;

    fpsItem->setText(1,QString("%1 FPS").arg(1/frametime));
    frameTimeItem->setText(1,QString("%1ms").arg(frametime*1000.0));

#ifdef Q_OS_LINUX
//        QStringList pdata = FileHandler::getStringFromFile("/proc/self/stat").split(' ');
        QStringList mdata = FileHandler::getStringFromFile("/proc/self/statm").split(' ');
        if(mdata.size()>2)
            memoryUsageItem->setText(1,QString("mem:%1MB,res:%2MB").
                                     arg(mdata.at(0).toDouble()/1024.0).
                                     arg(mdata.at(1).toDouble()/1024.0));
#else
    memoryUsageItem->setText(1,"[unsupported feature]");
#endif

    scene->addPlot(frametime*1000.0);
    scene->invalidate(QRect(),QGraphicsScene::ForegroundLayer);
}

void CoffeeRendererInspector::on_plotResizer_valueChanged(int value)
{
    scene->setPlotSize(value);
}
