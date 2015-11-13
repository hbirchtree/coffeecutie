#include "plat_file.h"

#include "coffee/core/plat/plat_core.h"
#include "coffee/core/base/cdebug.h"

namespace Coffee{
namespace CResources{
namespace CFiles{

/*!
 * \brief A wrapper of file handles. Should be used instead of FILE
 */
struct CFile
{
    FILE* handle;
};

CFile *coffee_file_open(cstring fname, cstring mode)
{
    CFile* f = new CFile;
    f->handle = fopen(fname,mode);
    return f;
}

szptr coffee_file_size(CFile *file)
{
    fseek(file->handle,0,SEEK_END);
    szptr fsize = ftell(file->handle);
    rewind(file->handle);
    return fsize;
}

cstring_w coffee_file_read(CFile *file, c_ptr ptr, szptr *size, bool textmode)
{
    szptr esize = CFiles::coffee_file_size(file);
    szptr msize = esize*sizeof(byte);

    //Extra byte for null terminator
    if(textmode)
        msize+=1;

    byte* data = (byte*)c_realloc(ptr,msize);
    *size = fread(data,sizeof(byte),esize,file->handle);
    //In text mode, we terminate the array
    if(textmode)
        data[esize*sizeof(byte)] = '\0';

    if(*size<esize)
        cFatal("Read error: expected %ld bytes, got %ld",esize,*size);

    return data;
}

bool coffee_file_write(CFile *file, c_cptr data, szptr size)
{
    szptr wsize = fwrite(data,sizeof(byte),size,file->handle);

    if(wsize<size)
        return false;
    return true;
}

int coffee_file_close(CFile *file)
{
    FILE* handle = file->handle;
    delete file;
    return fclose(handle);
}

}
}
}
