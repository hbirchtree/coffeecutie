#include "coffeescriptengine.h"


#include "qtscriptconstructors.h"

#include <QResizeEvent>
#include <QRegExp>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

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

CoffeeScriptEngine::CoffeeScriptEngine(QObject *parent) : QObject(parent),e(this)
{
    agent = new CoffeeScriptEngineAgent(this,&e);
    e.setAgent(agent);

    qRegisterMetaType<ScalarDataType>("ScalarDataType");
    qRegisterMetaType<VectorData*>("VectorData*");
    qRegisterMetaType<CoffeeTexture*>("CoffeeTexture*");
    qRegisterMetaType<CoffeePlayerController*>("CoffeePlayerController*");
    qRegisterMetaType<CoffeeInputEvent*>("CoffeeInputEvent*");

    CoffeeScriptConstructors::defineConstructors(e);
    QtScriptConstructors::defineConstructors(e);

    //Global meta-objects
    {
        QScriptValue MetaObj = e.newQMetaObject(&staticMetaObject);
        e.globalObject().setProperty("MOC",MetaObj);
    }
    ////////

    //Exported functions
    {
        //Read QVariantMap from file
        QScriptValue fun = e.newFunction(coffeeImportVariantMap);
        e.globalObject().setProperty("importVariantMap",fun);
    }
    {
        //Executes a script. Really.
        QScriptValue fun = e.newFunction(coffeeExecuteScriptFile);
        e.globalObject().setProperty("executeScript",fun);
    }
    {
        //Setting and checking parenting of QObjects
        QScriptValue fun = e.newFunction(coffeeParentingFunc);
        e.globalObject().setProperty("parenting",fun);
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
        QScriptValue mo = e.newQMetaObject(&CoffeeInputEvent::staticMetaObject);
        e.globalObject().setProperty("CoffeeInputEventType",mo);
    }
    {
        QScriptValue mo = e.newQMetaObject(&CoffeeNeuron::staticMetaObject);
        e.globalObject().setProperty("CoffeeNeuronType",mo);
    }
    ////////
}

QScriptEngine *CoffeeScriptEngine::getEngine()
{
    return &e;
}

CoffeeScriptEngineAgent *CoffeeScriptEngine::getAgent()
{
    return agent;
}

void CoffeeScriptEngine::execFile(QString file, bool *result, QString *logOut)
{
    execFile(&e,file,result,logOut);
}

QScriptValue CoffeeScriptEngine::execFile(QScriptEngine *e, QString file, bool *result, QString *logOut, QScriptContext* ctxt)
{
    QFileInfo fileTest(file);
    QFile script(file);
    QScriptValue out;
    if(!file.isEmpty()&&fileTest.exists()&&fileTest.isFile()&&script.open(QIODevice::ReadOnly)){
        QString src = script.readAll();
        src = importFile(fileTest,src);
        QScriptProgram p(src,file); //we might get use for these QScriptProgram objects, but for now we live it at this
        out = e->evaluate(p);

        if(logOut)
            *logOut = out.toString();
        if(result)
            *result = true;
    }else{
        if(result)
            *result = false;
    }
    return out;
}

void CoffeeScriptEngine::addObject(QObject *o)
{
    QScriptValue wrapper = e.newQObject(o);
    e.globalObject().setProperty(o->objectName(),wrapper);
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

    bool status = false;

    QScriptValue res = execFile(eng,file,&status,nullptr,ctxt);

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

QScriptValue CoffeeScriptEngine::coffeeExceptionFunc(QScriptContext *ctxt, QScriptEngine *eng)
{
    QScriptValue exceptionValue = eng->uncaughtException();

    if(exceptionValue.isUndefined()||eng->uncaughtExceptionLineNumber()<0){
        return ctxt->throwError("No uncaught exceptions");
    }

    QString err("Uncaught exception: %1\n"
                "Error on line: %2\n"
                "Backtrace:\n");
    for(QString l : eng->uncaughtExceptionBacktrace())
        err.append(l+"\n");

    err = err
            .arg(exceptionValue.toString())
            .arg(eng->uncaughtExceptionLineNumber());

    QScriptValue outputExceptionValue = eng->toScriptValue(err);

    CoffeeScriptException exObj;
    exObj.backtrace = eng->uncaughtExceptionBacktrace();
    exObj.self = exceptionValue;
    exObj.lineNumber = eng->uncaughtExceptionLineNumber();
    CoffeeScriptEngine* e = qobject_cast<CoffeeScriptEngine*>(eng->parent());
    if(e)
        e->uncaughtException(exObj);

    eng->clearExceptions();

    return outputExceptionValue;
}

QString CoffeeScriptEngine::importFile(const QFileInfo &srcFile,QString &src)
{
    QRegExp r;
    r.setPattern("^#inc \"(.*)\"$");
    for(QString l : src.split("\n"))
        if(r.indexIn(l)>=0){
            QString fileSrc = srcFile.dir().absolutePath()+QDir::separator()+r.cap(1);
            QString replace = FileHandler::getStringFromFile(fileSrc);
            if(!replace.isNull()){
                QFileInfo tFile(fileSrc);
                replace = importFile(tFile,replace);
                src.replace(l,replace);
            }else{
                qWarning("Failed to include contents from file: %s",fileSrc.toStdString().c_str());
            }
        }
    return src;
}

