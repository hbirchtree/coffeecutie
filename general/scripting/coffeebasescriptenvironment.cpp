#include "coffeebasescriptenvironment.h"

#include "engine/scripting/coffeescriptengineagent.h"
#include "general/filehandler.h"

CoffeeBaseScriptEnvironment::CoffeeBaseScriptEnvironment(QObject *parent) : QObject(parent)
{
    m_engine = new QScriptEngine(this);
    m_agent = new CoffeeScriptEngineAgent(this,m_engine);

    m_engine->setAgent(m_agent);
}

QScriptEngine *CoffeeBaseScriptEnvironment::engine()
{
    return m_engine;
}

CoffeeScriptEngineAgent *CoffeeBaseScriptEnvironment::agent()
{
    return m_agent;
}

void CoffeeBaseScriptEnvironment::insertObject(const QString &name, QObject *object)
{
    QScriptValue wrapper = m_engine->newQObject(object);
    m_engine->globalObject().setProperty(name,wrapper);
}

void CoffeeBaseScriptEnvironment::insertObject(QObject *object)
{
    QScriptValue wrapper = m_engine->newQObject(object);
    m_engine->globalObject().setProperty(object->objectName(),wrapper);
}

void CoffeeBaseScriptEnvironment::executeFile(const QString &filename, int arg0)
{
    QScriptValue result = execFile(this->m_engine,filename);
    executionReturn(QString(),filename,result.toString());
}

void CoffeeBaseScriptEnvironment::executeCmd(const QString &program)
{
    QScriptValue result = this->m_engine->evaluate(program);
    executionReturn(program,QString(),result.toString());
}

QScriptValue CoffeeBaseScriptEnvironment::execFile(QScriptEngine *m_engine, QString file)
{
    QFileInfo fileTest(file);
    QFile script(file);
    QScriptValue out;
    if(!file.isEmpty()&&
            fileTest.exists()&&
            fileTest.isFile()&&
            script.open(QIODevice::ReadOnly)){
        QString src = script.readAll();
        src = importFile(fileTest,src);
        QScriptProgram p(src,file); //we might get use for these QScriptProgram objects, but for now we live it at this
        out = m_engine->evaluate(p);
    }
    return out;
}

QString CoffeeBaseScriptEnvironment::importFile(const QFileInfo &srcFile, QString &src)
{
    QRegExp r;
    r.setPattern("^#inc \"(.*)\"$");
    for(QString l : src.split("\n"))
        if(r.indexIn(l)>=0){
            QString fileSrc = srcFile.dir().absolutePath()+QDir::separator()+r.cap(1);
            QString replace = FileHandler::getStringFromFile(fileSrc);
            if(!replace.isNull()){
                QFileInfo tFile(fileSrc);
                replace = importFile(tFile,replace);
                src.replace(l,replace);
            }else{
                qWarning("Failed to include contents from file: %s",fileSrc.toStdString().c_str());
            }
        }
    return src;
}
