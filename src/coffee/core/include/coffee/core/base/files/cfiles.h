#pragma once

#include <coffee/core/libc_types.h>
#include <coffee/core/stl_types.h>
#include <coffee/core/types/chunk.h>
#include <coffee/core/types/rsca.h>
#include <coffee/interfaces/byte_provider.h>
#include <peripherals/enum/helpers.h>
#include <peripherals/semantic/chunk.h>
#include <url/url.h>

namespace Coffee {

using Url  = platform::url::Url;
using Path = platform::url::Path;

/*!
 * \brief A data resource which location cannot be changed.
 */
struct Resource : semantic::ByteProvider
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
    friend bool FileCommit(Resource& resc, semantic::RSCA acc);

    friend bool FileMap(Resource& resc, RSCA acc, szptr size);
    friend bool FileUnmap(Resource& resc);

    friend bool FileOpenMap(Resource& resc, szptr size, semantic::RSCA acc);

    friend bool FileExists(const Resource& resc);
    friend void FileFree(Resource& resc);

    /*!
     * \brief Constructs a resource
     * \param url Path to resource
     */
    explicit Resource(Url const& url);
    Resource(cstring rsrc, semantic::RSCA acc = semantic::RSCA::AssetFile);
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

    FORCEDINLINE Resource& operator=(Resource&& other)
    {
        this->data            = other.data;
        this->size            = other.size;
        this->flags           = other.flags;
        this->m_platform_data = std::move(other.m_platform_data);
        this->m_resource      = std::move(other.m_resource);

        other.data  = nullptr;
        other.size  = 0;
        other.flags = Undefined;
        other.m_platform_data.reset();
        other.m_resource.clear();

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

FORCEDINLINE Bytes FileGetDescriptor(Resource& resc)
{
    return {C_CAST<byte_t*>(resc.data), resc.size, resc.size};
}
FORCEDINLINE BytesConst FileGetDescriptor(Resource const& resc)
{
    return {C_CAST<byte_t const*>(resc.data), resc.size, resc.size};
}

FORCEDINLINE Resource operator"" _rsc(const char* fn, size_t)
{
    return Resource(fn, RSCA::AssetFile);
}

FORCEDINLINE Resource operator"" _config(const char* fn, size_t)
{
    return Resource(fn, RSCA::ConfigFile);
}

FORCEDINLINE Resource operator"" _sysfile(const char* fn, size_t)
{
    return Resource(fn, RSCA::SystemFile);
}
} // namespace Coffee
