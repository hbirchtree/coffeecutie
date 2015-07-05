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

#include "engine/physics/genericphysicsinterface.h"

#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.21");
    a.setApplicationDisplayName("Coffee Cutie");

    QString scriptFile;
    bool inspect = true;
    bool logStderr = true;
    bool logFile = false;

    //We parse user input first, to determine what to do.
    QCommandLineParser opts;
    opts.setApplicationDescription("A scriptable game engine");
    opts.addVersionOption();
    opts.addHelpOption();
    opts.addOption(QCommandLineOption("licenses","Get licensing information"));
    opts.addOption(QCommandLineOption("log-file","Log to file"));
    opts.addOption(QCommandLineOption("log-stderr","Log to stderr"));
    opts.addOption(QCommandLineOption("inspect"));
    opts.addPositionalArgument("script","Start-up script file","*.qts");

    opts.process(a);
    for(QString key : opts.optionNames()){
        if(key=="licenses"){
            //show license information
            return 0;
        }else if(key=="inspect"){
#ifndef COFFEE_INSPECTOR_RUN
            qDebug() << "Inspector not enabled, it may have been disabled at build-time and is thus not accessible.";
#else
            inspect = true;
#endif
        }else if(key=="log-file"){
            logFile = true;
        }else if(key=="log-stderr"){
            logStderr = true;
        }
    }

    CoffeeLogger logger(logStderr,logFile); Q_UNUSED(logger);

    for(int i=0;i<opts.positionalArguments().size();i++){
        switch(i){
        case 0:
            scriptFile = opts.positionalArguments().at(0);
            break;
        }
    }

    QFileInfo sf(scriptFile);
    if(!scriptFile.isEmpty()&&sf.exists()){
        QDir::setCurrent(sf.absolutePath());
        qDebug() << "Changing working directory to:" << sf.absolutePath();
    }else{
        qFatal("Failed to load script file: %s",sf.fileName().toStdString().c_str());
    }

    //Set the random seed for qrand()
    qsrand((rand()%RAND_MAX)/10000.0);

    //Set up root object (for destruction of objects)
    RenderLoop* loop;
    QObject* root = new QObject();
    root->setObjectName("coffeeroot");
#ifndef QOPENGL_CONTEXT_MANAGER
    CoffeeRenderer *renderer = new CoffeeRenderer(0,
                                                  1280,720,Qt::WindowNoState,
                                                  "Unlimited Frame Works");
#else
    QCoffeeRenderer *renderer = new QCoffeeRenderer(root,
                                                  1280,720,Qt::WindowNoState,
                                                  "Unlimited Frame Works");
#endif
    //for identification in script engine and inspector
    renderer->setObjectName("renderer");

    //Choose a render loop, advanced is our scripted one, BoxTest is a glbinding sample
#ifdef COFFEE_ADVANCED_RUN
    loop = new CoffeeAdvancedLoop(root,renderer);
#else
    //This demo taken from glbinding tests out general rendering
    loop = new BoxTest(renderer);
#endif //COFFEE_ADVANCED_RUN

    //Configure the loop, set default sampling
    renderer->setLoop(loop);
    renderer->setSamples(4);

    CoffeeInspector *inspector = nullptr;
    //configure script engine and inspector if it is enabled
    {
        QThreadPool::globalInstance()->setObjectName("QThreadPool");
        CoffeeScriptEngine* se = nullptr;
        QObject* fc = nullptr;
        if((CoffeeAdvancedLoop*)loop){
            fc = ((CoffeeAdvancedLoop*)loop)->getFactory();
            se = ((CoffeeAdvancedLoop*)loop)->getScriptEngine();
        }

        QObjectList objects;
        objects << root
               << QThreadPool::globalInstance()
               << renderer
               << fc;

        if((CoffeeAdvancedLoop*)loop){
            for(QObject* o : objects)
                se->addObject(o);
            QString out;
            bool res;
            se->execFile(sf.absoluteFilePath(),&res,&out);
            qDebug("Init script run: %i, %s",
                   res,
                   out.toStdString().c_str());
        }

#ifdef COFFEE_INSPECTOR_RUN
        if(inspect){
            inspector = new CoffeeInspector(0,
                                            objects,
                                            renderer,se);

        }
#endif //COFFEE_INSPECTOR_RUN
    }

#ifdef COFFEE_INSPECTOR_RUN
    if(inspect){
        inspector->run();
        inspector->show();
    }
#endif

#ifdef COFFEE_INSPECTOR_RUN
    QThreadPool::globalInstance()->start(renderer);

    a.exec();

    if(inspect)
        delete inspector;
#else
    renderer->run(); //in this case, nothing else runs in the main thread.
#endif

    delete root;

    return 0;
}
