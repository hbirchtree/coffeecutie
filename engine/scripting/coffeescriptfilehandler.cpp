#include "coffeescriptfilehandler.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QScriptEngine>

CoffeeScriptFileHandler::CoffeeScriptFileHandler(QObject *parent) : QObject(parent)
{
}

QString CoffeeScriptFileHandler::readTextFile(const QString &filename)
{
    QFile f(filename);
    if(!f.exists()||!f.open(QIODevice::ReadOnly))
        return QString();
    return QString(f.readAll());
}

bool CoffeeScriptFileHandler::exists(const QString &filename)
{
    return QFileInfo(filename).exists();
}

bool CoffeeScriptFileHandler::isFile(const QString &filename)
{
    return QFileInfo(filename).isFile();
}

bool CoffeeScriptFileHandler::isLink(const QString &filename)
{
    return QFileInfo(filename).isSymLink();
}

bool CoffeeScriptFileHandler::isDir(const QString &filename)
{
    return QFileInfo(filename).isDir();
}


CoffeeScriptJsonHandler::CoffeeScriptJsonHandler(QObject *parent) : QObject(parent)
{
}

QJsonDocument CoffeeScriptJsonHandler::fromText(const QString &src, QJsonParseError* err)
{
    return QJsonDocument::fromJson(src.toLocal8Bit(),err);
}

QVariantMap CoffeeScriptJsonHandler::toObject(const QString &source)
{
    QJsonParseError err;
    QJsonDocument t = fromText(source,&err);
    if(err.error!=QJsonParseError::NoError)
        return QVariantMap();
    return t.object().toVariantMap();
}

QVariantList CoffeeScriptJsonHandler::toArray(const QString &source)
{
    QJsonParseError err;
    QJsonDocument t = fromText(source,&err);
    if(err.error!=QJsonParseError::NoError)
        return QVariantList();
    return t.array().toVariantList();
}
