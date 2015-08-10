#ifndef COFFEESCRIPTENGINE_H
#define COFFEESCRIPTENGINE_H

#include "coffeescriptconstructors.h"
#include "coffeescriptengineagent.h"

class CoffeeScriptEngine : public QObject
{
    Q_OBJECT
public:
    CoffeeScriptEngine(QObject *parent);

    QScriptEngine *getEngine();
    CoffeeScriptEngineAgent* getAgent();

    static QScriptValue execFile(QScriptEngine* m_engine, QString file);
    void addObject(QObject* o);

public slots:
    void engine_execFile(const QString& filename);
    void engine_execCmd(const QString& program);

signals:
    void executionReturn(QString program, QString file, QString result);

private:
    QScriptEngine* m_engine;
    CoffeeScriptEngineAgent* m_agent;

    //Functions to export
    static QScriptValue coffeeImportVariantMap(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeExecuteScriptFile(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeParentingFunc(QScriptContext *ctxt, QScriptEngine *eng);
//    static QScriptValue coffeeExceptionFunc(QScriptContext *ctxt, QScriptEngine *eng);

    //ScriptEngine internal functions
    static QString importFile(const QFileInfo &srcFile, QString &src);

};

#endif // COFFEESCRIPTENGINE_H

