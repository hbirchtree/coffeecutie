#ifndef COFFEE_CORE_PLAT_FILE_DEF_H
#define COFFEE_CORE_PLAT_FILE_DEF_H

#include <coffee/core/CTypes>

namespace Coffee{
namespace CResources{

struct FileFunDef
{
    struct FileHandle;

    static CString NativePath(cstring);

    static FileHandle* Open(cstring, ResourceAccess);
    static bool Close(FileHandle*);

    static CByteData Read(FileHandle*,uint64,bool);
    static void Seek(FileHandle*,uint64);
    static bool Write(FileHandle*,CByteData const&,bool);

    static void* Map(cstring, ResourceAccess, szptr, szptr, int*);
    static bool  Unmap(void*, szptr);

    static szptr Size(cstring);
    static bool Rm(cstring);
};

struct DirFunDef
{
    static bool MkDir(cstring, bool);
    static bool RmDir(cstring);
};

}
}

#endif