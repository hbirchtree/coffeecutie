#pragma once

#include "../../coffee_macros.h"
#include "../tdef/integertypes.h"
#include "../tdef/stltypes.h"
#include <utility>

namespace Coffee {

struct Path;

template<typename T>
struct _cbasic_data_chunk
{
    using value_type = T;

    FORCEDINLINE
    _cbasic_data_chunk(_cbasic_data_chunk<T>&& other) :
        data(other.data), size(other.size), elements(other.elements),
        m_destr(other.m_destr)
    {
        other.data     = nullptr;
        other.size     = 0;
        other.elements = 0;
        other.m_destr  = nullptr;
    }

    FORCEDINLINE
    _cbasic_data_chunk() : data(0), size(0), elements(0)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(T* data, szptr size, szptr elements) :
        data(data), size(size), elements(elements)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(T* data, szptr size) :
        data(data), size(size), elements(0)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(T& value) : data(&value), size(sizeof(T)), elements(1)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(Vector<T>& array) :
        data(array.data()), size(sizeof(T) * array.size()),
        elements(array.size())
    {
    }

    template<
        typename T2,
        typename std::enable_if<
            std::is_same<T2, typename std::remove_cv<T>::type>::value>::type* =
            nullptr>
    FORCEDINLINE _cbasic_data_chunk(_cbasic_data_chunk<T2> const& other) :
        data(C_RCAST<T*>(other.data)), size(other.size),
        elements(other.elements)
    {
    }

    ~_cbasic_data_chunk()
    {
        if(m_destr)
            m_destr(*this);
    }

    template<typename T2>
    NO_DISCARD explicit operator _cbasic_data_chunk<T2>()
    {
        _cbasic_data_chunk<T2> out;

        out.data     = C_RCAST<T2*>(this->data);
        out.size     = this->size;
        out.elements = this->elements;

        return out;
    }

    C_DELETE_COPY_CONSTRUCTOR(_cbasic_data_chunk);

    _cbasic_data_chunk& operator=(_cbasic_data_chunk&& other)
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

    STATICINLINE void SetDestr(
        _cbasic_data_chunk<T>& inst, void (*d)(_cbasic_data_chunk<T>&))
    {
#if !defined(NDEBUG)
        if(!d)
            Throw(implementation_error("abuse of SetDestr()"));
#endif
        inst.m_destr = d;
    }

    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> Alloc(szptr num)
    {
#if !defined(NDEBUG)
        if(num == 0)
            Throw(implementation_error("allocating 0 bytes is bad"));
#endif
        _cbasic_data_chunk<T> out;

        out.data     = C_RCAST<T*>(calloc(num * sizeof(T), 1));
        out.size     = sizeof(T) * num;
        out.elements = num;

        _cbasic_data_chunk<T>::SetDestr(
            out, [](_cbasic_data_chunk<T>& d) { free(d.data); });

        return out;
    }

    template<typename DT, typename is_not_virtual<DT>::type* = nullptr>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> Create(DT& obj)
    {
        return {C_FCAST<T*>(&obj), sizeof(DT), 1};
    }

    template<typename is_pod<T>::type* = nullptr>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> CreateString(cstring src)
    {
        return {C_FCAST<T*>(src), strlen(src), 0};
    }

    template<typename T2, typename is_not_virtual<T2>::type* = nullptr>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> CreateFrom(Vector<T2>& data)
    {
        static_assert(sizeof(T2) >= sizeof(T), "incompatible size to wrap");

        return {
            C_RCAST<T*>(data.data()), sizeof(T2) * data.size(), data.size()};
    }

    template<typename T2>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> From(Vector<T2>& data)
    {
        return CreateFrom(data);
    }

    template<
        typename T2,
        typename std::enable_if<
            !std::is_same<typename std::remove_cv<T2>::type, void>::value>::
            type* = nullptr>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> From(T2* data, szptr size)
    {
        static_assert(sizeof(T2) >= sizeof(T), "incompatible size to wrap");

        return {C_FCAST<T*>(data), size * sizeof(T2) / sizeof(T), 1};
    }

    template<
        typename T2,
        typename std::enable_if<
            std::is_same<typename std::remove_cv<T2>::type, void>::value>::
            type* = nullptr>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> From(T2* data, szptr size)
    {
        return {C_FCAST<T*>(data), size / sizeof(T), 1};
    }

    template<typename T2, typename is_pod<T2>::type* = nullptr>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> From(T2& data)
    {
        return Create(data);
    }

