#ifndef COFFEE_TYPES
#define COFFEE_TYPES

#include "../plat/plat_memory.h"
#include "vector_types.h"
#include "composite_types.h"
#include "graphics_types.h"
#include "../plat/plat_timing.h"

namespace Coffee{

template<typename T>
/*!
 * \brief For storing arrays
 */
struct chunk_mem
{
    szptr size = 0;
    T* d = nullptr;
};

template<typename T>
struct _cbasic_data_chunk
{
    /*!
     * \brief Size in bytes
     */
    szptr size;
    /*!
     * \brief Pointer to data
     */
    T* data;
    /*!
     * \brief Number of elements, if applicable
     */
    szptr elements;
};

/*!
 * \brief A template for version structures
 */
template<typename T> struct _cbasic_version
{
    T major;
    T minor;
    T revision;

    inline bool operator>=(const _cbasic_version<T>& v)
    {
        if(this->major>v.major)
            return true;
        if(this->major==v.major)
        {
            if(this->minor>v.minor)
                return true;
            if(this->minor==v.minor)
            {
                if(this->revision>=v.revision)
                    return true;
                else
                    return false;
            }else
                return false;
        }else
            return false;
    }
    inline bool operator<(const _cbasic_version<T>& v)
    {
        return !((*this)>=v);
    }
    inline bool operator<=(const _cbasic_version<T>& v)
    {
        if(this->major<v.major)
            return true;
        if(this->major==v.major)
        {
            if(this->minor<v.minor)
                return true;
            if(this->minor==v.minor)
            {
                if(this->revision<=v.revision)
                    return true;
                else
                    return false;
            }else
                return false;
        }else
            return false;
    }
    inline bool operator>(const _cbasic_version<T>& v)
    {
        return !((*this)<=v);
    }
    inline bool operator==(const _cbasic_version<T>& v)
    {
        return this->major==v.major
                &&this->minor==v.minor
                &&this->revision==v.revision;
    }
};

/*!
 * \brief Expands an array contained in chunk_mem<T> types
 * \param mem
 * \param size
 */
template<typename T> static void coffee_mem_expand_array(
        chunk_mem<T> *mem, szptr size)
{
    szptr osize = mem->size;
    mem->size += size;
    mem->d = (T*)CRealloc(mem->d,sizeof(T)*mem->size);
    CMemClear(&mem->d[osize],sizeof(T)*(mem->size-osize));
}

typedef _cbasic_data_chunk<void> CVoidData;
typedef _cbasic_data_chunk<byte_t> CByteData;

}

#endif
