#pragma once

#include "stldef.h"

/*Container types*/
#include <string>
#include <queue>
#include <list>
#include <vector>
#include <set>
#include <map>

/*Memory management*/
#include <atomic>
#include <mutex>
#include <memory>

/*Shit*/
#include <exception>
#include <complex>

namespace Coffee{

using CString   = std::string; /*!< Typical string object */
using CWString   = std::wstring; /*!< Typical string object suited for interfaces */

using CStdFault = std::runtime_error; /*!< Exception to be thrown by default */

using Mutex = std::mutex;
using Lock  = std::lock_guard<Mutex>;

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

template<typename Tag, typename T>
using Iterator = std::iterator<Tag,T>;

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
template<typename T, class Deleter, typename... Args>
inline UqPtr<T,Deleter> MkUqDST(Args... a)
{
    return UqPtr<T, Deleter>(new T(a...));
}

struct Range : Vector<size_t>
{
    Range(size_t len) : Vector<size_t>()
    {
        reserve(len);
        for(size_t i = 0; i<len; i++)
            push_back(i);
    }
};


}
