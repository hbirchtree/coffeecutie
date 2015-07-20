#include "coffeescriptengineagent.h"

#include <QScriptEngine>
#include <QDebug>

CoffeeScriptEngineAgent::CoffeeScriptEngineAgent(QObject *parent, QScriptEngine* engine) :
    QObject(parent),
    QScriptEngineAgent(engine)
{
}

void CoffeeScriptEngineAgent::scriptLoad(qint64 id, const QString &program,
                                         const QString &fileName, int baseLineNumber)
{
    qDebug() << "Loading script:" << id << fileName;
    CoffeeScriptInstance* _s = new CoffeeScriptInstance();
    _s->baseLn = baseLineNumber;
    _s->program = program;
    _s->filename = fileName;

    m_loaded_scripts.insert(id,_s);
}

void CoffeeScriptEngineAgent::scriptUnload(qint64 id)
{
    qDebug() << "Unloading script:" << id;
    if(m_loaded_scripts.contains(id))
        delete m_loaded_scripts.value(id);
    m_loaded_scripts.remove(id);
}

void CoffeeScriptEngineAgent::exceptionThrow(qint64 scriptId, const QScriptValue &exception, bool hasHandler)
{
    if(hasHandler)
        return;

    CoffeeScriptException exc;
    exc.backtrace = this->engine()->currentContext()->backtrace();
    exc.self = exception;
    exc.scriptReference = scriptId;

    exceptionReport(exc);
}

const QString &CoffeeScriptEngineAgent::getProgram(qint64 id)
{
    if(m_loaded_scripts.contains(id))
        return m_loaded_scripts.value(id)->program;
    return QString();
}
