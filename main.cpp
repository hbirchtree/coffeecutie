#include <QApplication>
#include <QDateTime>
#include "coffeelogger.h"
#include "inspector/coffeeinspector.h"
#include "opengl/context/coffeerenderer.h"
#include "engine/rendering/coffeeadvancedloop.h"
#include "tests/boxtest.h"
#include "engine/physics/bulletphysics.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.12");

    qsrand((rand()%RAND_MAX)/10000.0);

    int mode = 0;

    QObject* root = new QObject();

    CoffeeLogger logger;
    Q_UNUSED(logger);

    CoffeeRenderer *renderer = new CoffeeRenderer(root,1280,720,Qt::WindowNoState,"Unlimited Frame Works");
    renderer->setObjectName("root.renderer");

    int initStat = renderer->init();

    //This demo taken from glbinding tests out general rendering
    RenderLoop* loop;

    switch(mode){
    case 0:
        loop = new CoffeeAdvancedLoop(renderer,"ubw/ubw.json");
        loop->setObjectName("advanced-loop");
        break;
    case 1:
        loop = new BoxTest(renderer);
        break;
    }

    renderer->setLoop(loop);
    renderer->setSamples(4);

    CoffeeInspector inspector(0,renderer,renderer);
    inspector.show();
    switch(initStat){
    case 0:
        initStat = renderer->loop();
        break;
    default:
        qDebug("init() with abnormal code %i",initStat);
    }
    delete root;
    return initStat;
}
