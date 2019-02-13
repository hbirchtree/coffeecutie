#pragma once

#include <peripherals/identify/identify.h>
#include <peripherals/libc/string_ops.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/enum/rsca.h>
#include <peripherals/stl/types.h>

namespace semantic {

using namespace ::type_safety;

#define BYTE_API "mem_chunk<T>::"

template<typename T>
/*!
 * \brief Like std::vector<T>, but allows borrowing memory.
 * It is also compatible with STL algorithms.
 */
struct mem_chunk
{
    using size_type       = libc_types::szptr;
    using difference_type = libc_types::ptroff;
    /* Here's some iterator magic, for compatbility with STL containers */

    template<typename T_access = T>
    struct iterator_base : stl_types::Iterator<stl_types::ForwardIteratorTag, T>
    {
        using value_type      = T_access;
        using difference_type = typename mem_chunk<T_access>::difference_type;

        friend struct mem_chunk;

        iterator_base(mem_chunk<T>& chunk, size_type i = 0) :
            m_chunk(&chunk), m_idx(i), m_readonly(false)
        {
        }
        iterator_base(mem_chunk<T> const& chunk, size_type i = 0) :
            m_chunk(C_CCAST<mem_chunk<T>*>(&chunk)), m_idx(i), m_readonly(true)
        {
        }

        iterator_base& operator++()
        {
            m_idx++;
            return *this;
        }

        iterator_base& operator--()
        {
            m_idx--;
            return *this;
        }

        iterator_base operator+(difference_type inc)
        {
            auto copy = *this;
            copy.m_idx += inc;
            return *this;
        }

        iterator_base operator-(difference_type inc)
        {
            auto copy = *this;
            copy.m_idx -= inc;
            return *this;
        }

        iterator_base& operator+=(difference_type inc)
        {
            m_idx += inc;
            return *this;
        }

        iterator_base& operator-=(difference_type inc)
        {
            m_idx -= inc;
            return *this;
        }

        difference_type operator-(iterator_base<T_access> const& other)
        {
            return m_idx - other.m_idx;
        }

        bool operator==(iterator_base const& other) const
        {
            return other.m_idx == m_idx;
        }

        bool operator!=(iterator_base const& other) const
        {
            return other.m_idx != m_idx;
        }

        T_access& operator*() const
        {
#if MODE_DEBUG
            if(m_idx > m_chunk->elements)
                Throw(std::out_of_range("index out of bounds"));
#endif
            return m_chunk->data[m_idx];
        }

      private:
        mem_chunk*        m_chunk;
        libc_types::szptr m_idx;
        bool              m_readonly;
    };

    using iterator       = iterator_base<>;
    using const_iterator = iterator_base<const T>;

    using value_type = T;

    FORCEDINLINE
    mem_chunk(mem_chunk<T>&& other) :
        data(other.data), size(other.size), elements(other.elements),
        m_destr(other.m_destr)
    {
        other.data     = nullptr;
        other.size     = 0;
        other.elements = 0;
        other.m_destr  = nullptr;
    }

    FORCEDINLINE
    mem_chunk() : data(nullptr), size(0), elements(0)
    {
    }

    FORCEDINLINE
    mem_chunk(T* data, size_type size, size_type elements) :
        data(data), size(size), elements(elements)
    {
#if MODE_DEBUG
        if((size && !data) || (size && !elements) || (!size && data))
            Throw(implementation_error(BYTE_API "invalid construction"));
#endif
    }

    template<
        typename Dummy = void,
        typename std::enable_if<
            !std::is_void<typename std::remove_cv<T>::type>::value,
            Dummy>::type* = nullptr>
    FORCEDINLINE mem_chunk(T& value) :
        data(&value), size(sizeof(T)), elements(1)
    {
    }

    FORCEDINLINE
    mem_chunk(stl_types::Vector<T>& array) :
        data(array.data()), size(sizeof(T) * array.size()),
        elements(array.size())
    {
    }

