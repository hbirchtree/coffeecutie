#include "coffeeadvancedloop.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(QObject *parent, CoffeeRenderer* renderer, QString fileSource) : RenderLoop(parent)
{
    evloop = new QEventLoop(this);
    connectSignals(renderer);

    qDebug("Creating default rendering method");
//    defaultRenderingMethod->addVertexAttribute("vert",3);
//    defaultRenderingMethod->addVertexAttribute("vertTexCoord",2);
//    defaultRenderingMethod->addVertexAttribute("vertNormal",3);
//    defaultRenderingMethod->addVertexAttribute("vertTangent",3);

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
            CoffeeStandardObject* stdobj = dynamic_cast<CoffeeStandardObject*>(o);
            if(stdobj)
                setupRenderer(stdobj);
            if(o->physics())
                o->rotation()->bindValue(o->physics()->getPhysicalRotation());
            qDebug("Set up for rendering: %s",o->objectName().toStdString().c_str());
        }

        CoffeeSkybox* skb = dynamic_cast<CoffeeSkybox*>(skybox);
        if(skb)
            skb->setCamera(world->getCamera());

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
        skybox->render();
        world->renderWorld();

        //testing area
//        test->render();

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
    skybox = new CoffeeSkybox(this,0);

    CoffeeSkybox* skb = dynamic_cast<CoffeeSkybox*>(skybox);
    if(skb){
        skb->addMap(GL_TEXTURE_CUBE_MAP_NEGATIVE_X,"ubw/models/textures/scratchy.png");
        skb->addMap(GL_TEXTURE_CUBE_MAP_POSITIVE_X,"ubw/models/textures/scratchy.png");
        skb->addMap(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,"ubw/models/textures/scratchy.png");
        skb->addMap(GL_TEXTURE_CUBE_MAP_POSITIVE_Y,"ubw/models/textures/scratchy.png");
        skb->addMap(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,"ubw/models/textures/scratchy.png");
        skb->addMap(GL_TEXTURE_CUBE_MAP_POSITIVE_Z,"ubw/models/textures/scratchy.png");
    }

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

void CoffeeAdvancedLoop::setupRenderer(CoffeeStandardObject *object)
{
    object->shader()->buildProgram();

    for(int t : object->material()->getTextureKeys()){
        switch(t){
        case CoffeeTexture::Texture_Diffusion:
            object->setTexture("materialTex",object->material()->getTexture(t),GL_TEXTURE0);
            break;
        case CoffeeTexture::Texture_Bumpmap:
            object->setTexture("materialBump",object->material()->getTexture(t),GL_TEXTURE1);
            break;
        case CoffeeTexture::Texture_Specular:
            object->setTexture("materialSpecular",object->material()->getTexture(t),GL_TEXTURE2);
            break;
        case CoffeeTexture::Texture_Highlight:
            object->setTexture("materialHighlight",object->material()->getTexture(t),GL_TEXTURE3);
            break;
        case CoffeeTexture::Texture_Transparency:
            object->setTexture("materialTransparency",object->material()->getTexture(t),GL_TEXTURE4);
            break;
        }
    }

    object->setUniform("camera",new ShaderVariant([=](){
        return world->getCamera()->getMatrix();
    }));
    object->setUniform("cameraPosition",new ShaderVariant([=](){
        return world->getCamera()->getPosition()->getValue();
    }));

    object->setUniform("fogParams.fDensity",new ShaderVariant([=](){
        return world->getFogDensity();
    }));
    object->setUniform("fogParams.fColor",new ShaderVariant([=](){
        return world->getFogColor();
    }));

    object->setUniform("light.position",new ShaderVariant([=](){
        return world->getLights().first()->getPosition()->getValue();
    }));
    object->setUniform("light.intensities",new ShaderVariant([=](){
        return world->getLights().first()->getColor()->getValue();
    }));
    object->setUniform("light.attenuation",new ShaderVariant([=](){
        return world->getLights().first()->getAttenuation()->getValue();
    }));
    object->setUniform("light.ambientCoefficient",new ShaderVariant([=](){
        return world->getLights().first()->getAmbientCoefficient()->getValue();
    }));


    object->setUniform("materialShininess",new ShaderVariant([=](){
        return object->material()->shininess()->getValue();
    }));
    object->setUniform("materialTransparencyValue",new ShaderVariant([=](){
        return object->material()->transparency()->getValue();
    }));
    object->setUniform("colorMul",new ShaderVariant([=](){
        return object->material()->colorMultiplier();
    }));
    object->setUniform("model",new ShaderVariant([=](){
        return RenderingMethods::translateObjectMatrix(object->position()->getValue(),
                                                       object->rotation()->getValue(),
                                                       object->scale()->getValue());
    }));
}
