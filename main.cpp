#include <QApplication>
#include <QDateTime>
#include <QCommandLineParser>
#include <QThreadPool>
#include "coffeelogger.h"
#include "inspector/coffeeinspector.h"
#include "opengl/context/coffeerenderer.h"
#include "engine/rendering/coffeeadvancedloop.h"
#include "tests/boxtest.h"
#include "engine/physics/bulletphysics.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
//#define RUNNABLE_RENDERER

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.16");
    a.setApplicationDisplayName("Coffee Cutie");

    bool inspect = true;

    QCommandLineParser opts;
    opts.setApplicationDescription("A scriptable game engine");
    opts.addVersionOption();
    opts.addHelpOption();
    opts.addOption(QCommandLineOption("licenses","Get licensing information"));
    opts.addOption(QCommandLineOption("inspect"));
    opts.addPositionalArgument("configuration file","Source .json file","*.json");

    opts.process(a);
    for(QString key : opts.optionNames()){
        if(key=="licenses"){
            //show license information
            return 0;
        }else if(key=="inspect"){
            inspect = true;
        }
    }

    QString sourceFile = "ubw/ubw.json";
    if(opts.positionalArguments().size()>0){
        sourceFile = opts.positionalArguments().at(0);
    }

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
    loop = new CoffeeAdvancedLoop(root,renderer,sourceFile);
    loop->setObjectName("advanced-loop");
#else
    //This demo taken from glbinding tests out general rendering
    loop = new BoxTest(renderer);
#endif //COFFEE_ADVANCED_RUN

    renderer->setLoop(loop);
    renderer->setSamples(4);

#ifdef COFFEE_INSPECTOR_RUN
    QThreadPool::globalInstance()->setObjectName("QThreadPool");
    CoffeeInspector *inspector;
    if(inspect)
         inspector = new CoffeeInspector(0,
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
        if(inspect)
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
        if(inspect)
            inspector->deleteLater();
#endif

    delete root;

#ifndef RUNNABLE_RENDERER
    return initStat;
#else
    return 0;
#endif
}
