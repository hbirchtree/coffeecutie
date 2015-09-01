#ifndef COFFEESCRIPTFILEHANDLER_H
#define COFFEESCRIPTFILEHANDLER_H

#define SCRIPT_ALLOW_WRITE

#include <QObject>
#include <QScriptValue>
#include <QVariantMap>
#include <QVariantList>

class QJsonParseError;

class CoffeeScriptFileHandler : public QObject
{
    Q_OBJECT
public:
    CoffeeScriptFileHandler(QObject *parent);

public slots:
    static QString readTextFile(const QString &filename);

    static bool exists(const QString &filename);

    static bool isFile(const QString &filename);
    static bool isLink(const QString &filename);
    static bool isDir(const QString &filename);
};

class CoffeeScriptJsonHandler : public QObject
{
    Q_OBJECT
public:
    CoffeeScriptJsonHandler(QObject *parent);
private:
    static QJsonDocument fromText(const QString &src, QJsonParseError *err);
public slots:
    static QVariantMap toObject(const QString& source);
    static QVariantList toArray(const QString& source);
};

#endif // COFFEESCRIPTFILEHANDLER_H
