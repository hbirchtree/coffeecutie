#ifndef SCRIPTHELPERS
#define SCRIPTHELPERS

#include <QScriptEngine>
#include <QString>

//Template functions for QObject and QMetaObject handling in script environment
template<class T> QScriptValue constructObject(QScriptContext* ctxt, QScriptEngine* eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new T(parent);
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}

template<class QType> void defineQMetaObject(QScriptEngine* e,const QString& typeName){
    QScriptValue mo = e->newQMetaObject(&QType::staticMetaObject);
    e->globalObject().setProperty(typeName,mo);
}
template<class QType> void defineQMetaObject(QScriptEngine* e){
    defineQMetaObject<QType>(e,QType::staticMetaObject.className());
}

template<class QType> void defineQObjectScriptType(QScriptEngine* e,const QString& typeName){
    QScriptValue pdCt = e->newFunction(constructObject<QType>);
    QScriptValue mo = e->newQMetaObject(&QType::staticMetaObject,pdCt);
    e->globalObject().setProperty(typeName,mo);
}
template<class QType> void defineQObjectScriptType(QScriptEngine* e){
    defineQObjectScriptType<QType>(e,QType::staticMetaObject.className());
}

template<class QType> void defineObjectConstructor(QScriptEngine* e,
                                                   const QString& name,
                                                   QScriptEngine::FunctionSignature constructor)
{
    QScriptValue ct = e->newFunction(constructor);
    QScriptValue m = e->newQMetaObject(&QType::staticMetaObject,ct);
    e->globalObject().setProperty(name,m);
}

template<class QType> void defineObjectConstructor(QScriptEngine* e,
                                                   QScriptEngine::FunctionSignature constructor)
{
    defineObjectConstructor<QType>(e,QType::staticMetaObject.className(),constructor);
}

static void defineQMetaObjectByValue(QScriptEngine *e,const QString& name,const QMetaObject *ob){
    QScriptValue m = e->newQMetaObject(ob);
    e->globalObject().setProperty(name,m);
}

static void defineScriptFunction(QScriptEngine* e,const QString& name,
                                    QScriptEngine::FunctionSignature fun,int len){
    QScriptValue v = e->newFunction(fun,len);
    e->globalObject().setProperty(name,v);
}

#endif // SCRIPTHELPERS

