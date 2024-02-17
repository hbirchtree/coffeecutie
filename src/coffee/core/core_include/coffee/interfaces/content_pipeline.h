#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/VirtualFS>
#include <coffee/core/internal_state.h>
#include <coffee/core/libc_types.h>
#include <peripherals/stl/string_ops.h>
#include <peripherals/stl/types.h>

namespace Coffee {
struct TerminalCursor;
}

namespace CoffeePipeline {

using namespace Coffee;

struct FileProcessor
{
    std::map<Path, Resource> cachedFiles;
    Url                      cacheBaseDir;
    u32                      numWorkers;

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
    virtual void process(
        std::vector<vfs::desc>& files, TerminalCursor& cursor) = 0;

    virtual void setInternalState(
        std::shared_ptr<State::InternalState>                 state,
        std::shared_ptr<State::InternalThreadState>           tstate,
        std::unique_ptr<platform::detail::state_pimpl> const& pimpl)
    {
        State::SetInternalState(state);
        State::SetInternalThreadState(tstate);

        *platform::state = *pimpl;
    }

    virtual void setBaseDirectories(std::vector<std::string> const& dirs) = 0;

    virtual void setCacheBaseDirectory(Path const& basedir)
    {
        cacheBaseDir = basedir.url();
    }

    virtual Url cacheTransform(Path const& f)
    {
        return cacheBaseDir +
               Path(stl_types::str::replace::str<char>(f.internUrl, "/", "_"));
    }

    virtual bool isCached(Path const& file)
    {
        Resource test(cacheTransform(file));
        return FileExists(test);
    }

    virtual semantic::Bytes getCached(Path const& file)
    {
        if(auto it = cachedFiles.find(file); it != cachedFiles.end())
            return it->second;
        else
        {
            auto out =
                cachedFiles.insert({file, Resource(cacheTransform(file))});
            return C_OCAST<semantic::Bytes>(out.first->second);
        }
    }

    virtual void cacheFile(Path const& file, semantic::Bytes const& content)
    {
        platform::file::create(
            cacheBaseDir,
            {
                .mode      = platform::file::mode_t::directory,
                .recursive = true,
            });
        Path outputPath = cacheTransform(file);

        Resource output(outputPath.url());
        output = content;
        FileCommit(output, RSCA::Discard);
    }

    virtual cstring name() const
    {
        return nullptr;
    }
};

} // namespace CoffeePipeline
