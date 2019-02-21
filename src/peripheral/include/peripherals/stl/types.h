#pragma once

#include "standard_exceptions.h"
#include <peripherals/base.h>
#include <peripherals/identify/quirks.h>

/*Container types*/
#include <array>
#include <deque>
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

#if __cplusplus >= 201703L
#include <optional>
#endif

#if defined(COFFEE_GEKKO)
using mutex_handle_t = long unsigned int;
#endif

namespace stl_types {

using namespace type_safety;

using CString  = std::string; /*!< Typical string object */
using CWString = std::wstring;

#if defined(COFFEE_NO_THREADLIB)
struct Mutex
{
    Mutex();
    ~Mutex();
    void lock();
    bool try_lock();
    void unlock();

  private:
#if defined(COFFEE_GEKKO)
    long unsigned int m_mutexHandle;
#endif
};

struct RecMutex
{
    RecMutex();
    ~RecMutex();
    void lock();
    bool try_lock();
    void unlock();

  private:
#if defined(COFFEE_GEKKO)
    long unsigned int m_mutexHandle;
#endif
};

enum class cv_status
{
    no_timeout,
    timeout,
};

template<typename MutexType = Mutex>
struct BaseLock
{
    BaseLock(MutexType& m) : m_mutex(m)
    {
        m_mutex.lock();
    }
    ~BaseLock()
    {
        m_mutex.unlock();
    }

  private:
    MutexType& m_mutex;
};

using Lock    = BaseLock<>;
using RecLock = BaseLock<RecMutex>;

struct UqLock
{
    // TODO: implement std::defer_lock and friends
    UqLock(Mutex& m) : m_mutex(m)
    {
        m_mutex.lock();
    }
    ~UqLock()
    {
        m_mutex.unlock();
    }

    Mutex* mutex()
    {
        return &m_mutex;
    }

  private:
    Mutex& m_mutex;
};

struct CondVar
{
    CondVar();
    ~CondVar();

    void wait(UqLock& lock);

    template<typename Rep, typename Dur>
    cv_status wait_for(UqLock& lock, std::chrono::duration<Rep, Dur> const& dur)
    {
        return wait_limited(
            std::chrono::duration_cast<std::chrono::nanoseconds>(dur).count());
    }

    template<typename Rep, typename Dur>
    cv_status wait_until(UqLock& lock)
    {
        return cv_status::timeout;
    }

    void notify_all();
    void notify_one();

  private:
    cv_status wait_limited(long wait_ns);

#if defined(COFFEE_GEKKO)
    Mutex                     m_signalLock;
    std::atomic<unsigned int> m_waiters;
    std::atomic<unsigned int> m_signals;
    long unsigned int         m_syncQueue;
#endif
};

#else
using RecMutex = std::recursive_mutex;
using Mutex    = std::mutex;
using Lock     = std::lock_guard<Mutex>;
using RecLock  = std::lock_guard<RecMutex>;

using UqLock    = std::unique_lock<Mutex>;
using UqRecLock = std::unique_lock<RecMutex>;
using CondVar   = std::condition_variable;
using cv_status = std::cv_status;
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

using atomic_bool = std::atomic_bool;
using atomic_i8   = std::atomic_int_fast8_t;
using atomic_i16  = std::atomic_int_fast16_t;
using atomic_i32  = std::atomic_int_fast32_t;
using atomic_i64  = std::atomic_int_fast64_t;

using atomic_u8  = std::atomic_uint_fast8_t;
using atomic_u16 = std::atomic_uint_fast16_t;
using atomic_u32 = std::atomic_uint_fast32_t;
using atomic_u64 = std::atomic_uint_fast64_t;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using Deque = std::deque<T>;

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

template<typename T>
using ShPtr = std::shared_ptr<T>;

template<typename T, class Deleter = std::default_delete<T>>
using UqPtr = std::unique_ptr<T, Deleter>;

template<typename T>
using WkPtr = std::weak_ptr<T>;

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

        iterator operator++(int)
        {
            iterator copy = *this;

            if(range_param::reversed)
                m_idx--;
            else
                m_idx++;

            if(m_idx >= m_end)
                m_idx = npos;

            return copy;
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

    using value_type = T;

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

template<
    typename ContainerType,
    typename Iterator = typename ContainerType::iterator>
struct slice
{
    using container_value_type = ContainerType;
    using difference_type      = typename container_value_type::difference_type;
    using iterator             = Iterator;
    using size_type            = typename container_value_type::size_type;

    iterator m_begin;
    iterator m_end;

    slice(iterator const& begin, iterator const& end) :
        m_begin(begin), m_end(end)
    {
    }

    explicit slice(
        container_value_type& container,
        difference_type       offset = 0,
        difference_type       num    = 0) :
        m_begin(container.begin() + offset),
        m_end(container.begin() + offset + num)
    {
    }

    iterator begin() const
    {
        return m_begin;
    }

    iterator end() const
    {
        return m_end;
    }

    typename container_value_type::size_type size() const
    {
        return m_end - m_begin;
    }
};

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

    void reassign(int error_code)
    {
        assign(error_code, category());
    }

    FORCEDINLINE std::string message() const
    {
        auto msg = std::error_code::message();
        return error_message.size() ? msg + ": " + error_message : msg;
    }
};

struct sentinel_error_code
{
};

template<typename DomainError, typename ErrorCategory>
struct domain_error_code : error_code
{
    using error_type    = DomainError;
    using category_type = ErrorCategory;

    ErrorCategory m_category;

    FORCEDINLINE domain_error_code()
    {
        assign(0, m_category);
    }

    FORCEDINLINE domain_error_code(domain_error_code&& other) :
        m_category(std::move(other.m_category))
    {
        error_message = std::move(other.error_message);
        assign(other.value(), m_category);
    }

