#include "qtscriptconstructors.h"

#include <QtScript>
#include <QPointF>
#include <QColor>

QtScriptConstructors::QtScriptConstructors()
{

}

void QtScriptConstructors::defineConstructors(QScriptEngine &e)
{
    qScriptRegisterMetaType(&e,pointToScript,pointFromScript);
    {
        QScriptValue pdCt = e.newFunction(qtimerConstructor);
        QScriptValue mo = e.newQMetaObject(&QTimer::staticMetaObject,pdCt);
        e.globalObject().setProperty("QTimer",mo);
    }
    {
        QScriptValue fun = e.newFunction(qcolorConstructor);
        e.globalObject().setProperty("QColor",fun);
    }
}

QScriptValue QtScriptConstructors::qtimerConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QObject* parent = ctxt->argument(0).toQObject();
    QObject* o = new QTimer(parent);
    return eng->newQObject(o,QScriptEngine::ScriptOwnership);
}

QScriptValue QtScriptConstructors::qcolorConstructor(QScriptContext *ctxt, QScriptEngine *eng)
{
    QColor o;
    if(ctxt->argumentCount()==1&&ctxt->argument(0).isString()){
        QString s = ctxt->argument(0).toString();
        if(QColor::isValidColor(s)){
            o = QColor(s);
        }else{
            return ctxt->throwError("Invalid color format!");
        }
    }
    return eng->toScriptValue(o);
}

QScriptValue QtScriptConstructors::pointToScript(QScriptEngine *eng, const QPointF &v)
{
    QScriptValue out = eng->newObject();
    out.setProperty("x",QScriptValue(eng,v.x()));
    out.setProperty("y",QScriptValue(eng,v.y()));
    return out;
}

void QtScriptConstructors::pointFromScript(const QScriptValue &v, QPointF &o)
{
    o.setX(v.property("x").toNumber());
    o.setY(v.property("y").toNumber());
}
