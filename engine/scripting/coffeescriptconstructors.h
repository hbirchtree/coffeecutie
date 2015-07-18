#ifndef COFFEESCRIPTCONSTRUCTORS_H
#define COFFEESCRIPTCONSTRUCTORS_H

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

#include <QObject>
#include <QtScript>

class CoffeeScriptConstructors
{
    CoffeeScriptConstructors();
public:

    static void defineConstructors(QScriptEngine &e);

    static QScriptValue neuralNetConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue physicsObjectConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue physicsDescConstructor(QScriptContext *ctxt, QScriptEngine *eng);

    static QScriptValue scalarRandConstructor(QScriptContext *ctxt, QScriptEngine *eng);

    static QScriptValue scalarValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector2ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector3ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector4ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);

    static QScriptValue coffeePhysEvConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeePlayerControllerConstructor(QScriptContext *ctxt, QScriptEngine *eng);

    static QScriptValue coffeeLightConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeCameraConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeObjectConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeSkyboxConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeParticlesConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeWorldConstructor(QScriptContext *ctxt, QScriptEngine *eng);
};

#endif // COFFEESCRIPTCONSTRUCTORS_H
