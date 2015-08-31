#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
#define RENDERER_DO_DEBUG
#define RENDERER_FAST_DEBUG

//#include <QApplication>
#include "coffeelogger.h"

#include <iostream>
#include <unistd.h>

#include "coffee/cobject.h"
#include "coffee/cdebug.h"
#include "coffee/cdisplay.h"
#include "coffee_impl/display/cglfwrenderer.h"

using namespace Coffee::CDisplay;

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    a.setApplicationName("CoffeeCutie");
//    a.setApplicationVersion("0.0.1.105");
//    a.setApplicationDisplayName("Coffee Cutie");

    //Created so that the destructor closes the file
    CoffeeLogger logger(false,false); Q_UNUSED(logger);

    CGLFWRenderer* renderer = new CGLFWRenderer(nullptr);
    CSize s;
    s.w = 1280;
    s.h = 720;
    renderer->init(CGLFWRenderer::Windowed,s,0);
    renderer->run();
    renderer->cleanup();

    return 0;
}
