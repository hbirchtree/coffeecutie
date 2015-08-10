#include "coffeescriptconstructors.h"

#include "engine/physics/genericphysicsinterface.h"
#include "engine/physics/physicsobject.h"
#include "engine/physics/physicsdescriptor.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include "general/filehandler.h"
#include "opengl/components/coffeetexture.h"
#include "engine/scripting/coffeeinputevent.h"
#include "engine/models/coffeeinstancecontainer.h"
#include "general/input/coffeeplayercontroller.h"
#include "engine/ai/coffeeneuralnet.h"

#include "engine/objects/coffeestandardobject.h"
#include "engine/objects/coffeeskybox.h"
#include "engine/objects/coffeeparticlesystem.h"
#include "opengl/components/coffeecamera.h"
#include "opengl/components/coffeeomnilight.h"
#include "opengl/components/coffeeworldopts.h"

#include "engine/rendering/coffeerendergraph.h"
#include "inspector/information/coffeesystemmonitor.h"

CoffeeScriptConstructors::CoffeeScriptConstructors()
{

}

void CoffeeScriptConstructors::defineConstructors(QScriptEngine &e)
{
    qRegisterMetaType<ScalarDataType>("ScalarDataType");
    qRegisterMetaType<VectorData*>("VectorData*");
    qRegisterMetaType<CoffeeTexture*>("CoffeeTexture*");
    qRegisterMetaType<CoffeePlayerController*>("CoffeePlayerController*");
    qRegisterMetaType<CoffeeInputEvent*>("CoffeeInputEvent*");

    //System-related
    defineQObjectScriptType<CoffeeSystemMonitor>(&e);

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
        QScriptValue pdCt = e.newFunction(scalarRandConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("RandomScalarValue",mo);
    }
    {
        QScriptValue pdCt = e.newFunction(quatValueConstructor);
        QScriptValue mo = e.newQMetaObject(&QObject::staticMetaObject,pdCt);
        e.globalObject().setProperty("QuatValue",mo);
    }
    //Physics
    defineQObjectScriptType<CoffeePhysicsEvent>(&e);
    defineQObjectScriptType<PhysicsObject>(&e);
    //AI objects
    defineQObjectScriptType<CoffeeNeuralNet>(&e);
    defineQMetaObject<CoffeeNeuron>(&e);
    //Input
    defineQObjectScriptType<CoffeePlayerController>(&e);
    defineQMetaObject<CoffeeInputEvent>(&e);

    //Internal objects
    defineQMetaObject<CoffeeTexture>(&e);
    defineQObjectScriptType<CoffeeOmniLight>(&e);
    defineQObjectScriptType<CoffeeCamera>(&e);
    defineQObjectScriptType<CoffeeWorldOpts>(&e);
    defineQObjectScriptType<CoffeeStandardObject>(&e);
    defineQObjectScriptType<CoffeeParticleSystem>(&e);

    defineQObjectScriptType<CoffeeSkybox>(&e);

    //Rendering
    defineQObjectScriptType<CoffeeRenderGraph>(&e);
}

QScriptValue CoffeeScriptConstructors::scalarRandConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();

    if(ctxt->argumentCount()<3){
        return ctxt->throwError("Invalid amount of arguments!");
    }

    float floor = ctxt->argument(1).toNumber();
    float ceil = ctxt->argument(2).toNumber();

    QObject* o = new ScalarValue(parent,floor,ceil);
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}

QScriptValue CoffeeScriptConstructors::scalarValueConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new ScalarValue(parent,0.f);
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}

QScriptValue CoffeeScriptConstructors::vector2ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new Vector2Value(parent,glm::vec2(0,0));
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}

QScriptValue CoffeeScriptConstructors::vector3ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new Vector3Value(parent,glm::vec3(0,0,0));
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}

QScriptValue CoffeeScriptConstructors::vector4ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new Vector4Value(parent,glm::vec4(0,0,0,0));
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}

QScriptValue CoffeeScriptConstructors::quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng){
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new QuatValue(parent,glm::quat(1,0,0,0));
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}

QScriptValue CoffeeScriptConstructors::coffeeSkyboxConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    if(ctxt->argumentCount()!=2){
        return ctxt->throwError("Mandatory camera not specified!");
    }
    CoffeeCamera* camera = qobject_cast<CoffeeCamera*>(ctxt->argument(1).toQObject());
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new CoffeeSkybox(parent,camera);
    return eng->newQObject(o,QScriptEngine::AutoOwnership);
}
