#include <coffee/CCore>

using namespace Coffee;
using namespace CDisplay;
using namespace CLibraryLoader;
using namespace CResources;

void coffee_callback(CFilesystemWatcher const*,cstring fn,const void*,CFileWatchParameters const& flags)
{
    cDebug("Woop woop!: {0}, {1}",fn,(uint16)flags);
}

int32 coffee_main(int32 argc, cstring_w* argv)
{
    CFileWatchTask watcher;

    watcher.addTask("sample_data",
                    CFileWatchParameters::All,
                    coffee_callback,nullptr);

    CObjectLoader<CDRendererBase>* renderer_loader =
            GetLib<CDRendererBase>("DynamicLib",nullptr);

    CDRendererBase *renderer = renderer_loader->loader();

    std::future<void> rendertask = LaunchAsync(renderer,GetDefaultVisual());

    rendertask.get();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
