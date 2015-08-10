#ifndef COFFEESCRIPTCONSTRUCTORS_H
#define COFFEESCRIPTCONSTRUCTORS_H

#include <QObject>
#include <QtScript>

class CoffeeScriptConstructors
{
    CoffeeScriptConstructors();
public:
    static void defineConstructors(QScriptEngine &e);

    static QScriptValue scalarRandConstructor(QScriptContext *ctxt, QScriptEngine *eng);

    static QScriptValue scalarValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector2ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector3ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector4ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeSkyboxConstructor(QScriptContext *ctxt, QScriptEngine *eng);

private:
};

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

#endif // COFFEESCRIPTCONSTRUCTORS_H
