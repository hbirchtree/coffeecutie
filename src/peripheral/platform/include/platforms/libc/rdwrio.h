#pragma once

#include "fsio.h"

#include <peripherals/stl/string_ops.h>
#include <platforms/base/rdwrio.h>
#include <url/url.h>

namespace platform::file::libc {
using url::Url;

namespace detail {

FORCEDINLINE bool requires_delimiters(stl_types::String const& filename)
{
    return stl_types::str::find::starts_with<char>(filename.c_str(), "/proc/");
}

} // namespace detail

FORCEDINLINE result<mem_chunk<char>, int> read(
    file_t const& file, read_params_t const& params = {})
{
    auto [size, offset] = params;
    if(size == 0)
    {
        if(auto size_ = libc::size(file); size_.has_error())
            return failure(size_.error());
        else
            size = size_.value();
    }
    auto  out  = mem_chunk<char>::withSize(size);
    auto& view = out.view;
    if(offset != 0)
    {
        if(fseek(file, offset, SEEK_SET) != 0)
            return failure(ferror(file));
    }
    if(auto read = fread(out.data, 1, out.size, file);
       read != size && !feof(file))
        return failure(ferror(file));
    else
        return success(std::move(out));
}

FORCEDINLINE result<stl_types::Vector<stl_types::String>, int> read_lines(
    Url const& file)
{
    using stl_types::String;
    using stl_types::str::split::spliterator;

    if(auto fd = open_file(file); fd.has_error())
        return failure(fd.error());
    else
    {
        stl_types::Vector<String> out;
        if(detail::requires_delimiters(*file))
        {
            char*  arg  = nullptr;
            size_t size = 0;
            while(getdelim(&arg, &size, '\n', fd.value()) != -1)
                out.push_back(String(arg));
        } else
        {
            if(auto data = read(fd.value()); data.has_error())
                return failure(data.error());
            else
            {
                auto buffer = std::move(data.value());
                auto it =
                    spliterator<char>(std::string_view(buffer.data), '\n');
                do
                    out.push_back(String((*it).data(), (*it).size()));
                while(it++ != spliterator<char>());
            }
        }
        return success(out);
    }
}

FORCEDINLINE Optional<int> write(
    file_t const&                file,
    mem_chunk<const char> const& data,
    write_params_t const&        params = {})
{
    if(params.offset != 0)
    {
        if(fseek(file, params.offset, SEEK_SET))
            return ferror(file);
    }
    if(auto written = fwrite(data.data, 1, data.size, file);
       written != data.size)
        return ferror(file);
    return std::nullopt;
}

} // namespace platform::file::libc
