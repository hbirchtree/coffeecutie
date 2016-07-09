#pragma once

#include "../cfile.h"
#include "../../plat_primary_identify.h"

#if defined(COFFEE_APPLE)
#include <mach/vm_statistics.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>



namespace Coffee{
namespace CResources{
namespace Posix{

struct PosixApi
{
    struct FileHandle
    {
	FileHandle():
            fd(0)
        {
        }

        int fd;
    };
    using FileMapping = FileFunDef::FileMapping;
};

struct PosixFileMod_def : CommonFileFun
{
    STATICINLINE bool ErrnoCheck(cstring ref = nullptr)
    {
	if(errno!=0)
        {
            fprintf(stderr,"ERROR:%s: %s\n",ref,strerror(errno));
            errno = 0;
            return true;
        }
        return false;
    }

    STATICINLINE NodeType Stat(cstring fn)
    {
        struct stat fs = {};
        int s = lstat(fn,&fs);
        if(s != 0)
            return NodeType::None;
        mode_t m = fs.st_mode;
        if(m&S_IFDIR)
            return NodeType::Directory;
        else if(m&S_IFCHR)
            return NodeType::Character;
        else if(m&S_IFBLK)
            return NodeType::Block;
        else if(m&S_IFREG)
            return NodeType::File;
        else if(m&S_IFLNK)
            return NodeType::Link;
        else if(m&S_IFSOCK)
            return NodeType::Socket;
        else if(m&S_IFIFO)
            return NodeType::FIFO;
        return NodeType::None;
    }
    STATICINLINE bool Touch(NodeType t, cstring fn)
    {
	switch(t)
        {
        case NodeType::File:
        {
            int fd = creat(fn,S_IRWXU);
            close(fd);
            break;
        }
	default:
	    return false;
	}
	return true;
    }
    STATICINLINE bool Rm(cstring fn)
    {
	bool stat = unlink(fn) == 0;
	ErrnoCheck();
	return stat;
    }
};

template<typename FH, typename FM,typename ScratchBuf>
struct PosixFileFun_def : PosixFileMod_def
{
    STATICINLINE FH* Open(cstring fn, ResourceAccess ac)
    {
        int fd = open(fn,PosixRscFlags(ac),S_IRWXU|S_IRGRP);

	if(fd == -1)
	    return nullptr;

	FH* fh = new FH();
	fh->fd = fd;

        return fh;
    }
    STATICINLINE bool Close(FH* fh)
    {
	close(fh->fd);
	delete fh;
        return true;
    }

    STATICINLINE CByteData Read(FH* f_h, uint64 f_size, bool)
    {
	CByteData data;

	szptr sz = Size(f_h);

	if(f_size <= sz && f_size != -1)
	    sz = f_size;

	data.data = (byte_t*)Alloc(sz);
	data.size = sz;

        szptr i = 0;
        szptr chnk = 0;
        while(i<sz)
        {
            chnk = ((sz-i) < Int32_Max) ? (sz-i) : Int32_Max;
            i += read(f_h->fd,
                      &(((byte_t*)(data.data))[i]),
                      chnk);
            if(ErrnoCheck())
                break;
        }

        if(i < sz)
        {
            CFree(data.data);
            return {};
        }

	return data;
    }

    STATICINLINE bool Seek(FH*, uint64)
    {
	/* We won't use this, it's tedious */
	return false;
    }

    STATICINLINE bool Write(FH* f_h, CByteData const& d, bool)
    {
        szptr i = 0;
        szptr it = 0;
        szptr chnk = 0;
        while(i<d.size)
        {
            chnk = ((d.size-i) < Int32_Max) ? (d.size-i) : Int32_Max;
            i += write(f_h->fd,
                       &(((byte_t*)(d.data))[i]),
                       chnk);
            if(ErrnoCheck() && it != 0)
                break;
            it++;
        }
        return i == d.size;
    }

    STATICINLINE FM Map(cstring filename, ResourceAccess acc,
                    szptr offset, szptr size, int* error)
    {
        if(error)
            *error = 0;
        szptr pa_offset = offset & ~(PageSize());

        if(pa_offset!=offset)
            return {};

        /*Translate access flags*/
        int oflags = PosixRscFlags(acc);
        int prot = ProtFlags(acc);
        int mapping = MappingFlags(acc);

        /*... and then actually open it*/
        int fd = open(filename,oflags);
        if(fd < 0)
        {
            if(error)
                *error = errno;
            return {};
        }

        byte_t* addr = nullptr;

#if defined(COFFEE_LINUX)
        addr = (byte_t*)mmap64(
                    NULL,
                    offset+size-pa_offset,
                    prot,mapping,
                    fd,pa_offset);
#else
        addr = (byte_t*)mmap(
                    NULL,
                    offset+size-pa_offset,
                    prot,mapping,
                    fd,pa_offset);
#endif

        if(addr == MAP_FAILED)
        {
            *error = errno;
            return {};
        }

        FileMapping map;
        map.ptr = addr;
        map.size = size;
        map.acc = acc;

        return map;
    }

