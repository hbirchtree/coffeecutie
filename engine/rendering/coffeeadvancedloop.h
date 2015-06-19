#ifndef COFFEEADVANCEDLOOP_H
#define COFFEEADVANCEDLOOP_H

#include "general/common.h"

#include "general/data/coffeedatacontainer.h"
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

class CoffeeAdvancedLoop : public RenderLoop
{
public:
    CoffeeAdvancedLoop(QObject* parent,CoffeeRenderer* renderer, QString fileSource);
    ~CoffeeAdvancedLoop();

    std::function<void ()> *getInit();
    std::function<void ()> *getLoop();
    std::function<void ()> *getCleanup();

    void connectSignals(CoffeeRenderer* renderer);

    QObject* getFactory();
    CoffeeScriptEngine* getScriptEngine();

private:
    CoffeeObjectFactory* factory;
    CoffeeScriptEngine* scriptEngine;

    QTimer *secondbop;

    CoffeeOutputSurface* screenSurface;

    CoffeePlayerController* controller;
    CoffeeDataContainer<QString,double>* timers;
    CoffeeWorldOpts *world;
    CoffeeJoystick* js;
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;

};

#endif // COFFEEADVANCEDLOOP_H
