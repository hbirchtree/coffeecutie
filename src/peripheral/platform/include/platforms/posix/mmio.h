#pragma once

#include "detail.h"
#include "platforms/base/rdwrio.h"
#include "rdwrio.h"

#include <gsl/span>

#include <sys/stat.h>

namespace platform::file::posix {

struct mem_mapping_t
{
    using span_type = gsl::span<char>;

    span_type view;
    RSCA      access;

    operator posix::mem_mapping_t::span_type() const
    {
        return view;
    }
};
struct mapping_params_t
{
    RSCA  access{RSCA::ReadOnly};
    szptr offset{0};
    szptr size{0};
};

FORCEDINLINE result<mem_mapping_t, posix_error> map(
    Url const& file, mapping_params_t params)
{
    auto       resolved_filename = *file;
    int        mode              = (params.access & RSCA::NewFile) != RSCA::None
                                       ? S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
                                       : 0;
    posix_fd_t fd(
        open(resolved_filename.c_str(), detail::openmode(params.access), mode));

    if(fd < 0)
    {
        return failure(common::posix::get_error());
    }
    if(params.size == 0)
    {
        if(auto info = file_info(fd); info.has_value())
            params.size = info.value().size;
        else
            return failure(info.error());
    }

    void* mapping = detail::mmap_(
        nullptr,
        params.size,
        detail::protection(params.access),
        detail::mapping(params.access),
        fd,
        detail::align_offset(params.offset));

    if(mapping == MAP_FAILED)
    {
        return failure(common::posix::get_error());
    }

    return success(mem_mapping_t{
        .view = mem_mapping_t::span_type(C_RCAST<char*>(mapping), params.size),
        .access = params.access,
    });
}

FORCEDINLINE std::optional<posix_error> unmap(mem_mapping_t&& mapping)
{
    if(auto& view = mapping.view; munmap(view.data(), view.size()) != 0)
    {
        return common::posix::get_error();
    }
    return std::nullopt;
}

} // namespace platform::file::posix