    STATICINLINE bool Unmap(FM* mapp)
    {
        void* ptr = mapp->ptr;
        szptr size = mapp->size;
        if(munmap(ptr,size)==0)
        {
            return true;
        }else
            return false;
    }
    STATICINLINE bool MapCache(void* t_ptr, szptr t_size, szptr r_off, szptr r_size)
    {
        if(r_off+r_size>t_size)
            return false;
        byte_t* base = (byte_t*)t_ptr;
        base += r_off;
        return mlock(base,r_size)==0;
    }
    STATICINLINE bool MapUncache(void* t_ptr, szptr t_size, szptr r_off, szptr r_size)
    {
        if(r_off+r_size>t_size)
            return false;
        byte_t* base = (byte_t*)t_ptr;
        base += r_off;
        return munlock(base,r_size)==0;
    }
    STATICINLINE bool MapSync(void* ptr, szptr size)
    {
        return msync(ptr,size,MS_SYNC|MS_INVALIDATE);
    }

    STATICINLINE ScratchBuf ScratchBuffer(szptr size, ResourceAccess access)
    {
        int proto = ProtFlags(access);
        int mapflags = MappingFlags(access);

#if defined(COFFEE_LINUX)
        mapflags |= MAP_ANONYMOUS;
#elif defined(COFFEE_APPLE)
        mapflags |= MAP_ANON;
#endif
        
        
        ScratchBuf buf;
#if defined(COFFEE_LINUX)
        // mmap64() is a Linux-specific feature it seems
        buf.ptr = mmap64(nullptr,size,proto,mapflags,-1,0);
#elif defined(COFFEE_APPLE)
        buf.ptr = mmap(nullptr,size,proto,mapflags,-1,0);
#endif
        buf.acc = access;
        buf.size = size;

        if(!buf.ptr)
            return {};

        return buf;
    }
    STATICINLINE void ScratchUnmap(ScratchBuf* buf)
    {
        munmap(buf->ptr,buf->size);
    }

    STATICINLINE szptr Size(cstring fn)
    {
        struct stat st = {};
        if(stat(fn,&st)!=0)
            ErrnoCheck(fn);
        errno = 0;
        return st.st_size;
    }

    STATICINLINE bool Exists(cstring fn)
    {
        struct stat st;
        bool status = stat(fn,&st)==0;
        if(status)
        {
            errno = 0;
            return true;
        }else{
            return (errno!=ENOENT||errno==ENOTDIR) && (errno=0);
        }
    }

    STATICINLINE szptr Size(FH* fh)
    {
        struct stat st;
        if(fh->fd>0)
        {
            if(fstat(fh->fd,&st)!=0)
                ErrnoCheck();
            errno = 0;
            return st.st_size;
        }else{
            /* We somehow want a filename here. */
            return 0;
        }
    }
    STATICINLINE bool Exists(FH* fn)
    {
        struct stat st;
        bool status = fstat(fn->fd,&st)==0;
        errno = 0;
        if(status)
            return true;
        else
            return errno==ENOENT|| errno==ENOTDIR;
    }

protected:
    STATICINLINE uint32 PageSize()
    {
#if defined(COFFEE_LINUX) || defined(COFFEE_APPLE)
        return sysconf(_SC_PAGESIZE)-1;
#else
        return 8;
#endif
    }

    STATICINLINE int MappingFlags(ResourceAccess acc)
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
        if(feval(acc&ResourceAccess::HugeFile))
            mapping |= MAP_HUGETLB;

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
    STATICINLINE int ProtFlags(ResourceAccess acc)
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
    STATICINLINE int PosixRscFlags(ResourceAccess acc)
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
};

struct PosixFileFun : PosixFileFun_def<PosixApi::FileHandle,PosixApi::FileMapping,CommonFileFun::ScratchBuf>
{
    using FileHandle = PosixApi::FileHandle;
    using FileMapping = PosixApi::FileMapping;
    using ScratchBuf = CommonFileFun::ScratchBuf;
};

struct PosixDirFun : DirFunDef
{
    STATICINLINE CString Basename(cstring dname)
    {
        int64 idx = StrFind(dname,"/")-dname;
        if(idx <= 0)
            return {};
        CString out;
        out.insert(0,dname,idx);
        return out;
    }
    STATICINLINE bool MkDir(cstring dname, bool createParent)
    {
        if(!createParent)
            return mkdir(dname,S_IRWXU|S_IRWXG)==0;

        char tmp[256];
        char *p = NULL;
        size_t len;

        snprintf(tmp,sizeof(tmp),"%s",dname);
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
        return mkdir(tmp,S_IRWXU)==0;
    }
    STATICINLINE bool RmDir(cstring dname)
    {
        return rmdir(dname) == 0;
    }

    using Type = FileFunDef::NodeType;

    STATICINLINE bool Ls(cstring dname, DirList& entries)
    {
        DIR* dr = opendir(dname);

        if(!dr)
        {
            PosixFileFun::ErrnoCheck(dname);
            return false;
        }

        struct dirent* dir_ent = nullptr;

        while((dir_ent = readdir(dr)))
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
                t = PosixFileFun::Stat(dir_ent->d_name);
                break;
            }

            entries.push_back({dir_ent->d_name,t});
        }

        closedir(dr);

        return true;
    }
};

}
}
}
