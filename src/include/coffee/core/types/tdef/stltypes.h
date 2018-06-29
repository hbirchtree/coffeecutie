#pragma once

#include "../../coffee_mem_macros.h"
#include "../../plat/plat_quirks_toggling.h"
#include "../cdef/ptr_wrap.h"
#include "standard_exceptions.h"

/*Container types*/
#include <array>
#include <functional>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <vector>

/*Memory management*/
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

/*Shit*/
#include <complex>
#include <exception>
#include <system_error>
#include <type_traits>

#if defined(COFFEE_GEKKO)
#include <gccore.h>
#endif

namespace Coffee {

using CString = std::string; /*!< Typical string object */
using CWString =
    std::wstring; /*!< Typical string object suited for interfaces */

#if defined(COFFEE_NO_THREADLIB)
struct Mutex
{
    Mutex();
    ~Mutex();
    void lock();
    bool try_lock();
    void unlock();
#if defined(COFFEE_GEKKO)
  private:
    ::u32 m_mutexHandle;
#endif
};
struct Lock
{
    Lock(Mutex& m) : m_mutex(m)
    {
        m_mutex.lock();
    }
    ~Lock()
    {
        m_mutex.unlock();
    }

  private:
    Mutex& m_mutex;
};
#else
using RecMutex = std::recursive_mutex;
using Mutex    = std::mutex;
using Lock     = std::lock_guard<Mutex>;
using RecLock  = std::lock_guard<RecMutex>;

using UqLock  = std::unique_lock<Mutex>;
using CondVar = std::condition_variable;
#endif

using ErrCode = std::error_code;

template<typename T>
using Atomic = std::atomic<T>;

using AtomicInt8  = std::atomic_int_fast8_t;
using AtomicInt16 = std::atomic_int_fast16_t;
using AtomicInt32 = std::atomic_int;
using AtomicInt64 = std::atomic_int_fast64_t;

using AtomicUInt8  = std::atomic_uint_fast8_t;
using AtomicUInt16 = std::atomic_uint_fast16_t;
using AtomicUInt32 = std::atomic_uint;
using AtomicUInt64 = std::atomic_uint_fast64_t;

using AtomicBool = std::atomic_bool;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using LinkList = std::list<T>;
template<typename T>
using List = std::list<T>;

template<typename T>
using Vector = std::vector<T>;
template<typename T, size_t Size>
using Array = std::array<T, Size>;

template<typename T1, typename T2>
using Map = std::map<T1, T2>;

template<typename T1, typename T2>
using MultiMap = std::multimap<T1, T2>;

#ifndef COFFEE_LOWFAT
template<typename T>
using ShPtr = std::shared_ptr<T>;

template<typename T, class Deleter = std::default_delete<T>>
using UqPtr = std::unique_ptr<T, Deleter>;
#else
template<typename T>
using ShPtr = Ptr<T>;
template<typename T, class Deleter = std::default_delete<T>>
using UqPtr = Ptr<T>;
#endif

template<typename T>
using Complex = std::complex<T>;

template<typename T>
using Set = std::set<T>;

template<typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template<typename... Args>
using Tup = std::tuple<Args...>;

using ByteVector = Vector<uint8_t>;

using ExceptionPtr = std::exception_ptr;

template<typename T>
using NonConst = typename std::remove_const<T>::type;

template<typename Tag, typename T>
struct Iterator
{
    typedef NonConst<T> value_type;

