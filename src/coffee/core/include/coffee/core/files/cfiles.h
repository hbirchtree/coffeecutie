#pragma once

#include <peripherals/concepts/resource.h>

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
class Resource
{
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
    Resource(
        std::string_view rsrc, semantic::RSCA acc = semantic::RSCA::AssetFile);
    Resource(Resource&& rsc);
    ~Resource();

    semantic::Span<char>       data_rw;
    semantic::Span<const char> data_ro;

    cstring resource() const;
    bool    valid() const;

    semantic::Span<const char> data()
    {
        return C_OCAST<BytesConst>(*this).as<const char>().view;
    }

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
        this->data_rw = data;
        this->data_ro = data;
        return *this;
    }

    FORCEDINLINE Resource& operator=(BytesConst const& data)
    {
        this->data_rw = {};
        this->data_ro = data;
        return *this;
    }

    FORCEDINLINE Resource& operator=(Resource&& other)
    {
        this->data_rw         = std::move(other.data_rw);
        this->data_ro         = std::move(other.data_ro);
        this->flags           = other.flags;
        this->m_platform_data = std::move(other.m_platform_data);
        this->m_resource      = std::move(other.m_resource);

        other.flags = Undefined;
        other.m_platform_data.reset();
        other.m_resource.clear();

        return *this;
    }

    operator Bytes();
    operator BytesConst();

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
        acc
            | (append ? RSCA::NewFile | RSCA::WriteOnly | RSCA::Append
                      : RSCA::None));
}

/*!
 * \brief Create a directory
 * \param dirname
 * \param recursive Whether or not to create non-existent parent directories.
 * Equal to the "-p" option for mkdir. \return True if process succeeded
 */
extern bool FileMkdir(const Url& dirname, bool recursive);

namespace resource_literals {

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

FORCEDINLINE Resource operator"" _tmpfile(const char* fn, size_t)
{
    return Resource(fn, RSCA::TempFile);
}

}

using namespace resource_literals;
} // namespace Coffee
