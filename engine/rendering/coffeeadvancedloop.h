#ifndef COFFEEADVANCEDLOOP_H
#define COFFEEADVANCEDLOOP_H

#include "general/common.h"
#include "opengl/rendering/renderloop.h"
#include "opengl/context/coffeerenderer.h"
#include "opengl/components/coffeeobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/data/numbercontainer.h"
#include "opengl/rendering/coffeerenderingloop.h"
#include "general/data/coffeedatacontainer.h"
#include "general/input/coffeejoystick.h"
#include "opengl/helpers/renderingmethods.h"
#include "general/qstringfunctions.h"
#include "general/input/coffeeplayercontroller.h"
#include "opengl/components/coffeeframebufferobject.h"
#include "engine/objects/coffeeobjectfactory.h"
#include "engine/objects/coffeesimpleobject.h"

class CoffeeAdvancedLoop : public RenderLoop
{
public:
    CoffeeAdvancedLoop(CoffeeRenderer* renderer);
    ~CoffeeAdvancedLoop();

    std::function<void ()> getInit();
    std::function<void ()> getLoop();
    std::function<void ()> getCleanup();

    void connectSignals(CoffeeRenderer* renderer);

private:
    CoffeeFrameBufferObject* testfbo;
    CoffeeSimpleObject* test;

    CoffeePlayerController* controller;
    CoffeeDataContainer<QString,double>* timers;
    CoffeeWorldOpts *world;
    CoffeeJoystick* js;
    CoffeeRenderingMethod *defaultRenderingMethod;
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;

    void setupRenderer(CoffeeObject *object, CoffeeRenderingMethod *basicMethod);
};

#endif // COFFEEADVANCEDLOOP_H
