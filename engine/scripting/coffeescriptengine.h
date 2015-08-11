#ifndef COFFEESCRIPTENGINE_H
#define COFFEESCRIPTENGINE_H

#include "general/scripting/coffeebasescriptenvironment.h"
#include "coffeescriptengineagent.h"

class CoffeeScriptEngine : public CoffeeBaseScriptEnvironment
{
    Q_OBJECT
public:
    CoffeeScriptEngine(QObject *parent);

    //Functions to export
    static QScriptValue coffeeImportVariantMap(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeExecuteScriptFile(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeParentingFunc(QScriptContext *ctxt, QScriptEngine *eng);

};

#endif // COFFEESCRIPTENGINE_H

