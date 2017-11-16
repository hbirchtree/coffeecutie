#pragma once

#include "../../types/edef/enumfun.h"
#include "../../types/tdef/stltypes.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/edef/resenum.h"
#include "../../types/cdef/memtypes.h"

namespace Coffee{

struct Url;

namespace CResources{

/*!
 * \brief A data resource which location cannot be changed.
 */
struct Resource{
private:
    CString m_resource; /*!< URL for the resource*/

    struct ResourceData;

    ResourceData* m_platform_data;

public:

    friend bool FilePull(Resource &resc, bool textmode, bool bigendian);
    friend bool FileCommit(Resource &resc, bool append, ResourceAccess acc);

    friend bool FileMap(Resource &resc, ResourceAccess acc, szptr size);
    friend bool FileUnmap(Resource &resc);

    friend bool FileExists(const Resource& resc);

    /*!
     * \brief Constructs a resource
     * \param rsrc Path to resource
     * \param absolute False by default, true to not use global file redirection
     */
    Resource(cstring rsrc = nullptr,
             bool absolute = false,
             ResourceAccess acc = ResourceAccess::None);
    Resource(Url const& url);
    Resource(cstring rsrc,
             ResourceAccess acc);
    Resource(Resource &&rsc);
    ~Resource();

    void* data; /*!< Data pointer*/
    szptr size; /*!< Data size*/

    cstring resource() const;
    bool valid() const;

    enum FileFlags
    {
        Undefined,
        FileIO,
        Mapped,
    };

    FileFlags flags;
};

C_FLAGS(Resource::FileFlags,int);

extern bool FileExists(const Resource& resc);

/*!
 * \brief Memory map file as buffer
 * \return True if success
 */
extern bool FileMap(Resource& resc, ResourceAccess acc = ResourceAccess::ReadOnly, szptr size = 0);
/*!
 * \brief Unmap file
 * \return True if success
 */
extern bool FileUnmap(Resource& resc);

/*!
 * \brief Free data pointer
 */
extern void FileFree(Resource &resc);
extern bool FilePull(Resource &resc, bool textmode = false, bool bigendian = false);
/*!
 * \brief Save data to file
 * \return
 */
extern bool FileCommit(Resource& resc, bool append = false,
		       ResourceAccess acc = ResourceAccess::None);
/*!
 * \brief Save data to file, append null-terminator
 * \return
 */
extern void FileCommitTextmode(const Resource& resc, bool append = false);

/*!
 * \brief Create a directory
 * \param dirname
 * \param recursive Whether or not to create non-existent parent directories. Equal to the "-p" option for mkdir.
 * \return True if process succeeded
 */
extern bool FileMkdir(const Url &dirname, bool recursive);

}

FORCEDINLINE Bytes FileGetDescriptor(CResources::Resource& resc)
{
    return {C_CAST<byte_t*>(resc.data),resc.size, 0};
}
FORCEDINLINE BytesConst FileGetDescriptor(const CResources::Resource& resc)
{
    return {C_CAST<byte_t const*>(resc.data),resc.size, 0};
}

FORCEDINLINE CResources::Resource operator "" _rsc(const char* fn, size_t)
{
    return CResources::Resource(fn,
                                ResourceAccess::SpecifyStorage|
                                ResourceAccess::AssetFile);
}

FORCEDINLINE CResources::Resource operator "" _tmp(const char* fn, size_t)
{
    return CResources::Resource(fn,
                                ResourceAccess::SpecifyStorage|
                                ResourceAccess::TemporaryFile);
}

FORCEDINLINE CResources::Resource operator "" _cache(const char* fn, size_t)
{
    return CResources::Resource(fn,
                                ResourceAccess::SpecifyStorage|
                                ResourceAccess::CachedFile);
}

FORCEDINLINE CResources::Resource operator "" _config(const char* fn, size_t)
{
    return CResources::Resource(fn,
                                ResourceAccess::SpecifyStorage|
                                ResourceAccess::ConfigFile);
}

namespace Strings{
extern CString to_string(CResources::Resource const& r);
}
}

