#include "filehandler.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QByteArray>

QStringList FileHandler::getStringListFromFile(QString file){
    QFile data(file);
    if(!data.exists()||!data.open(QIODevice::ReadOnly))
        return QStringList();
    QTextStream cts(&data);
    return cts.readAll().split("\n");
}

QString FileHandler::getStringFromFile(QString file){
    QFile data(file);
    if(!data.exists()||!data.open(QIODevice::ReadOnly))
        return QString();
    QTextStream cts(&data);
    return cts.readAll();
}

unsigned char *FileHandler::getDataFromFile(QString file){
    QFile data(file);
    if(!data.exists()||data.open(QIODevice::ReadOnly))
        return 0;
    unsigned char* outData = NULL;
    data.write((const char*)outData,data.size());
    data.close();
    return outData;
}

QByteArray *FileHandler::getBytesFromFile(QString file){
    QFile data(file);
    if(!data.exists()||data.open(QIODevice::ReadOnly))
        return 0;
    QByteArray* ba = new QByteArray(data.readAll());
    data.close();
    return ba;
}
