#include <coffee/CCore>

using namespace Coffee;
using namespace CDisplay;
using namespace CLibraryLoader;
using namespace CResources;

int32 coffee_main(int32 argc, cstring_w* argv)
{
    CObjectLoader<CDRendererBase>* renderer_loader =
            GetLib<CDRendererBase>("DynamicLib",nullptr);

    CDRendererBase *renderer = renderer_loader->loader();

    std::future<void> rendertask = LaunchAsync(renderer,GetDefaultVisual());

    rendertask.get();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