    template<typename T2, typename is_not_virtual<T2>::type* = nullptr>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> CopyFrom(Vector<T2>& data)
    {
        static_assert(sizeof(T2) >= sizeof(T), "incompatible size to copy");

        using OutT = _cbasic_data_chunk<T>;

        OutT out = OutT::Alloc((data.size() * sizeof(T2)) / sizeof(T));

        //        std::copy(data.begin(), data.end(), out.begin());
        memcpy(out.data, data.data(), data.size() * sizeof(T2));

        return out;
    }

    STATICINLINE szptr nmax(szptr v1, szptr v2)
    {
        if(v1 < v2)
            return v2;
        else
            return v1;
    }

    template<typename T2>
    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> Copy(T2 const& obj)
    {
        static_assert(sizeof(T2) >= sizeof(T), "incompatible size to copy");

        using OutT = _cbasic_data_chunk<T>;

        OutT out = OutT::Alloc(sizeof(T2) / sizeof(T));

        T2* cpy = C_RCAST<T2*>(out.data);
        *cpy    = obj;

        return out;
    }

    NO_DISCARD STATICINLINE _cbasic_data_chunk<T> Copy(
        _cbasic_data_chunk<T> const& src)
    {
        using OutT = _cbasic_data_chunk<T>;

        OutT out = OutT::Alloc(src.elements ? src.elements : src.size);

        std::copy(
            src.begin(),
            src.end(),
            std::insert_iterator<_cbasic_data_chunk<T>>(out, out.begin()));

        return out;
    }

    template<
        typename std::enable_if<!std::is_void<T>::value, bool>::type* = nullptr>
    T& operator[](szptr i)
    {
        return data[i];
    }
    template<
        typename std::enable_if<!std::is_void<T>::value, bool>::type* = nullptr>
    T const& operator[](szptr i) const
    {
        return data[i];
    }

    template<typename VectorT>
    _cbasic_data_chunk<T>& operator=(VectorT const& vec)
    {
        data     = C_FCAST<T*>(vec.data());
        size     = vec.size() * sizeof(typename VectorT::value_type);
        elements = vec.size();

        return *this;
    }

    template<typename T2>
    _cbasic_data_chunk<T>& operator=(Vector<T2> const& vec)
    {
        data     = C_FCAST<T*>(vec.data());
        size     = vec.size() * sizeof(T2);
        elements = vec.size();

        return *this;
    }

    _cbasic_data_chunk<T> at(szptr offset, szptr size = 0)
    {
        if(offset > this->size || offset + size > this->size)
            return {};

        byte_t* basePtr = C_RCAST<byte_t*>(data);

        if(size == 0)
            size = this->size - offset;

        return From(&basePtr[offset], size);
    }

    void disown()
    {
        m_destr = nullptr;
    }

    /*!
     * \brief Pointer to data
     */
    T* data;
    /*!
     * \brief Size in bytes
     */
    szptr size;
    /*!
     * \brief Number of elements, if applicable
     */
    szptr elements;

    /* Here's some iterator magic, for compatbility with STL containers */

    template<typename T_access = T>
    struct iterator_base : Iterator<ForwardIteratorTag, T>
    {
        friend struct _cbasic_data_chunk;

        iterator_base(_cbasic_data_chunk<T>& chunk, szptr i = 0) :
            m_chunk(&chunk), m_idx(i), m_readonly(false)
        {
        }
        iterator_base(_cbasic_data_chunk<T> const& chunk, szptr i = 0) :
            m_chunk(C_CCAST<_cbasic_data_chunk<T>*>(&chunk)), m_idx(i),
            m_readonly(true)
        {
        }

        iterator_base& operator++()
        {
            if(m_idx < m_chunk->size)
                m_idx++;

            return *this;
        }

        bool operator==(iterator_base const& other) const
        {
            return other.m_idx == m_idx;
        }

        bool operator!=(iterator_base const& other) const
        {
            return other.m_idx != m_idx;
        }

        T const& operator*() const
        {
            return m_chunk->data[m_idx];
        }

        template<
            typename Dummy = void,
            typename std::enable_if<!std::is_const<T_access>::value, Dummy>::
                type* = nullptr>
        T& operator*()
        {
            return m_chunk->data[m_idx];
        }

      private:
        _cbasic_data_chunk* m_chunk;
        szptr               m_idx;
        bool                m_readonly;
    };

