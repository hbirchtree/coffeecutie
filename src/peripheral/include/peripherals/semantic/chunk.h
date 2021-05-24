#pragma once

#include <peripherals/identify/identify.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>

#include <gsl/span>

namespace semantic {

using namespace ::type_safety;

#define BYTE_API "mem_chunk<T>::"

enum class Ownership
{
    Owned,
    Borrowed,
};
template<typename T>
struct mem_chunk
{
    using size_type       = libc_types::szptr;
    using difference_type = libc_types::ptroff;
    using value_type      = typename std::remove_cv<T>::type;
    using span_type       = gsl::span<T, gsl::dynamic_extent>;
    using allocation_type = std::vector<value_type>;
    using iterator        = typename span_type::iterator;

    static constexpr RSCA type_access =
        std::is_const_v<T> ? RSCA::ReadOnly : RSCA::ReadWrite;

    /*
     * The legacy stuff
     */
    C_DEPRECATED_S("use of()")
    NO_DISCARD STATICINLINE mem_chunk From(T& obj)
    {
        return of(obj);
    }
    C_DEPRECATED_S("use ofBytes()")
    NO_DISCARD STATICINLINE mem_chunk FromBytes(void* data, size_type size)
    {
        return of(C_RCAST<T*>(data), size / sizeof(T));
    }
    C_DEPRECATED_S("use ofBytes()")
    NO_DISCARD STATICINLINE mem_chunk
                            FromBytes(const void* data, size_type size)
    {
        return of(C_RCAST<T*>(data), size / sizeof(T));
    }
    template<typename SizeT>
    C_DEPRECATED_S("use withSize()")
    NO_DISCARD STATICINLINE mem_chunk Alloc(SizeT size)
    {
        return withSize(size / sizeof(T));
    }
    C_DEPRECATED_S("... oh no")
    STATICINLINE void SetDestr(
        mem_chunk&, stl_types::Function<void(mem_chunk&)>&&)
    {
    }

    /*
     * The new shiny
     */
    STATICINLINE mem_chunk of(T& obj)
    {
        mem_chunk out = {
            .view = span_type(&obj, 1),
        };
        out.updatePointers();
        return out;
    }
    STATICINLINE mem_chunk of(T* obj, size_type size)
    {
        mem_chunk out = {
            .view = span_type(obj, size),
        };
        out.updatePointers();
        return out;
    }
    template<
        typename OtherT,
        typename std::enable_if<
            std::is_same_v<typename OtherT::value_type, value_type>,
            OtherT>::type* = nullptr>
    STATICINLINE mem_chunk ofContainer(OtherT& obj)
    {
        mem_chunk out = {
            .view = span_type(obj),
        };
        out.updatePointers();
        return out;
    }
    template<
        typename OtherT,
        typename std::enable_if<
            !std::is_same_v<typename OtherT::value_type, value_type>,
            OtherT>::type* = nullptr>
    STATICINLINE mem_chunk ofContainer(OtherT& obj)
    {
        mem_chunk out = {
            .view = span_type(C_RCAST<T*>(obj.data()), obj.size()),
        };
        out.updatePointers();
        return out;
    }
    template<typename OtherT>
    STATICINLINE mem_chunk ofBytes(OtherT* data, size_type size)
    {
        return of(C_RCAST<T*>(data), (size * sizeof(OtherT)) / sizeof(T));
    }
    template<typename OtherT>
    STATICINLINE mem_chunk ofBytes(OtherT& object)
    {
        return of(C_RCAST<T*>(&object), sizeof(OtherT));
    }
    template<
        typename Dummy                                            = void,
        typename std::enable_if<std::is_const_v<T>, Dummy>::type* = nullptr>
    STATICINLINE mem_chunk ofString(const char* data)
    {
        return ofBytes(data, libc::str::len(data));
    }
    template<typename CharType>
    STATICINLINE mem_chunk ofString(std::basic_string<CharType> data)
    {
        return ofBytes(data.data(), data.size());
    }
    template<typename CharType>
    STATICINLINE mem_chunk ofString(std::basic_string_view<CharType> data)
    {
        return ofBytes(data.data(), data.size());
    }
    NO_DISCARD STATICINLINE mem_chunk withSize(size_type size)
    {
        mem_chunk out;
        out.allocation = allocation_type(size * sizeof(T), value_type{});
        T* data_ptr    = out.allocation.data();
        out.view       = span_type(data_ptr, out.allocation.size());
        out.updatePointers(Ownership::Owned);
        return out;
    }

