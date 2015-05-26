#include "coffeeadvancedloop.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(QObject *parent, CoffeeRenderer* renderer, QString fileSource) : RenderLoop(parent)
{
    evloop = new QEventLoop(this);
    connectSignals(renderer);

    qDebug("Creating default rendering method");
    defaultRenderingMethod = new CoffeeRenderingMethod(this);
    //Here we use a template for all uniform variables as well as attributes. All objects need these.
    defaultRenderingMethod->addShaderUniform("camera",new ShaderVariant([=](){
        return world->getCamera()->getMatrix();
    }));
    defaultRenderingMethod->addShaderUniform("cameraPosition",new ShaderVariant([=](){
        return world->getCamera()->getPosition()->getValue();
    }));
    defaultRenderingMethod->addShaderUniform("light.position",new ShaderVariant([=](){
        return world->getLights().first()->getPosition()->getValue();
    }));
    defaultRenderingMethod->addShaderUniform("light.intensities",new ShaderVariant([=](){
        return world->getLights().first()->getColor()->getValue();
    }));
    defaultRenderingMethod->addShaderUniform("light.attenuation",new ShaderVariant([=](){
        return world->getLights().first()->getAttenuation()->getValue();
    }));
    defaultRenderingMethod->addShaderUniform("light.ambientCoefficient",new ShaderVariant([=](){
        return world->getLights().first()->getAmbientCoefficient()->getValue();
    }));
    defaultRenderingMethod->addShaderUniform("fogParams.fDensity",new ShaderVariant([=](){
        return world->getFogDensity();
    }));
    defaultRenderingMethod->addShaderUniform("fogParams.fColor",new ShaderVariant([=](){
        return world->getFogColor();
    }));
    defaultRenderingMethod->addVertexAttribute("vert",3);
    defaultRenderingMethod->addVertexAttribute("vertTexCoord",2);
    defaultRenderingMethod->addVertexAttribute("vertNormal",3);
    defaultRenderingMethod->addVertexAttribute("vertTangent",3);

    qDebug("Importing objects from file");
    CoffeeObjectFactory f;
    QList<CoffeeWorldOpts*> worlds = f.importObjects(fileSource,this);
    if(worlds.isEmpty())
        qDebug("Failed to load any world information! Brace for impact!");
    world = worlds.first();
    connect(renderer,SIGNAL(contextReportFrametime(float)),world,SLOT(tickObjects(float)));

    _rendering_loop_init = [=](){

        qDebug("Configuring renderer");
        renderer->setSamples(4);
        renderer->updateRendererClearColor(world->getClearColor());
        world->setRenderer(renderer);

        qDebug("Configuring objects for rendering");
        for(CoffeeObject* o : world->getObjects()){
            setupRenderer(o,defaultRenderingMethod);
            if(o->getPhysicsObject())
                o->getRotationObject()->bindValue(o->getPhysicsObject()->getPhysicalRotation());
            qDebug("Set up for rendering: %s",o->objectName().toStdString().c_str());
        }

        qDebug("Resizing viewport");
        QSize s = world->getRenderer()->getCurrentFramebufferSize();
        *world->getCamera()->getAspect()=(float)s.width()/(float)s.height();
        glViewport(0,0,s.width(),s.height());

        test = new CoffeeParticleSystem(this,world->getCamera());
        test->setObjectName("particle-system");
        test->setProperties(glm::vec3(-10.0f, 17.5f, 0.0f), // Where the particles are generated
                            glm::vec3(-5, 5, -5), // Minimal velocity
                            glm::vec3(5, 5, 5), // Maximal velocity
                            glm::vec3(0, -5, 0), // Gravity force applied to particles
                            glm::vec3(0.0f, 0.5f, 1.0f), // Color (light blue)
                            1.5f, // Minimum lifetime in seconds
                            3.0f, // Maximum lifetime in seconds
                            0.75f, // Rendered size
                            0.01f, // Spawn every 0.05 seconds
                            10);

        qDebug("Enabling standard OpenGL capabilities");
        glEnable(GL_TEXTURE_2D);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glFrontFace(GL_CCW);

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
        });

        qDebug("Creating output surface");
        screenSurface = new CoffeeOutputSurface(this,renderFbo);
    };
    _rendering_loop = [=](){
        js->update();
        //bind the framebuffer which we render to
        renderFbo->bindFramebuffer();
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        //render the current world
        world->renderWorld();

        //testing area
        test->render();

        //render for the user
        renderFbo->unbindFramebuffer();
        screenSurface->render();
    };
    _rendering_loop_cleanup = [=](){
        qDebug("Running the empty cleanup function");
    };
}

CoffeeAdvancedLoop::~CoffeeAdvancedLoop()
{for(CoffeeObject* o : world->getObjects()){
        o->render();
    }
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
            controller->setRotationYaw(val*val*val*80);
            break;
        case 4:
            if((val<0&&diff>0)||(val>0&&diff<0))
                break;
            controller->setRotationPitch(val*val*val*-80);
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

void CoffeeAdvancedLoop::setupRenderer(CoffeeObject *object, CoffeeRenderingMethod* basicMethod)
{
    object->setShader(new ShaderContainer(object));
    object->getShader()->setObjectName(object->objectName()+".shader");
    object->getShader()->buildProgram(object->getVertShader(),object->getFragShader());

    object->setRenderer(basicMethod->createInstance(object));
    object->getRenderer()->setObjectName(object->objectName()+".renderer");
    for(int t : object->getMaterial()->getTextureKeys()){
        switch(t){
        case CoffeeTexture::Texture_Diffusion:
            object->getRenderer()->addTextureMapping(object->getMaterial()->getTexture(t),GL_TEXTURE0,"materialTex");
            break;
        case CoffeeTexture::Texture_Bumpmap:
            object->getRenderer()->addTextureMapping(object->getMaterial()->getTexture(t),GL_TEXTURE1,"materialBump");
            break;
        case CoffeeTexture::Texture_Specular:
            object->getRenderer()->addTextureMapping(object->getMaterial()->getTexture(t),GL_TEXTURE2,"materialSpecular");
            break;
        case CoffeeTexture::Texture_Highlight:
            object->getRenderer()->addTextureMapping(object->getMaterial()->getTexture(t),GL_TEXTURE3,"materialHighlight");
            break;
        case CoffeeTexture::Texture_Transparency:
            object->getRenderer()->addTextureMapping(object->getMaterial()->getTexture(t),GL_TEXTURE4,"materialTransparency");
            break;
        }
    }

    object->getRenderer()->addShaderUniform("materialShininess",new ShaderVariant([=](){
        return object->getMaterial()->shininess()->getValue();
    }));
    object->getRenderer()->addShaderUniform("materialTransparencyValue",new ShaderVariant([=](){
        return object->getMaterial()->transparency()->getValue();
    }));
    object->getRenderer()->addShaderUniform("colorMul",new ShaderVariant([=](){
        return object->getMaterial()->colorMultiplier();
    }));
    object->getRenderer()->addShaderUniform("model",new ShaderVariant([=](){
        return RenderingMethods::translateObjectMatrix(object);
    }));
}
