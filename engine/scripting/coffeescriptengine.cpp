#include "coffeescriptengine.h"

#include "engine/physics/genericphysicsinterface.h"
#include "engine/physics/physicsobject.h"
#include "engine/physics/physicsdescriptor.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include "general/shadervariant.h"
#include "opengl/components/coffeetexture.h"
#include "engine/scripting/coffeeinputevent.h"
#include "engine/models/coffeeinstancecontainer.h"

CoffeeScriptEngine::CoffeeScriptEngine(QObject *parent) : QObject(parent)
{

//    qRegisterMetaType<QEvent::Type>("QEvent::Type");
    qRegisterMetaType<ShaderVariant*>("ShaderVariant*");
    qRegisterMetaType<CoffeeTexture*>("CoffeeTexture*");
//    qRegisterMetaType<CoffeeInstanceData*>("CoffeeInstanceData*");

    qRegisterMetaType<CoffeeInputEvent*>("CoffeeInputEvent*");

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
    {
        QScriptValue pdCt = e.newFunction(vectorValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("VectorValue",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(quatValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("QuaternionValue",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(vectorVariantConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("VectorVariant",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(coffeePhysEvConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeePhysicsEvent",mo);
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
        QScriptValue mo = e.newQMetaObject(&CoffeeTexture::staticMetaObject);
        e.globalObject().setProperty("CoffeeTexture",mo);
    }
    {
        QScriptValue mo = e.newQMetaObject(&CoffeePhysicsEvent::staticMetaObject);
        e.globalObject().setProperty("PhysicsProperty",mo);
    }
    {
        QScriptValue mo = e.newQMetaObject(&VectorVariant::staticMetaObject);
        e.globalObject().setProperty("VectorVariantType",mo);
    }
    {
        QScriptValue mo = e.newQMetaObject(&CoffeeInputEvent::staticMetaObject);
        e.globalObject().setProperty("CoffeeInputEventType",mo);
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

QScriptValue CoffeeScriptEngine::vectorValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new VectorValue(new NumberContainer<glm::vec3>(parent,glm::vec3(0,0,0)));
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptEngine::quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new QuaternionValue(new NumberContainer<glm::quat>(parent,glm::quat(1,0,0,0)));
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptEngine::vectorVariantConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new VectorVariant(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptEngine::coffeePhysEvConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeePhysicsEvent(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}
