#include <coffee/core/plat/file/unix/file.h>
#include <coffee/core/CDebug>

#include <stdlib.h>
#include <fcntl.h>

namespace Coffee{
namespace CResources{
namespace Posix{

FileFunDef::NodeType PosixFileMod_def::Stat(Url const& fn)
{
    auto url = *fn;
    struct stat fs = {};
    int s = lstat(url.c_str(),&fs);

    errno = 0;

    if(s != 0)
        return NodeType::None;

    mode_t m = fs.st_mode;

    if(S_ISDIR(m))
        return NodeType::Directory;
    else if(S_ISREG(m))
        return NodeType::File;
    else if(S_ISLNK(m))
        return NodeType::Link;
    else if(S_ISCHR(m))
        return NodeType::Character;
    else if(S_ISBLK(m))
        return NodeType::Block;
    else if(S_ISSOCK(m))
        return NodeType::Socket;
    else if(S_ISFIFO(m))
        return NodeType::FIFO;

    return NodeType::None;
}

bool PosixFileMod_def::Touch(FileFunDef::NodeType t, Url const& fn)
{
    auto url = *fn;
    switch(t)
    {
    case NodeType::File:
    {
        int fd = creat(url.c_str(),S_IRWXU);
        errno = 0;
        close(fd);
        break;
    }
    case NodeType::Directory:
    {
        return PosixDirFun::MkDir(fn,false);
    }
    default:
        return false;
    }
    return true;
}

CString PosixFileMod_def::DereferenceLink(Url const& fn)
{
    CString url = fn.internUrl;

    /* Avoid cyclic dependency in resolution of filenames, yo */
    if(!feval(fn.flags & RSCA::SystemFile))
        url = *fn;

    errno = 0;

    CString out;
    szptr name_size = PATH_MAX;
    out.resize(name_size+1);
    ssize_t sz = readlink(url.c_str(),&out[0],name_size);
    if(errno == EINVAL)
        return out = url.c_str();
    if(sz > 0)
        out.resize(C_FCAST<CString::size_type>(sz));
    else
        return out = url.c_str();

    ErrnoCheck(url.c_str());

    return out;
}

CString PosixFileMod_def::CanonicalName(Url const& fn)
{
    auto url = *fn;
#if defined(COFFEE_LINUX)
    cstring_w name = canonicalize_file_name(url.c_str());
    if(!name)
    {
        ErrnoCheck(url.c_str());
        return {};
    }
    CString out = name;
    free(name);
    return out;
#else
    CString out;
    out.resize(FILENAME_MAX);
    if(!realpath(url.c_str(),&out[0]))
        out.resize(0);
    out.resize(StrLen(out.c_str()));
    return out;
#endif
}

bool PosixFileMod_def::Ln(Url const& src, Url const& target)
{
    auto srcUrl = *src;
    auto targetUrl = *target;
    int sig = symlink(srcUrl.c_str(),targetUrl.c_str());
    if(sig == 0)
        return true;
    else
    {
        ErrnoCheck(targetUrl.c_str());
        return false;
    }
}

szptr PosixFileMod_def::Size(Url const& fn)
{
    auto url = *fn;
    struct stat st = {};
    if(lstat(url.c_str(),&st)!=0)
        ErrnoCheck(url.c_str());
    return C_FCAST<szptr>(st.st_size);
}

bool PosixFileMod_def::Exists(Url const& fn)
{
    auto url = *fn;
    struct stat st;
    bool status = lstat(url.c_str(),&st)==0;
    if(status)
    {
        errno = 0;
        return true;
    }else{
        return (errno!=ENOENT||errno==ENOTDIR) && (errno=0);
    }
}

bool PosixFileMod_def::Rm(Url const& fn)
{
    auto url = *fn;
    bool stat = unlink(url.c_str()) == 0;
    ErrnoCheck(url.c_str());
    return stat;
}

int PosixFileMod_def::MappingFlags(ResourceAccess acc)
{
    int mapping = 0;

    if(feval(acc&(ResourceAccess::Persistent)))
        mapping = MAP_SHARED;
    else
        mapping = MAP_PRIVATE;

#if defined(COFFEE_APPLE)
    if(feval(acc&ResourceAccess::NoCache))
        mapping |= MAP_NOCACHE;

    if(feval(acc&ResourceAccess::HugeFile))
        mapping |= VM_FLAGS_SUPERPAGE_SIZE_ANY;
#endif

#if defined(COFFEE_LINUX)
    #if !defined(COFFEE_NO_HUGETLB)
    if(feval(acc&ResourceAccess::HugeFile))
        mapping |= MAP_HUGETLB;
    #endif

    if(feval(acc&ResourceAccess::ExclusiveLocking))
        mapping |= MAP_LOCKED;

    if(feval(acc&(ResourceAccess::Streaming)))
        mapping |= MAP_POPULATE;
    if(feval(acc&ResourceAccess::GreedyCache))
        mapping |= MAP_POPULATE|MAP_LOCKED;
    if(feval(acc&ResourceAccess::NoCache))
        mapping |= MAP_NONBLOCK;
#endif

    return mapping;
}

int PosixFileMod_def::ProtFlags(ResourceAccess acc)
{
    int prot = PROT_NONE;

    if(feval(acc&(ResourceAccess::ReadOnly)))
        prot |= PROT_READ;
    if(feval(acc&ResourceAccess::WriteOnly))
        prot |= PROT_WRITE;
    if(feval(acc&ResourceAccess::Executable))
        prot |= PROT_EXEC;

    return prot;
}

int PosixFileMod_def::PosixRscFlags(ResourceAccess acc)
{
    int oflags = 0;

    if(feval(acc,ResourceAccess::ReadWrite))
    {
        oflags = O_RDWR;

        if(feval(acc&ResourceAccess::Discard))
            oflags |= O_TRUNC;
    }
    else if(feval(acc,ResourceAccess::Executable))
        oflags = O_RDONLY;
    else if(feval(acc,ResourceAccess::ReadOnly))
        oflags = O_RDONLY;
    else if(feval(acc, ResourceAccess::WriteOnly))
    {
        if(feval(acc&ResourceAccess::Append))
            oflags = O_APPEND;
        else
            oflags = O_WRONLY;

        if(feval(acc&ResourceAccess::Discard))
            oflags |= O_TRUNC;
    }

    if(feval(acc&ResourceAccess::NewFile))
        oflags |= O_CREAT;

    return oflags;
}

bool PosixDirFun::Ls(Url const& dname, DirFunDef::DirList &entries)
{
    auto url = *dname;
    DIR* dr = opendir(url.c_str());

    if(!dr)
    {
        PosixFileFun::ErrnoCheck(url.c_str());
        return false;
    }

    struct dirent* dir_ent = nullptr;

    while((dir_ent = readdir(dr)) != nullptr)
    {
        Type t = Type::None;

        switch(dir_ent->d_type)
        {
        case DT_BLK:
            t = Type::Block;
            break;
        case DT_CHR:
            t = Type::Character;
            break;
        case DT_DIR:
            t = Type::Directory;
            break;
        case DT_LNK:
            t = Type::Link;
            break;
        case DT_FIFO:
            t = Type::FIFO;
            break;
        case DT_REG:
            t = Type::File;
            break;
        case DT_SOCK:
            t = Type::Socket;
            break;
        default:
            t = PosixFileFun::Stat(MkUrl(dir_ent->d_name));
            break;
        }

        entries.push_back({dir_ent->d_name,t});
    }

    closedir(dr);

    PosixFileFun::ErrnoCheck(url.c_str());

    return true;
}

bool PosixDirFun::MkDir(Url const& dname, bool createParent)
{
    auto url = *dname;
    if(!createParent)
        return mkdir(url.c_str(),S_IRWXU|S_IRWXG)==0;

    char tmp[255];
    char *p = NULL;
    size_t len;

    snprintf(tmp,sizeof(tmp),"%s",url.c_str());
    len = strlen(tmp);
    if(tmp[len-1] == '/')
        tmp[len-1] = 0;
    for(p = tmp+1; *p;p++)
        if(*p == '/')
        {
            *p = 0;
            mkdir(tmp,S_IRWXU);
            *p = '/';
        }
    return mkdir(tmp,S_IRWXU)==0 || (errno == EEXIST);
}

bool Posix::PosixDirFun::ChDir(Url const& dir)
{
    auto url = *dir;
    bool stat = chdir(url.c_str()) == 0;
    if(!stat)
    {
        PosixFileFun::ErrnoCheck(url.c_str());
        return false;
    }
    return true;
}

bool Posix::PosixDirFun::RmDir(Url const& dname)
{
    auto url = *dname;
    return rmdir(url.c_str()) == 0 || (errno = 0);
}

uint32 Posix::PosixFileMod_def::PageSize()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    return static_cast<uint32>(sysconf(_SC_PAGESIZE)-1);
#else
    return 8;
#endif
}



}
}
}
