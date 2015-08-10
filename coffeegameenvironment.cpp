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
#include "inspector/debugger/coffeedebugview.h"

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
//    CoffeeRenderer* renderer = qobject_cast<CoffeeRenderer*>(m_rendererObject);

    //Create the inspector
    m_inspector = new CoffeeInspector(0,m_scriptObjects);

    //Create the renderer information
    m_information = new CoffeeRendererInspector(0);
    m_information->setObjectName("information_widget");

    m_scriptObjects.append(m_information);

    if(m_scriptEngine){

        //Create editor forms

        //Script terminal
        m_terminal = new CoffeeScriptTerminal(0);
        connect(m_scriptEngine,&CoffeeScriptEngine::executionReturn,
                [=](QString program, QString file, QString result){
            QString cmd;
            if(file.isNull()){
                cmd = program;
            }else{
                cmd = file;
            }
            m_terminal->appendLog(cmd,result);
        });
        connect(m_terminal,&CoffeeScriptTerminal::requestExecCmd,
                m_scriptEngine,&CoffeeScriptEngine::engine_execCmd);
        connect(m_terminal,&CoffeeScriptTerminal::requestExecFile,
                m_scriptEngine,&CoffeeScriptEngine::engine_execFile);

        //Debug view
        CoffeeDebugView* dbg = new CoffeeDebugView(0);

        //Exception handler, translation between CoffeeScriptException and interface
        CoffeeScriptExceptionHandler* m_exhandler = new CoffeeScriptExceptionHandler(m_terminal);
        m_exhandler->m_backtree = dbg->getBacktraceTree();
        m_exhandler->m_editor = dbg->getCodeEditor();

        connect(m_scriptEngine->getAgent(),&CoffeeScriptEngineAgent::exceptionReport,
                m_exhandler,&CoffeeScriptExceptionHandler::receiveScriptException);

        //Create the primary editor window
        m_editor = new CoffeeGameEditor(0);

        //Insert widgets
        m_editor->addInspectorTab(m_inspector,tr("Object inspector"));
        m_editor->addInspectorTab(m_information,tr("Renderer info"));

        m_editor->addInfoTab(m_terminal,tr("Shell"));
        m_editor->addInfoTab(dbg,tr("Debugger"));
    }

    connect(m_editor,&CoffeeGameEditor::requestShutdown,
            this,&CoffeeGameEnvironment::shutdownEnvironment);
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
    if(m_scriptEngine)
        m_scriptEngine->addObject(o);
}

void CoffeeGameEnvironment::onRendererInit()
{
    if(m_scriptEngine)
        m_scriptEngine->engine_execFile(m_initScript);

    QWindow* glfw = m_rendererObject->windowHandle();

    if(glfw){
        glfw->setObjectName("glfw_window");
        if(inspect())
            m_editor->setPrimaryView(QWidget::createWindowContainer(glfw));
        registerInspectionObject(glfw);
    }
}

void CoffeeGameEnvironment::shutdownEnvironment()
{
    if(m_rendererObject&&m_editor){
        m_rendererObject->requestWindowClose();
        m_editor->close();
    }
}
