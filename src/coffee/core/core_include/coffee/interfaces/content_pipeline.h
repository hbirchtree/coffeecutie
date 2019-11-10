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
    Path cacheBaseDir;
    u32  numWorkers;

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
        ShPtr<State::InternalState>          state,
        ShPtr<State::InternalThreadState>    tstate,
        UqPtr<platform::detail::state_pimpl> const& pimpl)
    {
        State::SetInternalState(state);
        State::SetInternalThreadState(tstate);

        *platform::state = *pimpl;
    }

    virtual void setBaseDirectories(Vector<CString> const& dirs) = 0;

    virtual void setCacheBaseDirectory(Path const& basedir)
    {
        cacheBaseDir = basedir;
    }

    virtual Path cacheTransform(Path const& f)
    {
        return cacheBaseDir + Path(str::replace::str(f.internUrl, "/", "_"));
    }

    virtual bool isCached(Path const& file)
    {
        Resource test(MkUrl(cacheTransform(file)));
        return FileExists(test);
    }

    virtual Bytes getCached(Path const& file)
    {
        Resource cache(MkUrl(cacheTransform(file)));
        return Bytes::Copy(C_OCAST<Bytes>(cache));
    }

    virtual void cacheFile(Path const& file, Bytes const& content)
    {
        file_error ec;

        DirFun::MkDir(MkUrl(cacheBaseDir), true, ec);
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
