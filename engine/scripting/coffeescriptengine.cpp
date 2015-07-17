#include "coffeescriptengine.h"

#include "qtscriptconstructors.h"

#include <QResizeEvent>
#include <QRegExp>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

CoffeeScriptEngine::CoffeeScriptEngine(QObject *parent) : QObject(parent)
{
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
        QScriptValue fun = e.newFunction(coffeeImportVariantMap);
        e.globalObject().setProperty("importVariantMap",fun);
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

void CoffeeScriptEngine::execFile(QString file, bool *result, QString *logOut)
{
    QFileInfo fileTest(file);
    QFile script(file);
    if(!file.isEmpty()&&fileTest.exists()&&fileTest.isFile()&&script.open(QIODevice::ReadOnly)){
        QString src = script.readAll();
        src = importFile(fileTest,src);
        QString out = e.evaluate(src).toString();
        if(logOut)
            *logOut = out;
        if(result)
            *result = true;
    }else{
        if(result)
            *result = false;
    }
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
