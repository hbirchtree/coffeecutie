#include "coffeeadvancedloop.h"

#include <QResizeEvent>

//Rendering
#include "opengl/context/coffeerendererbase.h"

//Display
#include "engine/objects/coffeeoutputsurface.h"
#include "opengl/components/coffeeframebufferobject.h"

//Engine
#include "engine/objects/coffeeobjectfactory.h"
#include "opengl/components/coffeeworldopts.h"
#include "engine/scripting/coffeescriptengine.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(QObject *parent, CoffeeRendererBase* renderer) : RenderLoop(parent)
{
    this->m_renderer = renderer;
    setObjectName("evloop");

    this->m_scriptEngine = new CoffeeScriptEngine(this);
    this->m_factory = new CoffeeObjectFactory(this);
    this->m_factory->setObjectName("factory");

    _rendering_loop_init = [=](){

        qDebug("Enabling standard OpenGL capabilities");

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        qDebug("Configuring framebuffer object");
    };
    _rendering_loop = [=](){
        //bind the framebuffer which we render to
//        screenSurface->bind();
        //clear the depth buffer, otherwise we won't see sh*t

        //render the current world
//        if(world)
//            world->renderWorld();

//        renderer->flushPipeline();

        //render for the user
//        m_screenSurface->render();
    };
    _rendering_loop_cleanup = [=](){
        qDebug("Running the empty cleanup function");
    };
}

std::function<void ()> *CoffeeAdvancedLoop::init()
{
    return &_rendering_loop_init;
}

std::function<void ()> *CoffeeAdvancedLoop::loop()
{
    return &_rendering_loop;
}

std::function<void ()> *CoffeeAdvancedLoop::cleanup()
{
    return &_rendering_loop_cleanup;
}

QObject *CoffeeAdvancedLoop::getFactory()
{
    return m_factory;
}

CoffeeScriptEngine *CoffeeAdvancedLoop::getScriptEngine()
{
    return m_scriptEngine;
}

QObject *CoffeeAdvancedLoop::getWorld()
{
    return m_world;
}

QObject *CoffeeAdvancedLoop::factory() const
{
    return m_factory;
}

QObject *CoffeeAdvancedLoop::createRenderSurface()
{
    CoffeeOutputSurface* m_screenSurface =
            new CoffeeOutputSurface(this,new CoffeeFramebufferObject(this));
    m_screenSurface->getFramebuffer()->
            setSize(m_renderer->getCurrentFramebufferSize(),1);
    connect(m_renderer,&CoffeeRendererBase::winFrameBufferResize,
            [=](QResizeEvent e){
        m_screenSurface->getFramebuffer()->setSize(e.size(),1);
    });
    return m_screenSurface;
}

void CoffeeAdvancedLoop::setWorld(QObject *world)
{
    CoffeeWorldOpts* w = qobject_cast<CoffeeWorldOpts*>(world);
    if(this->m_world){
        this->m_world->unloadWorld();
        this->m_world->disconnectSignals(); //avoid changing state of the world
    }
    if(w){
        w->setRenderer(m_renderer);
        this->m_world = w;
    }
}
