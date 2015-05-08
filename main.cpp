#include <QApplication>
#include <QDateTime>
#include "coffeelogger.h"
#include "opengl/context/coffeerenderer.h"
#include "engine/rendering/coffeeadvancedloop.h"
#include "tests/boxtest.h"
#include "engine/physics/bulletphysics.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.8");

    int mode = 0;

    CoffeeLogger logger;
    Q_UNUSED(logger);

    CoffeeRenderer *renderer = new CoffeeRenderer(0,1280,720,Qt::WindowNoState,"CoffeeCutie");
    renderer->setObjectName("root.renderer");

    int initStat = renderer->init();

    BulletPhysics* phys = new BulletPhysics(renderer,glm::vec3(0.f,-9.81f,0.f));

    //This demo taken from glbinding tests out general rendering
    RenderLoop* loop;

    switch(mode){
    case 0:
        loop = new CoffeeAdvancedLoop(renderer);
        break;
    case 1:
        loop = new BoxTest(renderer);
        break;
    }

    renderer->setLoop(loop);
    renderer->setSamples(4);

    switch(initStat){
    case 0:
        initStat = renderer->loop();
        break;
    default:
        qDebug("init() with abnormal code %i",initStat);
    }
    delete renderer;
    return initStat;
}