    template<
        typename T2,
        typename std::enable_if<
            std::is_same<T2, typename std::remove_cv<T>::type>::value>::type* =
            nullptr>
    FORCEDINLINE mem_chunk(mem_chunk<T2> const& other) :
        data(C_RCAST<T*>(other.data)), size(other.size),
        elements(other.elements)
    {
    }

    ~mem_chunk()
    {
        if(m_destr)
            m_destr(*this);
    }

    template<typename T2>
    NO_DISCARD explicit operator mem_chunk<T2>()
    {
        mem_chunk<T2> out;

        out.data     = C_RCAST<T2*>(this->data);
        out.size     = this->size;
        out.elements = this->elements;

        return out;
    }

    C_DELETE_COPY_CONSTRUCTOR(mem_chunk);

    mem_chunk& operator=(mem_chunk&& other)
    {
        data     = other.data;
        elements = other.elements;
        size     = other.size;
        m_destr  = other.m_destr;

        other.data     = nullptr;
        other.elements = 0;
        other.size     = 0;
        other.m_destr  = nullptr;

        return *this;
    }

    STATICINLINE void SetDestr(mem_chunk<T>& inst, void (*d)(mem_chunk<T>&))
    {
#if MODE_DEBUG
        if(!d)
            Throw(implementation_error(BYTE_API "abuse of SetDestr()"));
#endif
        inst.m_destr = d;
    }

    template<
        typename SizeT,
        typename std::enable_if<std::is_unsigned<SizeT>::value, SizeT>::type* =
            nullptr>
    NO_DISCARD STATICINLINE
        /*!
         * \brief Standard memory allocation with RAII semantics.
         * All memory is initialized to 0.
         * \param num
         * \return
         */
        mem_chunk<T>
        Alloc(SizeT num)
    {
#if MODE_DEBUG
        if(num == 0)
            Throw(implementation_error(BYTE_API "allocating 0 bytes is bad"));
#endif
        mem_chunk<T> out;

        out.data = C_RCAST<T*>(calloc(num * sizeof(T), 1));

        if(!out.data)
            Throw(memory_error("failed to allocate memory"));

        out.size     = sizeof(T) * num;
        out.elements = num;

        mem_chunk<T>::SetDestr(out, [](mem_chunk<T>& d) { free(d.data); });

        return out;
    }

    template<typename T2>
    NO_DISCARD STATICINLINE
        /*!
         * \brief For cases where safety needs to be disabled,
         *  eg. passing symbolic values.
         * This is used when talking to OpenGL with offsets.
         * Objects created with this should not be used with iterators!
         *
         * \param data
         * \param size
         * \param elements
         * \return
         */
        mem_chunk<T>
        Unsafe(T2* data = nullptr, size_type size = 0, size_type elements = 0)
    {
        mem_chunk<T> out;

        out.data     = C_RCAST<T*>(data);
        out.size     = size;
        out.elements = elements;

        return out;
    }

    template<
        typename SizeT,
        typename std::enable_if<std::is_signed<SizeT>::value, SizeT>::type* =
            nullptr>
    NO_DISCARD STATICINLINE mem_chunk<T> Alloc(SizeT num)
    {
        return Alloc(C_FCAST<size_type>(num));
    }

    template<typename DT, typename is_not_virtual<DT>::type* = nullptr>
    NO_DISCARD STATICINLINE mem_chunk<T> Create(DT& obj)
    {
        return {C_FCAST<T*>(&obj), sizeof(DT), sizeof(DT) / sizeof(T)};
    }

    template<
        typename Dummy                                               = void,
        typename std::enable_if<std::is_pod<T>::value, Dummy>::type* = nullptr>
    NO_DISCARD STATICINLINE mem_chunk<T> CreateString(libc_types::cstring src)
    {
        auto len = libc::str::len(src);

        return {C_FCAST<T*>(src), len, len / sizeof(T)};
    }

    template<
        typename Dummy                                                = void,
        typename std::enable_if<!std::is_pod<T>::value, Dummy>::type* = nullptr>
    NO_DISCARD STATICINLINE mem_chunk<T> CreateString(libc_types::cstring)
    {
        return {};
    }

