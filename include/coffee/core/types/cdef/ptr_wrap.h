#pragma once

#include <type_traits>

#include <coffee/core/coffee_mem_macros.h>
#include <coffee/core/types/tdef/standard_exceptions.h>

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
//    Ptr(Ptr<T>&& ptr) : ptr(ptr.ptr)
//    {
//    }

    STATICINLINE Ptr<T> Alloc(size_t num)
    {
        Ptr<T> outPtr = C_RCAST<T*>(calloc(num * sizeof(T), 1));
        return outPtr;
    }

    FORCEDINLINE ~Ptr()
    {
#if !defined(NDEBUG) && !defined(COFFEE_LOWFAT)
        if(ptr)
            Throw(memory_error("pointer was not freed!"));
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

    FORCEDINLINE T* operator->()
    {
        return ptr;
    }

    FORCEDINLINE const T* operator->() const
    {
        return ptr;
    }

    FORCEDINLINE T& operator*()
    {
#if !defined(NDEBUG)
        if(!ptr)
            Throw(undefined_behavior("nullptr deref"));
#endif
        return *ptr;
    }

    FORCEDINLINE T const& operator*() const
    {
#if !defined(NDEBUG)
        if(!ptr)
            Throw(undefined_behavior("nullptr deref"));
#endif
        return *ptr;
    }

//    FORCEDINLINE Ptr<T>& operator=(Ptr<T>&& other)
//    {
//        ptr       = other.ptr;
//        other.ptr = nullptr;
//        return *this;
//    }

    FORCEDINLINE Ptr<T>& operator=(T* otherPtr)
    {
        ptr = otherPtr;
        return *this;
    }

    /* For replacing other std::*_ptr types */
    FORCEDINLINE void release()
    {
        ptr = nullptr;
    }
    FORCEDINLINE void reset()
    {
        release();
    }

    FORCEDINLINE T* get()
    {
        return ptr;
    }
};

template<typename T>
FORCEDINLINE void CFree(Ptr<T>& ptr)
{
    free(ptr);
    ptr = nullptr;
}

} // namespace Coffee
