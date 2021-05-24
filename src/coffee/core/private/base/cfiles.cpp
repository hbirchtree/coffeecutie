#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <peripherals/stl/math.h>
#include <peripherals/stl/string_casting.h>
#include <platforms/environment.h>
#include <url/url.h>

#define CFILES_TAG "File::"

namespace Coffee {

using namespace ::platform::file;
using ::platform::url::constructors::MkUrl;

struct Resource::ResourceData
{
    platform::file::map_handle         m_mapping;
    UqPtr<platform::file::file_handle> m_handle;
    Bytes                              m_resourceBuffer;
    Url                                m_url;
};

void Resource::RscData_deleter::operator()(Resource::ResourceData* data)
{
    delete data;
}

Resource::Resource(cstring rsrc, RSCA acc) :
    Resource(MkUrl(rsrc, acc & RSCA::StorageMask))
{
}

Resource::Resource(const Url& url) :
    m_resource(*url), m_platform_data(MkUqDST<ResourceData, RscData_deleter>()),
    data(nullptr), size(0), flags(Undefined)
{
    m_platform_data->m_url = url;

    Profiler::DeepProfile(CFILES_TAG "Resource created");
}

Resource::Resource(Resource&& rsc)
{
    this->data            = rsc.data;
    this->size            = rsc.size;
    this->flags           = rsc.flags;
    this->m_platform_data = std::move(rsc.m_platform_data);
    this->m_resource      = std::move(rsc.m_resource);

    rsc.data            = nullptr;
    rsc.flags           = FileFlags::Undefined;
    rsc.m_platform_data = nullptr;
    rsc.size            = 0;
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

Resource& Resource::operator=(Bytes&& data)
{
    Bytes& internal_store = this->m_platform_data->m_resourceBuffer;

    internal_store = std::move(data);
    this->data     = internal_store.data;
    this->size     = internal_store.size;
    return *this;
}

Resource::operator Path() const
{
    return Path(m_resource);
}

Resource::operator Bytes()
{
    if(data && size)
        return Bytes::FromBytes(data, size);

    if(flags == Undefined && FileMap(*this, RSCA::ReadOnly))
        return Bytes::ofBytes(data, size);
    else
        return Bytes();
}

Resource::operator BytesConst()
{
    if(data && size)
        return BytesConst::ofBytes(data, size);

    if(flags == Undefined && FileMap(*this, RSCA::ReadOnly))
        return BytesConst::ofBytes(data, size);
    else
        return BytesConst();
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

    resc.size = platform::file::size(resc.m_platform_data->m_url).value();
    if(size != 0)
        resc.size = math::min(resc.size, size);

    if(resc.size == 0)
    {
        Profiler::DeepProfile(CFILES_TAG "File not found");
        Profiler::DeepPopContext();
        return false;
    }

    if(auto res = platform::file::map(
           resc.m_platform_data->m_url,
           {.access = acc, .offset = 0, .size = resc.size});
       res.has_error())
    {
        resc.size = 0;
        Profiler::DeepProfile(CFILES_TAG "Mapping failed");
        Profiler::DeepPopContext();
        return false;
    } else
        resc.m_platform_data->m_mapping = res.value();

    resc.data  = resc.m_platform_data->m_mapping.view.data();
    resc.flags = resc.flags | Resource::Mapped;

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

    if(auto res =
           platform::file::unmap(std::move(resc.m_platform_data->m_mapping));
       res.has_value())
    {
        Profiler::DeepProfile(CFILES_TAG "Unmapping failed");
        return false;
    }

    resc.m_platform_data->m_mapping = {};
    resc.data                       = nullptr;
    resc.size                       = 0;

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

    resc.data = resc.m_platform_data->m_mapping.view.data();
    resc.size = resc.m_platform_data->m_mapping.view.size();

    resc.flags |= Resource::Mapped;

    return true;
}

void FileFree(Resource& resc)
{
    if(!(resc.flags & Resource::FileIO))
        return;

    Bytes local = std::move(resc.m_platform_data->m_resourceBuffer);

    resc.m_platform_data->m_resourceBuffer = {};

    Profiler::DeepProfile(CFILES_TAG "File buffer free'd");
}

bool FilePull(Resource& resc)
{
    DProfContext a(CFILES_TAG "File reading");

    auto& data = resc.m_platform_data->m_resourceBuffer;

    if(auto fd = platform::file::open_file(
           resc.m_platform_data->m_url, RSCA::ReadOnly);
       fd.has_error())
    {
        Profiler::DeepProfile(CFILES_TAG "File not found");
        return false;
    } else if(auto rd = platform::file::read(fd.value()); rd.has_error())
    {
        Profiler::DeepProfile(CFILES_TAG "File read failed");
        return false;
    } else
        data = rd.value();

    resc.data  = data.data;
    resc.size  = data.size;
    resc.flags = resc.flags | Resource::FileIO;

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
    } else if(auto write =
                  platform::file::write(fd.value(), C_OCAST<BytesConst>(resc));
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

    if(auto res =
           platform::file::create_directory(dirname, {.recursive = recursive});
       res.has_value())
    {
        Profiler::DeepProfile(CFILES_TAG "Directory creation failed");
        return false;
    } else
        return true;
}

namespace Strings {
CString to_string(Resource const& r)
{
    return CString("rsc(") + str::print::pointerify(r.data) + "+" +
           cast_pod(r.size) + ")";
}
} // namespace Strings
} // namespace Coffee