    template<typename T2, typename is_not_virtual<T2>::type* = nullptr>
    NO_DISCARD STATICINLINE mem_chunk<T> CreateFrom(stl_types::Vector<T2>& data)
    {
        return {C_RCAST<T*>(data.data()),
                data.size() * sizeof(T2),
                (data.size() * sizeof(T2)) / sizeof(T)};
    }

    template<
        typename T2,
        typename std::enable_if<
            !std::is_same<typename std::remove_cv<T2>::type, void>::value>::
            type* = nullptr>
    NO_DISCARD STATICINLINE
        /*!
         * \brief From non-void pointer to bytes, sized in elements of type T2
         * \param data
         * \param size
         * \return
         */
        mem_chunk<T>
        From(T2* data, size_type size)
    {
        return {C_FCAST<T*>(data),
                size * sizeof(T2),
                (size * sizeof(T2)) / sizeof(T)};
    }

    template<
        typename T2,
        typename std::enable_if<
            std::is_same<typename std::remove_cv<T2>::type, void>::value>::
            type* = nullptr>
    NO_DISCARD STATICINLINE
        /*!
         * \brief From a void-like pointer to bytes, size is in bytes
         * \param data
         * \param size
         * \return
         */
        mem_chunk<T>
        From(T2* data, size_type size)
    {
        return {C_FCAST<T*>(data), size, size / sizeof(T)};
    }

    template<typename T2, typename is_not_virtual<T2>::type* = nullptr>
    NO_DISCARD STATICINLINE mem_chunk<T> From(T2& data)
    {
        return Create(data);
    }

    template<
        typename T2,
        typename is_not_virtual<T2>::type* = nullptr,
        typename std::enable_if<
            std::is_same<T, typename std::remove_const<T2>::type>::value>::
            type* = nullptr>
    NO_DISCARD STATICINLINE
        /*!
         * \brief Copy data from vector of same type, copies by value
         * \param data
         * \return
         */
        mem_chunk<T>
        CopyFrom(stl_types::Vector<T2>& data)
    {
        static_assert(
            sizeof(T2) >= sizeof(T), BYTE_API "incompatible size to copy");

        using OutT = mem_chunk<T>;

        OutT out = OutT::Alloc((data.size() * sizeof(T2)) / sizeof(T));

        std::copy(data.begin(), data.end(), out.begin());

        return out;
    }

    template<
        typename T2,
        typename is_not_virtual<T2>::type* = nullptr,
        typename std::enable_if<
            !std::is_same<T, typename std::remove_const<T2>::type>::value>::
            type* = nullptr>
    NO_DISCARD STATICINLINE
        /*!
         * \brief Copy data from vector of different type, copies byte-wise.
         * Ignores move semantics.
         * \param data
         * \return
         */
        mem_chunk<T>
        CopyFrom(stl_types::Vector<T2>& data)
    {
        static_assert(
            sizeof(T2) >= sizeof(T), BYTE_API "incompatible size to copy");

        using OutT = mem_chunk<T>;

        OutT out = OutT::Alloc((data.size() * sizeof(T2)) / sizeof(T));

        memcpy(out.data, data.data(), data.size() * sizeof(T2));

        return out;
    }

    STATICINLINE size_type nmax(size_type v1, size_type v2)
    {
        if(v1 < v2)
            return v2;
        else
            return v1;
    }

    template<typename T2>
    NO_DISCARD STATICINLINE mem_chunk<T> Copy(T2 const& obj)
    {
        static_assert(
            sizeof(T2) >= sizeof(T), BYTE_API "incompatible size to copy");

        using OutT = mem_chunk<T>;

        OutT out = OutT::Alloc(sizeof(T2) / sizeof(T));

        T2* cpy = C_RCAST<T2*>(out.data);
        *cpy    = obj;

        return out;
    }

    NO_DISCARD STATICINLINE mem_chunk<T> Copy(mem_chunk<T> const& src)
    {
        using OutT = mem_chunk<T>;

        OutT out = OutT::Alloc(src.elements ? src.elements : src.size);

        std::copy(
            src.begin(),
            src.end(),
            std::insert_iterator<mem_chunk<T>>(out, out.begin()));

        return out;
    }

