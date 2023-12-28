#pragma once

#include <peripherals/identify/identify.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>

#include <gsl/span>

namespace semantic {
namespace detail {

template<typename T>
requires(!std::is_same_v<std::decay_t<T>, void>)
    //
    constexpr inline size_t size_of_type()
{
    return sizeof(T);
}
template<typename T>
requires(std::is_same_v<std::decay_t<T>, void>)
    //
    constexpr inline size_t size_of_type()
{
    return 1;
}

} // namespace detail

using namespace ::type_safety;

#define BYTE_API "mem_chunk<T>::"

enum class Ownership
{
    Owned,
    Borrowed,
};
template<typename T = char>
struct mem_chunk
{
    using difference_type  = libc_types::ptroff;
    using value_type       = T;
    using value_type_alloc = typename std::decay_t<T>;
    using span_type        = gsl::span<T, gsl::dynamic_extent>;
    using size_type        = typename span_type::size_type;
    using allocation_type  = std::vector<value_type_alloc>;
    using iterator         = typename span_type::iterator;

    static constexpr RSCA type_access
        = std::is_const_v<T> ? RSCA::ReadOnly : RSCA::ReadWrite;

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
    template<typename OtherT>
    requires(std::is_same_v<typename OtherT::value_type, T>)
        //
        STATICINLINE mem_chunk ofContainer(OtherT& obj)
    {
        mem_chunk out = {
            .view = span_type(obj.data(), obj.size()),
        };
        out.updatePointers();
        return out;
    }
    template<typename OtherT>
    requires(!std::is_same_v<typename OtherT::value_type, value_type>)
        //
        STATICINLINE mem_chunk ofContainer(OtherT& obj)
    {
        using value_type_bare
            = std::remove_const_t<typename OtherT::value_type>;

        mem_chunk out = {
            .view = span_type(
                C_RCAST<T*>(const_cast<value_type_bare*>(obj.data())),
                (obj.size() * sizeof(typename OtherT::value_type))
                    / sizeof(value_type)),
        };
        out.updatePointers();
        return out;
    }
    template<typename OtherT>
    STATICINLINE mem_chunk ofBytes(OtherT* data, size_type size)
    {
        return of(
            C_RCAST<T*>(data),
            (size * detail::size_of_type<OtherT>()) / sizeof(T));
    }
    template<typename OtherT>
    STATICINLINE mem_chunk ofBytes(OtherT& object)
    {
        return of(
            C_RCAST<T*>(&object), detail::size_of_type<OtherT>() / sizeof(T));
    }
    template<typename Dummy = void>
    requires std::is_const_v<T>
        //
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
        out.updatePointers(Ownership::Owned);
        return out;
    }
    template<class Container>
    NO_DISCARD STATICINLINE mem_chunk copy(Container& c)
    {
        mem_chunk out;
        out.allocation = allocation_type(std::begin(c), std::end(c));
        out.updatePointers(Ownership::Owned);
        return out;
    }

    template<class Container>
    requires(!std::is_same_v<Container, allocation_type>)
        NO_DISCARD STATICINLINE mem_chunk move(Container const& c)
    {
        /* If the container_type is not the same, we need to copy */
        mem_chunk out;
        std::move(
            std::begin(c),
            std::end(c),
            std::back_insert_iterator(out.allocation));
        out.updatePointers(Ownership::Owned);
        return out;
    }
    template<class Container>
    requires(std::is_same_v<typename Container::value_type, value_type>)
        NO_DISCARD STATICINLINE mem_chunk move(Container&& c)
    {
        /* If the container_type is the same, steal the data */
        mem_chunk out;
        out.allocation = std::move(c);
        out.updatePointers(Ownership::Owned);
        return out;
    }

    /*
     * Member functions
     */
    template<typename OtherT>
    requires(std::is_same_v<OtherT, T>)
    //
    operator gsl::span<OtherT, gsl::dynamic_extent>()
    {
        return view;
    }
    template<typename OtherT>
    requires(std::is_same_v<OtherT, T>)
    //
    operator gsl::span<OtherT, gsl::dynamic_extent>() const
    {
        return view;
    }

    template<typename OtherT>
    requires(!std::is_same_v<OtherT, T>)
    //
    operator gsl::span<OtherT, gsl::dynamic_extent>()
    {
        return as<OtherT>().view;
    }
    template<typename OtherT>
    requires(!std::is_same_v<OtherT, T>)
    //
    operator gsl::span<OtherT, gsl::dynamic_extent>() const
    {
        return as<OtherT>().view;
    }
    template<typename OtherT>
    requires(!std::is_same_v<OtherT, T>)
    //
    operator mem_chunk<OtherT>() const
    {
        return as<OtherT>();
    }

    explicit operator bool() const
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

    typename span_type::iterator find(mem_chunk const& needle) const
    {
        if(needle.empty())
            return view.end();

        size_type needle_idx = 0;

        for(size_type i = 0; i < size; i++)
        {
            if(data[i] == needle[needle_idx])
            {
                needle_idx++;
                if(needle_idx == needle.elements)
                {
                    auto offset = i - (needle_idx - 1);
                    auto chunk = at((i - (needle_idx - 1)) * sizeof(value_type))
                                     .value();
                    return view.begin() + offset;
                }
            } else
                needle_idx = 0;
        }

        return view.end();
    }

    auto empty() const
    {
        return view.size() == 0;
    }

    NO_DISCARD auto at(size_type offset, size_type size = 0)
    {
        auto      translated_size = size == 0 ? view.size() - offset : size;
        mem_chunk out             = {
                        .view = view.subspan(offset, translated_size),
        };
        out.updatePointers(Ownership::Borrowed, access);
        return std::make_optional(std::move(out));
    }
    NO_DISCARD auto at(size_type offset, size_type size = 0) const
    {
        auto      translated_size = size == 0 ? view.size() - offset : size;
        mem_chunk out             = {
                        .view = view.subspan(offset, translated_size),
        };
        out.updatePointers(Ownership::Borrowed, access);
        return std::make_optional(std::move(out));
    }
    template<typename OtherT>
    requires(!std::is_same_v<T, OtherT>)
        //
        NO_DISCARD auto as()
    {
        return mem_chunk<OtherT>::ofBytes(data, size);
    }
    template<typename OtherT>
    requires(!std::is_same_v<T, OtherT>)
        //
        NO_DISCARD auto as() const
    {
        return mem_chunk<OtherT>::ofBytes(data, size);
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
        updatePointers(Ownership::Owned);
    }

    span_type       view;
    allocation_type allocation{};

    T*        data{nullptr};
    size_type size{0};
    size_type elements{0};
    RSCA      access{RSCA::None};
    Ownership ownership{Ownership::Borrowed};

    stl_types::non_copy move_sentry{};

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
};

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

template<typename T, typename It>
FORCEDINLINE auto SpanOver(It begin, It end)
{
    auto ptrBegin = &(*begin), ptrEnd = &(*end);
    return Span<T>(ptrBegin, ptrEnd);
}

template<typename T>
FORCEDINLINE auto SpanOver(T& container)
{
    return SpanOver<typename T::value_type, typename T::iterator>(
        std::begin(container), std::end(container));
}

} // namespace semantic
