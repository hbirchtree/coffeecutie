#pragma once

#include "standard_exceptions.h"
#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/identify/compiler/variable_attributes.h>
#include <peripherals/identify/quirks.h>
#include <peripherals/identify/system.h>
#include <peripherals/stl/constructors.h>
#include <peripherals/stl/type_safety.h>

/*Container types*/
#include <array>
#include <deque>
#include <functional>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <string_view>
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

#include <tinyutf8/tinyutf8.h>

#if __cplusplus >= 201703L && C_HAS_INCLUDE(<optional>)
#include <optional>
#endif

#if defined(COFFEE_GEKKO)
using mutex_handle_t = long unsigned int;
#endif

namespace stl_types {

using namespace type_safety;

using u8string  = tiny_utf8::utf8_string;

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
    unsigned int m_mutexHandle;
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
    unsigned int m_mutexHandle;
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
    UqLock()
    {
    }
    UqLock(Mutex& m) : m_mutex(&m), m_hasLock(true)
    {
        m_mutex->try_lock();
    }
    UqLock(Mutex& m, std::try_to_lock_t) : m_mutex(&m)
    {
        m_hasLock = m_mutex->try_lock() == 0;
    }
    UqLock(Mutex& m, std::defer_lock_t) : m_mutex(&m), m_hasLock(false)
    {
    }
    ~UqLock()
    {
        m_mutex->unlock();
    }

    bool owns_lock() const
    {
        return m_hasLock;
    }

    Mutex* mutex()
    {
        return m_mutex;
    }

  private:
    Mutex* m_mutex;
    bool   m_hasLock;
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
    unsigned int              m_syncQueue;
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

template<typename T, class Deleter, typename... Args>
inline std::unique_ptr<T, Deleter> make_unique_with_destructor(Args... a)
{
    return std::unique_ptr<T, Deleter>(new T(std::forward<Args>(a)...));
}

/*
 *
 * Here comes a couple of custom classes and wrappers
 *
 */

template<typename T, typename... Items>
requires (std::is_same_v<T, Items> && ...)
inline constexpr auto make_array(Items&&... items)
{
    return std::array<T, sizeof...(Items)>{{std::move(items)...}};
}

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
        using difference_type = intptr_t;

        iterator(T end) : m_idx(end), m_end(end), m_stride(0)
        {
        }

        iterator(T start, T end, T stride) :
            m_idx(start), m_end(end), m_stride(stride)
        {
            bool correct     = (!range_param::reversed) && start > end;
            bool correct_rev = range_param::reversed && start > end;

            if(correct && correct_rev)
                Throw(std::out_of_range("invalid range"));
        }

        iterator& operator++()
        {
            m_idx += m_stride;

            if(m_idx >= m_end)
            {
                m_idx = m_end;
                m_stride = 0;
            }

            return *this;
        }

        iterator operator++(int)
        {
            iterator copy = *this;

            ++(*this);

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
            if constexpr(range_param::reversed)
                return m_end - m_idx - 1;
            else
                return m_idx;
        }

        difference_type operator-(iterator const& other) const
        {
            return m_idx - other.m_idx;
        }

        auto operator+(intptr_t offset) const
        {
            auto out = *this;
            out.m_idx += offset;
            return out;
        }

      private:
        T m_idx;
        T m_end;
        T m_stride;
    };

    using value_type = T;

    range(T len, T stride = 1) : m_len(len), m_stride(stride)
    {
    }

    iterator begin()
    {
        return iterator(0, m_len, m_stride);
    }

    iterator end()
    {
        return iterator(m_len);
    }

  private:
    T m_len;
    T m_stride;
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

    iterator begin()
    {
        return m_begin;
    }

    iterator end()
    {
        return m_end;
    }

    typename container_value_type::size_type size() const
    {
        return m_end - m_begin;
    }
};

template<
    typename ContainerType,
    typename std::enable_if<!std::is_const<ContainerType>::value>::type* =
        nullptr>
static inline auto slice_num(
    ContainerType& c, typename ContainerType::difference_type num)
{
    return slice<ContainerType>(std::begin(c), std::begin(c) + num);
}

template<
    typename ContainerType,
    typename std::enable_if<std::is_const<ContainerType>::value>::type* =
        nullptr>
static inline auto slice_num(
    ContainerType& c, typename ContainerType::difference_type num)
{
    return slice<ContainerType>(std::cbegin(c), std::cbegin(c) + num);
}

struct non_copy
{
    non_copy() = default;
    C_MOVE_CONSTRUCTOR(non_copy);
    C_DELETE_COPY_CONSTRUCTOR(non_copy);
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
    std::string error_message;

    error_code& operator=(std::string error_msg)
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

} // namespace stl_types

namespace type_safety {

template<typename D, typename T>
FORCEDINLINE typename std::shared_ptr<D> C_DCAST(
    typename std::shared_ptr<T> const& from)
{
    return std::dynamic_pointer_cast<D>(from);
}

template<typename D, typename T>
FORCEDINLINE typename std::shared_ptr<D> C_PCAST(
    typename std::shared_ptr<T> const& from)
{
    return std::static_pointer_cast<D>(from);
}

} // namespace type_safety

#define C_ERROR_CODE_OUT_OF_BOUNDS() \
    Throw(undefined_behavior("invalid error code"))

#define C_PTR_CHECK(ptr)                                        \
    if(!ptr)                                                    \
        Throw(undefined_behavior("bad pointer deref: " __FILE__ \
                                 ":" C_STR(__LINE__)));
#define C_PTR_CHECK_MSG(ptr, msg) \
    if(!ptr)                      \
        Throw(undefined_behavior("bad pointer deref: " msg));

#define C_THIS_CHECK                                              \
    if(!this)                                                     \
        Throw(undefined_behavior("bad access to *this: " __FILE__ \
                                 ":" C_STR(__LINE__)));

#if MODE_DEBUG
#define C_ERROR_CHECK(ec)                                           \
    {                                                               \
        if(ec)                                                      \
            Throw(implementation_error(                             \
                __FILE__ ":" C_STR(__LINE__) ": " + ec.message())); \
    }

#define C_ERROR_CHECK_TYPED(ec, etype)                                      \
    {                                                                       \
        if(ec)                                                              \
            Throw(etype(__FILE__ ":" C_STR(__LINE__) ": " + ec.message())); \
    }

#else
#define C_ERROR_CHECK(ec)
#define C_ERROR_CHECK_TYPED(ec, etype)
#endif

#if MODE_DEBUG
#define CASSERT(condition) \
    if(!(condition))       \
        Throw(undefined_behavior("assertion failed: " C_STR(condition)));
#else
#define CASSERT(condition)
#endif

using stl_types::non_copy;
