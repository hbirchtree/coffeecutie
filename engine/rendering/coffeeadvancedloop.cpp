#include "coffeeadvancedloop.h"

//Data, etc
#include "general/data/numbercontainer.h"
#include "general/qstringfunctions.h"


//Rendering
#include "opengl/context/coffeerenderer.h"
#include "opengl/rendering/coffeerenderingloop.h"
#include "opengl/helpers/renderingmethods.h"

//Display
#include "opengl/components/coffeeframebufferobject.h"
#include "opengl/components/coffeedepthbufferobject.h"
#include "engine/objects/coffeeoutputsurface.h"

//Engine
#include "engine/objects/coffeeobjectfactory.h"
#include "engine/objects/coffeestandardobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/input/coffeejoystick.h"
#include "general/input/coffeeplayercontroller.h"

#include "engine/scripting/coffeescriptengine.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(QObject *parent, CoffeeRenderer* renderer, QString fileSource) : QObject(parent)
{
    connectSignals(renderer);
    setObjectName("evloop");

    this->scriptEngine = new CoffeeScriptEngine(this);
    this->factory = new CoffeeObjectFactory(this);
    this->factory->setObjectName("factory");
    factory->setParent(this);

    qDebug("Importing objects from file");
//    QList<CoffeeWorldOpts*> worlds = factory->importObjects(fileSource,this);
//    if(worlds.isEmpty())
//        qFatal("Failed to load any world information!");
//    world = worlds.first();
//    connect(renderer,SIGNAL(contextReportFrametime(float)),world,SLOT(tickObjects(float)));

//    world->setRenderer(renderer);
//    world->connectSignals(controller);

    _rendering_loop_init = [=](){

        qDebug("Enabling standard OpenGL capabilities");
        glEnable(GL_TEXTURE_2D);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        qDebug("Setting vertical sync mode");
        glfwSwapInterval(0);

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
    controller = new CoffeePlayerController(this);
    controller->setObjectName("cameracontrol");
    js = new CoffeeJoystick(renderer,GLFW_JOYSTICK_1);
    screenSurface = new CoffeeOutputSurface(this,new CoffeeFrameBufferObject(this));
    secondbop = new QTimer(this);
    secondbop->setInterval(1000);

    connect(secondbop,SIGNAL(timeout()),renderer,SLOT(requestMemoryCheck()));
    secondbop->start();
    connect(renderer,SIGNAL(contextReportFrametime(float)),controller,SLOT(tick(float)));
    timers = new CoffeeDataContainer<QString,double>(this); //this one needs to be slotted into QtScript somehow.

    qDebug("Setting up miscellaneous signals and slots");
    renderer->connect(renderer,&CoffeeRenderer::contextReportFrametime,[=](float frametime){ //needs to be moved into QtScript
        if(glfwGetTime()>=timers->getValue("fps")){
            qDebug("FPS: %.0f",1.f/frametime);
            timers->setValue("fps",glfwGetTime()+1);
        }
    });
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
    if(w)
        this->world = w;
}
