#ifndef FILEHANDLER
#define FILEHANDLER

#include "general/common.h"
#include <QFile>
#include <QTextStream>

class FileHandler {
public:
    static QStringList getStringListFromFile(QString file){
        QFile data(file);
        if(!data.exists()||!data.open(QIODevice::ReadOnly))
            return QStringList();
        QTextStream cts(&data);
        return cts.readAll().split("\n");
    }
    static QString getStringFromFile(QString file){
        QFile data(file);
        if(!data.exists()||!data.open(QIODevice::ReadOnly))
            return QString();
        QTextStream cts(&data);
        return cts.readAll();
    }
    static unsigned char* getDataFromFile(QString file){
        QFile data(file);
        if(!data.exists()||data.open(QIODevice::ReadOnly))
            return 0;
        unsigned char* outData = NULL;
        data.write((const char*)outData,data.size());
        data.close();
        return outData;
    }
};

#endif // FILEHANDLER

