#include <QCoreApplication>
#include "opengl/coffeerenderer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.0.1");

    CoffeeRenderer *renderer = new CoffeeRenderer(800,480);
    renderer->init();
    renderer->loop();

    return 0;
}
