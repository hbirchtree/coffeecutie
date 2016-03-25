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
        Device,
        File,
        Directory,
        Link,
        FIFO,
        Pipe,
    };

    static CString NativePath(cstring);

    static FileHandle* Open(cstring, ResourceAccess);
    static bool Close(FileHandle*);

    static CByteData Read(FileHandle* h,uint64 size,bool nterminate);
    static void Seek(FileHandle* h,uint64 off);
    static bool Write(FileHandle* h,CByteData const& d,bool);

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
    {return false;}
    static bool MapUncache(void* mapping_ptr,szptr mapping_size,
                           szptr cache_offset, szptr cache_size)
    {return false;}
    static bool MapSync(void*,szptr)
    {return false;}

    static szptr Size(FileHandle*);

    /*!
     * \brief Magically cache your whole application
     * \return
     */
    static bool SuperCache(){return false;}
    static bool SuperUncache(){return false;}

    static ScratchBuf* ScratchBuffer(szptr size, ResourceAccess access);
    static void ScratchUnmap(ScratchBuf*);

    /* We allow checking size of unopened files, convenience */
    static bool Exists(cstring);
    static szptr Size(cstring);

    static bool Touch(NodeType,cstring)
    {return false;}
    static bool Rm(cstring)
    {return false;}

    static NodeType Stat(cstring)
    {return NodeType::None;}
};

struct DirFunDef
{
    typedef struct DirItem
    {
        FileFunDef::NodeType type;
        CString name;
    } DirItem_t;

    using DirList = Vector<DirItem_t>;

    static bool MkDir(cstring dname, bool recurse){return false;}
    static bool RmDir(cstring dname){return false;}

    static bool Ls(cstring dname,DirList*){return false;}
};

}
}

#endif