#ifndef COFFEE_CORE_PLAT_FILE_DEF_H
#define COFFEE_CORE_PLAT_FILE_DEF_H

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"
#include "../../types/cdef/memtypes.h"
#include "../../types/edef/resenum.h"
#include "../../base/files/url.h"

namespace Coffee{
namespace CResources{

struct FileFunDef
{
    struct FileHandle;
    struct FileMapping
    {
        void* ptr;
        szptr size;
        ResourceAccess acc;
    };
    struct ScratchBuf
    {
        void* ptr;
        szptr size;
        ResourceAccess acc;
    };

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

    static CString NativePath(cstring);
    static CString NativePath(cstring,ResourceAccess);

    STATICINLINE bool VerifyAsset(cstring)
    {
        return true;
    }

    static FileHandle* Open(Url const&, ResourceAccess){return nullptr;}
    static bool Close(FileHandle*){return false;}

    static void Seek(FileHandle* h,uint64 off)
    {
        C_UNUSED(h);
        C_UNUSED(off);
    }

    static CByteData Read(FileHandle* h,uint64 size,bool)
    {
        C_UNUSED(h);
        C_UNUSED(size);

        return {};
    }
    static bool Write(FileHandle* h,CByteData const& d,bool)
    {
        C_UNUSED(h);
        C_UNUSED(d);

        return false;
    }

    static szptr Size(FileHandle*){return 0;}

    /*!
     * \brief Here's how the resource flags should work:
     * Must work:
     *  - ExclusiveLocking : don't allow others to open/write the file
     *  - SharedLocking : don't allow simultaneous writes (may happen autonomously)
     *  - Persistent : changes must be visisble to others, eg. flushed once in a while
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
    static FileMapping Map(Url const& fname, ResourceAccess access,
                           szptr size, szptr offset, int* err)
    {
        C_UNUSED(fname);
        C_UNUSED(access);
        C_UNUSED(size);
        C_UNUSED(offset);
        C_UNUSED(err);

        return {};
    }
    static bool Unmap(FileMapping* mapp)
    {
        C_UNUSED(mapp);

        return false;
    }

    static bool MapCache(void* mapping_ptr,szptr mapping_size,
                         szptr cache_offset, szptr cache_size)
    {
        C_UNUSED(mapping_ptr);
        C_UNUSED(mapping_size);
        C_UNUSED(cache_offset);
        C_UNUSED(cache_size);

        return false;
    }
    static bool MapUncache(void* mapping_ptr,szptr mapping_size,
                           szptr cache_offset, szptr cache_size)
    {
        C_UNUSED(mapping_ptr);
        C_UNUSED(mapping_size);
        C_UNUSED(cache_offset);
        C_UNUSED(cache_size);

        return false;
    }
    static bool MapSync(void* ptr,szptr size)
    {
        C_UNUSED(ptr);
        C_UNUSED(size);

        return false;
    }


    /*!
     * \brief Magically cache your whole application
     * \return
     */
    static bool SuperCache(){return false;}
    static bool SuperUncache(){return false;}

    static ScratchBuf ScratchBuffer(szptr, ResourceAccess){return {};}
    static void ScratchUnmap(ScratchBuf*){}

    /* We allow checking size of unopened files, convenience */
    static bool Exists(Url const&){return false;}
    static szptr Size(Url const&){return 0;}

    /*!
     * \brief Creates an entry in the filesystem
     * States:
     *  - Files and similar nodes are created without their directory paths
     *  - Directories are created non-recursively
     * \return
     */
    static bool Touch(NodeType, Url const&){return false;}

    static bool Ln(Url const&,Url const&)
    {return false;}

    static bool Rm(Url const&)
    {return false;}

    static NodeType Stat(Url const&)
    {return NodeType::None;}

    static CString DereferenceLink(Url const& d){return d.internUrl;}
    static CString CanonicalName(Url const& d){return d.internUrl;}

    static void Truncate(Url const&, szptr){}
};

struct DirFunDef
{
    using Type = FileFunDef::NodeType;

    struct DirItem_t
    {
        using Type = FileFunDef::NodeType;

        CString name;
        Type type;
    };

    using DirList = Vector<DirItem_t>;

    static bool ChDir(Url const&){return false;}

    static bool MkDir(Url const&, bool){return false;}
    static bool RmDir(Url const&){return false;}

    static bool Ls(Url const&,DirList&,bool = false){return false;}

    STATICINLINE CString Basename(CString fn)
    {
        return Basename(fn.c_str());
    }

    static CString Basename(cstring fn);
};

}

using namespace CResources;

}

#endif
