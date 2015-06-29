#include "coffeeresource.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>

CoffeeResource::CoffeeResource(QObject *parent, QString source) : QObject(parent)
{
    if(!checkSource(source))
        throw CoffeeResourceError(CoffeeResourceError::InvalidUrlError);
    m_source = source;
}

QString CoffeeResource::source() const
{
    return m_source;
}

CoffeeResource::ResourceType CoffeeResource::type() const
{
    return m_type;
}

bool CoffeeResource::remote() const
{
    return m_remote;
}

QByteArray *CoffeeResource::data()
{
    if(m_data.isNull())
        refresh();
    return &m_data;
}

void CoffeeResource::setSource(QString source)
{
    m_source = source;
    resourceChanged();
}

void CoffeeResource::setType(CoffeeResource::ResourceType type)
{
    if (m_type == type)
        return;

    m_type = type;
    emit typeChanged(type);
}

void CoffeeResource::setRemote(bool remote)
{
    if (m_remote == remote)
        return;

    m_remote = remote;
    emit remoteChanged(remote);
}

bool CoffeeResource::checkSource(QString u)
{
    Q_UNUSED(u)
    return true;
//    return u.startsWith("file://")||u.startsWith("http://");
}

void CoffeeResource::refresh()
{
    //reloads file, prompts all users to reload their data as well. for textures, this should unload and reload the new one for instance.
    if(remote())
        loadRemoteFile();
    else
        loadLocalFile();
    resourceChanged();
}

bool CoffeeResource::loadLocalFile()
{
    QFileInfo t(m_source);
    QFile f(m_source);
    if(!t.exists())
        throw CoffeeResourceError(CoffeeResourceError::MissingFileError);
    if(!f.open(QIODevice::ReadOnly))
        throw CoffeeResourceError(CoffeeResourceError::UnreadableFileError);
    m_data = f.readAll();
    qDebug() << "Loaded data from file";
    return true;
}

bool CoffeeResource::loadRemoteFile()
{
    throw CoffeeResourceError(CoffeeResourceError::Undefined); //unsupported for now
}

CoffeeResourceError::CoffeeResourceError(CoffeeResourceError::ResourceError e){
    t = e;
}

CoffeeResourceError::ResourceError CoffeeResourceError::type()
{
    return t;
}
