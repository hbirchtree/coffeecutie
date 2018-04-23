#pragma once

#include "../../coffee_macros.h"
#include "../../plat/memory/cmemory.h"
#include "../tdef/integertypes.h"
#include <utility>

namespace Coffee {

struct Path;

template<typename T>
struct _cbasic_data_chunk
{
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
    _cbasic_data_chunk() : data(0), size(0), elements(0), m_destr(nullptr)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(T* data, szptr size, szptr elements) :
        data(data), size(size), elements(elements), m_destr(nullptr)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(T* data, szptr size) :
        data(data), size(size), elements(0), m_destr(nullptr)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(T& value) :
        data(&value), size(sizeof(T)), elements(1), m_destr(nullptr)
    {
    }

    FORCEDINLINE
    _cbasic_data_chunk(Vector<T>& array) :
        data(array.data()), size(sizeof(T) * array.size()),
        elements(array.size()), m_destr(nullptr)
    {
    }

    ~_cbasic_data_chunk()
    {
        if(m_destr)
            m_destr(*this);
    }

    C_DELETE_COPY_CONSTRUCTOR(_cbasic_data_chunk);
    _cbasic_data_chunk& operator=(_cbasic_data_chunk&&) = default;

    STATICINLINE void SetDestr(
        _cbasic_data_chunk<T>& inst, void (*d)(_cbasic_data_chunk<T>&))
    {
        inst.m_destr = d;
    }

    template<typename DT, typename is_not_virtual<DT>::type* = nullptr>
    STATICINLINE _cbasic_data_chunk<T> Create(DT& obj)
    {
        return {C_FCAST<T*>(&obj), sizeof(DT), 1};
    }

    template<typename is_pod<T>::type* = nullptr>
    STATICINLINE _cbasic_data_chunk<T> CreateString(cstring src)
    {
        return {C_FCAST<T*>(src), strlen(src), 0};
    }

    template<typename T2, typename is_not_virtual<T2>::type* = nullptr>
    STATICINLINE _cbasic_data_chunk<T> CreateFrom(Vector<T2>& data)
    {
        return {
            C_RCAST<T*>(data.data()), sizeof(T2) * data.size(), data.size()};
    }

    template<typename T2>
    STATICINLINE _cbasic_data_chunk<T> From(Vector<T2>& data)
    {
        return CreateFrom(data);
    }

    template<typename T2>
    STATICINLINE _cbasic_data_chunk<T> From(T2* data, szptr size)
    {
        return {C_FCAST<T*>(data), size, 1};
    }

    template<typename T2, typename is_pod<T2>::type* = nullptr>
    STATICINLINE _cbasic_data_chunk<T> From(T2& data)
    {
        return Create(data);
    }

    template<typename T2, typename is_not_virtual<T2>::type* = nullptr>
    STATICINLINE _cbasic_data_chunk<T> CopyFrom(Vector<T2>& data)
    {
        _cbasic_data_chunk<T> out;
        out.size     = (data.size() * sizeof(T2)) / sizeof(T);
        out.elements = data.size();
        out.data     = C_RCAST<T*>(Calloc(out.size * sizeof(T), 1));
        MemCpy(out.data, data.data(), out.size);
        _cbasic_data_chunk<T>::SetDestr(
            out, [](_cbasic_data_chunk<T>& b) { CFree(b.data); });

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
    STATICINLINE _cbasic_data_chunk<T> Copy(T2 const& obj)
    {
        _cbasic_data_chunk<T> out;
        out.size     = nmax((sizeof(T2)) / sizeof(T), 8);
        out.elements = 1;
        out.data     = C_RCAST<T*>(Calloc(out.size * sizeof(T), 1));
        MemCpy(out.data, &obj, sizeof(T2));
        _cbasic_data_chunk<T>::SetDestr(
            out, [](_cbasic_data_chunk<T>& b) { CFree(b.data); });

        return out;
    }

    STATICINLINE _cbasic_data_chunk<T> Copy(_cbasic_data_chunk<T> const& src)
    {
        _cbasic_data_chunk<T> out;
        out.size = src.size;
        out.data = C_RCAST<T*>(Calloc(out.size * sizeof(T), 1));
        MemCpy(out.data, src.data, src.size * sizeof(T));
        _cbasic_data_chunk<T>::SetDestr(
            out, [](_cbasic_data_chunk<T>& b) { CFree(b.data); });
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

    struct iterator : Iterator<ForwardIteratorTag, T>
    {
        iterator() : m_chunk(nullptr), m_idx(C_CAST<szptr>(-1))
        {
        }
        iterator(_cbasic_data_chunk<T>& chunk) : m_chunk(&chunk), m_idx(0)
        {
        }

        iterator& operator++()
        {
            if(m_idx < (m_chunk->size - 1))
                m_idx++;
            else
                m_idx = C_CAST<szptr>(-1);

            return *this;
        }

        bool operator==(iterator const& other) const
        {
            return other.m_idx == m_idx;
        }

        bool operator!=(iterator const& other) const
        {
            return other.m_idx != m_idx;
        }

        template<
            typename std::enable_if<!std::is_same<T, void>::value>::type* =
                nullptr>
        T const& operator*() const
        {
            return m_chunk->data[m_idx];
        }

      private:
        _cbasic_data_chunk* m_chunk;
        szptr               m_idx;
    };

    iterator begin()
    {
        return iterator(*this);
    }

    iterator end()
    {
        return iterator();
    }

  private:
    void (*m_destr)(_cbasic_data_chunk<T>&);
};

using CVoidData = _cbasic_data_chunk<void>;
using CByteData = _cbasic_data_chunk<byte_t>;

using Bytes      = CByteData;
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
            if(m_idx > m_base->m_size)
                throw std::out_of_range("invalid access");
            else
                return (*m_base)[m_idx];
        }

      private:
        _cbasic_serial_array<T>* m_base;
        szptr                    m_idx;
    };

    iterator begin()
    {
        if(m_size == 0)
            return end();
        else
            return iterator(this);
    }

    iterator end()
    {
        return iterator();
    }

    T const& operator[](szptr i)
    {
        if(i > m_size)
            throw std::out_of_range("invalid access");
        else
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
