#pragma once

#include "../../types/cdef/memtypes.h"
#include "../../types/edef/enumfun.h"
#include "../../types/edef/resenum.h"
#include "../../types/tdef/integertypes.h"
#include "../../types/tdef/stltypes.h"
#include <coffee/interfaces/byte_provider.h>

namespace Coffee {

struct Url;

namespace CResources {

/*!
 * \brief A data resource which location cannot be changed.
 */
struct Resource : ByteProvider
{
  private:
    CString m_resource; /*!< URL for the resource*/

    struct ResourceData;

    struct RscData_deleter
    {
        void operator()(ResourceData* data);
    };

    UqPtr<ResourceData, RscData_deleter> m_platform_data;

  public:
    friend bool FilePull(Resource& resc);
    friend bool FileCommit(Resource& resc, RSCA acc);

    friend bool FileMap(Resource& resc, RSCA acc, szptr size);
    friend bool FileUnmap(Resource& resc);

    friend bool FileOpenMap(Resource& resc, szptr size, RSCA acc);

    friend bool FileExists(const Resource& resc);
    friend void FileFree(Resource& resc);

    /*!
     * \brief Constructs a resource
     * \param url Path to resource
     */
    Resource(Url const& url);
    Resource(cstring rsrc, RSCA acc = RSCA::AssetFile);
    Resource(Resource&& rsc);
    ~Resource();

    void* data; /*!< Data pointer*/
    szptr size; /*!< Data size*/

    cstring resource() const;
    bool    valid() const;

    /*!
     * \brief Owning data assignment
     * \param data
     * \return
     */
    Resource& operator=(Bytes&& data);

    /*!
     * \brief Borrowing data assignment
     * \param data
     * \return
     */
    FORCEDINLINE Resource& operator=(Bytes const& data)
    {
        this->data = data.data;
        this->size = data.size;
        return *this;
    }

    operator Bytes();

    operator Path() const;

    enum FileFlags
    {
        Undefined,
        FileIO,
        Mapped,
    };

    FileFlags flags;
};

C_FLAGS(Resource::FileFlags, int);

extern bool FileExists(const Resource& resc);

/*!
 * \brief Memory map file as buffer
 * \return True if success
 */
extern bool FileMap(Resource& resc, RSCA acc = RSCA::ReadOnly, szptr size = 0);
/*!
 * \brief Unmap file
 * \return True if success
 */
extern bool FileUnmap(Resource& resc);

extern bool FileOpenMap(Resource& resc, szptr size, RSCA acc = RSCA::ReadWrite);

/*!
 * \brief Free data pointer
 */
C_DEPRECATED extern void FileFree(Resource& resc);

extern bool FilePull(Resource& resc);

C_DEPRECATED FORCEDINLINE bool FilePull(Resource& resc, bool, bool)
{
    return FilePull(resc);
}

/*!
 * \brief Save data to file
 * \return
 */
extern bool FileCommit(Resource& resc, RSCA acc = RSCA::Discard);

C_DEPRECATED FORCEDINLINE bool FileCommit(
    Resource& resc, bool append, RSCA acc = RSCA::Discard)
{
    return FileCommit(
        resc,
        acc | (append ? RSCA::NewFile | RSCA::WriteOnly | RSCA::Append
                      : RSCA::None));
}

/*!
 * \brief Create a directory
 * \param dirname
 * \param recursive Whether or not to create non-existent parent directories.
 * Equal to the "-p" option for mkdir. \return True if process succeeded
 */
extern bool FileMkdir(const Url& dirname, bool recursive);

} // namespace CResources

FORCEDINLINE Bytes FileGetDescriptor(CResources::Resource& resc)
{
    return {C_CAST<byte_t*>(resc.data), resc.size, resc.size};
}
FORCEDINLINE BytesConst FileGetDescriptor(const CResources::Resource& resc)
{
    return {C_CAST<byte_t const*>(resc.data), resc.size, resc.size};
}

FORCEDINLINE CResources::Resource operator"" _rsc(const char* fn, size_t)
{
    return CResources::Resource(fn, RSCA::AssetFile);
}

FORCEDINLINE CResources::Resource operator"" _tmp(const char* fn, size_t)
{
    return CResources::Resource(fn, RSCA::TemporaryFile);
}

FORCEDINLINE CResources::Resource operator"" _cache(const char* fn, size_t)
{
    return CResources::Resource(fn, RSCA::CachedFile);
}

FORCEDINLINE CResources::Resource operator"" _config(const char* fn, size_t)
{
    return CResources::Resource(fn, RSCA::ConfigFile);
}

FORCEDINLINE CResources::Resource operator"" _sysfile(const char* fn, size_t)
{
    return CResources::Resource(fn, RSCA::SystemFile);
}

namespace Strings {
extern CString to_string(CResources::Resource const& r);
}
} // namespace Coffee