    typedef Tag            iterator_category;
    typedef std::ptrdiff_t difference_type;
    typedef T*             pointer;
    typedef T&             reference;
};

using ForwardIteratorTag = std::forward_iterator_tag;

template<typename T, typename... Args>
/*!
 * \brief A lot of our platforms only support C++11. std::make_unique is
 *      C++14 only. This is a helper function.
 * \param a
 * \return
 */
inline UqPtr<T> MkUq(Args... a)
{
    return UqPtr<T>(new T(a...));
}
template<typename T>
inline UqPtr<T> MkUqWrap(T* ptr)
{
    return UqPtr<T>(ptr);
}
template<typename T, class Deleter, typename... Args>
inline UqPtr<T, Deleter> MkUqDST(Args... a)
{
    return UqPtr<T, Deleter>(new T(a...));
}

template<typename T, typename... Args>
inline ShPtr<T> MkShared(Args... a)
{
    return ShPtr<T>(new T(a...));
}

/*
 *
 * Here comes a couple of custom classes and wrappers
 *
 */

template<bool Reversed>
struct range_params
{
    static const constexpr bool reversed = Reversed;
};

using range_reversed = range_params<true>;

template<typename T = size_t, typename range_param = range_params<false>>
struct range
{
    struct iterator : Iterator<ForwardIteratorTag, T>
    {
        static const constexpr T npos = std::numeric_limits<T>::max();

        iterator() : m_idx(npos)
        {
        }

        iterator(T start, T end) : m_idx(start), m_end(end)
        {
            bool correct     = (!range_param::reversed) && start > end;
            bool correct_rev = range_param::reversed && start > end;

            if(correct && correct_rev)
                Throw(std::out_of_range("invalid range"));

            if(start == end)
                m_idx = npos;
        }

        iterator& operator++()
        {
            if(range_param::reversed)
                m_idx--;
            else
                m_idx++;

            if(m_idx >= m_end)
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

        T operator*() const
        {
            return m_idx;
        }

      private:
        T m_idx;
        T m_end;
    };

    range(T len) : m_len(len)
    {
    }

    iterator begin()
    {
        return iterator(0, m_len);
    }

    iterator end()
    {
        return iterator();
    }

  private:
    T m_len;
};

template<typename T = size_t>
using Range = range<T>;

template<typename T = size_t>
using range_rev = range<T, range_reversed>;

struct non_copy
{
    C_MOVE_CONSTRUCTOR(non_copy);
    C_DELETE_COPY_CONSTRUCTOR(non_copy);

    non_copy()
    {
    }
};

template<typename IteratorType>
struct quick_container
{
    quick_container(
        std::function<IteratorType()>&& begin_con,
        std::function<IteratorType()>&& end_con) :
        m_begin(begin_con),
        m_end(end_con)
    {
    }

    IteratorType begin()
    {
        return m_begin();
    }

    IteratorType end()
    {
        return m_end();
    }

  private:
    std::function<IteratorType()> m_begin;
    std::function<IteratorType()> m_end;
};

using error_category = std::error_category;

struct error_code : std::error_code
{
    CString error_message;

    error_code& operator=(CString error_msg)
    {
        this->error_message = error_msg;
        return *this;
    }
};

template<typename DomainError, typename ErrorCategory>
struct domain_error_code : error_code
{
    domain_error_code()
    {
        assign(0, m_category);
    }

    using error_code::operator=;

    ErrorCategory m_category;

    domain_error_code& operator=(DomainError error_code)
    {
        assign(C_CAST<int>(error_code), m_category);
        return *this;
    }

    bool operator!=(DomainError error)
    {
        return C_CAST<int>(error) != value();
    }

    bool operator==(DomainError error)
    {
        return C_CAST<int>(error) == value();
    }
};

} // namespace Coffee

#define C_STR_HELPER(x) #x
#define C_STR(x) C_STR_HELPER(x)

#define C_PTR_CHECK(ptr)                                        \
    if(!ptr)                                                    \
        Throw(undefined_behavior("bad pointer deref: " __FILE__ \
                                 ":" C_STR(__LINE__)));
#define C_THIS_CHECK                                              \
    if(!this)                                                     \
        Throw(undefined_behavior("bad access to *this: " __FILE__ \
                                 ":" C_STR(__LINE__)));

#ifndef NDEBUG
#define C_ERROR_CHECK(ec)                            \
    {                                                \
        if(ec)                                       \
            Throw(std::runtime_error(ec.message())); \
    }
#else
#define C_ERROR_CHECK(ec)
#endif
