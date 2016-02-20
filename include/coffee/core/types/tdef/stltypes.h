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

namespace Coffee{

using CString   = std::string; /*!< Typical string object */
using CStdFault = std::runtime_error; /*!< Exception to be thrown by default */

using Mutex = std::mutex;
using Lock  = std::lock_guard<Mutex>;

template<typename T>
using Atomic = std::atomic<T>;

template<typename T>
using Queue = std::queue<T>;

template<typename T>
using LinkList = std::list<T>;

template<typename T>
using Vector = std::vector<T>;

template<typename T1, typename T2>
using Map = std::map<T1,T2>;

template<typename T>
using ShPtr = std::shared_ptr<T>;

}