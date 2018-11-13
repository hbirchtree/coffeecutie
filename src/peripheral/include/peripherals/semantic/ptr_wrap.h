#pragma once

#include <type_traits>

#include <coffee/core/base.h>
#include <peripherals/stl/standard_exceptions.h>

namespace semantic {

/* Some poorly written safety extensions */

namespace ptr_opts {

struct unowned_ptr
{
};

struct managed_ptr
{
};

} // namespace ptr_opts

namespace detail {

template<
    typename T,
    typename std::enable_if<!std::is_same<T, void>::value>::type* = nullptr>
struct PtrCommon
{
    T* ptr;

    PtrCommon() : ptr(nullptr)
    {
    }
    PtrCommon(T* ptr) : ptr(ptr)
    {
    }

    STATICINLINE PtrCommon<T> Alloc(size_t num)
    {
        PtrCommon<T> outPtr = C_RCAST<T*>(calloc(num * sizeof(T), 1));
        return outPtr;
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

    FORCEDINLINE const T* get() const
    {
        return ptr;
    }
};

template<typename T, typename Param = ptr_opts::unowned_ptr>
struct PtrDestructor : public PtrCommon<T>
{
    FORCEDINLINE ~PtrDestructor()
    {
#if MODE_DEBUG && !defined(COFFEE_LOWFAT)
        if(this->get())
            Throw(memory_error("pointer was not freed!"));
#endif
    }
};

template<typename T>
struct PtrDestructor<T, ptr_opts::managed_ptr> : PtrCommon<T>
{
    FORCEDINLINE ~PtrDestructor()
    {
        if(this->get())
            free(this->get());
    }
};

} // namespace detail

template<typename T, typename Param = ptr_opts::unowned_ptr>
struct Ptr : detail::PtrDestructor<T, Param>
{
    Ptr()
    {
        this->ptr = nullptr;
    }

    Ptr(T* from)
    {
        this->ptr = from;
    }

    C_DELETE_COPY_CONSTRUCTOR(Ptr);
    C_MOVE_CONSTRUCTOR(Ptr);

    FORCEDINLINE operator T*()
    {
        return this->get();
    }

    FORCEDINLINE operator const T*() const
    {
        return this->get();
    }

    FORCEDINLINE T* operator->()
    {
        return this->get();
    }

    FORCEDINLINE const T* operator->() const
    {
        return this->get();
    }

    FORCEDINLINE T& operator*()
    {
#if MODE_DEBUG
        if(!this->get())
            Throw(undefined_behavior("nullptr deref"));
#endif
        return *this->get();
    }

    FORCEDINLINE T const& operator*() const
    {
#if MODE_DEBUG
        if(!this->get())
            Throw(undefined_behavior("nullptr deref"));
#endif
        return *this->get();
    }

    FORCEDINLINE Ptr<T>& operator=(T* otherPtr)
    {
        this->ptr = otherPtr;
        return *this;
    }
};

namespace ptr_ops {

template<typename T>
FORCEDINLINE void free(Ptr<T>& ptr)
{
    ::free(ptr.ptr);
    ptr = nullptr;
}

}

} // namespace semantic
