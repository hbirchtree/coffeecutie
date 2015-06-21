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
#include "engine/objects/coffeeobject.h"
#include "opengl/components/coffeecamera.h"
#include "engine/objects/coffeestandardobject.h"
#include "opengl/components/coffeeomnilight.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/input/coffeejoystick.h"
#include "general/input/coffeeplayercontroller.h"
#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeematerial.h"
#include "general/shadervariant.h"
#include "opengl/components/shadercontainer.h"

#include "engine/models/coffeeinstancecontainer.h"

#include "engine/objects/coffeeparticlesystem.h"
#include "engine/scripting/coffeescriptengine.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(QObject *parent, CoffeeRenderer* renderer, QString fileSource) : RenderLoop(parent)
{
    connectSignals(renderer);

    this->scriptEngine = new CoffeeScriptEngine(this);
    this->factory = new CoffeeObjectFactory(this);
    this->factory->setObjectName("factory");
    factory->setParent(this);

    qDebug("Importing objects from file");
    QList<CoffeeWorldOpts*> worlds = factory->importObjects(fileSource,this);
    if(worlds.isEmpty())
        qFatal("Failed to load any world information!");
    world = worlds.first();
    connect(renderer,SIGNAL(contextReportFrametime(float)),world,SLOT(tickObjects(float)));

    world->setRenderer(renderer);
    world->connectSignals(controller);

    _rendering_loop_init = [=](){

        qDebug("Configuring renderer");
        renderer->setSamples(4);

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

        renderer->flushPipeline();

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

    // TODO : move stuff into QtScript
    // Problem : Controller should not know about "world", and it should not be used in its slots.
    // We will create new functions for that purpose.
    qDebug("Setting up miscellaneous signals and slots");
    renderer->connect(renderer,&CoffeeRenderer::contextReportFrametime,[=](float frametime){ //needs to be moved into QtScript
        if(glfwGetTime()>=timers->getValue("fps")){
            qDebug("FPS: %.0f",1.f/frametime);
            timers->setValue("fps",glfwGetTime()+1);
        }
    });

//    connect(renderer,&CoffeeRenderer::winKeyboardEvent,[=](QKeyEvent event){ //needs to be moved into QtScript
//        if(event.key()==GLFW_KEY_ESCAPE&&event.type()==QEvent::KeyPress)
//            renderer->requestWindowClose();
//        else if(event.key()==GLFW_KEY_W&&event.type()==QEvent::KeyPress)
//            controller->addSpeedForward(world->getCamera()->getCameraForwardNormal()*6.f);
//        else if(event.key()==GLFW_KEY_A&&event.type()==QEvent::KeyPress)
//            controller->addSpeedRight(world->getCamera()->getCameraRightNormal()*-6.f);
//        else if(event.key()==GLFW_KEY_D&&event.type()==QEvent::KeyPress)
//            controller->addSpeedRight(world->getCamera()->getCameraRightNormal()*6.f);
//        else if(event.key()==GLFW_KEY_S&&event.type()==QEvent::KeyPress)
//            controller->addSpeedForward(world->getCamera()->getCameraForwardNormal()*-6.f);
//        else if(event.key()==GLFW_KEY_W&&event.type()==QEvent::KeyRelease)
//            controller->addSpeedForward(glm::vec3(0,0,0));
//        else if(event.key()==GLFW_KEY_A&&event.type()==QEvent::KeyRelease)
//            controller->addSpeedRight(glm::vec3(0,0,0));
//        else if(event.key()==GLFW_KEY_D&&event.type()==QEvent::KeyRelease)
//            controller->addSpeedRight(glm::vec3(0,0,0));
//        else if(event.key()==GLFW_KEY_S&&event.type()==QEvent::KeyRelease)
//            controller->addSpeedForward(glm::vec3(0,0,0));
//        else if(event.key()==GLFW_KEY_M&&event.type()==QEvent::KeyPress){
////            setWireframeMode(!world->wireframeMode());
//        }else if(event.key()==GLFW_KEY_O&&event.type()==QEvent::KeyPress){
////            world->unloadWorld();
//        }
//    });
}

QObject *CoffeeAdvancedLoop::getFactory()
{
    return factory;
}

CoffeeScriptEngine *CoffeeAdvancedLoop::getScriptEngine()
{
    return scriptEngine;
}
