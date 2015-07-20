#include "coffeeadvancedloop.h"

#include <QResizeEvent>

//Data, etc
#include "general/data/numbercontainer.h"
#include "general/qstringfunctions.h"


//Rendering
#include "opengl/context/coffeerenderer.h"
#include "opengl/rendering/coffeerenderingloop.h"
#include "opengl/helpers/renderingmethods.h"

//Display
#include "opengl/components/coffeeframebufferobject.h"
#include "engine/objects/coffeeoutputsurface.h"

//Engine
#include "engine/objects/coffeeobjectfactory.h"
#include "engine/objects/coffeestandardobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/input/coffeejoystick.h"
#include "general/input/coffeeplayercontroller.h"

#include "engine/scripting/coffeescriptengine.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(QObject *parent, CoffeeRenderer* renderer) : QObject(parent)
{
    this->renderer = renderer;
    connectSignals(renderer);
    setObjectName("evloop");

    this->scriptEngine = new CoffeeScriptEngine(this);
    this->factory = new CoffeeObjectFactory(this);
    this->factory->setObjectName("factory");

    _rendering_loop_init = [=](){

        qDebug("Enabling standard OpenGL capabilities");
//        glEnable(GL_TEXTURE_2D);
//        glEnable(GL_TEXTURE_3D);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        qDebug("Configuring framebuffer object");
        screenSurface->getFramebuffer()->createFramebuffer(*renderer->getFramebufferSizePt(),1);
        connect(renderer,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent e){
            screenSurface->getFramebuffer()->resizeViewport(e.size());
        });
    };
    _rendering_loop = [=](){
        js->update();

        //bind the framebuffer which we render to
        screenSurface->bind();
        //clear the depth buffer, otherwise we won't see sh*t
        glClear(GL_DEPTH_BUFFER_BIT);

        //render the current world
        if(world)
            world->renderWorld();

//        renderer->flushPipeline();

        //render for the user
        screenSurface->render();
    };
    _rendering_loop_cleanup = [=](){
        qDebug("Running the empty cleanup function");
    };
}

CoffeeAdvancedLoop::~CoffeeAdvancedLoop()
{
}

std::function<void ()> *CoffeeAdvancedLoop::getInit()
{
    return &_rendering_loop_init;
}

std::function<void ()> *CoffeeAdvancedLoop::getLoop()
{
    return &_rendering_loop;
}

std::function<void ()> *CoffeeAdvancedLoop::getCleanup()
{
    return &_rendering_loop_cleanup;
}

void CoffeeAdvancedLoop::connectSignals(CoffeeRenderer *renderer)
{
    js = new CoffeeJoystick(renderer,GLFW_JOYSTICK_1);
    screenSurface = new CoffeeOutputSurface(this,new CoffeeFrameBufferObject(this));
}

QObject *CoffeeAdvancedLoop::getFactory()
{
    return factory;
}

CoffeeScriptEngine *CoffeeAdvancedLoop::getScriptEngine()
{
    return scriptEngine;
}

QObject *CoffeeAdvancedLoop::getWorld()
{
    return (QObject*)world;
}

void CoffeeAdvancedLoop::setWorld(QObject *world)
{
    CoffeeWorldOpts* w = qobject_cast<CoffeeWorldOpts*>(world);
    if(this->world){
        this->world->unloadWorld();
        this->world->disconnectSignals(); //avoid changing state of the world
    }
    if(w){
        w->setRenderer(renderer);
        this->world = w;
    }
}
