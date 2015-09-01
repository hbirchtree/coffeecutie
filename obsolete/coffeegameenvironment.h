#ifndef COFFEEGAMEENVIRONMENT_H
#define COFFEEGAMEENVIRONMENT_H

#include <QObject>
#include "opengl/context/coffeerenderer.h"

class CoffeeScriptTerminal;
class CoffeeRendererInspector;
class CoffeeGameEditor;

class CoffeeRendererBase;
class CoffeeInspector;
class CoffeeScriptEngine;
class CoffeeEditorShell;

class RenderLoop;

class QApplication;
class QWindow;
class QThreadPool;

class CoffeeGameEnvironment : public QObject
{
    Q_PROPERTY(bool inspect READ inspect WRITE setInspect)
    Q_PROPERTY(QString initScript READ initScript WRITE setInitScript)

    Q_PROPERTY(QObjectList scriptObjects READ scriptObjects)
    Q_PROPERTY(QObject* renderWindow READ renderWindow)
    Q_PROPERTY(QObject* scriptEngine READ scriptEngineQObject)

    Q_OBJECT
    bool m_inspect = true;

public:
    explicit CoffeeGameEnvironment(QObject *parent = 0);

    bool inspect() const;
    void startExecution(QApplication *app);
    CoffeeScriptEngine *scriptEngine();
    CoffeeRendererBase* renderer();
    QString initScript() const;

    QObject* renderWindow() const;
    QObject* scriptEngineQObject();

    QObjectList scriptObjects() const;

protected:
    void createInspector();

    void constructRenderer();
    void createGameWindow();

signals:
    void rendererInitialized();
    void newInspectionObject(QObject* o);

protected slots:
    void onRendererInit();
    void shutdownEnvironment(); //This is necessary to shut down both the game and the editor

public slots:
    void registerInspectionObject(QObject* o);
    void setInspect(bool inspect);
    void setScriptEngine(CoffeeScriptEngine *scriptEngine);
    void setRenderLoop(RenderLoop* loop);
    void setThreadPool(QThreadPool *threadPool);
    void setInitScript(QString initScript);

private:
    QThreadPool* m_threadPool = nullptr;
    QObjectList m_scriptObjects;

    RenderLoop* m_renderLoop = nullptr;

    QWindow* m_renderWindow = nullptr;

    CoffeeGameEditor* m_editor = nullptr;
    CoffeeRendererBase* m_rendererObject = nullptr;
    CoffeeScriptEngine* m_scriptEngine = nullptr;
    CoffeeEditorShell* m_editorShell = nullptr;

    QString m_initScript;
};

#endif // COFFEEGAMEENVIRONMENT_H
