#ifndef CFILES_H
#define CFILES_H

#include "coffee.h"
#include "coffee/cregex.h"
#include "coffee/cdebug.h"

namespace Coffee{
namespace CResources{

namespace CFiles{

static FILE* coffee_file_open(const char* fname, const char* mode)
{
    return fopen(fname,mode);
}

static size_t coffee_file_size(FILE* file)
{
    fseek(file,0,SEEK_END);
    size_t fsize = ftell(file);
    rewind(file);
    return fsize;
}

static char* coffee_file_read(FILE* file, void* ptr, size_t *size, bool textmode = false)
{
    size_t esize = CFiles::coffee_file_size(file);
    size_t msize = esize*sizeof(char);

    //Extra byte for null terminator
    if(textmode)
        msize+=1;

    char* data = reinterpret_cast<char*>(realloc(ptr,msize));
    *size = fread(data,sizeof(char),esize,file);
    //In text mode, we terminate the array
    if(textmode)
        data[esize*sizeof(char)] = '\0';

    if(*size<esize)
        cFatal("Read error: expected %ld bytes, got %ld",esize,*size);

    return data;
}

static bool coffee_file_write(FILE* file, const void* data, size_t size)
{
    size_t wsize = fwrite(data,sizeof(char),size,file);

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

    CResource(std::string resource){
        m_resource = resource;
        identify_resource();
    }

    const std::string resource(){
        return m_resource;
    }

    uint8_t flags   = 0;
    size_t size     = 0;
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

    bool append_text(const char* text){
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
        std::vector<std::string> remotes =
        {"http:.*","https:.*","ftp:.*","ftps:.*","sftp:.*"};

        for(const std::string& rem : remotes){
            CRegexMatch match = CFunctional::coffee_regex_match(rem,m_resource);
            if(match.b_match){
                flags|=Remote;
                break;
            }
        }
    }

    std::string m_resource;
};

}
}

#endif // CFILES_H
