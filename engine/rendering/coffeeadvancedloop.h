#ifndef COFFEEADVANCEDLOOP_H
#define COFFEEADVANCEDLOOP_H

#include "general/common.h"

//Data, etc
#include "general/data/numbercontainer.h"
#include "general/data/coffeedatacontainer.h"
#include "general/qstringfunctions.h"


//Rendering
#include "opengl/rendering/renderloop.h"
#include "opengl/context/coffeerenderer.h"
#include "opengl/rendering/coffeerenderingloop.h"
#include "opengl/helpers/renderingmethods.h"

//Display
#include "opengl/components/coffeeframebufferobject.h"
#include "opengl/components/coffeedepthbufferobject.h"
#include "engine/objects/coffeeoutputsurface.h"

//Engine
#include "engine/objects/coffeeobjectfactory.h"
#include "opengl/components/coffeeobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/input/coffeejoystick.h"
#include "general/input/coffeeplayercontroller.h"

#include "engine/objects/coffeeparticlesystem.h"


class CoffeeAdvancedLoop : public RenderLoop
{
public:
    CoffeeAdvancedLoop(CoffeeRenderer* renderer, QString fileSource);
    ~CoffeeAdvancedLoop();

    std::function<void ()> *getInit();
    std::function<void ()> *getLoop();
    std::function<void ()> *getCleanup();

    void connectSignals(CoffeeRenderer* renderer);

private:
    QEventLoop* evloop;

    CoffeeParticleSystem* test;

    CoffeeFrameBufferObject* renderFbo;
    CoffeeSimpleObject* screenSurface;

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
