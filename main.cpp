#include <QCoreApplication>
#include "opengl/coffeerenderer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.0.3");

    CoffeeRenderer *renderer = new CoffeeRenderer(0,1280,720,Qt::WindowNoState,"CoffeeCutie");
    int initStat = renderer->init();
    switch(initStat){
    case 0:
        renderer->loop();
        break;
    default:
        qDebug() << "Program exited with errorcode: " << initStat;
        return initStat;
    }
    return 0;
}