    /*
     * Member functions
     */
    template<typename OtherT>
    NO_DISCARD operator mem_chunk<OtherT>()
    {
        return mem_chunk<OtherT>::ofBytes(view.data(), view.size());
    }
    template<typename OtherT>
    NO_DISCARD operator mem_chunk<OtherT>() const
    {
        return mem_chunk<OtherT>::ofBytes(view.data(), view.size());
    }
    template<typename OtherT>
    operator gsl::span<OtherT, gsl::dynamic_extent>()
    {
        return as<OtherT>().view;
    }
    template<typename OtherT>
    operator gsl::span<OtherT, gsl::dynamic_extent>() const
    {
        return as<OtherT>().view;
    }

    operator bool() const
    {
        return view.data();
    }
    bool operator!() const
    {
        return !view.data();
    }

    auto& operator[](size_type i)
    {
        return view[i];
    }
    auto& operator[](size_type i) const
    {
        return view[i];
    }
    auto begin()
    {
        return view.begin();
    }
    auto end()
    {
        return view.end();
    }
    auto begin() const
    {
        return view.begin();
    }
    auto end() const
    {
        return view.end();
    }

    struct find_result
    {
        size_type offset{0};
    };

    std::optional<find_result> find(mem_chunk const& needle) const
    {
        stl_types::Optional<find_result> out;

        if(needle.empty())
            return std::nullopt;

        size_type needle_idx = 0;

        for(size_type i = 0; i < size; i++)
        {
            if(data[i] == needle[needle_idx])
            {
                needle_idx++;
                if(needle_idx == needle.elements)
                {
                    auto offset = i - (needle_idx - 1);
                    auto chunk =
                        at((i - (needle_idx - 1)) *
                           sizeof(value_type)).value();
                    return std::make_optional(find_result{offset});
                }
            } else
                needle_idx = 0;
        }

        return std::nullopt;
    }

    auto empty() const
    {
        return view.size() == 0;
    }

    NO_DISCARD auto at(size_type offset, size_type size = 0)
    {
        auto      translated_size = size == 0 ? gsl::dynamic_extent : size;
        mem_chunk out             = {
            .view = view.subspan(offset, translated_size),
        };
        out.updatePointers(Ownership::Borrowed, access);
        return std::make_optional(out);
    }
    NO_DISCARD auto at(size_type offset, size_type size = 0) const
    {
        auto      translated_size = size == 0 ? gsl::dynamic_extent : size;
        mem_chunk out             = {
            .view = view.subspan(offset, translated_size),
        };
        out.updatePointers(Ownership::Borrowed, access);
        return std::make_optional(out);
    }
    template<typename OtherT>
    NO_DISCARD auto as()
    {
        return C_OCAST<mem_chunk<OtherT>>(*this);
    }
    template<typename OtherT>
    NO_DISCARD auto as() const
    {
        return C_OCAST<mem_chunk<OtherT>>(*this);
    }

    /* Allocator methods */
    auto insert(iterator it, T&& value)
    {
        if(ownership != Ownership::Owned)
            Throw(undefined_behavior("cannot modify borrowed structure"));
        auto idx = it - view.begin();
        return allocation.insert(allocation.begin() + idx, std::move(value));
    }
    void resize(size_type new_size)
    {
        if(ownership != Ownership::Owned)
            Throw(undefined_behavior("cannot modify borrowed structure"));
        allocation.resize(new_size);
    }

    span_type       view;
    allocation_type allocation;

    T*        data{nullptr};
    size_type size{0};
    size_type elements{0};
    RSCA      access{RSCA::None};
    Ownership ownership{Ownership::Borrowed};

    void assignAccess(RSCA acc)
    {
        access = acc;
    }

    void updatePointers(
        Ownership ownership = Ownership::Borrowed, RSCA access = type_access)
    {
        this->ownership = ownership;
        this->access    = access;

        switch(ownership)
        {
        case Ownership::Owned:
            data     = C_RCAST<T*>(allocation.data());
            size     = allocation.size() * sizeof(T);
            elements = allocation.size();
            view     = span_type(data, allocation.size());
            break;
        case Ownership::Borrowed:
            data     = view.data();
            size     = view.size();
            elements = view.size() * sizeof(T);
            break;
        }
    }
}; // namespace semantic

#undef BYTE_API

using Bytes      = mem_chunk<libc_types::u8>;
using BytesConst = mem_chunk<const libc_types::u8>;

template<typename T, std::size_t Size = gsl::dynamic_extent>
using Span = gsl::span<T, Size>;

template<typename T>
FORCEDINLINE auto SpanOne(T const& value)
{
    return Span<const T, 1>(&value, 1);
}
template<typename T>
FORCEDINLINE auto SpanOne(T& value)
{
    return Span<T, 1>(&value, 1);
}
template<typename T, typename U>
FORCEDINLINE auto SpanOne(U& value)
{
    return mem_chunk<T>::ofBytes(value).view;
}
template<typename T, typename U>
FORCEDINLINE auto SpanOne(U const& value)
{
    return mem_chunk<T>::ofBytes(value).view;
}

} // namespace semantic
