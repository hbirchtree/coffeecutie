#include <coffee/CCore>

using namespace Coffee;
using namespace CDisplay;
using namespace CLibraryLoader;
using namespace CResources;

void coffee_callback(CFilesystemWatcher const*,cstring fn,const void*,CFileWatchParameters const& flags)
{
    cDebug("Woop woop!: {0}, {1}",fn,(uint16)flags);
}

int32 coffee_main(int32 argc, byte_t** argv)
{
    CFileWatchTask watcher;

    watcher.addTask("sample_data",
                    CFileWatchParameters::All,
                    coffee_callback,nullptr);

    CObjectLoader<CDRendererBase>* renderer_loader =
            coffee_get_lib<CDRendererBase>("DynamicLib",nullptr);

    CDRendererBase *renderer = renderer_loader->loader();

    std::atomic_bool started;
    started.store(false);

    coffee_display_start_async(&started,
                               renderer,
                               GetDefaultVisual());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