    template<
        typename Dummy = void,
        typename std::enable_if<!std::is_void<T>::value, Dummy>::type* =
            nullptr>
    T& operator[](size_type i)
    {
        return data[i];
    }
    template<
        typename Dummy = void,
        typename std::enable_if<!std::is_void<T>::value, Dummy>::type* =
            nullptr>
    T const& operator[](size_type i) const
    {
        return data[i];
    }

    template<typename VectorT>
    mem_chunk<T>& operator=(VectorT const& vec)
    {
        data     = C_FCAST<T*>(vec.data());
        size     = vec.size() * sizeof(typename VectorT::value_type);
        elements = vec.size();

        return *this;
    }

    template<typename T2>
    mem_chunk<T>& operator=(stl_types::Vector<T2> const& vec)
    {
        data     = C_FCAST<T*>(vec.data());
        size     = vec.size() * sizeof(T2);
        elements = vec.size();

        return *this;
    }

    FORCEDINLINE operator bool()
    {
        return data && size;
    }

    /*!
     * \brief Create a sub-view of the memory region,
     *  preferred way of doing offsets
     * \param offset
     * \param size
     * \return
     */
    NO_DISCARD FORCEDINLINE mem_chunk<T> at(
        size_type offset, size_type size = 0)
    {
        using namespace ::libc_types;

        if(offset > this->size || offset + size > this->size)
            return {};

        if(!data)
            return {};

        u8* basePtr = C_RCAST<u8*>(data);

        if(size == 0)
            size = this->size - offset;

        return From(&basePtr[offset], size);
    }

    template<typename T2>
    FORCEDINLINE mem_chunk<T2> as()
    {
        return mem_chunk<T2>(
            C_RCAST<T2*>(data),
            size * sizeof(T),
            (size * sizeof(T)) / sizeof(T2));
    }

    FORCEDINLINE mem_chunk<T>& resize(size_type newSize)
    {
        data = C_RCAST<T*>(realloc(data, newSize));

        if(!data)
            Throw(undefined_behavior(BYTE_API "reallocation failed"));

        size = newSize;

        return *this;
    }

    FORCEDINLINE mem_chunk<T>& assignAccess(RSCA acc)
    {
        m_access = acc;
        return *this;
    }

    /*!
     * \brief Remove ownership of memory.
     * All responsibility is left to the user.
     */
    void disown()
    {
        m_destr = nullptr;
    }

    NO_DISCARD iterator begin()
    {
#if MODE_DEBUG
        if(elements == 0)
            Throw(implementation_error(BYTE_API "no elements"));
#endif

        return iterator(*this);
    }

    NO_DISCARD iterator end()
    {
        return iterator(*this, elements);
    }

    NO_DISCARD const_iterator begin() const
    {
#if MODE_DEBUG
        if(elements == 0)
            Throw(implementation_error(BYTE_API "no elements"));
#endif

        return const_iterator(*this);
    }

    NO_DISCARD const_iterator end() const
    {
        return const_iterator(*this, elements);
    }

    iterator insert(iterator it, T&& value)
    {
#if MODE_DEBUG
        if(it.m_idx >= elements)
            Throw(std::out_of_range("iterator out of bounds"));
#endif

        data[it.m_idx] = std::move(value);
        return it;
    }

    iterator insert(iterator it, T const& value)
    {
#if MODE_DEBUG
        if(it.m_idx >= elements)
            Throw(std::out_of_range("iterator out of bounds"));
#endif

        data[it.m_idx] = value;
        return it;
    }

    /*!
     * \brief Pointer to data
     */
    T* data;
    /*!
     * \brief Size in bytes
     */
    size_type size;
    /*!
     * \brief Number of elements, if applicable
     */
    size_type elements;

  private:
    void (*m_destr)(mem_chunk<T>&) = nullptr;
    RSCA m_access;
};

#undef BYTE_API

using Bytes      = mem_chunk<libc_types::u8>;
using BytesConst = mem_chunk<const libc_types::u8>;

template<typename T>
using _cbasic_data_chunk = mem_chunk<T>;

template<typename T>
using Span = mem_chunk<T>;

} // namespace semantic
