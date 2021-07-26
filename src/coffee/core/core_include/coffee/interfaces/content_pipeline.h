#pragma once

#include <coffee/core/CFiles>
#include <coffee/core/VirtualFS>
#include <coffee/core/internal_state.h>
#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <peripherals/stl/string_ops.h>

namespace Coffee {
struct TerminalCursor;
}

namespace CoffeePipeline {

using namespace Coffee;

struct FileProcessor
{
    stl_types::Map<Path, Resource> cachedFiles;
    Url                            cacheBaseDir;
    u32                            numWorkers;

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
        Vector<VirtFS::VirtDesc>& files, TerminalCursor& cursor) = 0;

    virtual void setInternalState(
        ShPtr<State::InternalState>                 state,
        ShPtr<State::InternalThreadState>           tstate,
        UqPtr<platform::detail::state_pimpl> const& pimpl)
    {
        State::SetInternalState(state);
        State::SetInternalThreadState(tstate);

        *platform::state = *pimpl;
    }

    virtual void setBaseDirectories(Vector<CString> const& dirs) = 0;

    virtual void setCacheBaseDirectory(Path const& basedir)
    {
        cacheBaseDir = MkUrl(basedir);
    }

    virtual Url cacheTransform(Path const& f)
    {
        return cacheBaseDir +
               Path(str::replace::str<char>(f.internUrl, "/", "_"));
    }

    virtual bool isCached(Path const& file)
    {
        Resource test(MkUrl(cacheTransform(file)));
        return FileExists(test);
    }

    virtual Bytes getCached(Path const& file)
    {
        if(auto it = cachedFiles.find(file); it != cachedFiles.end())
            return it->second;
        else
        {
            auto out =
                cachedFiles.insert({file, Resource(cacheTransform(file))});
            return C_OCAST<Bytes>(out.first->second);
        }
    }

    virtual void cacheFile(Path const& file, Bytes const& content)
    {
        platform::file::create(
            MkUrl(cacheBaseDir),
            {
                .mode      = platform::file::mode_t::directory,
                .recursive = true,
            });
        Path outputPath = cacheTransform(file);

        Resource output(MkUrl(outputPath));
        output = content;
        FileCommit(output, RSCA::Discard);
    }

    virtual cstring name() const
    {
        return nullptr;
    }
};

} // namespace CoffeePipeline
