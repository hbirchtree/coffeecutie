#pragma once

#include "../iterator.h"
#include "../quick_container.h"

#include <peripherals/libc/types.h>

#include <iterator>
#include <string>
#include <string_view>

namespace stl_types::str::split {

template<typename CharType>
struct spliterator
    : Iterator<std::forward_iterator_tag, std::basic_string_view<CharType>>
{
    using string_type = std::basic_string_view<CharType>;
    using sep_type    = CharType;

    spliterator() : source(), sep(sep_type()), idx(string_type::npos)
    {
    }

    spliterator(string_type const& source, sep_type sep) :
        source(source), sep(sep), idx(0)
    {
    }

    [[nodiscard]] spliterator operator++(int) const
    {
        auto cpy = *this;
        cpy.idx  = next_idx();

        if(cpy.idx > source.size())
            cpy.idx = string_type::npos;

        return cpy;
    }

    spliterator& operator++()
    {
        idx = next_idx();

        if(idx > source.size())
            idx = string_type::npos;

        return *this;
    }

    string_type operator*() const
    {
        libc_types::u8 adjustment = idx != 0 ? 1 : 0;
        return source.substr(idx + adjustment, len() - adjustment);
    }

    bool operator!=(spliterator const& other) const
    {
        return idx != other.idx;
    }

    operator string_type() const
    {
        return *(*this);
    }

  private:
    typename string_type::size_type len() const
    {
        return next_idx() - idx;
    }

    typename string_type::size_type next_idx() const
    {
        auto next = source.find(sep, idx + 1);
        return next;
    }

    string_type                     source;
    sep_type                        sep;
    typename string_type::size_type idx;
};

template<typename CharType>
FORCEDINLINE quick_container<spliterator<CharType>> str(
    std::basic_string_view<CharType> const& source, CharType sep)
{
    using str_type = std::basic_string_view<CharType>;

    auto it = source.find(sep);

    if(it == str_type::npos)
        return quick_container<spliterator<CharType>>(
            []() { return spliterator<CharType>(); },
            []() { return spliterator<CharType>(); });

    return quick_container<spliterator<CharType>>(
        [source, sep]() { return spliterator<CharType>(source, sep); },
        []() { return spliterator<CharType>(); });
}

}
