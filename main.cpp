#include <QtConcurrent>
#include <iostream>
#include <unistd.h>

#include "coffee/cdebug.h"
#include "coffee/cfiles.h"
#include "coffee_impl/graphics/cshader.h"
#include "coffee_impl/context/cdrenderer.h"

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

    for(int i=0;i<argc;i++){
        std::cout << argv[i] << std::endl;
    }

    //Created so that the destructor closes the file
    Coffee::CoffeeInit();

    cDebug("CResource: %ld",sizeof(CResource));
    cDebug("CShader: %ld",sizeof(CShader));

    CDRenderer* renderer = new CDRenderer(nullptr);
    cDebug("%ld",sizeof(CDRenderer));

    //Just for prototyping, we'll be looking at std::thread later
    renderer->run(CDRenderer::Windowed,CSize(1280,720),0);
//    QFuture<void> rendererFuture = QtConcurrent::run(
//                QThreadPool::globalInstance(),
//                renderer,
//                &CDRenderer::run,
//                CDRenderer::Windowed,
//                CSize(1280,720),
//                0);

//    rendererFuture.waitForFinished();

    delete renderer;
    return 0;
}
