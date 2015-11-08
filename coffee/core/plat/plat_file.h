#ifndef COFFEE_PLAT_FILE_H
#define COFFEE_PLAT_FILE_H

#include "coffee/core/plat/plat_core.h"
#include "coffee/core/base/cdebug.h"

namespace Coffee{
namespace CResources{
namespace CFiles{

FILE *coffee_file_open(cstring fname, cstring mode)
{
    return fopen(fname,mode);
}

szptr coffee_file_size(FILE *file)
{
    fseek(file,0,SEEK_END);
    szptr fsize = ftell(file);
    rewind(file);
    return fsize;
}

cstring_w coffee_file_read(FILE *file, void *ptr, szptr *size, bool textmode)
{
    szptr esize = CFiles::coffee_file_size(file);
    szptr msize = esize*sizeof(byte);

    //Extra byte for null terminator
    if(textmode)
        msize+=1;

    byte* data = reinterpret_cast<byte*>(realloc(ptr,msize));
    *size = fread(data,sizeof(byte),esize,file);
    //In text mode, we terminate the array
    if(textmode)
        data[esize*sizeof(byte)] = '\0';

    if(*size<esize)
        cFatal("Read error: expected %ld bytes, got %ld",esize,*size);

    return data;
}

bool coffee_file_write(FILE *file, const void *data, szptr size)
{
    szptr wsize = fwrite(data,sizeof(byte),size,file);

    if(wsize<size)
        return false;
    return true;
}

int coffee_file_close(FILE *file)
{
    return fclose(file);
}

}
}
}

#endif
