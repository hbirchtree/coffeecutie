#include "coffeesystemmonitor.h"

#include "opengl/context/coffeerenderer.h"
#include "general/filehandler.h"

CoffeeSystemMonitor::CoffeeSystemMonitor(QObject *parent) : QObject(parent)
{

}

void CoffeeSystemMonitor::updateData()
{
#if defined Q_OS_LINUX
    QStringList mdata = FileHandler::getStringFromFile("/proc/self/stat").split(' ');
    m_thrds = mdata.at(19).toInt();
    m_rss = mdata.at(23).toLongLong();
    m_vrt = mdata.at(22).toLongLong();
    m_priority = mdata.at(17).toInt();
#endif
}

double CoffeeSystemMonitor::getVirtualMemory() const
{
    return m_vrt/1024.0;
}

double CoffeeSystemMonitor::getResidentMemory() const
{
    return m_rss/1024.0;
}

quint16 CoffeeSystemMonitor::getThreadCount() const
{
    return m_thrds;
}

qint16 CoffeeSystemMonitor::getPriority() const
{
    return m_priority;
}

QString CoffeeSystemMonitor::interpretVideoMemory(CoffeeRenderer *ren)
{
    if(!ren)
        return "Unavailable";

    qint32 c = 0;
    qint32 t = 0;
    ren->getVideoMemoryUsage(&c,&t);
    return QString("%1MB/%2MB")
            .arg((float)(t-c)/1024.f)
            .arg((float)t/1024.f);
}

void CoffeeSystemMonitor::setRenderer(CoffeeRenderer *renderer)
{
    m_renderer = renderer;
}

void CoffeeSystemMonitor::refresh()
{
    this->updateData();
}
