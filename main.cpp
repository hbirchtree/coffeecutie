#define COFFEE_ADVANCED_RUN
#define COFFEE_INSPECTOR_RUN
#define RENDERER_DO_DEBUG
#define RENDERER_FAST_DEBUG

#include <QCoreApplication>
#include "coffeelogger.h"

#include <QtConcurrent>

#include <iostream>
#include <unistd.h>

#include "coffee/cdebug.h"
#include "coffee_impl/context/cdrenderer.h"
#include "coffee_impl/graphics/cshader.h"
#include "coffee/cfunctional.h"

using namespace Coffee::CFunctional;
using namespace Coffee::CDisplay;
using namespace Coffee::CGraphicsWrappers;

int main(int argc, char *argv[])
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)
//    QCoreApplication a(argc, argv);
//    a.setApplicationName("CoffeeCutie");
//    a.setApplicationVersion("0.0.2.0");
//    a.setApplicationDisplayName("Coffee Cutie");

    //Created so that the destructor closes the file
    Coffee::CoffeeInit();

    CoffeeLogger logger(false,false); Q_UNUSED(logger);

    CDRenderer* renderer = new CDRenderer(nullptr);

    qDebug() << sizeof(CBuffer) << sizeof(CVertexArrayObject) << sizeof(CUniformBlock) << sizeof(CUniformValue);
    cDebug("%ld",sizeof(CDRenderer));


    CSize s;
    s.w = 1280;
    s.h = 720;
    QFuture<void> rendererFuture = QtConcurrent::run(QThreadPool::globalInstance(),renderer,&CDRenderer::run,CDRenderer::Windowed,s,0);
    rendererFuture.waitForFinished();

    delete renderer;

    return 0;
}

