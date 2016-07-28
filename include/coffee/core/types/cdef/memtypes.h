#pragma once

#include "../../coffee_macros.h"
#include "../tdef/integertypes.h"
#include "../../plat/memory/cmemory.h"

namespace Coffee{

template<typename T>
struct _cbasic_data_chunk
{
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
};

using CVoidData = _cbasic_data_chunk<void>;
using CByteData = _cbasic_data_chunk<byte_t>;

using Bytes = CByteData;
using BytesConst = _cbasic_data_chunk<const byte_t>;

template<typename T>
/*!
 * \brief For storing arrays
 */
struct chunk_mem
{
    FORCEDINLINE chunk_mem()
    {
        C_DEPRECATED();
    }
    szptr size = 0;
    T* d = nullptr;
};

/*!
 * \brief Expands an array contained in chunk_mem<T> types
 * \param mem
 * \param size
 */
template<typename T>
STATICINLINE void coffee_mem_expand_array(chunk_mem<T> *mem, szptr size)
{
    szptr osize = mem->size;
    mem->size += size;
    mem->d = (T*)Realloc(mem->d,sizeof(T)*mem->size);
    MemClear(&mem->d[osize],sizeof(T)*(mem->size-osize));
}

struct CMimeData
{
    FORCEDINLINE CMimeData(cstring id, void* data,
                           const szptr& size,
                           bool doClean = false):
        b_doClean(doClean),
        m_data(data),
        m_size(size),
        m_id(id)
    {
    }
    FORCEDINLINE ~CMimeData()
    {
        if(b_doClean)
            CFree(m_data);
    }
    FORCEDINLINE const CString& id(){return m_id;}
    FORCEDINLINE const void* data(){return m_data;}
    FORCEDINLINE const szptr& dataSize(){return m_size;}
private:
    CString m_id;
    void* m_data;
    szptr m_size;
    bool b_doClean;
};

/*!
 * \brief Allows for buffering of any type of object, texture, buffer, etc. according to conditions.
 */
template<typename T, size_t BufferSize>
struct _cbasic_nbuffer
{
    FORCEDINLINE _cbasic_nbuffer()
    {
        ptr = 0;
    }

    static constexpr size_t size = BufferSize;
    T data[BufferSize];
    size_t ptr;

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
        return (ptr+1)%BufferSize;
    }
    FORCEDINLINE size_t current_idx()
    {
        return ptr;
    }
};

template<typename T>
/*!
 * \brief Allows for RAII-consistent objects. The destructor should be specific to each specialization such that it behaves properly.
 */
class _cbasic_raii_container
{
protected:
    T* m_data;
public:
    FORCEDINLINE _cbasic_raii_container():
        m_data(new T)
    {
    }
    FORCEDINLINE _cbasic_raii_container(T* d):
        m_data(d)
    {
    }
    virtual ~_cbasic_raii_container()
    {
    }

    FORCEDINLINE T *data()
    {
        return m_data;
    }
    FORCEDINLINE T& data_ref()
    {
        return *m_data;
    }
};

template<typename T>
/*!
 * \brief Locks access to a resource, unlocks on destruction.
 */
class _cbasic_cookie_container
{
    Mutex& _access;
    T& _cookie;
public:
    FORCEDINLINE _cbasic_cookie_container(std::mutex& acc, T& cookie):
        _access(acc),
        _cookie(cookie)
    {
        _access.lock();
    }
    ~_cbasic_cookie_container()
    {
        _access.unlock();
    }
    FORCEDINLINE T& object()
    {
        return _cookie;
    }
};

}
