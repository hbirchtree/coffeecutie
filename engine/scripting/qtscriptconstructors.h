#ifndef QTSCRIPTCONSTRUCTORS_H
#define QTSCRIPTCONSTRUCTORS_H

class QPointF;
class QScriptValue;
class QScriptContext;
class QScriptEngine;

class QtScriptConstructors
{
    QtScriptConstructors();
public:

    static void defineConstructors(QScriptEngine &e);

    //QTimer
    static QScriptValue qtimerConstructor(QScriptContext *ctxt, QScriptEngine *eng);

    //QPointF
    static QScriptValue pointToScript(QScriptEngine* eng, const QPointF &v);
    static void pointFromScript(const QScriptValue& v, QPointF &o);
};

#endif // QTSCRIPTCONSTRUCTORS_H

