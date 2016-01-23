#include <plat/plat_file.h>

#include <cstring>

#include <plat/platform_detect.h>
#include <plat/environment_details.h>

#include <plat/plat_core.h>
#include <base/cdebug.h>
#include <coffee_strings.h>

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

CString _coffee_file_get_plat_name(cstring fname)
{
    CString filename = fname;

#if defined(COFFEE_WINDOWS)
    filename = c_str_replace(fname,"/","\\");
#endif

    return filename;
}

CFile *coffee_file_open(cstring fname, cstring mode)
{
    CFile* f = new CFile;
    CString plat_fname = _coffee_file_get_plat_name(fname);
    f->handle = fopen(plat_fname.c_str(),mode);
    int err = errno;
    if(!f->handle)
    {
        cWarning(CFStrings::Plat_File_Open_Error,fname);
        cWarning(CFStrings::Plat_File_Native_Error,strerror(err));
    }
    return f;
}

szptr coffee_file_size(CFile *file)
{
    fseek(file->handle,0,SEEK_END);
    szptr fsize = ftell(file->handle);
    rewind(file->handle);
    return fsize;
}

cstring_w coffee_file_read(CFile *file, c_ptr ptr, uint64 *size, bool textmode)
{
    szptr esize = CFiles::coffee_file_size(file);
    szptr msize = esize*sizeof(byte_t);

    //Extra byte for null terminator
    if(textmode)
        msize+=1;

    cstring_w data = (cstring_w)CRealloc(ptr,msize);
    *size = fread(data,sizeof(byte_t),esize,file->handle);
    //In text mode, we terminate the array
    if(textmode)
        data[esize*sizeof(byte_t)] = 0;

    if(*size<esize)
        cFatal(CFStrings::Plat_File_Native_SizeErr,esize,*size);

    return data;
}

bool coffee_file_write(CFile *file, c_cptr data, uint64 size)
{
    szptr wsize = fwrite(data,sizeof(byte_t),size,file->handle);

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
