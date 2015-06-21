#ifndef COFFEESCRIPTENGINE_H
#define COFFEESCRIPTENGINE_H

#include <QObject>
#include <QtScript>

class CoffeeScriptEngine : public QObject
{
    Q_OBJECT
public:
    CoffeeScriptEngine(QObject *parent);

    QScriptEngine *getEngine();

    void execFile(QString file, bool *result = nullptr, QString *logOut = nullptr);
    void addObject(QObject* o);

signals:

public slots:

private:
    QScriptEngine e;

    //QPointF
    static QScriptValue pointToScript(QScriptEngine* eng, const QPointF &v);
    static void pointFromScript(const QScriptValue& v, QPointF &o);

    static QScriptValue physicsObjectConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue physicsDescConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vectorValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vectorVariantConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeePhysEvConstructor(QScriptContext *ctxt, QScriptEngine *eng);
};

#endif // COFFEESCRIPTENGINE_H
