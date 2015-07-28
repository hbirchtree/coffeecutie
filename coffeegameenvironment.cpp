#include "coffeegameenvironment.h"

#include <QApplication>

#include "inspector/coffeeinspector.h"
#include "engine/rendering/coffeeadvancedloop.h"
#include "engine/scripting/coffeescriptengine.h"
#include "opengl/context/coffeerendererbase.h"
#include "opengl/context/coffeerenderer.h"

#include "inspector/coffeerendererinspector.h"
#include "inspector/coffeescriptterminal.h"
#include "inspector/editor/coffeegameeditor.h"

CoffeeGameEnvironment::CoffeeGameEnvironment(QObject *parent) : QObject(parent)
{
}

bool CoffeeGameEnvironment::inspect() const
{
    return m_inspect;
}

void CoffeeGameEnvironment::setInspect(bool inspect)
{
    m_inspect = inspect;
}
CoffeeScriptEngine *CoffeeGameEnvironment::scriptEngine()
{
    return m_scriptEngine;
}

CoffeeRendererBase *CoffeeGameEnvironment::renderer()
{
    if(!m_rendererObject)
        constructRenderer();
    return m_rendererObject;
}

QString CoffeeGameEnvironment::initScript() const
{
    return m_initScript;
}

void CoffeeGameEnvironment::setScriptEngine(CoffeeScriptEngine *scriptEngine)
{
    m_scriptEngine = scriptEngine;
}

void CoffeeGameEnvironment::setThreadPool(QThreadPool *threadPool)
{
    m_threadPool = threadPool;
}

void CoffeeGameEnvironment::setInitScript(QString initScript)
{
    m_initScript = initScript;
}

void CoffeeGameEnvironment::startExecution(QApplication* app)
{
    m_threadPool->setObjectName("QThreadPool");

    createGameWindow();

    CoffeeAdvancedLoop* advloop = qobject_cast<CoffeeAdvancedLoop*>(m_renderLoop);
    if(advloop){
        registerInspectionObject(advloop->getFactory());
    }

    registerInspectionObject(this);
    registerInspectionObject(m_threadPool);


    if(inspect()){
        createInspector();
    }

    if(m_scriptEngine){
        for(QObject* o : m_scriptObjects)
            m_scriptEngine->addObject(o);
    }

    connect(m_rendererObject,SIGNAL(rendererInitFinished()),SLOT(onRendererInit()));

    if(inspect()){
        m_editor->show();

        connect(m_rendererObject,SIGNAL(rendererShuttingDown()),m_editor,SLOT(close()));

        m_threadPool->start(m_rendererObject);

        app->exec();
    }else{
        m_rendererObject->run();
    }
}

void CoffeeGameEnvironment::setRenderLoop(RenderLoop *loop)
{
    m_renderLoop = loop;
}

void CoffeeGameEnvironment::createInspector()
{
    CoffeeRenderer* renderer = qobject_cast<CoffeeRenderer*>(m_rendererObject);

    m_inspector = new CoffeeInspector(0,m_scriptObjects);

    m_information = new CoffeeRendererInspector(0,renderer);
    if(m_scriptEngine)
        m_terminal = new CoffeeScriptTerminal(0,m_scriptEngine);

    connect(m_inspector,SIGNAL(showInformation()),m_information,SLOT(show()));
    if(m_scriptEngine)
        connect(m_inspector,SIGNAL(showTerminal()),m_terminal,SLOT(show()));

    m_editor = new CoffeeGameEditor(0,m_information,m_inspector,m_terminal);
}

void CoffeeGameEnvironment::constructRenderer()
{
    if(m_rendererObject)
        return;
    m_rendererObject = new CoffeeRenderer(0,
                                          1280,720,Qt::WindowNoState,
                                          "Unlimited Frame Works");
}

void CoffeeGameEnvironment::createGameWindow()
{
    constructRenderer();

    //Set standard name for the QObject
    m_rendererObject->setObjectName("renderer");
    //Assign its RenderLoop object
    m_rendererObject->setLoop(m_renderLoop);
    //Register it for inclusion in inspector
    registerInspectionObject(m_rendererObject);
}

void CoffeeGameEnvironment::registerInspectionObject(QObject *o)
{
    m_scriptObjects.append(o);
}

void CoffeeGameEnvironment::onRendererInit()
{
    if(m_scriptEngine)
        m_scriptEngine->execFile(m_initScript);
}
