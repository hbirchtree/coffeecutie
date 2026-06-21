#pragma once

#include <peripherals/identify/system.h>

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace stl_types {

using RecMutex = std::recursive_mutex;
using Mutex    = std::mutex;
using Lock     = std::lock_guard<Mutex>;
using RecLock  = std::lock_guard<RecMutex>;

using UqLock    = std::unique_lock<Mutex>;
using UqRecLock = std::unique_lock<RecMutex>;
using CondVar   = std::condition_variable;
using cv_status = std::cv_status;

} // namespace stl_types
