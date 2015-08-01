#include "coffeerendererinspector.h"
#include "ui_coffeerendererinspector.h"

#include "general/filehandler.h"

#include <QDateTime>

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
    tableHeader << "Property" << "Data";
    ui->infoView->setHeaderLabels(tableHeader);

    connect(renderer,
            SIGNAL(contextReportFrametime(float)),
            SLOT(plotGraph(float)),
            Qt::QueuedConnection);
    this->renderer = renderer;
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

    m_sysinfo.updateData();

    setSysInfoField("Framerate (FPS)",QString("%1 FPS").arg(1/frametime));
    setSysInfoField("Frametime (ms)",QString("%1ms").arg(frametime*1000.0));

    setSysInfoField("System threads",QString::number(m_sysinfo.getThreadCount()));
    setSysInfoField("Process priority",QString::number(m_sysinfo.getPriority()));

    setSysInfoField("Resident memory",QString("%1MB").arg(m_sysinfo.getResidentMemory()));
    setSysInfoField("Virtual memory",QString("%1MB").arg(m_sysinfo.getVirtualMemory()));

    {
        qint32 c = 0;
        qint32 t = 0;
        renderer->getVideoMemoryUsage(&c,&t);
        if(c!=0&&t!=0){
            setSysInfoField("Video memory",QString("%1MB/%2MB")
                            .arg((float)(t-c)/1024.f)
                            .arg((float)t/1024.f));
        }
    }

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


void CoffeeSystemInformation::updateData()
{
#if defined Q_OS_LINUX
    QStringList mdata = FileHandler::getStringFromFile("/proc/self/stat").split(' ');
    m_thrds = mdata.at(19).toInt();
    m_rss = mdata.at(23).toLongLong();
    m_vrt = mdata.at(22).toLongLong();
    m_priority = mdata.at(17).toInt();
#endif
}

double CoffeeSystemInformation::getVirtualMemory() const
{
    return m_vrt/1024.0;
}

double CoffeeSystemInformation::getResidentMemory() const
{
    return m_rss/1024.0;
}

quint16 CoffeeSystemInformation::getThreadCount() const
{
    return m_thrds;
}

qint8 CoffeeSystemInformation::getPriority() const
{
    return m_priority;
}
