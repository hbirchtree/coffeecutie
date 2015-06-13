#include "coffeescriptengine.h"

#include "engine/physics/genericphysicsinterface.h"
#include "engine/physics/physicsobject.h"
#include "engine/physics/physicsdescriptor.h"

CoffeeScriptEngine::CoffeeScriptEngine(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<PhysicalPropertyClass::PhysicsProperty>("PhysicalPropertyClass::PhysicsProperty");
    qRegisterMetaType<QEvent::Type>("QEvent::Type");
    qRegisterMetaType<QObjectList>("QObjectList");

    //Global meta-objects
    {
        QScriptValue MetaObj = e.newQMetaObject(&staticMetaObject);
        e.globalObject().setProperty("MOC",MetaObj);
    }
    {
        QScriptValue QtMeta = e.newQMetaObject(&staticQtMetaObject);
        e.globalObject().setProperty("Qt",QtMeta);
    }
    ////////
    //Constructors
    {
        QScriptValue poCt = e.newFunction(physicsObjectConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,poCt);
        e.globalObject().setProperty("PhysicsObject",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(physicsDescConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("PhysicsDescriptor",mo);
    }
    ////////

    //Enums
    {
        QScriptValue mo = e.newQMetaObject(&PhysicsDescriptor::staticMetaObject);
        e.globalObject().setProperty("PhysicalShape",mo);
    }
    {
        QScriptValue mo = e.newQMetaObject(&QEvent::staticMetaObject);
        e.globalObject().setProperty("QEvent",mo);
    }
    {
        QScriptValue mo = e.newQMetaObject(&PhysicalPropertyClass::staticMetaObject);
        e.globalObject().setProperty("PhysicalProperty",mo);
    }
    ////////

}

QScriptEngine *CoffeeScriptEngine::getEngine()
{
    return &e;
}

QScriptValue CoffeeScriptEngine::physicsObjectConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new PhysicsObject(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptEngine::physicsDescConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new PhysicsDescriptor(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}
