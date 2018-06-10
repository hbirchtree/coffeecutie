#pragma once

#include <functional>
#include <future>

#if defined(COFFEE_APPLE)
#include <pthread.h>
#endif

#include "thread_id.h"
#include "async_tasks.h"
#include "job_system.h"
