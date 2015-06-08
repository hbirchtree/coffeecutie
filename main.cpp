#include <QApplication>
#include <QCommandLineParser>
#include <QThreadPool>
#include "coffeelogger.h"
#include "inspector/coffeeinspector.h"
#include "engine/rendering/coffeeadvancedloop.h"

#ifndef QOPENGL_CONTEXT_MANAGER
#include "opengl/context/coffeerenderer.h"
#include "tests/boxtest.h"
#else
#include "opengl/context/qcoffeerenderer.h"
#endif

#include <QtScript>
#include "general/data/numbercontainer.h"

#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
//#define RUNNABLE_RENDERER

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.21");
    a.setApplicationDisplayName("Coffee Cutie");

    bool inspect = true;
    bool logStderr = true;
    bool logFile = false;

    QCommandLineParser opts;
    opts.setApplicationDescription("A scriptable game engine");
    opts.addVersionOption();
    opts.addHelpOption();
    opts.addOption(QCommandLineOption("licenses","Get licensing information"));
    opts.addOption(QCommandLineOption("log-file","Log to file"));
    opts.addOption(QCommandLineOption("log-stderr","Log to stderr"));
    opts.addOption(QCommandLineOption("inspect"));
    opts.addPositionalArgument("configuration file","Source .json file","*.json");

    opts.process(a);
    for(QString key : opts.optionNames()){
        if(key=="licenses"){
            //show license information
            return 0;
        }else if(key=="inspect"){
            inspect = true;
        }else if(key=="log-file"){
            logFile = true;
        }else if(key=="log-stderr"){
            logStderr = true;
        }
    }

    QString sourceFile = "ubw/ubw.json";
    if(opts.positionalArguments().size()>0){
        sourceFile = opts.positionalArguments().at(0);
    }

    QFileInfo srcInfo(sourceFile);
    if(!srcInfo.exists()){
        qFatal("Source file does not exist: %s",sourceFile.toStdString().c_str());
    }

    qsrand((rand()%RAND_MAX)/10000.0);

    RenderLoop* loop;
    QObject* root = new QObject();
    CoffeeLogger logger(logStderr,logFile); Q_UNUSED(logger);
    root->setObjectName("[main,unlimited-root]");
#ifndef QOPENGL_CONTEXT_MANAGER
    CoffeeRenderer *renderer = new CoffeeRenderer(root,
                                                  1280,720,Qt::WindowNoState,
                                                  "Unlimited Frame Works");
#else
    QCoffeeRenderer *renderer = new QCoffeeRenderer(root,
                                                  1280,720,Qt::WindowNoState,
                                                  "Unlimited Frame Works");
#endif
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
