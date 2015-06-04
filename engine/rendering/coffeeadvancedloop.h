#ifndef COFFEEADVANCEDLOOP_H
#define COFFEEADVANCEDLOOP_H

#include "general/common.h"

#include "general/data/coffeedatacontainer.h"
#include "opengl/rendering/renderloop.h"

class CoffeeJoystick;
class CoffeeParticleSystem;
class CoffeeWorldOpts;
class CoffeeFrameBufferObject;
class CoffeeObject;
class CoffeeStandardObject;
class CoffeePlayerController;
class CoffeeRenderer;

class CoffeeAdvancedLoop : public RenderLoop
{
public:
    CoffeeAdvancedLoop(QObject* parent,CoffeeRenderer* renderer, QString fileSource);
    ~CoffeeAdvancedLoop();

    QList<QObject *> getThreadObjects() const;
    std::function<void ()> *getInit();
    std::function<void ()> *getLoop();
    std::function<void ()> *getCleanup();

    void connectSignals(CoffeeRenderer* renderer);

private:
    QEventLoop* evloop;

    CoffeeFrameBufferObject* renderFbo;
    CoffeeObject* screenSurface;

    CoffeePlayerController* controller;
    CoffeeDataContainer<QString,double>* timers;
    CoffeeWorldOpts *world;
    CoffeeJoystick* js;
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;

    void setupRenderer(CoffeeStandardObject *object);

};

#endif // COFFEEADVANCEDLOOP_H