    using iterator       = iterator_base<>;
    using const_iterator = iterator_base<const T>;

    NO_DISCARD iterator begin()
    {
        return iterator(*this);
    }

    NO_DISCARD iterator end()
    {
        return iterator(*this, size);
    }

    NO_DISCARD const_iterator begin() const
    {
        return const_iterator(*this);
    }

    NO_DISCARD const_iterator end() const
    {
        return const_iterator(*this, size);
    }

    iterator insert(iterator it, T&& value)
    {
        data[it.m_idx] = std::move(value);
        return it;
    }

    iterator insert(iterator it, T const& value)
    {
        data[it.m_idx] = value;
        return it;
    }

  private:
    void (*m_destr)(_cbasic_data_chunk<T>&) = nullptr;
};

using Bytes      = _cbasic_data_chunk<byte_t>;
using BytesConst = _cbasic_data_chunk<const byte_t>;

template<typename T>
using Span = _cbasic_data_chunk<T>;

template<typename T>
struct _cbasic_serial_array
{
    _cbasic_serial_array() : m_data(nullptr), m_size(0)
    {
    }

    _cbasic_serial_array(Bytes const& source)
    {
        if((source.size % sizeof(T)) == 0)
        {
            m_data = C_RCAST<T const*>(source.data);
            m_size = source.size / sizeof(T);
        } else
            m_size = 0;
    }

    static const constexpr szptr npos = C_CAST<szptr>(-1);

    struct iterator : Iterator<ForwardIteratorTag, T>
    {
        iterator(_cbasic_serial_array<T>* base) : m_base(base), m_idx(0)
        {
        }
        iterator() : m_base(nullptr), m_idx(npos)
        {
        }

        iterator& operator++()
        {
            m_idx++;
            if(m_idx >= m_base->m_size)
                m_idx = npos;
            return *this;
        }

        bool operator==(iterator const& other) const
        {
            return m_idx == other.m_idx;
        }
        bool operator!=(iterator const& other) const
        {
            return m_idx != other.m_idx;
        }

        T const& operator*() const
        {
#if !defined(NDEBUG)
            if(m_idx > m_base->m_size)
                Throw(std::out_of_range("invalid access"));
            else
#endif
                return (*m_base)[m_idx];
        }

      private:
        _cbasic_serial_array<T>* m_base;
        szptr                    m_idx;
    };

    NO_DISCARD iterator begin()
    {
        if(m_size == 0)
            return end();
        else
            return iterator(this);
    }

    NO_DISCARD iterator end()
    {
        return iterator();
    }

    T const& operator[](szptr i)
    {
#if !defined(NDEBUG)
        if(i > m_size)
            Throw(std::out_of_range("invalid access"));
        else
#endif
            return m_data[i];
    }

    szptr size() const
    {
        return m_size;
    }

  private:
    T const* m_data;
    szptr    m_size;
};

template<typename T>
using SerialArray = _cbasic_serial_array<T>;

struct CMimeData
{
    FORCEDINLINE CMimeData(
        cstring id, void* data, const szptr& size, bool doClean = false) :
        m_id(id),
        m_data(data), m_size(size), b_doClean(doClean)
    {
    }
    FORCEDINLINE ~CMimeData()
    {
        if(b_doClean)
            CFree(m_data);
    }
    FORCEDINLINE const CString& id()
    {
        return m_id;
    }
    FORCEDINLINE const void* data()
    {
        return m_data;
    }
    FORCEDINLINE const szptr& dataSize()
    {
        return m_size;
    }

  private:
    CString m_id;
    void*   m_data;
    szptr   m_size;
    bool    b_doClean;
};

/*!
 * \brief Allows for buffering of any type of object, texture, buffer, etc.
 * according to conditions.
 */
template<typename T, size_t BufferSize>
struct _cbasic_nbuffer
{
    FORCEDINLINE _cbasic_nbuffer()
    {
        ptr = 0;
    }

    static constexpr size_t size = BufferSize;
    T                       data[BufferSize];
    size_t                  ptr;

    virtual void advance()
    {
        ptr = next_idx();
    }
    FORCEDINLINE T& current()
    {
        return data[current_idx()];
    }
    FORCEDINLINE T& next()
    {
        return data[next_idx()];
    }

    FORCEDINLINE size_t next_idx()
    {
        return (ptr + 1) % BufferSize;
    }
    FORCEDINLINE size_t current_idx()
    {
        return ptr;
    }
};
} // namespace Coffee
