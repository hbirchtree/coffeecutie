#pragma once

#include <coffee/core/types/cdef/memtypes.h>
#include <coffee/core/types/tdef/stltypes.h>

namespace Coffee {

/* Some poorly written safety extensions */

template<
    typename T,
    typename std::enable_if<!std::is_same<T, void>::value>::type* = nullptr>
struct Ptr
{
    T* ptr;

    Ptr() : ptr(nullptr)
    {
    }
    Ptr(T* ptr) : ptr(ptr)
    {
    }
    Ptr(Ptr<T>&& ptr) : ptr(ptr.ptr)
    {
    }

    STATICINLINE Ptr<T> Alloc(size_t num)
    {
        Ptr<T> outPtr = C_RCAST<T*>(calloc(num * sizeof(T), 1));
        return outPtr;
    }

    FORCEDINLINE ~Ptr()
    {
#if !defined(NDEBUG)
        if(ptr)
            throw memory_error("pointer was not freed!");
#endif
    }

    FORCEDINLINE operator T*()
    {
        return ptr;
    }

    FORCEDINLINE operator const T*() const
    {
        return ptr;
    }

    FORCEDINLINE T& operator*()
    {
#if !defined(NDEBUG)
        if(!ptr)
            throw undefined_behavior("nullptr deref");
#endif
        return *ptr;
    }

    FORCEDINLINE T const& operator*() const
    {
#if !defined(NDEBUG)
        if(!ptr)
            throw undefined_behavior("nullptr deref");
#endif
        return *ptr;
    }

    FORCEDINLINE Ptr<T>& operator=(Ptr<T>&& other)
    {
        ptr       = other.ptr;
        other.ptr = nullptr;
        return *this;
    }

    FORCEDINLINE Ptr<T>& operator=(T* otherPtr)
    {
        ptr = otherPtr;
        return *this;
    }
};

template<typename T>
FORCEDINLINE void CFree(Ptr<T>& ptr)
{
    free(ptr);
    ptr = nullptr;
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        !is_specialized<T2, _cbasic_data_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2& target)
{
    std::copy(src.begin(), src.end(), std::back_insert_iterator<T2>(target));
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        is_specialized<T2, _cbasic_data_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2& target)
{
    std::copy(
        src.begin(),
        src.end(),
        std::insert_iterator<T2>(target, target.begin()));
}

template<
    typename T1,
    typename T2,
    typename std::enable_if<
        is_specialized<T2, _cbasic_data_chunk>::value>::type* = nullptr>
FORCEDINLINE void MemCpy(T1 const& src, T2&& target)
{
    std::copy(
        src.begin(),
        src.end(),
        std::insert_iterator<T2>(target, target.begin()));
}

} // namespace Coffee
