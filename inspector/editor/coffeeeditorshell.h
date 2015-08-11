#ifndef COFFEEEDITORSHELL_H
#define COFFEEEDITORSHELL_H

#include "general/scripting/coffeebasescriptenvironment.h"
#include <QWidget>

class QMainWindow;

class CoffeeEditorShell : public CoffeeBaseScriptEnvironment
{
    Q_OBJECT
public:
    CoffeeEditorShell(QObject* parent);
};

template<class T> QScriptValue constructWidget(QScriptContext* ctxt, QScriptEngine* eng)
{
    QWidget* parent = qobject_cast<QWidget*>(ctxt->argument(0).toQObject());
    QWidget* o = new T(parent);
    return eng->newQObject(o,QScriptEngine::QtOwnership);
}
template<class QType> void defineWidgetScriptType(QScriptEngine* e){
    QScriptValue pdCt = e->newFunction(constructWidget<QType>);
    QScriptValue mo = e->newQMetaObject(&QType::staticMetaObject,pdCt);
    e->globalObject().setProperty(QType::staticMetaObject.className(),mo);
}

#endif // COFFEEEDITORSHELL_H
