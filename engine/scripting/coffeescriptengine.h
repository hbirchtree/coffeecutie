#ifndef COFFEESCRIPTENGINE_H
#define COFFEESCRIPTENGINE_H

#include "coffeescriptconstructors.h"

class CoffeeScriptException
{
public:
    QScriptValue message;
    QStringList backtrace;
    QScriptValue callee;
    QScriptValue self;
    QScriptValue activator;
    int lineNumber;
//    QScriptContext context;
};

class CoffeeScriptEngine : public QObject
{
    Q_OBJECT
public:
    CoffeeScriptEngine(QObject *parent);

    QScriptEngine *getEngine();

    void execFile(QString file, bool *result = nullptr, QString *logOut = nullptr);
    static QScriptValue execFile(QScriptEngine* e, QString file, bool *result = nullptr, QString *logOut = nullptr, QScriptContext *ctxt = nullptr);
    void addObject(QObject* o);

signals:
    void uncaughtException(CoffeeScriptException ex);

private:
    QScriptEngine e;

    //Functions to export
    static QScriptValue coffeeImportVariantMap(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeExecuteScriptFile(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeParentingFunc(QScriptContext *ctxt, QScriptEngine *eng);
    static QScriptValue coffeeExceptionFunc(QScriptContext *ctxt, QScriptEngine *eng);

    //ScriptEngine internal functions
    static QString importFile(const QFileInfo &srcFile, QString &src);
};

#endif // COFFEESCRIPTENGINE_H

