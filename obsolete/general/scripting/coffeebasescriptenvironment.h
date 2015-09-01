#ifndef COFFEEBASESCRIPTENVIRONMENT_H
#define COFFEEBASESCRIPTENVIRONMENT_H

#include <QtScript>
#include "scripthelpers.h"

class CoffeeScriptEngineAgent;

class CoffeeBaseScriptEnvironment : public QObject
{
    Q_OBJECT
public:
    CoffeeBaseScriptEnvironment(QObject *parent);

    QScriptEngine* engine();
    virtual CoffeeScriptEngineAgent* agent();

signals:
    void executionReturn(QString program, QString file, QString result);

public slots:
    virtual void insertObject(const QString& name,QObject* object);
    virtual void insertObject(QObject* object);

    virtual void executeFile(const QString& filename, int arg0 = 0);
    virtual void executeCmd(const QString& program);

protected:
    QScriptEngine* m_engine = nullptr;

    //Internal function for executing script files
    static QScriptValue execFile(QScriptEngine* m_engine, QString file);

    static QString importFile(const QFileInfo &srcFile, QString &src);

private:
    CoffeeScriptEngineAgent* m_agent = nullptr;
};


#endif // COFFEEBASESCRIPTENVIRONMENT_H
