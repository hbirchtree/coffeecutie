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

    enum NodeType
    {
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

    static void* Map(cstring fname, ResourceAccess access,
                     szptr size, szptr offset, int* err);
    static bool  Unmap(void* ptr, szptr size);

    static bool MapCache(void* mapping_ptr,szptr mapping_size,
                         szptr cache_offset, szptr cache_size);
    static bool MapUncache(void* mapping_ptr,szptr mapping_size,
                           szptr cache_offset, szptr cache_size);
    static bool MapSync(void*,szptr);

    static bool Exists(FileHandle*);
    static szptr Size(FileHandle*);

    /* We allow checking size of unopened files, convenience */
    static bool Exists(cstring);
    static szptr Size(cstring);

    static bool Touch(NodeType,cstring);
    static bool Rm(cstring);

    static NodeType Stat(cstring);

    /*!
     * \brief Magically cache your whole application
     * \return
     */
    static bool SuperCache();
    static bool SuperUncache();

    static void* ScratchBuffer(szptr size, ResourceAccess access);
    static void ScratchUnmap(void* ptr, szptr size);
};

struct DirFunDef
{
    static bool MkDir(cstring dname, bool recurse);
    static bool RmDir(cstring dname);

    static bool Ls(cstring dname,uint32*,FileFunDef::NodeType**,CString**);
};

}
}

#endif