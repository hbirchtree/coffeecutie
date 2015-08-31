#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
#define RENDERER_DO_DEBUG
#define RENDERER_FAST_DEBUG

//#include <QApplication>
#include "coffeelogger.h"

#include <QtConcurrent>

#include <iostream>
#include <unistd.h>

#include "coffee/cdebug.h"
#include "coffee_impl/context/cdrenderer.h"
#include "coffee_impl/graphics/cgraphicswrappers.h"

using namespace Coffee::CDisplay;
using namespace Coffee::CGraphicsWrappers;

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    a.setApplicationName("CoffeeCutie");
//    a.setApplicationVersion("0.0.1.105");
//    a.setApplicationDisplayName("Coffee Cutie");

    //Created so that the destructor closes the file
    CoffeeLogger logger(false,false); Q_UNUSED(logger);

    CDRenderer* renderer = new CDRenderer(nullptr);

    CSize s;
    s.w = 1280;
    s.h = 720;
    QtConcurrent::run(QThreadPool::globalInstance(),renderer,&CDRenderer::run,CDRenderer::Windowed,s,0);

    usleep(1000000000);

    return 0;
}
