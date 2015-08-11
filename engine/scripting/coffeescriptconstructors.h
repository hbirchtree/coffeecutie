#ifndef COFFEESCRIPTCONSTRUCTORS_H
#define COFFEESCRIPTCONSTRUCTORS_H

#include <QObject>
#include <QtScript>

class CoffeeScriptConstructors
{
    CoffeeScriptConstructors();
public:
    static void defineConstructors(QScriptEngine &e);

    static QScriptValue scalarRandConstructor(QScriptContext *ctxt, QScriptEngine *eng);

    static QScriptValue scalarValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector2ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector3ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vector4ValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);

};


#endif // COFFEESCRIPTCONSTRUCTORS_H
