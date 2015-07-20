#ifndef COFFEESCRIPTENGINEAGENT_H
#define COFFEESCRIPTENGINEAGENT_H

#include <QObject>
#include <QScriptEngineAgent>
#include <QScriptValue>

class CoffeeScriptException
{
public:
    QStringList backtrace;
    QScriptValue self;
    int lineNumber;
    qint64 scriptReference;
};

class CoffeeScriptEngineAgent : public QObject,public QScriptEngineAgent
{
    Q_OBJECT

    class CoffeeScriptInstance
    {
    public:
        int baseLn = 0;
        QString filename;
        QString program;
    };

public:
    CoffeeScriptEngineAgent(QObject* parent,QScriptEngine *engine);

    void scriptLoad(qint64 id, const QString &program, const QString &fileName, int baseLineNumber);
    void scriptUnload(qint64 id);

    void exceptionThrow(qint64 scriptId, const QScriptValue &exception, bool hasHandler);

    const QString& getProgram(qint64 id);

signals:
    void exceptionReport(CoffeeScriptException ex);

private:
    QHash<qint64,CoffeeScriptInstance*> m_loaded_scripts;
};

#endif // COFFEESCRIPTENGINEAGENT_H
