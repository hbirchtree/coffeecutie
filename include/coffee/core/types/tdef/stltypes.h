#pragma once

/*Container types*/
#include <string>
#include <queue>
#include <list>
#include <vector>
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
using CStdFault = std::runtime_error; /*!< Exception to be thrown by default */

using Mutex = std::mutex;
using Lock  = std::lock_guard<Mutex>;

template<typename T>
using Atomic = std::atomic<T>;

using AtomicInt8 = std::atomic_int_fast8_t;
using AtomicInt16 = std::atomic_int_fast16_t;
using AtomicInt32 = std::atomic_int_fast32_t;
using AtomicInt64 = std::atomic_int_fast64_t;

using AtomicUInt8 = std::atomic_uint_fast8_t;
using AtomicUInt16 = std::atomic_uint_fast16_t;
using AtomicUInt32 = std::atomic_uint_fast32_t;
using AtomicUInt64 = std::atomic_uint_fast64_t;

using AtomicBool = std::atomic_bool;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using LinkList = std::list<T>;

template<typename T>
using Vector = std::vector<T>;

template<typename T1, typename T2>
using Map = std::map<T1,T2>;

template<typename T1, typename T2>
using MultiMap = std::multimap<T1,T2>;

template<typename T>
using ShPtr = std::shared_ptr<T>;

template<typename T>
using Complex = std::complex<T>;

}