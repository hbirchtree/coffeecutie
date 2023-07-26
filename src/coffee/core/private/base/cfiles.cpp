#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <peripherals/stl/math.h>
#include <peripherals/stl/string/hex.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>
#include <url/url.h>

#define CFILES_TAG "File::"

namespace Coffee {

using namespace ::platform::file;
using ::platform::url::constructors::MkUrl;

struct Resource::ResourceData
{
    platform::file::map_handle                   m_mapping;
    std::unique_ptr<platform::file::file_handle> m_handle;
    semantic::Bytes                              m_resourceBuffer;
    Url                                          m_url;
};

void Resource::RscData_deleter::operator()(Resource::ResourceData* data)
{
    delete data;
}

Resource::Resource(std::string_view rsrc, RSCA acc) :
    Resource(
        MkUrl(std::string(rsrc.begin(), rsrc.end()), acc & RSCA::StorageMask))
{
}

Resource::Resource(const Url& url) :
    m_resource(*url),
    m_platform_data(
        stl_types::
            make_unique_with_destructor<ResourceData, RscData_deleter>()),
    flags(Undefined)
{
    m_platform_data->m_url = url;

    Profiler::DeepProfile(CFILES_TAG "Resource created");
}

Resource::Resource(Resource&& rsc)
{
    this->data_rw         = rsc.data_rw;
    this->data_ro         = rsc.data_ro;
    this->flags           = rsc.flags;
    this->m_platform_data = std::move(rsc.m_platform_data);
    this->m_resource      = std::move(rsc.m_resource);

    rsc.flags           = FileFlags::Undefined;
    rsc.m_platform_data = nullptr;
}

Resource::~Resource()
{
}

cstring Resource::resource() const
{
    return m_resource.c_str();
}

bool Resource::valid() const
{
    return !m_resource.empty();
}

Resource& Resource::operator=(semantic::Bytes&& data)
{
    semantic::Bytes& internal_store = this->m_platform_data->m_resourceBuffer;

    internal_store = std::move(data);
    this->data_rw  = internal_store;
    this->data_ro  = internal_store;
    return *this;
}

Resource::operator Path() const
{
    return Path(m_resource);
}

Resource::operator semantic::Bytes()
{
    if(!data_rw.empty())
        return semantic::Bytes::ofContainer(data_rw);

    if(flags == Undefined && FileMap(*this, RSCA::ReadOnly))
        return semantic::Bytes::ofContainer(data_rw);
    else
        return semantic::Bytes();
}

Resource::operator semantic::BytesConst()
{
    if(!data_ro.empty())
        return semantic::BytesConst::ofContainer(data_ro);

    if(flags == Undefined && FileMap(*this, RSCA::ReadOnly))
    {
        data_ro = data_rw;
        return semantic::BytesConst::ofContainer(data_rw);
    } else
        return semantic::BytesConst();
}

bool FileExists(const Resource& resc)
{
    return platform::file::exists(resc.m_platform_data->m_url).has_value();
}

bool FileMap(Resource& resc, RSCA acc, szptr size)
{
    if(resc.flags & Resource::Mapped)
    {
        Profiler::DeepProfile(CFILES_TAG "File already mapped");
        return true;
    }

    Profiler::DeepPushContext(CFILES_TAG "File mapping");

    szptr map_size = 0;
    resc.data_rw   = {};
    resc.data_ro   = {};

    if(auto fsize = platform::file::size(resc.m_platform_data->m_url);
       fsize.has_value() && fsize.value() > 0)
        map_size = size != 0 ? std::min(fsize.value(), size) : fsize.value();

    if(map_size == 0)
    {
        Profiler::DeepProfile(CFILES_TAG "File not found");
        Profiler::DeepPopContext();
        return false;
    }

    if(auto res = platform::file::map(
           resc.m_platform_data->m_url,
           {.access = acc, .offset = 0, .size = map_size});
       res.has_error())
    {
        Profiler::DeepProfile(CFILES_TAG "Mapping failed");
        Profiler::DeepPopContext();
        return false;
    } else
        resc.m_platform_data->m_mapping = res.value();

    resc.data_rw = resc.m_platform_data->m_mapping;
    resc.data_ro = resc.data_rw;
    resc.flags   = resc.flags | Resource::Mapped;

    Profiler::DeepProfile(CFILES_TAG "File mapped");

    Profiler::DeepPopContext();

    return true;
}

bool FileUnmap(Resource& resc)
{
    DProfContext a(CFILES_TAG "File mapping");
    if(!(resc.flags & Resource::Mapped))
    {
        Profiler::DeepProfile(CFILES_TAG "Non-mapped file called for unmap");
        return false;
    }

    if(auto res
       = platform::file::unmap(std::move(resc.m_platform_data->m_mapping));
       res.has_value())
    {
        Profiler::DeepProfile(CFILES_TAG "Unmapping failed");
        return false;
    }

    resc.m_platform_data->m_mapping = {};
    resc.data_rw                    = {};
    resc.data_ro                    = {};

    resc.flags ^= Resource::Mapped;

    return true;
}

bool FileOpenMap(Resource& resc, szptr size, RSCA acc)
{
    DProfContext a(CFILES_TAG "File mapping");
    //    FileFun::Truncate(resc.m_platform_data->m_url, size, ec);

    if(auto res = platform::file::map(
           resc.m_platform_data->m_url,
           {.access = acc, .offset = 0, .size = size});
       res.has_error())
    {
        Profiler::DeepProfile(CFILES_TAG "Mapping failed");
        return false;
    } else
        resc.m_platform_data->m_mapping = res.value();

    resc.data_rw = resc.m_platform_data->m_mapping;
    resc.data_ro = resc.data_rw;
    resc.flags |= Resource::Mapped;

    return true;
}

void FileFree(Resource& resc)
{
    if(!(resc.flags & Resource::FileIO))
        return;

    semantic::Bytes local = std::move(resc.m_platform_data->m_resourceBuffer);

    resc.m_platform_data->m_resourceBuffer = {};

    Profiler::DeepProfile(CFILES_TAG "File buffer free'd");
}

bool FilePull(Resource& resc)
{
    DProfContext a(CFILES_TAG "File reading");

    auto& data = resc.m_platform_data->m_resourceBuffer;

    if(auto fd
       = platform::file::open_file(resc.m_platform_data->m_url, RSCA::ReadOnly);
       fd.has_error())
    {
        Profiler::DeepProfile(CFILES_TAG "File not found");
        return false;
    } else if(auto rd = platform::file::read(fd.value()); rd.has_error())
    {
        Profiler::DeepProfile(CFILES_TAG "File read failed");
        return false;
    } else
        data = rd.value().as<libc_types::u8>();

    resc.data_rw = data;
    resc.data_ro = data;
    resc.flags   = resc.flags | Resource::FileIO;

    return true;
}

bool FileCommit(Resource& resc, RSCA acc)
{
    DProfContext a(CFILES_TAG "File write");

    if(auto fd = platform::file::open_file(resc.m_platform_data->m_url, acc);
       fd.has_error())
    {
        Profiler::DeepProfile(CFILES_TAG "File not created");
        return false;
    } else if(auto write = platform::file::write(
                  fd.value(), C_OCAST<semantic::BytesConst>(resc));
              write.has_value())
    {
        Profiler::DeepProfile(CFILES_TAG "File write failed");
        return false;
    } else
        return true;
}

bool FileMkdir(Url const& dirname, bool recursive)
{
    Profiler::DeepProfile(CFILES_TAG "Directory creation");

    if(auto res
       = platform::file::create_directory(dirname, {.recursive = recursive});
       res.has_value())
    {
        Profiler::DeepProfile(CFILES_TAG "Directory creation failed");
        return false;
    } else
        return true;
}

namespace Strings {
std::string to_string(Resource const& r)
{
    return std::string("rsc(")
           + stl_types::str::fmt::pointerify(r.data_ro.data()) + "+"
           + std::to_string(r.data_ro.size()) + ")";
}
} // namespace Strings
} // namespace Coffee
