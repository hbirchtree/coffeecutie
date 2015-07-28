#ifndef COFFEEGAMEENVIRONMENT_H
#define COFFEEGAMEENVIRONMENT_H

#include <QObject>
#include <QThread>
#include <QThreadPool>

class CoffeeScriptTerminal;
class CoffeeRendererInspector;
class CoffeeGameEditor;

class CoffeeRendererBase;
class CoffeeInspector;
class CoffeeScriptEngine;
class RenderLoop;
class QApplication;

class CoffeeGameEnvironment : public QObject
{
    Q_PROPERTY(bool inspect READ inspect WRITE setInspect)
    Q_PROPERTY(QString initScript READ initScript WRITE setInitScript)

    Q_OBJECT
    bool m_inspect = true;

public:
    explicit CoffeeGameEnvironment(QObject *parent = 0);

    bool inspect() const;
    void startExecution(QApplication *app);
    CoffeeScriptEngine *scriptEngine();
    CoffeeRendererBase* renderer();
    QString initScript() const;

protected:
    void createInspector();

    void constructRenderer();
    void createGameWindow();

    void registerInspectionObject(QObject* o);

protected slots:
    void onRendererInit();

signals:

public slots:
    void setInspect(bool inspect);
    void setScriptEngine(CoffeeScriptEngine *scriptEngine);
    void setRenderLoop(RenderLoop* loop);
    void setThreadPool(QThreadPool *threadPool);
    void setInitScript(QString initScript);

private:
    QThreadPool* m_threadPool = nullptr;
    QObjectList m_scriptObjects;

    RenderLoop* m_renderLoop = nullptr;

    CoffeeGameEditor* m_editor = nullptr;
    CoffeeScriptTerminal* m_terminal = nullptr;
    CoffeeRendererInspector* m_information = nullptr;
    CoffeeInspector* m_inspector = nullptr;

    CoffeeRendererBase* m_rendererObject = nullptr;
    CoffeeScriptEngine* m_scriptEngine = nullptr;
    QString m_initScript;
};

#endif // COFFEEGAMEENVIRONMENT_H
