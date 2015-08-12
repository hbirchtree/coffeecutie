#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
#define RENDERER_DO_DEBUG
#define RENDERER_FAST_DEBUG

#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "coffeelogger.h"

#include "coffeegameenvironment.h"

#ifdef COFFEE_ADVANCED_RUN
#include "engine/rendering/coffeeadvancedloop.h"
#else
#include "tests/boxtest.h"
#endif

#include "engine/scripting/coffeeinputevent.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.100");
    a.setApplicationDisplayName("Coffee Cutie");

    QString scriptFile;
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
    opts.addPositionalArgument("script","Start-up script file","*.qts");

    opts.process(a);
    for(QString key : opts.optionNames()){
        if(key=="licenses"){
            //show license information
            return 0;
        }else if(key=="log-file"){
            logFile = true;
        }else if(key=="log-stderr"){
            logStderr = true;
        }
    }

    //Created so that the destructor closes the file
    CoffeeLogger logger(logStderr,logFile); Q_UNUSED(logger);

    for(int i=0;i<opts.positionalArguments().size();i++){
        switch(i){
        case 0:
            scriptFile = opts.positionalArguments().at(0);
            break;
        }
    }

    QFileInfo sf(scriptFile);
#ifdef COFFEE_ADVANCED_RUN
    if(!scriptFile.isEmpty()&&sf.exists()){
        QDir::setCurrent(sf.absolutePath());
        qDebug() << "Changing working directory to:" << sf.absolutePath();
    }else{
        qFatal("Failed to load script file: %s",sf.fileName().toStdString().c_str());
    }
#endif

    //Set the random seed for qrand()
    qsrand((rand()%RAND_MAX)/10000.0);

    //Set up root object (for destruction of objects)

    qRegisterMetaType<std::function<void()>>("std::function<void()>");

    CoffeeGameEnvironment* root = new CoffeeGameEnvironment();
    root->setObjectName("coffeeroot");

    root->setThreadPool(QThreadPool::globalInstance());
    root->setInitScript("ubw.qts");

    //Choose a render loop, advanced is our scripted one, BoxTest is a glbinding sample
    CoffeeAdvancedLoop* advancedLoop = nullptr;
#ifdef COFFEE_ADVANCED_RUN
    advancedLoop = new CoffeeAdvancedLoop(root,root->renderer());
    //Provide the script engine
    root->setScriptEngine(advancedLoop->getScriptEngine());
    root->setRenderLoop(advancedLoop);
#else
    //This demo taken from glbinding tests out general rendering
    root->setRenderLoop(new BoxTest(root->renderer()));
#endif //COFFEE_ADVANCED_RUN

    root->startExecution(&a);

    delete root;

    return 0;
}
