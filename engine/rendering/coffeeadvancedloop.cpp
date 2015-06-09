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

#include <QtScript>

CoffeeAdvancedLoop::CoffeeAdvancedLoop(QObject *parent, CoffeeRenderer* renderer, QString fileSource) : RenderLoop(parent)
{
//    scriptEngine = new QScriptEngine(this);

    evloop = new QEventLoop(this);
    connectSignals(renderer);

    qDebug("Importing objects from file");
    CoffeeObjectFactory f;
    QList<CoffeeWorldOpts*> worlds = f.importObjects(fileSource,this);
    if(worlds.isEmpty())
        qFatal("Failed to load any world information!");
    world = worlds.first();
    connect(renderer,SIGNAL(contextReportFrametime(float)),world,SLOT(tickObjects(float)));

//    QScriptValue worldValue = scriptEngine->newQObject(world);
//    QScriptValue rendererValue = scriptEngine->newQObject(renderer);

//    scriptEngine->globalObject().setProperty("root",rendererValue);
//    scriptEngine->globalObject().setProperty(world->objectName().toStdString().c_str(),worldValue);

//    qDebug() << scriptEngine->evaluate("world1.blade.setPosition(5.,0.0,5.0)\n").toString();
//    qDebug() << scriptEngine->evaluate("world1.blade.position\n").toString();
//    qDebug() << scriptEngine->evaluate("world1.blade.material.opacity\n").toString();

    _rendering_loop_init = [=](){

        qDebug("Configuring renderer");
        renderer->setSamples(4);
        renderer->updateRendererClearColor(world->getClearColor());
        world->setRenderer(renderer);

        qDebug("Configuring objects for rendering");
        for(CoffeeObject* o : world->getObjects()){
            CoffeeStandardObject* stdobj = dynamic_cast<CoffeeStandardObject*>(o);
            if(stdobj){
                setupRenderer(stdobj);
                if(stdobj->objectName().contains("gear")){
                    stdobj->mesh()->setUseInstancing(true);
                    stdobj->mesh()->getInstances()->createInstance();
                    test = stdobj;
                }
            }
            if(o->physics())
                o->rotation()->bindValue(o->physics()->getPhysicalRotation());
            qDebug("Set up for rendering: %s",stdobj->objectName().toStdString().c_str());
        }

        world->getLights().first()->getPosition()->bindValue(world->getCamera()->getPosition());

        qDebug("Resizing viewport");
        QSize s = world->getRenderer()->getCurrentFramebufferSize();
        *world->getCamera()->getAspect()=(float)s.width()/(float)s.height();
        glViewport(0,0,s.width(),s.height());

        qDebug("Enabling standard OpenGL capabilities");
//        glEnable(GL_TEXTURE_2D);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        qDebug("Setting vertical sync mode");
        glfwSwapInterval(0);

        qDebug("Configuring framebuffer object");
        renderFbo->createFramebuffer(renderer->getWindowDimensions(),1);
        connect(renderer,&CoffeeRenderer::winResize,[=](QResizeEvent e){ //We need to resize the FBO when the window dimensions change
            renderFbo->resizeViewport(e.size());
            world->getCamera()->setAspect((float)e.size().width()/(float)e.size().height());
        });

        qDebug("Creating output surface");
        screenSurface = new CoffeeOutputSurface(this,renderFbo);
    };
    _rendering_loop = [=](){
        test->mesh()->getInstances()->getInstance(0)->getPos()->setValue(
                    glm::vec3(std::fmod(renderer->getLoopTime()*5,10),0,0));
        test->mesh()->updateModelMatrices();
        js->update();
        //bind the framebuffer which we render to
        renderFbo->bindFramebuffer();
        //clear the depth buffer, otherwise we won't see sh*t
        glClear(GL_DEPTH_BUFFER_BIT);

        //render the current world
        world->renderWorld();

        glFlush();

        //render for the user
        renderFbo->unbindFramebuffer();
        screenSurface->render();
    };
    _rendering_loop_cleanup = [=](){
        qDebug("Running the empty cleanup function");
    };
}

CoffeeAdvancedLoop::~CoffeeAdvancedLoop()
{
}

