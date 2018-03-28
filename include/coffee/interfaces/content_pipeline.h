#pragma once

#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/VirtualFS>

namespace Coffee{
struct TerminalCursor;
}

namespace CoffeePipeline
{

using namespace Coffee;


struct FileProcessor
{
    virtual ~FileProcessor()
    {
    }

    /*!
     * \brief We may here do whatever we want with all the files.
     * An example:
     *  Say you have a `mesh.fbx` file, and want to process
     *  it into `mesh.smsh` and `mesh.smat`.
     * You could emit the necessary files into the `files` variable
     *  while removing the original entry.
     *
     * Same can be done for PNG, which can be multiplexed into S3TC,
     *  ASTC and etc. for faster hardware-specific textures.
     *
     * \param files
     * \return
     */
    virtual void process(Vector<VirtFS::VirtDesc>& files,
                         TerminalCursor& cursor) = 0;
    
    virtual void receiveAssetPath(CString const& assetPath) = 0;

    virtual void setBaseDirectories(Vector<CString> const& dirs) = 0;
};

}
