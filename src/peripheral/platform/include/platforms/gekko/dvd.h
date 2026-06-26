#pragma once

/* Read-only GameCube DVD (FST) access exposed as a newlib "dvd:" devoptab, so
 * plain fopen/open/stat (and therefore Coffee's Url/Resource layer) read disc
 * files with no DVD-specific code at the call site.
 *
 * On Dolphin the disc is a DirectoryBlob mounted via MAIN_DEFAULT_ISO; Dolphin
 * patches the disc header with the synthesized FST offset/size, so we read the
 * header at disc offset 0, parse the FST once at mount, and resolve files by
 * basename (files/ is flat). GameCube uses no address shift -- offsets/lengths
 * are raw byte values.
 *
 * mount() registers the device; RSCA::AssetFile resolves onto dvd:/ via the
 * GameCube branch of the Url layer (see url.cpp).
 */

#include <ogc/dvd.h>

#include <sys/iosupport.h>
#include <sys/reent.h>
#include <sys/stat.h>

#include <errno.h>
#include <malloc.h>
#include <string.h>

#include <string>
#include <vector>

namespace platform::file::gekko::dvd {

namespace detail {

inline u32 be32(u8 const* p)
{
    return (u32(p[0]) << 24) | (u32(p[1]) << 16) | (u32(p[2]) << 8) | u32(p[3]);
}

/* Read len bytes from absolute disc offset `abs` into dst. DVD hardware needs a
 * 32-byte aligned offset/buffer/length, so bounce through an aligned buffer
 * covering the rounded range and copy out the requested slice. */
inline bool read_at(void* dst, u32 len, u32 abs)
{
    u32 const start = abs & ~31u;
    u32 const span  = ((abs + len + 31u) & ~31u) - start;
    u8*       tmp   = static_cast<u8*>(memalign(32, span));
    if(!tmp)
        return false;
    dvdcmdblk blk;
    bool const ok =
        DVD_ReadPrio(&blk, tmp, span, static_cast<s64>(start), 2) > 0;
    if(ok)
        memcpy(dst, tmp + (abs - start), len);
    free(tmp);
    return ok;
}

struct file_entry
{
    std::string name;
    u32         off;
    u32         size;
};

inline std::vector<file_entry> g_files;
inline bool                    g_ready = false;

/* Reduce a path to its filename: strip any directory and any "device:" prefix.
 * Sufficient because the disc's files/ folder is flat. */
inline std::string basename_of(char const* path)
{
    std::string s(path);
    if(auto slash = s.find_last_of('/'); slash != std::string::npos)
        s = s.substr(slash + 1);
    if(auto colon = s.find_last_of(':'); colon != std::string::npos)
        s = s.substr(colon + 1);
    return s;
}

inline file_entry const* find(char const* path)
{
    auto const name = basename_of(path);
    for(auto const& e : g_files)
        if(e.name == name)
            return &e;
    return nullptr;
}

/* Read + cache the flat file table from the disc FST. */
inline bool parse_fst()
{
    g_files.clear();

    u8* h = static_cast<u8*>(memalign(32, 0x440));
    if(!h)
        return false;
    dvdcmdblk blk;
    if(DVD_ReadPrio(&blk, h, 0x440, 0, 2) <= 0)
    {
        free(h);
        return false;
    }
    u32 const fst_off  = be32(h + 0x424);
    u32 const fst_size = be32(h + 0x428);
    free(h);
    if(!fst_off || !fst_size)
        return false;

    u8* fst = static_cast<u8*>(memalign(32, (fst_size + 31u) & ~31u));
    if(!fst)
        return false;
    if(!read_at(fst, fst_size, fst_off))
    {
        free(fst);
        return false;
    }

    u32 const   count   = be32(fst + 8); // root entry length = #entries
    char const* strings = reinterpret_cast<char const*>(fst + count * 12);
    for(u32 i = 1; i < count; i++)
    {
        u8 const* e = fst + i * 12;
        if(e[0] != 0) // 0 = file, 1 = directory
            continue;
        u32 const noff = (u32(e[1]) << 16) | (u32(e[2]) << 8) | u32(e[3]);
        g_files.push_back({std::string(strings + noff), be32(e + 4),
                           be32(e + 8)});
    }
    free(fst);
    return true;
}

/* --- devoptab callbacks (read-only) ----------------------------------------*/

struct dvd_file_t
{
    u32 off;
    u32 size;
    u32 pos;
};

inline int dvd_open_r(
    struct _reent* r, void* fileStruct, const char* path, int, int)
{
    auto const* e = find(path);
    if(!e)
    {
        r->_errno = ENOENT;
        return -1;
    }
    auto* f = static_cast<dvd_file_t*>(fileStruct);
    f->off  = e->off;
    f->size = e->size;
    f->pos  = 0;
    return 0;
}

inline int dvd_close_r(struct _reent*, void*)
{
    return 0;
}

inline ssize_t dvd_read_r(struct _reent* r, void* fd, char* ptr, size_t len)
{
    auto* f = static_cast<dvd_file_t*>(fd);
    if(f->pos >= f->size)
        return 0;
    if(len > f->size - f->pos)
        len = f->size - f->pos;
    if(!read_at(ptr, static_cast<u32>(len), f->off + f->pos))
    {
        r->_errno = EIO;
        return -1;
    }
    f->pos += static_cast<u32>(len);
    return static_cast<ssize_t>(len);
}

inline off_t dvd_seek_r(struct _reent* r, void* fd, off_t pos, int dir)
{
    auto*       f    = static_cast<dvd_file_t*>(fd);
    off_t const base = dir == SEEK_SET   ? 0
                       : dir == SEEK_CUR ? static_cast<off_t>(f->pos)
                                         : static_cast<off_t>(f->size);
    off_t const np = base + pos;
    if(np < 0)
    {
        r->_errno = EINVAL;
        return -1;
    }
    f->pos = static_cast<u32>(np);
    return np;
}

inline void fill_stat(struct stat* st, u32 size)
{
    memset(st, 0, sizeof(*st));
    st->st_size = size;
    st->st_mode = S_IFREG | 0444;
}

inline int dvd_fstat_r(struct _reent*, void* fd, struct stat* st)
{
    fill_stat(st, static_cast<dvd_file_t*>(fd)->size);
    return 0;
}

inline int dvd_stat_r(struct _reent* r, const char* path, struct stat* st)
{
    auto const* e = find(path);
    if(!e)
    {
        r->_errno = ENOENT;
        return -1;
    }
    fill_stat(st, e->size);
    return 0;
}

inline devoptab_t g_devoptab = {
    .name       = "dvd",
    .structSize = sizeof(dvd_file_t),
    .open_r     = dvd_open_r,
    .close_r    = dvd_close_r,
    .read_r     = dvd_read_r,
    .seek_r     = dvd_seek_r,
    .fstat_r    = dvd_fstat_r,
    .stat_r     = dvd_stat_r,
};

} // namespace detail

/* Spin up the drive, parse the FST and register the dvd: device. Idempotent. */
inline bool mount()
{
    if(detail::g_ready)
        return true;

    DVD_Init();
    DVD_Mount();

    u8* h = static_cast<u8*>(memalign(32, 0x40));
    if(!h)
        return false;
    dvdcmdblk blk;
    bool const present =
        DVD_ReadPrio(&blk, h, 0x40, 0, 2) > 0
        && detail::be32(h + 0x1c) == 0xC2339F3D;
    free(h);
    if(!present || !detail::parse_fst())
        return false;

    AddDevice(&detail::g_devoptab);
    detail::g_ready = true;
    return true;
}

/* Convenience direct read (whole file by basename). */
inline bool read_file(std::string const& name, std::vector<u8>& out)
{
    if(!detail::g_ready && !mount())
        return false;
    auto const* e = detail::find(name.c_str());
    if(!e)
        return false;
    out.resize(e->size);
    return detail::read_at(out.data(), e->size, e->off);
}

} // namespace platform::file::gekko::dvd