QList<QObject *> CoffeeAdvancedLoop::getThreadObjects() const
{
    return QList<QObject*>() << world->getPhysicsRoot();
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
    js = new CoffeeJoystick(renderer,GLFW_JOYSTICK_1);
    renderFbo = new CoffeeFrameBufferObject(this);

    qDebug("Setting up miscellaneous signals and slots");
    renderer->connect(renderer,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        *world->getCamera()->getAspect()=(float)ev.size().width()/(float)ev.size().height();
    });
    timers = new CoffeeDataContainer<QString,double>(this);
    renderer->connect(renderer,&CoffeeRenderer::contextReportFrametime,[=](float frametime){
        if(glfwGetTime()>=timers->getValue("fps")){
            qDebug("FPS: %.0f",1.f/frametime);
            timers->setValue("fps",glfwGetTime()+1);
        }
    });
    renderer->connect(renderer,&CoffeeRenderer::winClose,[=](){
        qDebug("Window closing request received");
        renderer->requestWindowClose();
    });
    connect(renderer,SIGNAL(contextReportFrametime(float)),controller,SLOT(tick(float)));

    qDebug("Configuring input handling");
    renderer->connect(renderer,&CoffeeRenderer::winMouseEvent,[=](QMouseEvent event){
        if(event.type()==QMouseEvent::MouseMove&&renderer->isMouseGrabbed()){
            renderer->setMousePos(0,0);
            world->getCamera()->offsetOrientation(event.pos().x()*0.1,event.pos().y()*0.1);
        }
        if(event.type()==QMouseEvent::MouseButtonPress){
            if(event.button()==Qt::LeftButton)
                renderer->updateMouseGrabbing(true);
            else if(event.button()==Qt::RightButton)
                renderer->updateMouseGrabbing(false);
        }
    });
    renderer->connect(renderer,&CoffeeRenderer::winKeyboardEvent,[=](QKeyEvent event){
        if(event.key()==GLFW_KEY_ESCAPE&&event.type()==QEvent::KeyPress)
            renderer->requestWindowClose();
        else if(event.key()==GLFW_KEY_W&&event.type()==QEvent::KeyPress)
            controller->addSpeedForward(world->getCamera()->getCameraForwardNormal()*6.f);
        else if(event.key()==GLFW_KEY_A&&event.type()==QEvent::KeyPress)
            controller->addSpeedRight(world->getCamera()->getCameraRightNormal()*-6.f);
        else if(event.key()==GLFW_KEY_D&&event.type()==QEvent::KeyPress)
            controller->addSpeedRight(world->getCamera()->getCameraRightNormal()*6.f);
        else if(event.key()==GLFW_KEY_S&&event.type()==QEvent::KeyPress)
            controller->addSpeedForward(world->getCamera()->getCameraForwardNormal()*-6.f);
        else if(event.key()==GLFW_KEY_W&&event.type()==QEvent::KeyRelease)
            controller->addSpeedForward(glm::vec3(0,0,0));
        else if(event.key()==GLFW_KEY_A&&event.type()==QEvent::KeyRelease)
            controller->addSpeedRight(glm::vec3(0,0,0));
        else if(event.key()==GLFW_KEY_D&&event.type()==QEvent::KeyRelease)
            controller->addSpeedRight(glm::vec3(0,0,0));
        else if(event.key()==GLFW_KEY_S&&event.type()==QEvent::KeyRelease)
            controller->addSpeedForward(glm::vec3(0,0,0));
        else if(event.key()==GLFW_KEY_M&&event.type()==QEvent::KeyPress)
            world->setWireframeMode(!world->wireframeMode());
        else if(event.key()==GLFW_KEY_O&&event.type()==QEvent::KeyPress){
//            world->unloadWorld();
        }
    });
    renderer->connect(js,&CoffeeJoystick::buttonPressed,[=](int btn){
        if(btn==6){
            world->getCamera()->setOrthographic(!world->getCamera()->isOrthographic());
        }
    });
    renderer->connect(js,&CoffeeJoystick::axisMoved,[=](int axe,float val, float diff){
        switch(axe){
        case 0:
            controller->addSpeedForward(world->getCamera()->getCameraRightNormal()*val*5.f);
            break;
        case 1:
            controller->addSpeedRight(world->getCamera()->getCameraForwardNormal()*-val*5.f);
            break;
        case 2:
//            world->getCamera()->getFieldOfView()->setValue(120+val*70);
            break;
        case 3:
            if((val<0&&diff>0)||(val>0&&diff<0))
                break;
            controller->setRotationYaw(std::pow(val,3)*80);
            break;
        case 4:
            if((val<0&&diff>0)||(val>0&&diff<0))
                break;
            controller->setRotationPitch(std::pow(val,3)*-80);
            break;
        }
    });
    connect(controller,&CoffeePlayerController::rotateCamera,[=](glm::vec3 d){
        world->getCamera()->offsetOrientation(d.y,d.x);
    });
    connect(controller,&CoffeePlayerController::movePlayer,[=](glm::vec4 d){
        *world->getCamera()->getPosition()+=glm::vec3(d);
    });
}

