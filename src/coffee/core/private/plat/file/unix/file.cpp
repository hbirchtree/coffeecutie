#include <coffee/core/plat/file/unix/file.h>

#include <coffee/core/CDebug>

#include <fcntl.h>
#include <stdlib.h>

namespace Coffee {
namespace CResources {
namespace Posix {

const char* posix_error_category::name() const noexcept
{
    return "posix_error_code";
}

std::string posix_error_category::message(int error_code) const
{
    char* posix_error_msg = strerror(error_code);

    if(!posix_error_msg)
        return "No error";

    return posix_error_msg;
}

FileFunDef<>::NodeType PosixFileMod_def::Stat(Url const& fn, file_error&)
{
    auto        url = *fn;
    struct stat fs  = {};
    int         s   = lstat(url.c_str(), &fs);

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

bool PosixFileMod_def::Touch(NodeType t, Url const& fn, file_error& ec)
{
    auto url = *fn;
    switch(t)
    {
    case NodeType::File:
    {
        int fd = creat(url.c_str(), S_IRWXU);
        errno  = 0;
        close(fd);
        break;
    }
    case NodeType::Directory:
    {
        return PosixDirFun::MkDir(fn, false, ec);
    }
    default:
        return false;
    }
    return true;
}

bool PosixFileMod_def::Rm(Url const& fn, file_error& ec)
{
    errno = 0;

    auto url  = *fn;
    bool stat = unlink(url.c_str()) == 0;
    ErrnoCheck(ec, url.c_str());
    return stat;
}

CString PosixFileMod_def::DereferenceLink(Url const& fn, file_error& ec)
{
    errno = 0;

    CString url = fn.internUrl;

    /* Avoid cyclic dependency in resolution of filenames, yo */
    if(!feval(fn.flags & RSCA::SystemFile))
        url = *fn;

    CString out;
    szptr   name_size = PATH_MAX;
    out.resize(name_size + 1);
    ssize_t sz = readlink(url.c_str(), &out[0], name_size);
    if(errno == EINVAL)
    {
        errno      = 0;
        return out = url.c_str();
    }
    if(sz > 0)
        out.resize(C_FCAST<CString::size_type>(sz));
    else
        return out = url.c_str();

    ErrnoCheck(ec, url.c_str());

    return out;
}

CString PosixFileMod_def::CanonicalName(Url const& fn, file_error& ec)
{
    errno = 0;

    auto url = *fn;
#if defined(COFFEE_LINUX) && !defined(COFFEE_NO_CANONICALIZE)
    cstring_w name = canonicalize_file_name(url.c_str());
    if(!name)
    {
        ErrnoCheck(ec, url.c_str());
        return {};
    }
    CString out = name;
    free(name);
    return out;
#else
    CString out;
    out.resize(FILENAME_MAX);
    if(!realpath(url.c_str(), &out[0]))
        out.resize(0);
    out.resize(StrLen(out.c_str()));
    return out;
#endif
}

bool PosixFileMod_def::Ln(Url const& src, Url const& target, file_error& ec)
{
    errno = 0;

    auto srcUrl    = *src;
    auto targetUrl = *target;
    if(symlink(srcUrl.c_str(), targetUrl.c_str()) == 0)
        return true;
    else
    {
        ErrnoCheck(ec, targetUrl.c_str());
        return false;
    }
}

szptr PosixFileMod_def::Size(Url const& fn, file_error& ec)
{
    auto        url = *fn;
    struct stat st  = {};

    if(lstat(url.c_str(), &st) != 0)
        ErrnoCheck(ec, url.c_str());

    return C_FCAST<szptr>(st.st_size);
}

bool PosixFileMod_def::Exists(Url const& fn, file_error&)
{
    auto        url = *fn;
    struct stat st;
    bool        status = lstat(url.c_str(), &st) == 0;
    if(status)
    {
        errno = 0;
        return true;
    } else
    {
        return (errno != ENOENT || errno == ENOTDIR) && (errno = 0);
    }
}

void PosixFileMod_def::Truncate(const Url& fn, szptr size, file_error& ec)
{
    auto url = *fn;

    int fd = -1;

    struct stat file_info;
    if(stat(url.c_str(), &file_info) != 0)
        fd = creat(url.c_str(), S_IRWXU);
    else
        fd = open(url.c_str(), PROT_WRITE);

    errno = 0;

#if defined(COFFEE_LINUX)
    if(ftruncate64(fd, C_FCAST<ptroff>(size)) != 0)
#else
    if(ftruncate(fd, C_FCAST<ptroff>(size)) != 0)
#endif
    {
        ErrnoCheck(ec, url.c_str(), -1);
    }

    close(fd);
}

bool Posix::PosixDirFun::ChDir(Url const& dir, file_error& ec)
{
    auto url  = *dir;
    bool stat = chdir(url.c_str()) == 0;
    if(!stat)
    {
        PosixFileFun::ErrnoCheck(ec, url.c_str());
        return false;
    }
    return true;
}

bool PosixDirFun::MkDir(Url const& dname, bool createParent, file_error& ec)
{
    auto url = *dname;
    if(!createParent)
    {
        bool stat = mkdir(url.c_str(), S_IRWXU | S_IRWXG) == 0;
        PosixFileFun::ErrnoCheck(ec, url.c_str());
        return stat;
    }

    char   tmp[255];
    char*  p = nullptr;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", url.c_str());
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
        if(*p == '/')
        {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            PosixFileFun::ErrnoCheck(ec, tmp);
            *p = '/';
        }
    bool stat = mkdir(tmp, S_IRWXU) == 0 || (errno == EEXIST);
    PosixFileFun::ErrnoCheck(ec, tmp);
    return stat;
}

bool Posix::PosixDirFun::RmDir(Url const& dname, file_error& ec)
{
    auto url = *dname;
    bool stat = rmdir(url.c_str()) == 0 || (errno = 0);
    PosixFileFun::ErrnoCheck(ec, url.c_str());
    return stat;
}

bool PosixDirFun::Ls(
    Url const& dname, DirFunDef::DirList& entries, file_error& ec)
{
    auto url = *dname;
    DIR* dr  = opendir(url.c_str());

    if(!dr)
    {
        PosixFileFun::ErrnoCheck(ec, url.c_str());
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
            t = PosixFileFun::Stat(MkUrl(dir_ent->d_name), ec);
            break;
        }

        if(StrCmp(dir_ent->d_name, ".") || StrCmp(dir_ent->d_name, ".."))
            continue;

        entries.push_back({dir_ent->d_name, t});
    }

    closedir(dr);

    PosixFileFun::ErrnoCheck(ec, url.c_str());
    errno = 0;

    return true;
}

Url PosixDirFun::Basename(CString const& n, file_error& ec)
{
#if !defined(COFFEE_USE_POSIX_BASENAME)
    if(StrLen(n) < 1)
        return ".";
    // This one is fast, but does not handle rootfs
    int64 idx = Search::ChrFindR(n, '/') - n;
    if(idx < 0)
        return n;
    CString out;
    out.insert(0, &n[idx + 1], StrLen(n) - idx - 1);
    if(out.empty())
        out = ".";
    return out;
#else
    // This one is slower, but more compliant
    CString out   = n;
    CString out_s = basename(&out[0]);
    PosixFileFun::ErrnoCheck(ec);
    return MkUrl(out_s, RSCA::SystemFile);
#endif
}

Url PosixDirFun::Dirname(CString const& fname, file_error& ec)
{
#if !defined(COFFEE_USE_POSIX_BASENAME)
    int64 idx = Search::ChrFindR(fname, '/') - fname;
    if(idx < 0)
        return fname;
    CString out;
    out.insert(0, &fname[0], idx);
    if(out.empty())
        out = ".";
    return out;
#else
    // This one is slower, but more compliant
    CString out   = fname;
    CString out_s = dirname(&out[0]);
    PosixFileFun::ErrnoCheck(ec);
    return MkUrl(out_s, RSCA::SystemFile);
#endif
}

uint32 Posix::PosixFileMod_def::PageSize()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    return static_cast<uint32>(sysconf(_SC_PAGESIZE) - 1);
#else
    return 8;
#endif
}

int PosixFileMod_def::MappingFlags(ResourceAccess acc)
{
    int mapping = 0;

    if(feval(acc & (ResourceAccess::Persistent)))
        mapping = MAP_SHARED;
    else
        mapping = MAP_PRIVATE;

#if defined(COFFEE_APPLE)
    if(feval(acc & ResourceAccess::NoCache))
        mapping |= MAP_NOCACHE;

    if(feval(acc & ResourceAccess::HugeFile))
        mapping |= VM_FLAGS_SUPERPAGE_SIZE_ANY;
#endif

#if defined(COFFEE_LINUX)
#if !defined(COFFEE_NO_HUGETLB)
    if(feval(acc & ResourceAccess::HugeFile))
        mapping |= MAP_HUGETLB;
#endif

    if(feval(acc & ResourceAccess::ExclusiveLocking))
        mapping |= MAP_LOCKED;

//    if(feval(acc&(ResourceAccess::Streaming)))
//        mapping |= MAP_POPULATE;
//    if(feval(acc&ResourceAccess::GreedyCache))
//        mapping |= MAP_POPULATE|MAP_LOCKED;
//    if(feval(acc&ResourceAccess::NoCache))
//        mapping |= MAP_NONBLOCK;
#endif

    return mapping;
}

int PosixFileMod_def::ProtFlags(ResourceAccess acc)
{
    int prot = PROT_NONE;

    if(feval(acc & (ResourceAccess::ReadOnly)))
        prot |= PROT_READ;
    if(feval(acc & ResourceAccess::WriteOnly))
        prot |= PROT_WRITE;
    if(feval(acc & ResourceAccess::Executable))
        prot |= PROT_EXEC;

    return prot;
}

int PosixFileMod_def::PosixRscFlags(ResourceAccess acc)
{
    int oflags = 0;

    if(feval(acc, ResourceAccess::ReadWrite))
    {
        oflags = O_RDWR;

        if(feval(acc & ResourceAccess::Discard))
            oflags |= O_TRUNC;
    } else if(feval(acc, ResourceAccess::Executable))
        oflags = O_RDONLY;
    else if(feval(acc, ResourceAccess::ReadOnly))
        oflags = O_RDONLY;
    else if(feval(acc, ResourceAccess::WriteOnly))
    {
        if(feval(acc & ResourceAccess::Append))
            oflags = O_APPEND;
        else
            oflags = O_WRONLY;

        if(feval(acc & ResourceAccess::Discard))
            oflags |= O_TRUNC;
    }

    if(feval(acc & ResourceAccess::NewFile))
        oflags |= O_CREAT;

    return oflags;
}

} // namespace Posix
} // namespace CResources
} // namespace Coffee
