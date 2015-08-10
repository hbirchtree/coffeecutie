#include "coffeescriptengine.h"


#include "qtscriptconstructors.h"

#include <QResizeEvent>
#include <QRegExp>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

//#include "engine/physics/genericphysicsinterface.h"
//#include "engine/physics/physicsobject.h"
//#include "engine/physics/physicsdescriptor.h"
//#include "engine/scripting/qscriptvectorvalue.h"
#include "general/filehandler.h"
//#include "opengl/components/coffeetexture.h"
//#include "engine/scripting/coffeeinputevent.h"
//#include "engine/models/coffeeinstancecontainer.h"
//#include "general/input/coffeeplayercontroller.h"
//#include "engine/ai/coffeeneuralnet.h"

//#include "engine/objects/coffeestandardobject.h"
//#include "engine/objects/coffeeskybox.h"
//#include "engine/objects/coffeeparticlesystem.h"
//#include "opengl/components/coffeecamera.h"
//#include "opengl/components/coffeeomnilight.h"
//#include "opengl/components/coffeeworldopts.h"

//#include "engine/rendering/coffeerendergraph.h"

CoffeeScriptEngine::CoffeeScriptEngine(QObject *parent) : QObject(parent)
{
    m_engine = new QScriptEngine(this);
    m_agent = new CoffeeScriptEngineAgent(this,m_engine);
    m_engine->setAgent(m_agent);

    CoffeeScriptConstructors::defineConstructors(*m_engine);
    QtScriptConstructors::defineConstructors(*m_engine);

    //Global meta-objects
    {
        QScriptValue MetaObj = m_engine->newQMetaObject(&staticQtMetaObject);
        m_engine->globalObject().setProperty("Qt",MetaObj);
    }
    {
        QScriptValue MetaObj = m_engine->newQMetaObject(&staticMetaObject);
        m_engine->globalObject().setProperty("MOC",MetaObj);
    }
    ////////

    //Exported functions
    {
        //Read QVariantMap from file
        QScriptValue fun = m_engine->newFunction(coffeeImportVariantMap);
        m_engine->globalObject().setProperty("importVariantMap",fun);
    }
    {
        //Executes a script. Really.
        QScriptValue fun = m_engine->newFunction(coffeeExecuteScriptFile);
        m_engine->globalObject().setProperty("executeScript",fun);
    }
    {
        //Setting and checking parenting of QObjects
        QScriptValue fun = m_engine->newFunction(coffeeParentingFunc);
        m_engine->globalObject().setProperty("parenting",fun);
    }
    ////////

    //Enums
    {
        QScriptValue mo = m_engine->newQMetaObject(&QEvent::staticMetaObject);
        m_engine->globalObject().setProperty("QEvent",mo);
    }
    ////////
}

QScriptEngine *CoffeeScriptEngine::getEngine()
{
    return m_engine;
}

CoffeeScriptEngineAgent *CoffeeScriptEngine::getAgent()
{
    return m_agent;
}

void CoffeeScriptEngine::engine_execFile(const QString &filename)
{
    QScriptValue result = execFile(this->m_engine,filename);
    executionReturn(QString(),filename,result.toString());
}

void CoffeeScriptEngine::engine_execCmd(const QString &program)
{
    QScriptValue result = this->m_engine->evaluate(program);
    executionReturn(program,QString(),result.toString());
}

QScriptValue CoffeeScriptEngine::execFile(QScriptEngine *e, QString file)
{
    QFileInfo fileTest(file);
    QFile script(file);
    QScriptValue out;
    if(!file.isEmpty()&&
            fileTest.exists()&&
            fileTest.isFile()&&
            script.open(QIODevice::ReadOnly)){
        QString src = script.readAll();
        src = importFile(fileTest,src);
        QScriptProgram p(src,file); //we might get use for these QScriptProgram objects, but for now we live it at this
        out = e->evaluate(p);
    }
    return out;
}

void CoffeeScriptEngine::addObject(QObject *o)
{
    QScriptValue wrapper = m_engine->newQObject(o);
    m_engine->globalObject().setProperty(o->objectName(),wrapper);
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

//QScriptValue CoffeeScriptEngine::coffeeExceptionFunc(QScriptContext *ctxt, QScriptEngine *eng)
//{
//    QScriptValue exceptionValue = eng->uncaughtException();

//    if(exceptionValue.isUndefined()||eng->uncaughtExceptionLineNumber()<0){
//        return ctxt->throwError("No uncaught exceptions");
//    }

//    QString err("Uncaught exception: %1\n"
//                "Error on line: %2\n"
//                "Backtrace:\n");
//    for(QString l : eng->uncaughtExceptionBacktrace())
//        err.append(l+"\n");

//    err = err
//            .arg(exceptionValue.toString())
//            .arg(eng->uncaughtExceptionLineNumber());

//    QScriptValue outputExceptionValue = eng->toScriptValue(err);

//    CoffeeScriptException exObj;
//    exObj.backtrace = eng->uncaughtExceptionBacktrace();
//    exObj.self = exceptionValue;
//    exObj.lineNumber = eng->uncaughtExceptionLineNumber();
//    CoffeeScriptEngine* e = qobject_cast<CoffeeScriptEngine*>(eng->parent());
//    if(e)
//        e->uncaughtException(exObj);

//    eng->clearExceptions();

//    return outputExceptionValue;
//}

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
