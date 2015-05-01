#include <QApplication>
#include "opengl/coffeerenderer.h"
#include "opengl/qcoffeerenderer.h"
#include "opengl/rendering/coffeerenderingloop.h"
#include "opengl/components/coffeeobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "general/models/wavefrontmodelreader.h"
#include "general/input/coffeejoystick.h"
#include "opengl/helpers/renderingmethods.h"
#include "tests/boxtest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.0.3");

    CoffeeRenderer *renderer = new CoffeeRenderer(0,1280,720,Qt::WindowNoState,"CoffeeCutie");

    CoffeeObject *test = new CoffeeObject(renderer);
    CoffeeWorldOpts *world = new CoffeeWorldOpts(renderer);

    CoffeeRenderingLoop loop(
    [=](){
        renderer->setSamples(4);
        renderer->updateRendererClearColor(glm::vec4(0,0.2,0.2,1));

        WavefrontModelReader rdr(world->getRenderer());
        QHash<QString,QPointer<WavefrontModelReader::ModelContainer> > mdls = rdr.parseModel("testgame/models/3dbox_bumped.obj");
        test->setShader(new ShaderContainer(world->getRenderer()));
        test->getShader()->buildProgram("testgame/shaders/vsh.txt","testgame/shaders/fsh.txt");
        QList<QPointer<WavefrontModelReader::ModelContainer> > vals = mdls.values();
        test->setModel(vals.first()->model);
        test->getScaleObject()->setValue(glm::vec3(1,1,1));
        test->setMaterial(vals.first()->material);

        world->setCamera(new CoffeeCamera(world->getRenderer(),1.6,1.f,100.f,90.0,glm::vec3(20,0,20),glm::vec3(0,0,0)));
        world->addLight(new CoffeeOmniLight(world->getRenderer(),"sun",glm::vec3(0,5,0),glm::vec3(1,1,1),float(0.01),float(0.01)));

        CoffeeJoystick js(world->getRenderer(),GLFW_JOYSTICK_1);

        QSize s = world->getRenderer()->getCurrentFramebufferSize();
        glViewport(0,0,s.width(),s.height());
    },
    [test,world](){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        if(std::fmod(glfwGetTime(),0.25)>0.125){
            world->getCamera()->offsetOrientation(5,5);
        }
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
