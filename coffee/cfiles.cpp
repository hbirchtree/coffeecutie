#include "cfiles.h"

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

CResource::CResource(cstring resource){
    m_resource = resource;
    identify_resource();
}

cstring CResource::resource(){
    return m_resource.c_str();
}

bool CResource::exists(){
    FILE *f = fopen(m_resource.c_str(),"r");
    if(f)
        fclose(f);
    return f;
}

bool CResource::read_data(bool textmode){
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

bool CResource::memory_map()
{
    this->size = coffee_file_get_size(m_resource.c_str());
    this->data = CMemoryManagement::coffee_memory_map_file(m_resource.c_str(),0,size);
    if(!this->data)
        this->size = 0;
}

bool CResource::memory_unmap()
{
    bool s = CMemoryManagement::coffee_memory_unmap_file(this->data,this->size);
    this->data = nullptr;
    this->size = 0;
    return s;
}

bool CResource::save_data(){
    FILE *fp = CFiles::coffee_file_open(m_resource.c_str(),"wb");
    bool stat = CFiles::coffee_file_write(fp,data,size);
    if(CFiles::coffee_file_close(fp))
        cWarning("Failed to close file: %s",m_resource.c_str());
    return stat;
}

bool CResource::append_text(cstring text){
    FILE *fp = CFiles::coffee_file_open(m_resource.c_str(),"ab+");
    bool stat = CFiles::coffee_file_write(fp,text,strlen(text));
    if(CFiles::coffee_file_close(fp))
        cWarning("Failed to close file: %s",m_resource.c_str());
    return stat;
}

void CResource::free_data(){
    free(data);
    data = nullptr;
}

void CResource::identify_resource(){
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

}
}
