#include "coffeeadvancedloop.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(CoffeeRenderer* renderer)
{
    connectSignals(renderer);

    defaultRenderingMethod = new CoffeeRenderingMethod(renderer);
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

    CoffeeObjectFactory f;
    QList<CoffeeWorldOpts*> worlds = f.importObjects("testgame/cutie.json",this);
    world = worlds.first();

    _rendering_loop_init = [=](){

        renderer->setSamples(4);
        renderer->updateRendererClearColor(world->getClearColor());
        world->setRenderer(renderer);

        for(CoffeeObject* o : world->getObjects())
            setupRenderer(o,defaultRenderingMethod);

//        WavefrontModelReader rdr(world->getRenderer());
//        QHash<QString,QPointer<WavefrontModelReader::ModelContainer> > mdls = rdr.parseModel("testgame/models/terrain.obj");
//        QHash<QString,QPointer<WavefrontModelReader::ModelContainer> > q = rdr.parseModel("testgame/models/quad.obj");
//        QList<QPointer<WavefrontModelReader::ModelContainer> > vals = mdls.values();
//        test->setModel(vals.first()->model);
//        *test->getScaleObject()=glm::vec3(1,1,1);
//        test->setMaterial(vals.first()->material);

//        world->setCamera(new CoffeeCamera(world->getRenderer(),
//                                          1.6f,.001f,100.f,90.0f,
//                                          glm::vec3(0,5,0),glm::vec3(0,0,0)));
//        world->addLight(new CoffeeOmniLight(world->getRenderer(),"sun",glm::vec3(0,10,0),
//                                            glm::vec3(1,1,1),0.0005f,0.007f));
//        world->setFogColor(glm::vec4(0.f,0.2f,0.2f,1.f));
//        world->setFogDensity(0.07f);

        QSize s = world->getRenderer()->getCurrentFramebufferSize();
        *world->getCamera()->getAspect()=(float)s.width()/(float)s.height();
        glViewport(0,0,s.width(),s.height());

        glEnable(GL_TEXTURE_2D);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glFrontFace(GL_CCW);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        glfwSwapInterval(0);

        renderer->updateMouseGrabbing(true);
    };
    _rendering_loop = [=](){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        js->update();
//        testfbo->bindFramebuffer();
        for(CoffeeObject* o : world->getObjects())
            o->render();
//        testfbo->unbindFramebuffer();
    };
    _rendering_loop_cleanup = [=](){
    };
}

CoffeeAdvancedLoop::~CoffeeAdvancedLoop()
{
}

std::function<void ()> CoffeeAdvancedLoop::getInit()
{
    return _rendering_loop_init;
}

std::function<void ()> CoffeeAdvancedLoop::getLoop()
{
    return _rendering_loop;
}

std::function<void ()> CoffeeAdvancedLoop::getCleanup()
{
    return _rendering_loop_cleanup;
}

void CoffeeAdvancedLoop::connectSignals(CoffeeRenderer *renderer)
{
    controller = new CoffeePlayerController(this);
    js = new CoffeeJoystick(renderer,GLFW_JOYSTICK_1);
    testfbo = new CoffeeFrameBufferObject(this);

    testfbo->createFramebuffer();

    renderer->connect(renderer,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        *world->getCamera()->getAspect()=(float)ev.size().width()/(float)ev.size().height();
    });
    timers = new CoffeeDataContainer<QString,double>(renderer);
    renderer->connect(renderer,&CoffeeRenderer::contextReportFrametime,[=](float frametime){
//        test->getRotationObject()->setValue(test->getRotationObject()->getValue()+glm::vec3(0,frametime*1,0));
        if(glfwGetTime()>=timers->getValue("fps")){
            qDebug("FPS: %.0f",1.f/frametime);
            timers->setValue("fps",glfwGetTime()+1);
        }
    });
    renderer->connect(renderer,&CoffeeRenderer::winMouseEvent,[=](QMouseEvent event){
        if(event.type()==QMouseEvent::MouseMove){
            renderer->setMousePos(0,0);
            world->getCamera()->offsetOrientation(event.pos().x()*0.1,event.pos().y()*0.1);
        }
    });
    renderer->connect(renderer,&CoffeeRenderer::winKeyboardEvent,[=](QKeyEvent event){
        if(event.key()==GLFW_KEY_ESCAPE&&event.type()==QEvent::KeyPress)
            renderer->requestWindowClose();
        else if(event.key()==GLFW_KEY_W&&event.type()==QEvent::KeyPress)
            controller->addSpeed(world->getCamera()->getCameraForwardNormal()*0.2f);
        else if(event.key()==GLFW_KEY_S&&event.type()==QEvent::KeyPress)
            controller->addSpeed(world->getCamera()->getCameraForwardNormal()*-0.2f);
    });
    renderer->connect(js,&CoffeeJoystick::buttonPressed,[=](int btn){
        if(btn==6){
            world->getCamera()->setOrthographic(!world->getCamera()->isOrthographic());
        }
    });
    renderer->connect(js,&CoffeeJoystick::axisMoved,[=](int axe,float val){
        float normalized = val/js->getAxisfactor();
        switch(axe){
        case 0:
            controller->addSpeed(world->getCamera()->getCameraRightNormal()*normalized);
            break;
        case 1:
            controller->addSpeed(world->getCamera()->getCameraForwardNormal()*-normalized);
            break;
        case 3:
            if(val<0)
                break;
            world->getCamera()->offsetOrientation(val*0.00008,0);
            break;
        case 4:
            if(val<0)
                break;
            world->getCamera()->offsetOrientation(0,val*0.00008);
            break;
        }
    });
    renderer->connect(renderer,&CoffeeRenderer::winClose,[=](){
        renderer->requestWindowClose();
    });

    connect(renderer,SIGNAL(contextReportFrametime(float)),controller,SLOT(tick(float)));
    connect(controller,&CoffeePlayerController::movePlayer,[=](glm::vec4 d){
        *world->getCamera()->getPosition()+=glm::vec3(d);
    });
    connect(controller,&CoffeePlayerController::rotateCamera,[=](glm::vec3 d){
        world->getCamera()->offsetOrientation(d.y,d.x);
    });
}

void CoffeeAdvancedLoop::setupRenderer(CoffeeObject *object, CoffeeRenderingMethod* basicMethod)
{
    object->setShader(new ShaderContainer(world->getRenderer()));
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
