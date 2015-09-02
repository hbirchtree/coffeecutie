#include <QtConcurrent>
#include <iostream>
#include <unistd.h>

#include "coffee/cdebug.h"
#include "coffee_impl/context/cdrenderer.h"
#include "coffee_impl/graphics/cshader.h"
#include "coffee/cfunctional.h"
#include "coffee/cfiles.h"

using namespace Coffee::CFunctional;
using namespace Coffee::CResources;
using namespace Coffee::CDisplay;
using namespace Coffee::CGraphicsWrappers;

int main(int argc, char *argv[])
{
    C_UNUSED(argc)C_UNUSED(argv);
//    QCoreApplication a(argc, argv);
//    a.setApplicationName("CoffeeCutie");
//    a.setApplicationVersion("0.0.2.0");
//    a.setApplicationDisplayName("Coffee Cutie");

    //Created so that the destructor closes the file
    Coffee::CoffeeInit();

    CDRenderer* renderer = new CDRenderer(nullptr);

    qDebug() << sizeof(CBuffer) << sizeof(CVertexArrayObject) << sizeof(CUniformBlock) << sizeof(CUniformValue);
    cDebug("%ld",sizeof(CDRenderer));

    const char* string = "Hello World!\nHeh.";
    CResource t = CResource("testfile.txt");
    t.data = malloc(strlen(string));
    t.size = strlen(string);
    memcpy(t.data,string,strlen(string));

    qDebug() << t.size << t.data << t.exists();
    qDebug() << t.save_data();
    qDebug() << t.exists();

    QFuture<void> rendererFuture = QtConcurrent::run(
                QThreadPool::globalInstance(),
                renderer,
                &CDRenderer::run,
                CDRenderer::Windowed,
                CSize(1280,720),
                0);

    rendererFuture.waitForFinished();

    return 0;
}
