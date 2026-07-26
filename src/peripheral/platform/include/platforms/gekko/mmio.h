#pragma once

#include "../base/rdwrio.h"
#include "fsio.h"

#include <gsl/span>

#include <stdio.h>
#include <stdlib.h>

// There is no storage to memory-map on the Gamecube/Wii, so "mapping" a file
// simply reads its contents into a heap buffer. unmap() frees that buffer.
// Write-back mappings are not persisted - the platform has no writable medium
// in the general case - but reading files into memory works as expected.

namespace platform::file::gekko {

using gekko_error = int;

struct mem_mapping_t
{
    using span_type = gsl::span<char>;

    span_type view;
    RSCA      access;

    operator span_type() const
    {
        return view;
    }
};

struct mapping_params_t
{
    RSCA              access{RSCA::ReadOnly};
    libc_types::szptr offset{0};
    libc_types::szptr size{0};
};

FORCEDINLINE result<mem_mapping_t, gekko_error> map(
    Url const& file, mapping_params_t params)
{
    auto resolved = *file;

    if(params.size == 0)
    {
        if(auto info = file_info(file); info.has_value())
            params.size = info.value().size;
        else
            return failure(info.error());
    }

    auto* buffer = C_RCAST<char*>(::malloc(params.size));
    if(!buffer)
        return failure(ENOMEM);

    if(auto* fd = ::fopen(resolved.c_str(), "rb"))
    {
        if(params.offset != 0)
            ::fseek(fd, params.offset, SEEK_SET);
        auto read = ::fread(buffer, 1, params.size, fd);
        ::fclose(fd);
        // Zero-fill any tail we could not read (e.g. fresh write mapping).
        if(read < params.size)
            __builtin_memset(buffer + read, 0, params.size - read);
    } else
    {
        __builtin_memset(buffer, 0, params.size);
    }

    return success(
        mem_mapping_t{
            .view   = mem_mapping_t::span_type(buffer, params.size),
            .access = params.access,
        });
}

FORCEDINLINE std::optional<gekko_error> unmap(mem_mapping_t&& mapping)
{
    if(mapping.view.data())
        ::free(mapping.view.data());
    mapping.view = mem_mapping_t::span_type();
    return std::nullopt;
}

} // namespace platform::file::gekko
