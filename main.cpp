#include <QApplication>
#include <QDateTime>
#include "opengl/coffeerenderer.h"
#include "opengl/qcoffeerenderer.h"
#include "opengl/rendering/coffeerenderingloop.h"
#include "opengl/components/coffeeobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/models/wavefrontmodelreader.h"
#include "general/input/coffeejoystick.h"
#include "opengl/helpers/renderingmethods.h"
#include "tests/boxtest.h"
#include "general/data/coffeedatacontainer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.0.4");

    CoffeeRenderer *renderer = new CoffeeRenderer(0,1280,720,Qt::WindowNoState,"CoffeeCutie");
    renderer->setObjectName("root.renderer");

    CoffeeObject *test = new CoffeeObject(renderer);
    CoffeeWorldOpts *world = new CoffeeWorldOpts(renderer);

    CoffeeRenderingLoop loop(
    [=](){
        renderer->setSamples(4);
        renderer->updateRendererClearColor(glm::vec4(0,0.2,0.2,1));

        WavefrontModelReader rdr(world->getRenderer());
        QHash<QString,QPointer<WavefrontModelReader::ModelContainer> > mdls = rdr.parseModel("testgame/models/3dbox_bumped.obj");
        test->setShader(new ShaderContainer(world->getRenderer()));
        test->getShader()->setObjectName("test:shader");
        test->getShader()->buildProgram("testgame/shaders/vsh.txt","testgame/shaders/fsh_bumpmap.txt");
        QList<QPointer<WavefrontModelReader::ModelContainer> > vals = mdls.values();
        test->setModel(vals.first()->model);
        test->getScaleObject()->setValue(glm::vec3(1,1,1));
        test->setMaterial(vals.first()->material);

        world->setCamera(new CoffeeCamera(world->getRenderer(),1.6f,1.f,100.f,90.0f,glm::vec3(0,0,5),glm::vec3(0,0,0)));
        world->getCamera()->setFramebufferSizeObject(renderer->getFramebufferSizePt());
        world->addLight(new CoffeeOmniLight(world->getRenderer(),"sun",glm::vec3(0,0,5),glm::vec3(1,1,1),0.00005f,0.007f));
        world->setFogColor(glm::vec4(0.f,0.2f,0.2f,1.f));
        world->setFogDensity(0.00000007f);

//        CoffeeJoystick js(world->getRenderer(),GLFW_JOYSTICK_1);

        QSize s = world->getRenderer()->getCurrentFramebufferSize();
        world->getCamera()->getAspect()->setValue((float)s.width()/(float)s.height());
        glViewport(0,0,s.width(),s.height());

        glEnable(GL_TEXTURE_2D);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

//        glEnable(GL_STENCIL_TEST);

        glFrontFace(GL_CCW);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

//        glfwSwapInterval(0);
    },
    [test,world](){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        RenderingMethods::rendering_simple(test,world);
    },
    [test,world](){
        test->unloadAssets();
    });
    renderer->connect(renderer,&CoffeeRenderer::winFrameBufferResize,[=](QResizeEvent ev){
        qDebug() << "Resizing: " << ev.size() << (float)ev.size().width()/(float)ev.size().height();
        world->getCamera()->getAspect()->setValue((float)ev.size().width()/(float)ev.size().height());
        glViewport(0,0,ev.size().width(),ev.size().height());
    });
    CoffeeDataContainer<QString,double>* timers = new CoffeeDataContainer<QString,double>(renderer);
    renderer->connect(renderer,&CoffeeRenderer::contextReportFramerate,[=](float frametime){
        test->getRotationObject()->setValue(test->getRotationObject()->getValue()+glm::vec3(0,frametime*1,0));
        if(glfwGetTime()>=timers->getValue("fps")){
            qDebug("FPS: %f",1.f/frametime);
            timers->setValue("fps",glfwGetTime()+1);
            world->getCamera()->setOrthographic(!world->getCamera()->isOrthographic());
        }
    });

//    BoxTest loop(renderer);

    renderer->setLoop(&loop);
    renderer->setSamples(4);

    int initStat = renderer->init();
    switch(initStat){
    case 0:
        return renderer->loop();
    default:
        qDebug() << "Program exited with errorcode: " << initStat;
        return initStat;
    }
}
