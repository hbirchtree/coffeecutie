#include <coffee/core/task_queue/task.h>

#include <algorithm>
#include <coffee/core/CProfiling>
#include <coffee/core/internal_state.h>

#include <peripherals/stl/range.h>
#include <platforms/stacktrace.h>

#include <coffee/core/CDebug>

#define RQ_API "runtime_queue::"

using Profiler     = Coffee::Profiler;
using DProfContext = Coffee::DProfContext;

namespace rq {

using RQE  = RuntimeQueueError;
using RQVE = RuntimeQueueVerboseError;

runtime_queue::queue_context_ptr runtime_queue::context;

namespace {

std::function<rq::detail::time_point()> clock_now = []() {
    return detail::clock::now();
};

}

runtime_queue* runtime_queue::find_queue(detail::thread_id id)
{
    std::shared_lock _(context->global_lock);

    auto it = context->queues.find(id);
    if(it == context->queues.end())
        return nullptr;
    return &(*it).second;
}

std::string_view to_string(RuntimeQueueError error)
{
    using namespace std::string_view_literals;
    switch(error)
    {
    case RQE::ExpiredTaskDeadline:
        return "RuntimeTask's deadline is already expired"sv;
    case RQE::IncompatibleTaskAwait:
        return "Provided RuntimeTask cannot be awaited"sv;
    case RQE::InvalidQueue:
        return "Invalid reference to runtime_queue"sv;
    case RQE::InvalidTaskFlags:
        return "Invalid combination of RuntimeTask flags"sv;
    case RQE::InvalidTaskId:
        return "Invalid reference to RuntimeTask"sv;
    case RQE::InvalidTaskLifetime:
        return "RuntimeTask cannot be awaited"sv;
    case RQE::TaskAlreadyBlocked:
        return "Attempt to block already blocked task"sv;
    case RQE::TaskAlreadyStarted:
        return "Attempt to unblock already running task"sv;
    case RQE::ThreadSpawn:
        return "Failed to spawn dedicated task thread"sv;
    case RQE::UncaughtException:
        return "Thread died from uncaught exception"sv;
    case RQE::SameThread:
        return "Cannot be performed on same thread"sv;
    case RQE::QueueCreationFailed:
        return "Failed to create queue"sv;
    case RQE::ShuttingDown:
        return "Operation unavailable on shutdown"sv;
    }

    throw implementation_error("unimplemented error message");
}

static std::optional<RQE> VerifyTask(runtime_task const& t)
{
    if(enum_helpers::feval(
           t.flags, task_flags::single_shot | task_flags::periodic))
        return RQE::InvalidTaskFlags;
    if(enum_helpers::feval(t.flags, task_flags::single_shot) &&
       enum_helpers::feval(t.flags, task_flags::periodic))
        return RQE::InvalidTaskFlags;
    if(enum_helpers::feval(t.flags, task_flags::single_shot) &&
       !enum_helpers::feval(t.flags, task_flags::immediate) &&
       t.time < clock_now())
        return RQE::ExpiredTaskDeadline;
    return std::nullopt;
}

/*!
 * \brief We can look, but we cannot touch. This is used to
 *  inspect RuntimeTasks as an observer. Only the controlling
 *  thread may actually modify them.
 * \param ids
 * \param tasks
 * \param taskId
 * \return
 */
static detail::result<std::pair<runtime_task const*, size_t>, RQE> GetTask(
    std::vector<runtime_queue::task_data_t> const& tasks, u64 taskId)
{
    /* Locate the task in our vector */
    for(size_t i = 0; i < tasks.size(); i++)
        if(tasks[i].index == taskId)
            return detail::success(std::pair{&tasks[i].task, i});
    return detail::failure(RQE::InvalidTaskId);
}

static void NotifyThread(
    std::shared_ptr<runtime_queue::QueueContext> context,
    detail::thread_id                            threadId,
    std::optional<detail::duration>   previousDeadline = std::nullopt,
    std::optional<detail::time_point> currentBase      = std::nullopt)
{
    C_PTR_CHECK(context);

    auto threadFlags = context->queue_flags.find(threadId);
    auto queueRef    = context->queues.find(threadId);

    if(threadFlags == context->queue_flags.end() ||
       queueRef == context->queues.end())
        return;

    C_PTR_CHECK(threadFlags->second);

    /* We want to notify when:
     * - A task is scheduled to run sooner than the thread will wake up
     * - A task that will run is cancelled, and sleep can be rescheduled
     *
     * In essence, if our new event comes first in the timeline,
     *  we need to reschedule the worker.
     */

    auto& condition = threadFlags->second->condition;
    auto& notified  = threadFlags->second->notified;
    auto& queue     = queueRef->second;

    auto wakeupTime = queue.time_till_next(currentBase.value_or(clock_now()));

    if(wakeupTime <= previousDeadline.value_or(wakeupTime))
    {
        Profiler::DeepProfile(RQ_API "Notifying thread");
        notified.store(true);
        condition.notify_one();
    } else
        Profiler::DeepProfile(RQ_API "Skipping thread");
}

detail::result<runtime_queue*, RuntimeQueueError> runtime_queue::CreateNewQueue(
    std::string_view name)
{
    C_PTR_CHECK(context);

    std::unique_lock _(context->global_lock);

    auto t_id = detail::current_thread_id();
    if(auto q_it = context->queues.find(t_id); q_it == context->queues.end())
    {
        auto [_, inserted] = context->queues.try_emplace(t_id);
        if(!inserted)
            return detail::failure(RuntimeQueueError::QueueCreationFailed);
        stl_types::CurrentThread::SetName(
            std::string(name.begin(), name.end()));
        auto& q       = context->queues.at(t_id);
        q.m_thread_id = t_id;
        return detail::success(&q);
    } else
        return detail::success(&(*q_it).second);
}

STATICINLINE void ThreadQueueSleep(
    runtime_queue*                queue,
    std::unique_lock<std::mutex>& thread_lock,
    runtime_queue::semaphore_t*   sem_)
{
    using namespace std::chrono_literals;

    if(!sem_->running)
        return;

    DProfContext ___(RQ_API "Sleeping");

    detail::duration sleepTime;

    {
        std::unique_lock _(queue->m_tasks_lock);
        auto             currentTime = clock_now();
        sleepTime                    = queue->time_till_next(currentTime);
        queue->m_next_wakeup         = currentTime + sleepTime;
    }

    Profiler::DeepProfile(fmt::format("Sleeping for {}ns", sleepTime.count()));
    sem_->condition.wait_for(thread_lock, sleepTime, [queue, sem_]() {
        return queue->time_till_next(clock_now()) < 20ms ||
               sem_->notified.load();
    });
    if(sem_->notified.load())
        Profiler::DeepProfile("Thread notified");
    else
        Profiler::DeepProfile("Wakeup");
    sem_->notified.store(false);
}

static void ImpCreateNewThreadQueue(
    std::string_view                            name,
    std::shared_ptr<runtime_queue::semaphore_t> sem,
    std::promise<void>                          started)
{
#if !defined(COFFEE_LOWFAT)
    try
    {
#endif
        /* Enable profiler and etc. */
        Coffee::State::SetInternalThreadState(
            Coffee::State::CreateNewThreadState());

        /* First create the queue object... */
        runtime_queue* queue = nullptr;
        if(auto queue_create = runtime_queue::CreateNewQueue(name);
           queue_create.has_error())
        {
            Coffee::cWarning(
                RQ_API "Failed to create queue: {0}",
                to_string(queue_create.error()));
        } else
            queue = queue_create.value();

        /* Then notify our parent that everything is done */
        started.set_value();

        /* We use a mutex to allow our parent to notify us of work
         *  or changes in the queue, to allow rescheduling. */
        std::unique_lock thread_lock(sem->mutex);

        {
            DProfContext _(RQ_API "Running queue");
            while(sem->running)
            {
                queue->execute_tasks();
                ThreadQueueSleep(queue, thread_lock, sem.get());
            }
        }
#if !defined(COFFEE_LOWFAT)
    } catch(std::exception const& e)
    {
        Coffee::cWarning(RQ_API "Error encountered: {0}", e.what());
    }
#endif

    //    if(ec)
    //        cWarning(
    //            RQ_API "Uncaught exception!\n"
    //                   "\n"
    //                   "{0}: {1}"
    //                   "\n",
    //            ec.message(),
    //            ec.error_message);
}

detail::result<runtime_queue*, RuntimeQueueVerboseError> runtime_queue::
    CreateNewThreadQueue(std::string_view name)
{
    using namespace std::chrono_literals;

    constexpr auto thread_timeout =
        compile_info::platform::is_emscripten ? 200ms : 10ms;

    C_PTR_CHECK(context);

    try
    {
        DProfContext _(DTEXT(RQ_API "Creating new task queue"));

        std::shared_ptr<semaphore_t> sem = std::make_shared<semaphore_t>();
        std::promise<void>           thread_started;
        std::future<void> thread_started_signal = thread_started.get_future();

        sem->running.store(true);

        /* Spawn the thread */
        detail::thread worker(
            [name, sem, started = std::move(thread_started)]() mutable {
                ImpCreateNewThreadQueue(name, sem, std::move(started));
            });
        auto tid = std::hash<detail::thread::id>()(worker.get_id());

        /* Wait for the runtime_queue to be created on the thread */
        if(thread_started_signal.wait_for(10ms) != std::future_status::ready)
        {
            Coffee::cWarning("Creation of thread {} timed out", name);
            return RuntimeQueueVerboseError{
                RQE::ThreadSpawn, "thread creation timed out"};
        }

        {
            std::unique_lock _(context->global_lock);

            context->queue_threads[tid] = std::move(worker);
            context->queue_flags.insert({tid, sem});
        }

        auto& q       = context->queues.at(tid);
        q.m_thread_id = tid;
        return &q;
    } catch(std::exception const& e)
    {
        return detail::failure(RQVE{RQE::ThreadSpawn, e.what()});
    }
}

void runtime_queue::OverrideClock(std::function<detail::time_point()>&& clock)
{
    clock_now = std::move(clock);
}

detail::time_point runtime_queue::ClockNow()
{
    return clock_now();
}

detail::result<runtime_queue*, RuntimeQueueError> runtime_queue::
    GetCurrentQueue()
{
    C_PTR_CHECK(context);

    std::shared_lock _(context->global_lock);

    auto q_id = detail::current_thread_id();
    auto q_it = context->queues.find(q_id);

    if(q_it != context->queues.end())
        return detail::success(&(*q_it).second);
    else
        return detail::failure(RQE::InvalidQueue);
}

detail::result<runtime_task*, RuntimeQueueError> runtime_queue::GetSelf()
{
    if(auto res = GetCurrentQueue(); res.has_error())
    {
        return res.error();
    } else
    {
        auto* queue = res.value();
        for(auto i : stl_types::Range<>(queue->m_tasks.size()))
            if(queue->m_tasks[i].index == queue->m_current_task_id)
            {
                return &queue->m_tasks[i].task;
            }
    }
    return RQE::InvalidTaskId;
}

detail::result<u64, RuntimeQueueError> runtime_queue::GetSelfId()
{
    if(auto res = GetCurrentQueue(); res.has_error())
        return res.error();
    else
        return res.value()->m_current_task_id;
}

detail::result<u64, RuntimeQueueError> runtime_queue::Queue(runtime_task&& task)
{
    return Queue(detail::current_thread_id(), std::move(task));
}

detail::result<u64, RuntimeQueueError> runtime_queue::Queue(
    detail::thread_id targetThread, runtime_task&& task)
{
    if(auto error = VerifyTask(task))
        return *error;

    if(enum_helpers::feval(task.flags, task_flags::periodic))
        task.time = clock_now() + task.interval;

    std::shared_lock _(context->global_lock);

    auto q_it = context->queues.find(targetThread);

    if(q_it == context->queues.end())
    {
        return RQE::InvalidQueue;
    } else
    {
        return Queue(&q_it->second, std::move(task));
    }
}

detail::result<u64, RuntimeQueueError> runtime_queue::Queue(
    runtime_queue* queue, runtime_task&& task)
{
    if(context->shutdown_flag.load())
    {
        return RQE::ShuttingDown;
    }

    DProfContext _(RQ_API "Adding task to Queue");
    auto&        ref = *queue;

    std::unique_lock __(queue->m_tasks_lock);

    auto currentBase      = clock_now();
    auto previousNextTime = ref.time_till_next(currentBase);

    auto id = ref.enqueue(std::move(task));

    NotifyThread(context, queue->m_thread_id, previousNextTime, currentBase);

    return id;
}

detail::result<u64, RuntimeQueueError> runtime_queue::Queue(
    runtime_queue* queue, std::unique_ptr<dependent_task_invoker>&& task)
{
    if(context->shutdown_flag.load())
        return RQE::ShuttingDown;

    DProfContext _(RQ_API "Adding dependent task to queue");
    auto&        ref = *queue;

    std::unique_lock __(queue->m_tasks_lock);

    auto currentBase      = clock_now();
    auto previousNextTime = ref.time_till_next(currentBase);

    auto id = ref.enqueue(std::move(task));

    NotifyThread(context, queue->m_thread_id, previousNextTime, currentBase);

    return id;
}

detail::result<u64, RuntimeQueueError> runtime_queue::Queue(
    std::unique_ptr<dependent_task_invoker>&& task)
{
    if(auto q = GetCurrentQueue(); q.has_error())
        return detail::failure(RQE::InvalidQueue);
    else
        return Queue(q.value(), std::move(task));
}

std::optional<RuntimeQueueError> runtime_queue::Block(
    detail::thread_id targetThread, u64 taskId)
{
    if(context->shutdown_flag.load())
    {
        return RQE::ShuttingDown;
    }

    DProfContext __(RQ_API "Blocking task");

    runtime_queue* queue = find_queue(targetThread);
    if(!queue)
        return RQE::InvalidQueue;

    if(auto res = GetTask(queue->m_tasks, taskId); res.has_error())
        return res.error();
    else
    {
        auto [task, idx] = res.value();
        /* We do this check in case we are executing in the queue */
        /* Otherwise we deadlock */
        std::unique_lock ___(queue->m_tasks_lock);

        if(!queue->m_tasks[idx].alive)
        {
            return RQE::TaskAlreadyBlocked;
        }

        auto currentBase      = clock_now();
        auto previousNextTime = queue->time_till_next(currentBase);

        queue->m_tasks[idx].alive = false;

        NotifyThread(context, targetThread, previousNextTime, currentBase);
    }

    return std::nullopt;
}

std::optional<RuntimeQueueError> runtime_queue::Unblock(
    detail::thread_id targetThread, u64 taskId)
{
    if(context->shutdown_flag.load())
        return RQE::ShuttingDown;

    DProfContext __(RQ_API "Unblocking task");

    runtime_queue* queue = find_queue(targetThread);
    if(!queue)
        return RQE::InvalidQueue;

    std::unique_lock _(queue->m_tasks_lock);

    if(auto res = GetTask(queue->m_tasks, taskId); res.has_error())
        return res.error();
    else
    {
        auto [task, idx] = res.value();
        if(queue->m_tasks[idx].alive)
            return RQE::TaskAlreadyStarted;

        auto currentBase      = clock_now();
        auto previousNextTime = queue->time_till_next(currentBase);

        queue->m_tasks[idx].alive = true;

        NotifyThread(context, targetThread, previousNextTime, currentBase);
    }
    return std::nullopt;
}

std::optional<RuntimeQueueError> runtime_queue::CancelTask(
    detail::thread_id targetThread, u64 taskId)
{
    if(context->shutdown_flag.load())
        return RQE::ShuttingDown;

    runtime_queue* queue = find_queue(targetThread);
    if(!queue)
        return RQE::InvalidQueue;

    std::unique_lock _(queue->m_tasks_lock);

    for(auto& dependent_task : queue->m_dependent_tasks)
    {
        if(dependent_task.index != taskId)
            continue;
        dependent_task.alive = false;
        return std::nullopt;
    }

    if(auto res = GetTask(queue->m_tasks, taskId); res.has_error())
        return res.error();
    else
    {
        auto [task, idx] = res.value();

        queue->m_tasks[idx].alive      = false;
        queue->m_tasks[idx].to_dispose = true;

        auto currentBase      = clock_now();
        auto previousNextTime = queue->time_till_next(currentBase);

        NotifyThread(context, targetThread, previousNextTime, currentBase);

        return std::nullopt;
    }
}

std::optional<RuntimeQueueError> runtime_queue::AwaitTask(
    detail::thread_id targetThread, u64 taskId)
{
    if(taskId == 0)
        return RQE::InvalidTaskId;

    if(context->shutdown_flag.load())
        return RQE::ShuttingDown;

    if(detail::current_thread_id() == targetThread)
        return RQE::SameThread;

    runtime_queue* queueRef = find_queue(targetThread);
    if(!queueRef)
        return RQE::InvalidQueue;

    if(!queueRef)
        return RuntimeQueueError::InvalidQueue;

    if(auto res = GetTask(queueRef->m_tasks, taskId); res.has_error())
        return res.error();
    else
    {
        auto [task, idx] = res.value();

        /* We cannot reliably await periodic tasks */
        if(enum_helpers::feval(task->flags, task_flags::periodic))
            return RQE::IncompatibleTaskAwait;

        /* Do not await a past event */
        if(!enum_helpers::feval(task->flags, task_flags::immediate) &&
           clock_now() > task->time)
        {
            return RQE::ExpiredTaskDeadline;
        }

        DProfContext _(RQ_API "Awaiting task...");
        stl_types::CurrentThread::sleep_until(task->time);

        {
            DProfContext _(RQ_API "Busy-waiting task");

            /* I know this is bad, but we must await the task */
            auto taskAlive = [&queueRef, taskId] {
                std::unique_lock _(queueRef->m_tasks_lock);
                auto             it = std::find_if(
                    queueRef->m_tasks.begin(),
                    queueRef->m_tasks.end(),
                    [taskId](auto const& task) {
                        return task.index == taskId;
                    });
                if(it == queueRef->m_tasks.end())
                    return false;
                return it->alive;
            };
            while(taskAlive())
                /* TODO: Implement waiting on the next time the queue has
                 * finished a loop */
                stl_types::CurrentThread::yield();
        }
        return std::nullopt;
    }
}

detail::result<bool, RuntimeQueueError> runtime_queue::IsRunning(
    runtime_queue* thread)
{
    if(!thread)
        return RQE::InvalidQueue;
    auto             tid = thread->thread_id();
    std::shared_lock _(context->global_lock);
    return detail::success(context->queue_flags[tid]->running.load());
}

std::optional<RuntimeQueueError> runtime_queue::TerminateThread(
    runtime_queue* thread)
{
    if(!thread)
        return RQE::InvalidQueue;

    auto tid = thread->thread_id();

    std::unique_lock _(context->global_lock);

    auto& queueFlags = context->queue_flags[tid];

    queueFlags->running.store(false);
    queueFlags->condition.notify_one();

    context->queue_threads[tid].join();

    context->queue_flags.erase(tid);
    context->queue_threads.erase(tid);

    return std::nullopt;
}

std::optional<RuntimeQueueError> runtime_queue::TerminateThreads()
{
    context->shutdown_flag.store(true);
    std::unique_lock _(context->global_lock);

    for(auto const& t : context->queue_flags)
    {
        t.second->running.store(false);
        t.second->condition.notify_one();
    }

    for(auto& t : context->queue_threads)
        t.second.join();

    context->queue_flags.clear();
    context->queue_threads.clear();
    context->queues.clear();

    return std::nullopt;
}

runtime_queue::queue_context_ptr runtime_queue::CreateContext()
{
    return std::make_shared<QueueContext>();
}

bool runtime_queue::SetQueueContext(runtime_queue::queue_context_ptr i)
{
    if(context)
        return false;

    context = i;
    return true;
}

runtime_queue::queue_context_ptr runtime_queue::GetQueueContext()
{
    return context;
}

void runtime_queue::execute_tasks()
{
    DProfContext __(DTEXT(RQ_API "Executing thread tasks"));

    Profiler::DeepPushContext(RQ_API "Locking queue");
    std::unique_lock _(m_tasks_lock);
    Profiler::DeepPopContext();

    auto currTime = clock_now();

    auto tasks = std::move(m_tasks);
    for(task_data_t& task : tasks)
    {
        /* If this task has to be run in the future,
         *  all proceeding tasks will do as well */
        if(task.task.time > currTime)
        {
            break;
        }

        /* Skip dead tasks, clean it up later */
        if(!task.alive)
        {
            continue;
        }

        /* In this case we will let it run */
        m_current_task_id = task.index;
        Profiler::DeepPushContext(RQ_API "Running task");
        task.task.task();
        Profiler::DeepPopContext();
        m_current_task_id = 0;

        /* Now, if a task is single-shot, remove it */
        if(enum_helpers::feval(task.task.flags, task_flags::single_shot))
        {
            task.alive      = false;
            task.to_dispose = true;
        } else if(enum_helpers::feval(task.task.flags & task_flags::periodic))
        {
            task.task.time = clock_now() + task.task.interval;
        } else
        {
            Throw(implementation_error("unknown task type"));
        }
    }
    std::move(
        std::begin(m_tasks), std::end(m_tasks), std::back_inserter(tasks));
    m_tasks = std::move(tasks);

    auto dependent_tasks = std::move(m_dependent_tasks);
    for(dependent_task_data_t& task : dependent_tasks)
    {
        if(!task.alive)
            continue;
        if(task.task->cancelled())
        {
            task.alive = false;
            continue;
        }
        if(!task.task->ready())
            continue;

        m_current_task_id = task.index;
        Profiler::DeepPushContext(RQ_API "Running dependent task");
        task.task->execute();
        Profiler::DeepPopContext();
        m_current_task_id = 0;

        task.alive = false;
    }
    std::move(
        std::begin(m_dependent_tasks),
        std::end(m_dependent_tasks),
        std::back_inserter(dependent_tasks));
    m_dependent_tasks = std::move(dependent_tasks);

    // TODO: Do this only occasionally
    {
        auto trimmed_tasks = std::remove_if(
            m_dependent_tasks.begin(),
            m_dependent_tasks.end(),
            [](dependent_task_data_t const& task) { return !task.alive; });
        m_dependent_tasks.erase(trimmed_tasks, m_dependent_tasks.end());
    }
    {
        auto trimmed_tasks = std::remove_if(
            m_tasks.begin(), m_tasks.end(), [](task_data_t const& task) {
                return task.to_dispose;
            });
        m_tasks.erase(trimmed_tasks, m_tasks.end());
    }
}

detail::duration runtime_queue::time_till_next() const
{
    detail::time_point current = clock_now();
    return time_till_next(current);
}

detail::duration runtime_queue::time_till_next(detail::time_point current) const
{
    using namespace std::chrono_literals;
    // Dependent tasks currently need to check on a regular interval
    if(!m_dependent_tasks.empty())
        return 20ms;
    // For regular tasks, look at the time for the first task in the list
    for(auto const& task : m_tasks)
        if(task.alive)
        {
            if(task.task.time < current)
                return 0ms;
            else
                return task.task.time - current;
        }

    Profiler::DeepProfile(RQ_API "Entering deep sleep");
    // If nothing is found, go to deep sleep for a while
    if(m_next_wakeup < current || m_tasks.empty())
        return 10s;
    else
        return m_next_wakeup - current;
}

std::string_view runtime_queue::name()
{
    std::unique_lock _(m_tasks_lock);
    return stl_types::Threads::GetName(m_thread_id);
}

detail::thread_id runtime_queue::thread_id() const
{
    return m_thread_id;
}

size_t runtime_queue::task_count()
{
    std::unique_lock _(m_tasks_lock);
    return std::count_if(
               m_dependent_tasks.begin(),
               m_dependent_tasks.end(),
               [](auto const& task) { return task.alive; }) +
           std::count_if(m_tasks.begin(), m_tasks.end(), [](auto const& task) {
               return task.alive;
           });
}

u64 runtime_queue::enqueue(runtime_task&& task)
{
    std::unique_lock _(m_tasks_lock);
    u64              output = ++m_task_index;
    m_tasks.emplace_back(task_data_t{
        .task  = std::move(task),
        .index = output,
        .alive = true,
        .to_dispose = false,
    });
    sortTasks();
    return output;
}

u64 runtime_queue::enqueue(std::unique_ptr<dependent_task_invoker>&& task)
{
    if(!task)
        Throw(undefined_behavior("nullptr passed as dependent_task!"));
    std::unique_lock _(m_tasks_lock);
    u64 output = ++m_task_index;
    m_dependent_tasks.emplace_back(dependent_task_data_t{
        .task = std::move(task),
        .index = output,
    });
    return output;
}

void runtime_queue::sortTasks()
{
    std::sort(m_tasks.begin(), m_tasks.end());
}

dependent_task_invoker::~dependent_task_invoker()
{
}

} // namespace rq
