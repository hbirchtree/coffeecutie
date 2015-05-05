#include "coffeeadvancedloop.h"

CoffeeAdvancedLoop::CoffeeAdvancedLoop(CoffeeRenderer* renderer)
{
    connectSignals(renderer);
    _rendering_loop_init = [=](){
        renderer->setSamples(4);
        renderer->updateRendererClearColor(glm::vec4(0,0.2,0.2,1));

        WavefrontModelReader rdr(world->getRenderer());
        QHash<QString,QPointer<WavefrontModelReader::ModelContainer> > mdls = rdr.parseModel("testgame/models/terrain.obj");
        test->setShader(new ShaderContainer(world->getRenderer()));
        test->getShader()->setObjectName("test:shader");
        test->getShader()->buildProgram("testgame/shaders/vsh.txt","testgame/shaders/fsh.txt");
        QList<QPointer<WavefrontModelReader::ModelContainer> > vals = mdls.values();
        test->setModel(vals.first()->model);
        *test->getScaleObject()=glm::vec3(1,1,1);
        test->setMaterial(vals.first()->material);

        world->setCamera(new CoffeeCamera(world->getRenderer(),
                                          1.6f,.001f,100.f,90.0f,
                                          glm::vec3(0,5,0),glm::quat(1,0,0,0)));
        world->addLight(new CoffeeOmniLight(world->getRenderer(),"sun",glm::vec3(0,10,0),
                                            glm::vec3(1,1,1),0.0005f,0.007f));
        world->setFogColor(glm::vec4(0.f,0.2f,0.2f,1.f));
        world->setFogDensity(0.07f);

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

        //Here we use ntest as a template for all uniform variables as well as attributes. All objects need these.
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

        //However, only the test object requires these uniforms and textures.
        test->setRenderer(defaultRenderingMethod->createInstance(test));
        test->getRenderer()->addTextureMapping(test->getMaterial()->getTexture(CoffeeTexture::Texture_Diffusion),GL_TEXTURE0,"materialTex");
//        test->getRenderer()->addTextureMapping(test->getMaterial()->getTexture(CoffeeTexture::Texture_Bumpmap),GL_TEXTURE1,"materialBump");
//        test->getRenderer()->addTextureMapping(test->getMaterial()->getTexture(CoffeeTexture::Texture_Specular),GL_TEXTURE2,"materialSpecular");
//        test->getRenderer()->addTextureMapping(test->getMaterial()->getTexture(CoffeeTexture::Texture_Highlight),GL_TEXTURE3,"materialHighlight");
//        test->getRenderer()->addTextureMapping(test->getMaterial()->getTexture(CoffeeTexture::Texture_Transparency),GL_TEXTURE4,"materialTransparency");
        test->getRenderer()->addShaderUniform("materialShininess",new ShaderVariant([=](){
            return test->getMaterial()->shininess()->getValue();
        }));
        test->getRenderer()->addShaderUniform("materialTransparencyValue",new ShaderVariant([=](){
            return test->getMaterial()->transparency()->getValue();
        }));
        test->getRenderer()->addShaderUniform("colorMul",new ShaderVariant([=](){
            return test->getMaterial()->colorMultiplier();
        }));
        test->getRenderer()->addShaderUniform("model",new ShaderVariant([=](){
            return RenderingMethods::translateObjectMatrix(test);
        }));

        renderer->updateMouseGrabbing(true);
    };
    _rendering_loop = [=](){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        test->render();
        js->update();
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
    test = new CoffeeObject(renderer);
    world = new CoffeeWorldOpts(renderer);
    js = new CoffeeJoystick(renderer,GLFW_JOYSTICK_1);
    defaultRenderingMethod = new CoffeeRenderingMethod(renderer);

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
            world->getCamera()->getPosition()->addValue(world->getCamera()->getCameraForwardNormal());
        else if(event.key()==GLFW_KEY_S&&event.type()==QEvent::KeyPress)
            world->getCamera()->getPosition()->addValue(world->getCamera()->getCameraForwardNormal()*glm::vec3(-1,-1,-1));
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
            *world->getCamera()->getPosition()+=
                        world->getCamera()->getCameraRightNormal()*
                        glm::vec3(normalized,normalized,normalized);
            break;
        case 1:
            *world->getCamera()->getPosition()+=
                        world->getCamera()->getCameraForwardNormal()*
                        glm::vec3(-normalized,-normalized,-normalized);
            break;
        case 3:
            world->getCamera()->offsetOrientation(val*0.00008,0);
            break;
        case 4:
            world->getCamera()->offsetOrientation(0,val*0.00008);
            break;
        }
    });
    renderer->connect(renderer,&CoffeeRenderer::winClose,[=](){
        renderer->requestWindowClose();
    });
}