    using error_code::operator=;

    FORCEDINLINE domain_error_code& operator=(domain_error_code&& other)
    {
        error_message = std::move(other.error_message);
        return *this;
    }

    FORCEDINLINE domain_error_code& operator=(DomainError error_code)
    {
        assign(C_CAST<int>(error_code), m_category);
        return *this;
    }

    FORCEDINLINE bool operator!=(DomainError error)
    {
        return C_CAST<int>(error) != value();
    }

    FORCEDINLINE bool operator==(DomainError error)
    {
        return C_CAST<int>(error) == value();
    }

    template<
        typename TDomain,
        typename TErrorCategory,
        typename std::enable_if<
            std::is_same<TDomain, DomainError>::value &&
                std::is_same<TErrorCategory, ErrorCategory>::value,
            TDomain>::type* = nullptr>
    FORCEDINLINE domain_error_code& to_domain_deref()
    {
        return *this;
    }
};

template<typename DomainError, typename ErrorCategory, typename NestedError>
struct nested_domain_error_code : error_code
{
    using nested_error_type = NestedError;
    using error_type        = DomainError;
    using category_type     = ErrorCategory;

    ErrorCategory m_category;
    NestedError   m_internalError;

    FORCEDINLINE nested_domain_error_code()
    {
        assign(0, m_category);
    }

    FORCEDINLINE nested_domain_error_code(nested_domain_error_code&& other) :
        m_category(std::move(other.m_category)),
        m_internalError(std::move(other.m_internalError))
    {
        error_message = std::move(other.error_message);
    }

    template<
        typename Dummy = int,
        typename std::enable_if<
            !std::is_same<NestedError, sentinel_error_code>::value,
            Dummy>::type* = nullptr>
    FORCEDINLINE std::string message() const
    {
        if(m_internalError)
            return error_code::message() + ": " + m_internalError.message();
        else
            return error_code::message();
    }

    using error_code::operator=;

    FORCEDINLINE nested_domain_error_code& operator=(
        nested_domain_error_code&& other)
    {
        m_category      = std::move(other.m_category);
        m_internalError = std::move(other.m_internalError);
        error_message   = std::move(other.error_message);
        return *this;
    }

    FORCEDINLINE nested_domain_error_code& operator=(DomainError error_code)
    {
        assign(C_CAST<int>(error_code), m_category);
        return *this;
    }

    FORCEDINLINE nested_domain_error_code& operator=(NestedError&& error_code)
    {
        m_internalError = std::move(error_code);
        return *this;
    }

    FORCEDINLINE bool operator!=(DomainError error)
    {
        return C_CAST<int>(error) != value();
    }

    FORCEDINLINE bool operator==(DomainError error)
    {
        return C_CAST<int>(error) == value();
    }

    template<
        typename TargetError,
        typename std::enable_if<
            is_specialized<TargetError, domain_error_code>::value,
            TargetError>::type* = nullptr>
    FORCEDINLINE TargetError& as()
    {
        return to_domain_deref<
            typename TargetError::error_type,
            typename TargetError::category_type>();
    }

    template<
        typename TDomain,
        typename TErrorCategory,
        typename std::enable_if<
            std::is_same<TDomain, error_type>::value,
            TDomain>::type* = nullptr,
        typename std::enable_if<
            std::is_same<TErrorCategory, category_type>::value,
            TErrorCategory>::type* = nullptr>
    /*!
     * \brief Casting to domain_error_code of similar type, no nesting
     */
    FORCEDINLINE domain_error_code<TDomain, TErrorCategory>& to_domain_deref()
    {
        using out_type = domain_error_code<TDomain, TErrorCategory>;
        return *C_RCAST<out_type*>(this);
    }

    template<
        typename TDomain,
        typename TErrorCategory,
        typename std::enable_if<
            !std::is_same<TDomain, error_type>::value ||
                !std::is_same<TErrorCategory, category_type>::value,
            TDomain>::type* = nullptr>
    /*!
     * \brief If casting from the current type doesn't work, try the nested type
     * This works recursively
     */
    FORCEDINLINE domain_error_code<TDomain, TErrorCategory>& to_domain_deref()
    {
        return m_internalError
            .template to_domain_deref<TDomain, TErrorCategory>();
    }
};

template<typename NestedError>
struct nested_empty_error_code
{
    using nested_error_type = NestedError;
};

#if __cplusplus < 201703L
using bad_optional_access = undefined_behavior;

template<typename T>
struct Optional
{
    Optional() : valid(false)
    {
    }
    Optional(T&& value) : value(value), valid(true)
    {
    }

    T& operator->() const
    {
        if(!valid)
            Throw(bad_optional_access("invalid optional"));

        return value;
    }

    T& operator*() const
    {
        if(!valid)
            Throw(bad_optional_access("invalid optional"));

        return value;
    }

    operator bool() const
    {
        return valid;
    }

    Optional& operator=(T&& v)
    {
        value = std::move(v);
        valid = true;

        return *this;
    }

    T value;
    bool valid;
};
#else
template<typename T>
using Optional = std::optional<T>;
#endif

} // namespace stl_types

#define C_ERROR_CODE_OUT_OF_BOUNDS() \
    Throw(undefined_behavior("invalid error code"))

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

#if MODE_DEBUG
#define C_ERROR_CHECK(ec)                              \
    {                                                  \
        if(ec)                                         \
            Throw(implementation_error(ec.message())); \
    }
#else
#define C_ERROR_CHECK(ec)
#endif

#if MODE_DEBUG
#define CASSERT(condition) \
    if(!(condition))       \
        Throw(undefined_behavior("assertion failed: " C_STR(condition)));
#else
#define CASSERT(condition)
#endif
