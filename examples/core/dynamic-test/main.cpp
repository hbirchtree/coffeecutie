#include <coffee/core/CApplication>
#include <coffee/core/plat/plat_linking.h>
#include <coffee/core/CDRendererBase>

using namespace Coffee;
using namespace CDisplay;
using namespace Library;

int32 coffee_main(int32 argc, cstring_w* argv)
{
    FunctionLoader::Library* lib = FunctionLoader::GetLibrary("DynamicLib");
    ObjectLoader::ObjConstructor<CDRendererBase> renderer_loader =
            ObjectLoader::GetConstructor<CDRendererBase>(
                lib,Library::DefaultConstructorFunction);

    if(!renderer_loader.loader)
        return 1;

    CDRendererBase *renderer = renderer_loader.loader();

    std::future<void> rendertask = LaunchAsync(renderer,GetDefaultVisual());

    rendertask.get();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
