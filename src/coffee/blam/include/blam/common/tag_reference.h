#pragma once

#include "block_layout.h"
#include "tag_class.h"
#include "tag_index.h"

#include <array>

namespace blam {

template<typename Layout, tag_class_t... Classes>
struct tagref_storage;

/*! Halo 2: the class repeated in front of the handle */
template<tag_class_t... Classes>
struct tagref_storage<layout::dimeter, Classes...>
{
    basic_tag_class<std::endian::little>               tag_class;
    basic_tag_index<std::endian::little, Classes...> tag_id;
};

/*! Halo 3: class, two runtime words and the handle, big-endian */
template<tag_class_t... Classes>
struct tagref_storage<layout::elegy, Classes...>
{
    basic_tag_class<std::endian::big>               tag_class;
    std::array<libc_types::u32, 2>                  runtime;
    basic_tag_index<std::endian::big, Classes...> tag_id;
};

/*!
 * \brief Reference to another tag, resolved through the tag table by its
 * handle. The classes say which tags the field may name; none admits any,
 * and a typed reference converts to the untyped one.
 */
template<typename V, tag_class_t... Classes>
struct alignas(4) tag_reference
    : tagref_storage<typename V::block_layout, Classes...>
{
    inline tag_class_t class_id() const
    {
        return this->tag_class.id();
    }

    inline bool valid() const
    {
        return this->tag_id.valid() && class_id() != tag_class_t::none;
    }

    inline bool matches(tag_class_t cls) const
    {
        return class_id() == cls;
    }

    /*! Whether the tag named is of a class the field admits */
    inline bool admitted() const
    {
        if constexpr(sizeof...(Classes) == 0)
            return true;
        else
            return ((class_id() == Classes) || ...);
    }

    /*! Halo 1 references carry the tag's name */
    inline auto const& to_name() const
        requires requires(tag_reference const& ref) { ref.name; }
    {
        return this->name;
    }

    inline fourcc_t tag_class_name() const
    {
        return this->tag_class.str();
    }

    inline tag_reference<V> const& untyped() const
    {
        return *reinterpret_cast<tag_reference<V> const*>(this);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-conversion"

    template<typename Dummy = void>
    requires(sizeof...(Classes) > 0)
    inline operator tag_reference<V> const&() const
    {
        return untyped();
    }

#pragma GCC diagnostic pop
};

} // namespace blam
