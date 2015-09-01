#ifndef COFFEEADVANCEDLOOP_H
#define COFFEEADVANCEDLOOP_H

#include "opengl/rendering/renderloop.h"

class QScriptEngine;
class QScriptValue;

class CoffeeWorldOpts;
class CoffeeOutputSurface;
class CoffeeObject;
class CoffeeStandardObject;
class CoffeePlayerController;
class CoffeeRendererBase;

class CoffeeObjectFactory;
class CoffeeScriptEngine;

class CoffeeAdvancedLoop : public RenderLoop
{
    Q_PROPERTY(QObject* world READ getWorld WRITE setWorld)
    Q_PROPERTY(QObject* factory READ factory)

    Q_OBJECT
public:
    CoffeeAdvancedLoop(QObject* parent, CoffeeRendererBase* m_renderer);

    std::function<void ()> *init();
    std::function<void ()> *loop();
    std::function<void ()> *cleanup();

    QObject* getFactory();
    CoffeeScriptEngine* getScriptEngine();

    QObject* getWorld();

    QObject* factory() const;

    Q_INVOKABLE QObject* createRenderSurface();

public slots:
    void setWorld(QObject* m_world);

private:
    CoffeeObjectFactory* m_factory = nullptr;
    CoffeeScriptEngine* m_scriptEngine = nullptr;
    CoffeeRendererBase* m_renderer = nullptr;
    CoffeeWorldOpts* m_world = nullptr;

    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;
};

#endif // COFFEEADVANCEDLOOP_H
