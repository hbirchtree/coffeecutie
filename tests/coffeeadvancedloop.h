#ifndef COFFEEADVANCEDLOOP_H
#define COFFEEADVANCEDLOOP_H

#include "general/common.h"
#include "opengl/rendering/renderloop.h"
#include "opengl/coffeerenderer.h"
#include "opengl/components/coffeeobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/models/wavefrontmodelreader.h"
#include "opengl/rendering/coffeerenderingmethod.h"
#include "general/numbercontainer.h"
#include "opengl/rendering/coffeerenderingloop.h"
#include "general/data/coffeedatacontainer.h"
#include "general/input/coffeejoystick.h"
#include "opengl/helpers/renderingmethods.h"
#include "general/qstringfunctions.h"
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
    CoffeeDataContainer<QString,double>* timers;
    CoffeeObject *test;
    CoffeeWorldOpts *world;
    CoffeeJoystick* js;
    CoffeeRenderingMethod *defaultRenderingMethod;
    std::function<void()> _rendering_loop_init;
    std::function<void()> _rendering_loop;
    std::function<void()> _rendering_loop_cleanup;
};

#endif // COFFEEADVANCEDLOOP_H
