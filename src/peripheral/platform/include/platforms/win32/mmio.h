#pragma once

#include "../posix/detail.h"

#include <gsl/span>
#include <peripherals/semantic/enum/rsca.h>
#include <url/url.h>

namespace platform::file::win32 {

using platform::url::Url;

struct mem_mapping_t
{
    using span_type = gsl::span<char>;
    enum class mapping_source
    {
        file,
        resource,
    };

    void*          file{nullptr};
    void*          mapping{nullptr};
    span_type      view;
    semantic::RSCA access;
    mapping_source source{mapping_source::file};

    operator span_type() const
    {
        return view;
    }
};

struct mapping_params_t
{
    semantic::RSCA    access{semantic::RSCA::ReadOnly};
    libc_types::szptr offset{0};
    libc_types::szptr size{0};
};

result<mem_mapping_t, posix::posix_error> map(
    Url const& file, mapping_params_t params);

std::optional<posix::posix_error> unmap(mem_mapping_t&& mapping);

} // namespace platform::file::win32
