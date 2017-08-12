#ifndef COFFEE_CORE_PLAT_FILE_DEF_H
#define COFFEE_CORE_PLAT_FILE_DEF_H

#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"
#include "../../types/cdef/memtypes.h"
#include "../../types/edef/resenum.h"

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

    static FileHandle* Open(cstring, ResourceAccess);
    static bool Close(FileHandle*);

    static void Seek(FileHandle* h,uint64 off);

    static CByteData Read(FileHandle* h,uint64 size,bool);
    static bool Write(FileHandle* h,CByteData const& d,bool);

    static szptr Size(FileHandle*);

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
    static FileMapping Map(cstring fname, ResourceAccess access,
                     szptr size, szptr offset, int* err);
    static bool Unmap(FileMapping* mapp);

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
    static bool Exists(cstring);
    static szptr Size(cstring);

    /*!
     * \brief Creates an entry in the filesystem
     * States:
     *  - Files and similar nodes are created without their directory paths
     *  - Directories are created non-recursively
     * \return
     */
    static bool Touch(NodeType, cstring){return false;}

    static bool Ln(cstring,cstring)
    {return false;}

    static bool Rm(cstring)
    {return false;}

    static NodeType Stat(cstring)
    {return NodeType::None;}

    static CString DereferenceLink(cstring){return {};}
    static CString CanonicalName(cstring){return {};}
};

struct DirFunDef
{
    struct DirItem_t
    {
        using Type = FileFunDef::NodeType;

        CString name;
        Type type;
    };

    using DirList = Vector<DirItem_t>;

    static bool ChDir(cstring){return false;}

    static bool MkDir(cstring, bool){return false;}
    static bool RmDir(cstring){return false;}

    static bool Ls(cstring,DirList&){return false;}

    static CString Basename(cstring){return {};}
};

}

using namespace CResources;

}

#endif
