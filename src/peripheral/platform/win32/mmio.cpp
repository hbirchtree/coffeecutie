#include <platforms/win32/mmio.h>

#include <peripherals/error/windows.h>
#include <windows.h>

namespace platform::file::win32 {

using enum_helpers::feval;
using libc_types::u32;
using semantic::RSCA;

std::pair<DWORD, DWORD> access_to_win32(semantic::RSCA access)
{
    DWORD open   = 0x0;
    DWORD create = 0x0;

    if(feval(access, RSCA::ReadOnly))
        open |= GENERIC_READ;
    if(feval(access, RSCA::WriteOnly))
        open |= GENERIC_WRITE;
    if(feval(access, RSCA::Executable))
        open |= GENERIC_EXECUTE;

    if(feval(access, RSCA::Append))
        open |= FILE_APPEND_DATA;

    if(feval(access, RSCA::Discard))
        create |= CREATE_ALWAYS;
    else if(feval(access, RSCA::NewFile))
        create |= OPEN_ALWAYS;
    else
        create |= OPEN_EXISTING;

    return {open, create};
}

std::pair<DWORD, DWORD> mapping_to_win32(semantic::RSCA access)
{
    DWORD mapping = 0x0;
    DWORD view    = 0x0;

    if(feval(access, RSCA::ReadWrite))
    {
        mapping |= PAGE_READWRITE;
        view |= FILE_MAP_COPY;
    } else if(feval(access, RSCA::WriteOnly))
    {
        mapping |= PAGE_WRITECOPY;
        view |= FILE_MAP_WRITE;
    } else if(feval(access, RSCA::ReadOnly))
    {
        mapping |= PAGE_READONLY;
        view |= FILE_MAP_READ;
    }

    if(feval(access, RSCA::NoCache))
        mapping |= SEC_NOCACHE;
    if(feval(access, RSCA::HugeFile))
        mapping |= SEC_LARGE_PAGES;

    return {mapping, view};
}

std::pair<DWORD, DWORD> size_to_parts(szptr size)
{
    return {size & 0xFFFFFFFF, size >> 32};
}

result<mem_mapping_t, posix::posix_error> map(
    Url const& file, mapping_params_t params)
{
    if(detail::is_resource(file, params.access))
    {
        fprintf(stderr, "Mapping %s\n", file.internUrl.c_str());
        auto fd_ = open_file(file, params.access);
        if(!fd_.has_value())
        {
            fprintf(stderr, " - Failed opening\n");
            return posix::posix_error{EPERM};
        }
        auto fd       = std::move(fd_.value());
        auto map_size = params.size;
        if(map_size == 0)
            map_size = detail::resource_length(fd);
        auto data = detail::resource_data(fd);
        if(!data.has_value())
        {
            fprintf(stderr, " - Failed mapping\n");
            return posix::posix_error{ENOENT};
        }
        fprintf(stderr, " - Success\n");
        return mem_mapping_t{
            .view   = data.value().subspan(params.offset, map_size),
            .access = params.access,
            .resource = fd.resource,
        };
    }

    fprintf(stderr, "Win32 mapping %s\n", file.internUrl.c_str());
    auto file_name            = *file;
    auto [open, create]       = access_to_win32(params.access);
    constexpr auto share_mode = FILE_SHARE_READ;

    auto fd = CreateFile(
        file_name.c_str(), open, share_mode, nullptr, create, 0x0, nullptr);
    if(fd == INVALID_HANDLE_VALUE)
    {
        fprintf(
            stderr, "CreateFile: %s\n", platform::win32::last_error().c_str());
        return posix::posix_error{ENOENT};
    }

    auto [mapping_flags, view] = mapping_to_win32(params.access);
    auto [size_lo, size_hi]    = size_to_parts(params.size);
    auto mapping               = CreateFileMapping(
        fd, nullptr, mapping_flags, size_hi, size_lo, nullptr);
    if(mapping == INVALID_HANDLE_VALUE)
    {
        fprintf(
            stderr,
            "CreateFileMapping: %s\n",
            platform::win32::last_error().c_str());
        return posix::posix_error{EPERM};
    }

    auto [off_lo, off_hi] = size_to_parts(params.offset);
    auto pointer = MapViewOfFile(mapping, view, off_hi, off_lo, params.size);
    if(!pointer)
    {
        CloseHandle(fd);
        CloseHandle(mapping);
        fprintf(
            stderr,
            "MapViewOfFile: %s\n",
            platform::win32::last_error().c_str());
        return posix::posix_error{EPERM};
    }
    return mem_mapping_t{
        .file    = fd,
        .mapping = mapping,
        .view    = mem_mapping_t::span_type(
            reinterpret_cast<char*>(pointer), params.size),
        .access = params.access,
    };
}

std::optional<posix::posix_error> unmap(mem_mapping_t&& mapping)
{
    if(mapping.resource)
        return std::nullopt;

    UnmapViewOfFile(mapping.view.data());
    CloseHandle(mapping.file);
    CloseHandle(mapping.mapping);
    return std::nullopt;
}

} // namespace platform::file::win32
