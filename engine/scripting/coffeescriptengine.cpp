#include "coffeescriptengine.h"

#include "coffeescriptconstructors.h"
#include "qtscriptconstructors.h"

#include <QResizeEvent>
#include <QRegExp>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

#include "general/filehandler.h"

CoffeeScriptEngine::CoffeeScriptEngine(QObject *parent) : CoffeeBaseScriptEnvironment(parent)
{
    CoffeeScriptConstructors::defineConstructors(*m_engine);
    QtScriptConstructors::defineConstructors(*m_engine);

    //Global meta-objects
    defineQMetaObjectByValue(m_engine,"Qt",&staticQtMetaObject);

    //Exported functions
    defineScriptFunction(m_engine,"importVariantMap",coffeeImportVariantMap,0);
    defineScriptFunction(m_engine,"executeScript",coffeeExecuteScriptFile,0);
    defineScriptFunction(m_engine,"parenting",coffeeParentingFunc,0);

    //Enums
    defineQMetaObject<QEvent>(m_engine);
}

QScriptValue CoffeeScriptEngine::coffeeImportVariantMap(QScriptContext *ctxt, QScriptEngine *eng)
{
    if(ctxt->argumentCount()!=1)
        return ctxt->throwError("Invalid amount of arguments!");
    QString file = ctxt->argument(0).toString();

    QFileInfo f(file);
    if(!f.exists())
        return ctxt->throwError(QString("File does not exist: %1").arg(file));

    QJsonParseError error;
    QVariantMap data =
            QJsonDocument::fromJson(FileHandler::getStringFromFile(file).toLocal8Bit(),&error)
            .object().toVariantMap();
    if(error.error!=QJsonParseError::NoError)
        return ctxt->throwError(QString("Error while importing: %1").arg(error.errorString()));

    return eng->toScriptValue(data);
}

QScriptValue CoffeeScriptEngine::coffeeExecuteScriptFile(QScriptContext *ctxt, QScriptEngine *eng)
{
    if(ctxt->argumentCount()<1){
        return ctxt->throwError("Invalid amount of arguments!");
    }
    QString file = ctxt->argument(0).toString();

    QScriptValue res = execFile(eng,file);

    return res;
}

QScriptValue CoffeeScriptEngine::coffeeParentingFunc(QScriptContext *ctxt, QScriptEngine *eng)
{
    if(ctxt->argumentCount()<1){
        return ctxt->throwError("Invalid amount of arguments!");
    }
    QObject* target = ctxt->argument(0).toQObject();

    if(!target)
        return ctxt->throwError("Not a valid pointer to QObject");

    if(ctxt->argumentCount()<2){
        QObject *parent = target->parent();
        return eng->toScriptValue(parent);
    }else{
        QObject* parent = ctxt->argument(1).toQObject();
        if(!parent)
            return ctxt->throwError("Not a valid pointer to QObject");
        target->setParent(parent);
        return eng->toScriptValue(parent==target->parent());
    }
}
