#pragma once

#include <peripherals/error/result.h>

#include <codecvt>
#include <locale>

#include "blam_magic_data.h"
#include "blam_reflexive.h"

namespace blam {

using stl_types::result;

using ucs_string = std::basic_string_view<char16_t>;

struct string_segment_ref
{
    struct string_ref
    {
        std::string_view data;
        u32              offset;

        inline std::string_view str() const
        {
            return data;
        }

        inline operator std::string_view() const
        {
            return str();
        }
    };

    std::string_view data;

    inline string_ref at(u32 offset = 0) const
    {
        if(offset > data.size())
            Throw(std::out_of_range("outside string segment"));

        auto end = data.find('\0', offset);
        auto out = data.substr(offset, end - offset);

        if(!out.size())
            Throw(undefined_behavior("string out of bounds"));
        return {out, offset};
    }

    inline string_ref indexed(u32 i = 0) const
    {
        auto loc = data;

        u32 s_i = 0;
        while(s_i != i)
        {
            loc = loc.substr(loc.find('\0'));
            if(loc.size())
                loc = loc.substr(1);
            s_i++;
        }

        return {loc, C_FCAST<u32>(loc.data() - data.data())};
    }

    inline u32 get_index(bl_string const& str) const
    {
        std::array<char, bl_string::size + 1> search;
        search.fill(0);

        u32 num_chars = 2;
        for(size_t i = 0; i < bl_string::size; i++)
        {
            if(str.data[i] == 0)
                break;
            search[1 + i] = str.data[i];
            num_chars++;
        }

#if defined(COFFEE_WINDOWS)
        const char* ptr = data.data;
#else
        const char* ptr = C_RCAST<const char*>(
            ::memmem(data.data(), data.size(), search.data(), num_chars));
#endif

        if(!ptr)
            return 0;

        ptr++;

        return ptr - data.data();
    }
};

template<size_t Size>
struct unicode_var
{
    using value_type                       = ucs_string::value_type;
    static constexpr ucs_string terminator = u"\0";

    static_assert(sizeof(value_type) == 2, "size of char16_t unexpected");
    static_assert(sizeof(u"\0") == 4, "size of terminator wrong");

    value_type data[Size];

    inline ucs_string view(u32 len, u16 off = 0) const
    {
        return ucs_string(data + off, len);
    }

    inline ucs_string str(u16 off = 0) const
    {
        auto string_data = ucs_string(data + off, 256);
        auto res         = string_data.find(terminator);
        return ucs_string(data + off, res != ucs_string::npos ? res : 0);
    }

    inline std::string flat_str(u16 /*off*/ = 0) const
    {
        std::wstring_convert<std::codecvt_utf8<value_type>> converter;
        return converter.to_bytes(str());
    }
};

struct alignas(4) string_ref
{
    u32 offset;

    inline std::string_view to_string(magic_data_t const& magic) const
    {
        using namespace std::string_view_literals;
        if(offset == 0)
            return "[empty string]"sv;

        return C_RCAST<cstring>(magic.base_ptr + offset - magic.magic_offset);
    }
};

struct unicode_reflexive
{
    reflexive_t<char16_t> data;

    inline result<ucs_string, error_msg> str(magic_data_t const& magic) const
    {
        if(auto seg = data.data(magic); seg.has_error())
            return seg.error();
        else
        {
            return ucs_string(seg.value().data());
        }
    }
};

} // namespace blam
