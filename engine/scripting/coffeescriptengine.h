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

signals:

public slots:

private:
    QScriptEngine e;

    static QScriptValue physicsObjectConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue physicsDescConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue vectorValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue quatValueConstructor(QScriptContext *ctxt, QScriptEngine *eng);
};

#endif // COFFEESCRIPTENGINE_H
