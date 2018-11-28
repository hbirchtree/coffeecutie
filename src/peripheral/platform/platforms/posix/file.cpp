#include <platforms/posix/file.h>

#include <fcntl.h>
#include <stdlib.h>

using namespace enum_helpers;

namespace platform {
namespace file {
namespace posix {

NodeType PosixFileMod_def::Stat(Url const& fn, file_error&)
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
        return posix::DirFun::MkDir(fn, false, ec);
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
    out.resize(libc::str::len(out.c_str()));
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

bool posix::DirFun::ChDir(Url const& dir, file_error& ec)
{
    auto url  = *dir;
    bool stat = chdir(url.c_str()) == 0;
    if(!stat)
    {
        posix::FileFun::ErrnoCheck(ec, url.c_str());
        return false;
    }
    return true;
}

bool posix::DirFun::MkDir(Url const& dname, bool createParent, file_error& ec)
{
    auto url = *dname;
    if(!createParent)
    {
        bool stat = mkdir(url.c_str(), S_IRWXU | S_IRWXG) == 0;
        posix::FileFun::ErrnoCheck(ec, url.c_str());
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
            posix::FileFun::ErrnoCheck(ec, tmp);
            *p = '/';
        }
    bool stat = mkdir(tmp, S_IRWXU) == 0 || (errno == EEXIST);
    posix::FileFun::ErrnoCheck(ec, tmp);
    return stat;
}

bool posix::DirFun::RmDir(Url const& dname, file_error& ec)
{
    auto url  = *dname;
    bool stat = rmdir(url.c_str()) == 0 || (errno = 0);
    posix::FileFun::ErrnoCheck(ec, url.c_str());
    return stat;
}

bool posix::DirFun::Ls(
    Url const& dname, DirFunDef::DirList& entries, file_error& ec)
{
    auto url = *dname;
    DIR* dr  = opendir(url.c_str());

    if(!dr)
    {
        posix::FileFun::ErrnoCheck(ec, url.c_str());
        return false;
    }

    struct dirent* dir_ent = nullptr;

    while((dir_ent = readdir(dr)) != nullptr)
    {
        NodeType t = NodeType::None;

        switch(dir_ent->d_type)
        {
        case DT_BLK:
            t = NodeType::Block;
            break;
        case DT_CHR:
            t = NodeType::Character;
            break;
        case DT_DIR:
            t = NodeType::Directory;
            break;
        case DT_LNK:
            t = NodeType::Link;
            break;
        case DT_FIFO:
            t = NodeType::FIFO;
            break;
        case DT_REG:
            t = NodeType::File;
            break;
        case DT_SOCK:
            t = NodeType::Socket;
            break;
        default:
            t = posix::FileFun::Stat(constructors::MkUrl(dir_ent->d_name), ec);
            break;
        }

        if(libc::str::cmp(dir_ent->d_name, ".") ||
           libc::str::cmp(dir_ent->d_name, ".."))
            continue;

        entries.push_back({dir_ent->d_name, t});
    }

    closedir(dr);

    posix::FileFun::ErrnoCheck(ec, url.c_str());
    errno = 0;

    return true;
}

Url posix::DirFun::Basename(CString const& n, file_error& ec)
{
#if !defined(COFFEE_USE_POSIX_BASENAME)
    if(str::len(n) < 1)
        return ".";
    // This one is fast, but does not handle rootfs
    int64 idx = Search::ChrFindR(n, '/') - n;
    if(idx < 0)
        return n;
    CString out;
    out.insert(0, &n[idx + 1], str::len(n) - idx - 1);
    if(out.empty())
        out = ".";
    return out;
#else
    // This one is slower, but more compliant
    CString out   = n;
    CString out_s = basename(&out[0]);
    posix::FileFun::ErrnoCheck(ec);
    return constructors::MkUrl(out_s, RSCA::SystemFile);
#endif
}

Url posix::DirFun::Dirname(CString const& fname, file_error& ec)
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
    posix::FileFun::ErrnoCheck(ec);
    return constructors::MkUrl(out_s, RSCA::SystemFile);
#endif
}

u32 PosixFileMod_def::PageSize()
{
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
    return static_cast<u32>(sysconf(_SC_PAGESIZE) - 1);
#else
    return 8;
#endif
}

int PosixFileMod_def::MappingFlags(RSCA acc)
{
    int mapping = 0;

    if(feval(acc & (RSCA::Persistent)))
        mapping = MAP_SHARED;
    else
        mapping = MAP_PRIVATE;

#if defined(COFFEE_APPLE)
    if(feval(acc & RSCA::NoCache))
        mapping |= MAP_NOCACHE;

    if(feval(acc & RSCA::HugeFile))
        mapping |= VM_FLAGS_SUPERPAGE_SIZE_ANY;
#endif

#if defined(COFFEE_LINUX)
#if !defined(COFFEE_NO_HUGETLB)
    if(feval(acc & RSCA::HugeFile))
        mapping |= MAP_HUGETLB;
#endif

    if(feval(acc & RSCA::ExclusiveLocking))
        mapping |= MAP_LOCKED;
#endif

    return mapping;
}

int PosixFileMod_def::ProtFlags(RSCA acc)
{
    int prot = PROT_NONE;

    if(feval(acc & (RSCA::ReadOnly)))
        prot |= PROT_READ;
    if(feval(acc & RSCA::WriteOnly))
        prot |= PROT_WRITE;
    if(feval(acc & RSCA::Executable))
        prot |= PROT_EXEC;

    return prot;
}

int PosixFileMod_def::PosixRscFlags(RSCA acc)
{
    int oflags = 0;

    if(feval(acc, RSCA::ReadWrite))
    {
        oflags = O_RDWR;

        if(feval(acc & RSCA::Discard))
            oflags |= O_TRUNC;
        if(feval(acc & RSCA::Append))
            oflags |= O_APPEND;
    } else if(feval(acc, RSCA::Executable))
        oflags = O_RDONLY;
    else if(feval(acc, RSCA::ReadOnly))
        oflags = O_RDONLY;
    else if(feval(acc, RSCA::WriteOnly))
    {
        if(feval(acc & RSCA::Append))
            oflags = O_APPEND | O_WRONLY;
        else
            oflags = O_WRONLY;

        if(feval(acc & RSCA::Discard))
            oflags |= O_TRUNC;
    }

    if(feval(acc & RSCA::NewFile))
        oflags |= O_CREAT;

#if defined(COFFEE_LINUX)
    if(feval(acc & RSCA::TempFile))
        oflags |= O_TMPFILE;
#endif

    return oflags;
}

} // namespace posix
} // namespace file
} // namespace platform
