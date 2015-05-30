#include <QApplication>
#include <QDateTime>
#include <QThreadPool>
#include "coffeelogger.h"
#include "inspector/coffeeinspector.h"
#include "opengl/context/coffeerenderer.h"
#include "engine/rendering/coffeeadvancedloop.h"
#include "tests/boxtest.h"
#include "engine/physics/bulletphysics.h"

//#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
//#define RUNNABLE_RENDERER

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.16");

    qsrand((rand()%RAND_MAX)/10000.0);

    RenderLoop* loop;
    QObject* root = new QObject();
    CoffeeLogger logger; Q_UNUSED(logger);
    root->setObjectName("[main,unlimited-root]");
    CoffeeRenderer *renderer = new CoffeeRenderer(root,
                                                  1280,720,Qt::WindowNoState,
                                                  "Unlimited Frame Works");
    renderer->setObjectName("root.renderer");

#ifdef COFFEE_ADVANCED_RUN
    loop = new CoffeeAdvancedLoop(root,renderer,"ubw/ubw.json");
    loop->setObjectName("advanced-loop");
#else
    //This demo taken from glbinding tests out general rendering
    loop = new BoxTest(renderer);
#endif //COFFEE_ADVANCED_RUN

    renderer->setLoop(loop);
    renderer->setSamples(4);

#ifdef COFFEE_INSPECTOR_RUN
    QThreadPool::globalInstance()->setObjectName("QThreadPool");
    CoffeeInspector *inspector = new CoffeeInspector(0,
                                                     loop->getThreadObjects()
                                                     << root
                                                     << QThreadPool::globalInstance(),
                                                     renderer);
#endif //COFFEE_INSPECTOR_RUN

#ifndef RUNNABLE_RENDERER
    int initStat = renderer->init();
    switch(initStat){
    case 0:{
#ifdef COFFEE_INSPECTOR_RUN
        inspector->show();
#endif

        initStat = renderer->loop();
        break;
    }
    default:
        qDebug("init() with abnormal code %i",initStat);
    }
#else
        QThreadPool::globalInstance()->start(renderer);
#endif //RUNNABLE_RENDERER


#ifdef COFFEE_INSPECTOR_RUN
    inspector->deleteLater();
#endif

    delete root;

#ifndef RUNNABLE_RENDERER
    return initStat;
#else
    return 0;
#endif
}
