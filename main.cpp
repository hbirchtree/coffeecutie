#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
#define RENDERER_DO_DEBUG
#define RENDERER_FAST_DEBUG

#include <QApplication>
#include "coffeelogger.h"

#include "coffee/cobject.h"

using namespace Coffee;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CoffeeCutie");
    a.setApplicationVersion("0.0.1.105");
    a.setApplicationDisplayName("Coffee Cutie");

    //Created so that the destructor closes the file
    CoffeeLogger logger(false,false); Q_UNUSED(logger);

    CObject* t1 = new CObject();
    CObject* c1 = new CObject(t1);
    CObject* cc1 = new CObject(c1);
    CObject* ccc1 = new CObject(cc1);

    delete t1;

    fprintf(stderr,"Hello!\n");

    qDebug() << "Our object:" << ccc1->objectName().c_str();

    qDebug() << "It's a whole new woooorld!";

    return 0;
}
