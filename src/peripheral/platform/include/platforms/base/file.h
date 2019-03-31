#pragma once

#include <peripherals/error/file_base.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/types.h>
#include <url/url.h>

namespace platform {
namespace file {

using namespace url;
using namespace semantic;

enum NodeType
{
    None,
    Block,
    Character,
    Device,
    File,
    Directory,
    Link,
    FIFO,
    Socket,
};

template<typename NestedError = sentinel_error_code>
struct FileFunDef : FileBase<NestedError>
{
    using size_type       = libc_types::szptr;
    using difference_type = libc_types::ptroff;

    using file_error = typename FileBase<NestedError>::file_error;

    struct FileHandle
    {
    };

    using FileMapping = Bytes;
    using ScratchBuf  = Bytes;

    static FileHandle Open(Url const&, RSCA, file_error&)
    {
        return {};
    }
    static bool Valid(FileHandle const&, file_error&)
    {
        return false;
    }
    static bool Close(FileHandle&&, file_error&)
    {
        return false;
    }

    static Bytes Read(FileHandle const&, szptr, file_error&)
    {
        return {};
    }
    static bool Write(FileHandle const&, Bytes const&, file_error&)
    {
        return false;
    }

    static szptr Size(FileHandle const&, file_error&)
    {
        return 0;
    }

    /*!
     * \brief Here's how the resource flags should work:
     * Must work:
     *  - ExclusiveLocking : don't allow others to open/write the file
     *  - SharedLocking : don't allow simultaneous writes (may happen
     * autonomously)
     *  - Persistent : changes must be visisble to others, eg. flushed once in a
     * while
     *  - ReadOnly : allow read access
     *  - WriteOnly : allow write access
     *  - Executable : allow execute access
     * Optional (good for perf):
     *  - NoCache : avoid caching
     *  - GreedyCache : cache as much as possible
     *  - Virtual : load sections, but not all (for big files)
     *  - HugeFile : indicates a wish for large pages
     * \param fname
     * \param access
     * \param size
     * \param offset
     * \param err
     * \return
     */
    static FileMapping Map(
        C_UNUSED(Url const& fname),
        C_UNUSED(RSCA access),
        C_UNUSED(szptr size),
        C_UNUSED(szptr offset),
        file_error&)
    {
        return {};
    }
    static bool Unmap(FileMapping&&, file_error&)
    {
        return false;
    }

    static bool MapCache(
        C_UNUSED(void* mapping_ptr),
        C_UNUSED(szptr mapping_size),
        C_UNUSED(szptr cache_offset),
        C_UNUSED(szptr cache_size),
        file_error&)
    {
        return false;
    }
    static bool MapUncache(
        C_UNUSED(c_ptr mapping_ptr),
        C_UNUSED(szptr mapping_size),
        C_UNUSED(szptr cache_offset),
        C_UNUSED(szptr cache_size),
        file_error&)
    {
        return false;
    }
    static bool MapSync(C_UNUSED(c_ptr ptr), C_UNUSED(szptr size), file_error&)
    {
        return false;
    }

    /*!
     * \brief Magically cache your whole application
     * \return
     */
    static bool SuperCache()
    {
        return false;
    }
    static bool SuperUncache()
    {
        return false;
    }

    static ScratchBuf ScratchBuffer(szptr, RSCA, file_error&)
    {
        return {};
    }
    static void ScratchUnmap(ScratchBuf&&, file_error&)
    {
    }

    /* We allow checking size of unopened files, convenience */
    static bool Exists(Url const&, file_error&)
    {
        return false;
    }
    static szptr Size(Url const&, file_error&)
    {
        return 0;
    }

    /*!
     * \brief Creates an entry in the filesystem
     * States:
     *  - Files and similar nodes are created without their directory paths
     *  - Directories are created non-recursively
     * \return
     */
    static bool Touch(NodeType, Url const&, file_error&)
    {
        return false;
    }

    static bool Ln(Url const&, Url const&, file_error&)
    {
        return false;
    }

    static bool Rm(Url const&, file_error&)
    {
        return false;
    }

    static NodeType Stat(Url const&, file_error&)
    {
        return NodeType::None;
    }

    static CString DereferenceLink(Url const& d, file_error&)
    {
        return d.internUrl;
    }
    static CString CanonicalName(Url const& d, file_error&)
    {
        return d.internUrl;
    }

    static void Truncate(Url const&, szptr, file_error&)
    {
    }
};

template<typename NestedError = sentinel_error_code>
struct DirFunDef : FileBase<NestedError>
{
    using file_error = typename FileBase<NestedError>::file_error;

    struct DirItem_t
    {
        CString  name;
        NodeType type;
    };

    using DirList = Vector<DirItem_t>;

    static bool ChDir(Url const&, file_error&)
    {
        return false;
    }

    static bool MkDir(Url const&, bool, file_error&)
    {
        return false;
    }
    static bool RmDir(Url const&, file_error&)
    {
        return false;
    }

    static bool Ls(Url const&, DirList&, file_error&)
    {
        return false;
    }

    /*!
     * \brief Get base name of an arbitrary path, should work like basename
     * \return
     */
    STATICINLINE Url Basename(CString const&, file_error&)
    {
        return Url();
    }

    /*!
     * \brief Directory name from arbitrary path, should work like dirname
     * \return
     */
    STATICINLINE Url Dirname(CString const&, file_error&)
    {
        return Url();
    }

    /*!
     * \brief Get path separation character, / or \\
     * \return
     */
    STATICINLINE CString GetPathSep()
    {
        return CString("/");
    }
};

} // namespace file
} // namespace platform
