#ifndef COFFEEADVANCEDLOOP_H
#define COFFEEADVANCEDLOOP_H

//#include "general/common.h"
#include "opengl/rendering/renderloop.h"

class QScriptEngine;
class QScriptValue;

class CoffeeJoystick;
class CoffeeWorldOpts;
class CoffeeOutputSurface;
class CoffeeObject;
class CoffeeStandardObject;
class CoffeePlayerController;
class CoffeeRenderer;

class CoffeeObjectFactory;
class CoffeeScriptEngine;

class CoffeeAdvancedLoop : public QObject,public RenderLoop
{
    Q_PROPERTY(QObject* world READ getWorld WRITE setWorld)

    Q_OBJECT
public:
    CoffeeAdvancedLoop(QObject* parent, CoffeeRenderer* renderer);
    ~CoffeeAdvancedLoop();

    std::function<void ()> *getInit();
    std::function<void ()> *getLoop();
    std::function<void ()> *getCleanup();

    void connectSignals(CoffeeRenderer* renderer);

    QObject* getFactory();
    CoffeeScriptEngine* getScriptEngine();

    QObject* getWorld();

public slots:
    void setWorld(QObject* world);

private:
    CoffeeObjectFactory* factory = nullptr;

    QThread* scriptThread = nullptr;
    CoffeeScriptEngine* scriptEngine = nullptr;

    CoffeeOutputSurface* screenSurface = nullptr;

    CoffeeRenderer* renderer = nullptr;
    CoffeeWorldOpts* world = nullptr;
    CoffeeJoystick* js = nullptr;
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;
};

#endif // COFFEEADVANCEDLOOP_H