void CoffeeAdvancedLoop::setupRenderer(CoffeeStandardObject *object)
{
    object->shader()->buildProgram();

    for(CoffeeTexture::CoffeeTextureType t : object->material()->getTextureKeys()){
        switch(t){
        case CoffeeTexture::CoffeeTexture_Ambient:
            object->setTexture("mtl.ambientSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Heightmap:
            object->setTexture("mtl.heightmapSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Emissive:
            object->setTexture("mtl.emissiveSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Displacement:
            object->setTexture("mtl.displacementSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Lightmap:
            object->setTexture("mtl.lightmapSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Reflection:
            object->setTexture("mtl.reflectivitySampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Diffusion:
            object->setTexture("mtl.diffuseSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Bumpmap:
            object->setTexture("mtl.bumpSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Specular:
            object->setTexture("mtl.specularSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Highlight:
            object->setTexture("mtl.highlightSampler",object->material()->getTexture(t));
            break;
        case CoffeeTexture::CoffeeTexture_Transparency:
            object->setTexture("mtl.transparencySampler",object->material()->getTexture(t));
            break;
        default:
            break;
        }
    }

    object->setUniform("camera",new ShaderVariant([=](){
        return world->getCamera()->getMatrix();
    }),false);
    object->setUniform("cameraPosition",new ShaderVariant([=](){
        return world->getCamera()->getPosition()->getValue();
    }),false);

    object->setUniform("fogParams.fDensity",new ShaderVariant([=](){
        return world->getFogDensity();
    }),false);
    object->setUniform("fogParams.fColor",new ShaderVariant([=](){
        return world->getFogColor();
    }),false);

    object->setUniform("light.position",new ShaderVariant([=](){
        return world->getLights().first()->getPosition()->getValue();
    }),false);
    object->setUniform("light.intensities",new ShaderVariant([=](){
        return world->getLights().first()->getColor()->getValue();
    }),false);
    object->setUniform("light.attenuation",new ShaderVariant([=](){
        return world->getLights().first()->getAttenuation()->getValue();
    }),false);
    object->setUniform("light.ambientCoefficient",new ShaderVariant([=](){
        return world->getLights().first()->getAmbientCoefficient()->getValue();
    }),false);

    object->setUniform("mtl.shininess",new ShaderVariant([=](){
        return object->material()->shininess();
    }),false);
    object->setUniform("mtl.opacity",new ShaderVariant([=](){
        return object->material()->opacity();
    }),false);
    object->setUniform("mtl.shininessStrength",new ShaderVariant([=](){
        return object->material()->shininessStrength();
    }),false);
    object->setUniform("mtl.transparencyValue",new ShaderVariant([=](){
        return object->material()->transparency();
    }),false);
    object->setUniform("mtl.colorMultiplier",new ShaderVariant([=](){
        return object->material()->colorMultiplier();
    }),false);
    object->setUniform("mtl.specularColor",new ShaderVariant([=](){
        return object->material()->specularColor();
    }),false);
    object->setUniform("mtl.diffuseColor",new ShaderVariant([=](){
        return object->material()->diffuseColor();
    }),false);
    object->setUniform("mtl.emissiveColor",new ShaderVariant([=](){
        return object->material()->emissiveColor();
    }),false);
    object->setUniform("mtl.refraction",new ShaderVariant([=](){
        return object->material()->refraction();
    }),false);
    object->setUniform("mtl.reflectivity",new ShaderVariant([=](){
        return object->material()->reflectivity();
    }),false);
    object->setUniform("mtl.ambientColor",new ShaderVariant([=](){
        return object->material()->ambientColor();
    }),false);
    object->setUniform("model",new ShaderVariant([=](){
        return RenderingMethods::translateObjectMatrix(object->position()->getValue(),
                                                       object->rotation()->getValue(),
                                                       object->scale()->getValue());
    }),false);
}
