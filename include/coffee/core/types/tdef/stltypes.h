#pragma once

#include "../../plat/plat_quirks_toggling.h"
#include "../../coffee_mem_macros.h"

/*Container types*/
#include <string>
#include <queue>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <array>

/*Memory management*/
#include <atomic>
#include <mutex>
#include <memory>

/*Shit*/
#include <exception>
#include <complex>
#include <type_traits>

#if defined(COFFEE_GEKKO)
#include <gccore.h>
#endif

namespace Coffee{

using CString   = std::string; /*!< Typical string object */
using CWString  = std::wstring; /*!< Typical string object suited for interfaces */


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
using Mutex = std::mutex;
using Lock  = std::lock_guard<Mutex>;
#endif

using ErrCode = std::error_code;

template<typename T>
using Atomic = std::atomic<T>;

using AtomicInt8 = std::atomic_int_fast8_t;
using AtomicInt16 = std::atomic_int_fast16_t;
using AtomicInt32 = std::atomic_int;
using AtomicInt64 = std::atomic_int_fast64_t;

using AtomicUInt8 = std::atomic_uint_fast8_t;
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
using Map = std::map<T1,T2>;

template<typename T1, typename T2>
using MultiMap = std::multimap<T1,T2>;

template<typename T>
using ShPtr = std::shared_ptr<T>;

template<typename T,class Deleter = std::default_delete<T>>
using UqPtr = std::unique_ptr<T,Deleter>;

template<typename T>
using Complex = std::complex<T>;

template<typename T>
using Set = std::set<T>;

template<typename T1, typename T2>
using Pair = std::pair<T1,T2>;

using ByteVector = Vector<uint8_t>;

//template<typename Tag, typename T>
//using Iterator = std::iterator<Tag,T>;

template<typename T>
using NonConst = typename std::remove_const<T>::type;

template<typename Tag, typename T>
struct Iterator
{
	typedef NonConst<T> value_type;

	typedef Tag iterator_category;
	typedef std::ptrdiff_t difference_type;
	typedef T* pointer;
	typedef T& reference;
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
    return  UqPtr<T>(ptr);
}
template<typename T, class Deleter, typename... Args>
inline UqPtr<T,Deleter> MkUqDST(Args... a)
{
    return UqPtr<T, Deleter>(new T(a...));
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
        static const constexpr T npos = std::numeric_limits<T>::max();

        iterator():
            m_idx(npos)
        {
        }

        iterator(T start, T end):
            m_idx(start),
            m_end(end)
        {
            bool correct = (!range_param::reversed) && start > end;
            bool correct_rev = range_param::reversed && start > end;

            if(correct && correct_rev)
                throw std::out_of_range("invalid range");

            if(start == end)
                m_idx = npos;
        }

        iterator& operator++()
        {
            if(range_param::reversed)
                m_idx --;
            else
                m_idx ++;

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

    range(T len):
        m_len(len)
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

}

struct resource_error : std::runtime_error
{
    using std::runtime_error::runtime_error;
};
