#include <QApplication>
#include <QDateTime>
#include "opengl/coffeerenderer.h"
#include "tests/coffeeadvancedloop.h"
#include "tests/boxtest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.0.4");

    CoffeeRenderer *renderer = new CoffeeRenderer(0,1280,720,Qt::WindowNoState,"CoffeeCutie");
    renderer->setObjectName("root.renderer");

    int initStat = renderer->init();

    //This demo taken from glbinding tests out general rendering
//    BoxTest loop(renderer);


    CoffeeAdvancedLoop loop(renderer);

    renderer->setLoop(&loop);
    renderer->setSamples(4);

    switch(initStat){
    case 0:
        return renderer->loop();
    default:
        qDebug() << "Program exited with errorcode: " << initStat;
        return initStat;
    }
}
