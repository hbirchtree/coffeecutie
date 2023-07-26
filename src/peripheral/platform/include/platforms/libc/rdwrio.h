#pragma once

#include "fsio.h"

#include <peripherals/stl/string/split.h>
#include <platforms/base/rdwrio.h>
#include <url/url.h>

namespace platform::file::libc {
using url::Url;

namespace detail {

FORCEDINLINE bool requires_delimiters(std::string const& filename)
{
    return filename.starts_with("/proc/");
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

struct line_iterator
{
    line_iterator(file_t&& fd) : fd(std::make_shared<file_t>(std::move(fd)))
    {
    }
    line_iterator()
    {
    }

    inline line_iterator& operator++()
    {
        advance();
        return *this;
    }
    inline line_iterator operator++(int) const
    {
        auto cpy = *this;
        cpy.advance();
        return cpy;
    }

    inline bool empty() const
    {
        return feof(*fd);
    }

    inline std::string_view operator*() const
    {
        return std::string_view(current_line.data(), current_line.size());
    }

  private:
    inline void advance()
    {
        char*  line = nullptr;
        size_t size = 0;
        if(auto len = getdelim(&line, &size, '\n', *fd); len != -1)
        {
            current_line.insert(current_line.begin(), line, line + len);
            current_line.resize(len);
        } else
            current_line = {};
        ::free(line);
        if(current_line.starts_with('\n'))
            current_line = current_line.substr(1);
        current_line = current_line.substr(0, current_line.find('\n'));
    }

    std::shared_ptr<file_t> fd;
    std::string        current_line;
};

FORCEDINLINE result<line_iterator, int> read_lines(Url const& file)
{
    using stl_types::str::split::spliterator;

    if(auto fd = open_file(file); fd.has_error())
        return failure(fd.error());
    else
        return success(line_iterator(std::move(fd.value())));
}

FORCEDINLINE std::optional<int> write(
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
