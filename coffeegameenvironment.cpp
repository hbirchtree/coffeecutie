#include "coffeegameenvironment.h"

#include <QApplication>

#include "engine/rendering/coffeeadvancedloop.h"
#include "engine/scripting/coffeescriptengine.h"

#include "inspector/editor/coffeegameeditor.h"
#include "inspector/editor/coffeeeditorshell.h"

#include <QMessageBox>
#include <QThread>
#include <QThreadPool>
#include <QWindow>

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

QObject *CoffeeGameEnvironment::renderWindow() const
{
    return m_renderWindow;
}

QObject *CoffeeGameEnvironment::scriptEngineQObject()
{
    return m_scriptEngine;
}

QObjectList CoffeeGameEnvironment::scriptObjects() const
{
    return m_scriptObjects;
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
            m_scriptEngine->insertObject(o);
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
    m_editorShell = new CoffeeEditorShell(this);

    m_editor = new CoffeeGameEditor(0);

    m_editorShell->insertObject("root",this);
    m_editorShell->insertObject("app",m_editor);
    m_editorShell->insertObject("cutie_engine",m_scriptEngine);
    m_editorShell->insertObject("cutie_renderer",m_rendererObject);

    connect(m_editorShell,&CoffeeEditorShell::executionReturn,
            [=](QString p, QString f, QString r){
        if(r!=m_editorShell->engine()->undefinedValue().toString()){
            QMessageBox::warning(0,tr("Error initializing editor"),r);
            qWarning() << r;
        }
    });

    m_editorShell->executeFile("editor.qts");
    registerInspectionObject(m_editor);

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
    newInspectionObject(o);
    if(m_scriptEngine)
        m_scriptEngine->insertObject(o);
}

void CoffeeGameEnvironment::onRendererInit()
{
    if(m_scriptEngine)
        m_scriptEngine->executeFile(m_initScript);

    m_renderWindow = m_rendererObject->windowHandle();

    if(m_renderWindow){
        m_renderWindow->setObjectName("cutie_window");
//        if(inspect())
//            m_editor->setPrimaryView(QWidget::createWindowContainer(m_renderWindow));
        registerInspectionObject(m_renderWindow);
    }
    rendererInitialized();
}

void CoffeeGameEnvironment::shutdownEnvironment()
{
    if(m_rendererObject&&m_editor){
        m_rendererObject->requestWindowClose();
        m_editor->close();
    }
}
