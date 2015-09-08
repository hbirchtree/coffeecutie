#ifndef CFILES_H
#define CFILES_H

#include "coffee.h"
#include "coffee/cregex.h"
#include "coffee/cdebug.h"

namespace Coffee{
namespace CResources{

namespace CFiles{

static FILE* coffee_file_open(cstring fname, cstring mode)
{
    return fopen(fname,mode);
}

static szptr coffee_file_size(FILE* file)
{
    fseek(file,0,SEEK_END);
    szptr fsize = ftell(file);
    rewind(file);
    return fsize;
}

static cstring_w coffee_file_read(FILE* file, void* ptr, szptr *size, bool textmode = false)
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

static bool coffee_file_write(FILE* file, const void* data, szptr size)
{
    szptr wsize = fwrite(data,sizeof(byte),size,file);

    if(wsize<size)
        return false;
    return true;
}

static int coffee_file_close(FILE* file)
{
    return fclose(file);
}

}

struct CResource{
    enum ResourceFlags{
        Remote      = 0b1,
        ReadOnly    = 0b10,
        ReadWrite   = 0b100,
        WriteOnly   = 0b1000,
    };

    CResource(cstring resource){
        m_resource = resource;
        identify_resource();
    }

    cstring resource(){
        return m_resource.c_str();
    }

    uint8_t flags   = 0;
    szptr size     = 0;
    void* data      = nullptr;

    bool exists(){
        FILE *f = fopen(m_resource.c_str(),"r");
        if(f)
            fclose(f);
        return f;
    }

    bool read_data(bool textmode = false){
        FILE *fp = CFiles::coffee_file_open(m_resource.c_str(),"rb");

        if(!fp){
            cWarning("Failed to read file: %s",m_resource.c_str());
            return false;
        }

        data = CFiles::coffee_file_read(fp,data,&size,textmode);
        if(CFiles::coffee_file_close(fp))
            cWarning("Failed to close file: %s",m_resource.c_str());
        return true;
    }

    bool save_data(){
        FILE *fp = CFiles::coffee_file_open(m_resource.c_str(),"wb");
        bool stat = CFiles::coffee_file_write(fp,data,size);
        if(CFiles::coffee_file_close(fp))
            cWarning("Failed to close file: %s",m_resource.c_str());
        return stat;
    }

    bool append_text(cstring text){
        FILE *fp = CFiles::coffee_file_open(m_resource.c_str(),"ab+");
        bool stat = CFiles::coffee_file_write(fp,text,strlen(text));
        if(CFiles::coffee_file_close(fp))
            cWarning("Failed to close file: %s",m_resource.c_str());
        return stat;
    }

    void free_data(){
        free(data);
        data = nullptr;
    }

private:
    void identify_resource(){
        //Not all of these are implemented!
        std::vector<CString> remotes =
        {"http:.*","https:.*","ftp:.*","ftps:.*","sftp:.*"};

        for(const CString& rem : remotes){
            CRegexMatch match = CFunctional::coffee_regex_match(rem,m_resource);
            if(match.b_match){
                flags|=Remote;
                break;
            }
        }
    }

    CString m_resource;
};

}
}

#endif // CFILES_H
