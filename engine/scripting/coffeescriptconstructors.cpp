#include "coffeescriptconstructors.h"

CoffeeScriptConstructors::CoffeeScriptConstructors()
{

}

void CoffeeScriptConstructors::defineConstructors(QScriptEngine &e)
{
    //Data storage
    {
        QScriptValue pdCt = e.newFunction(vector2ValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("Vector2Value",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(vector3ValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("Vector3Value",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(vector4ValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("Vector4Value",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(scalarValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("ScalarValue",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(quatValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("QuatValue",mo);
    }
    //Physics
    {
        QScriptValue pdCt = e.newFunction(coffeePhysEvConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeePhysicsEvent",mo);
    }
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
    //AI objects
    {
        QScriptValue pdCt = e.newFunction(neuralNetConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeeNeuralNet",mo);
    }
    //Input
    {
        QScriptValue pdCt = e.newFunction(coffeePlayerControllerConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeePlayerController",mo);
    }
    //Internal objects
    {
        QScriptValue pdCt = e.newFunction(coffeeLightConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeeOmniLight",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(coffeeCameraConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeeCamera",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(coffeeWorldConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeeWorldOpts",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(coffeeObjectConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeeStandardObject",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(coffeeSkyboxConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeeSkybox",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(coffeeParticlesConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("CoffeeParticleSystem",mo);
    }
}

QScriptValue CoffeeScriptConstructors::neuralNetConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeNeuralNet(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::physicsObjectConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new PhysicsObject(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::physicsDescConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new PhysicsDescriptor(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::scalarValueConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new ScalarValue(parent,0.f);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::vector2ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new Vector2Value(parent,glm::vec2(0,0));
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::vector3ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new Vector3Value(parent,glm::vec3(0,0,0));
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::vector4ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new Vector4Value(parent,glm::vec4(0,0,0,0));
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new QuatValue(parent,glm::quat(1,0,0,0));
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::vectorVariantConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new VectorVariant(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeePhysEvConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeePhysicsEvent(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeePlayerControllerConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeePlayerController(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeeLightConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeOmniLight(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeeCameraConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeCamera(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeeObjectConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeStandardObject(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeeSkyboxConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    if(ctxt->argumentCount()!=2){
        return ctxt->throwError("Mandatory camera not specified!");
    }
    CoffeeCamera* camera = qobject_cast<CoffeeCamera*>(ctxt->argument(1).toQObject());
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeSkybox(parent,camera);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeeParticlesConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeParticleSystem(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeeWorldConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeWorldOpts(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}
