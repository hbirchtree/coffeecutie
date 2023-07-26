#pragma once

#include <coffee/core/internal_state.h>
#include <peripherals/enum/helpers.h>
#include <peripherals/error/result.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/thread_types.h>
#include <peripherals/stl/time_types.h>

namespace rq {
namespace detail {

using clock      = std::chrono::steady_clock;
using time_point = clock::time_point;
using duration   = clock::duration;

using thread          = std::thread;
using thread_id       = stl_types::ThreadId::Hash;
using mutex           = stl_types::Mutex;
using recursive_mutex = stl_types::RecMutex;

template<typename T = std::mutex>
using lock_guard = std::lock_guard<T>;
template<typename T = std::mutex>
using unique_lock = std::unique_lock<T>;

template<typename T, typename E>
using result = stl_types::result<T, E>;

using stl_types::failure;
using stl_types::success;

inline thread_id current_thread_id()
{
    return std::hash<thread::id>()(std::this_thread::get_id());
}

inline void on_thread_created()
{
    Coffee::State::SetInternalThreadState(
        Coffee::State::CreateNewThreadState());
}

template<typename R, typename... Args>
requires std::is_same_v<R, void>
    //
    STATICINLINE void set_promise_value(
        std::promise<void>&           out,
        std::function<void(Args...)>& fun,
        Args... args)
{
    fun(args...);
    out.set_value();
}

template<typename R, typename... Args>
requires(!std::is_same_v<R, void>)
    //
    STATICINLINE void set_promise_value(
        std::promise<R>& out, std::function<R(Args...)>& fun, Args... args)
{
    out.set_value(fun(args...));
}

} // namespace detail

using libc_types::u32;
using libc_types::u64;

struct runtime_queue_error : std::runtime_error
{
    runtime_queue_error(std::string_view error) :
        std::runtime_error(std::string(error.begin(), error.end()))
    {
    }
};

enum class RuntimeQueueError
{
    ThreadSpawn = 1,   /*!< Upon failing to spawn a task thread */
    UncaughtException, /*!< Upon a thread dying from uncaught exception, it hits
                        a breakpad. Does not stop SIGSEGV from killing the
                        process. */
    ExpiredTaskDeadline, /*!< When a submitted SingleShot task is scheduled to
                            run at an expired time */
    InvalidTaskLifetime, /*!< Upon attempting to wait for a ::Periodic task */
    InvalidTaskFlags,    /*!< Upon attempting to ::Queue() a task which has an
                            invalid combination of flags */
    InvalidTaskId, /*!< When functions taking task IDs cannot find a task */
    IncompatibleTaskAwait, /*!< If ::Await() is called on a task that is
                              periodic, this is triggered */
    TaskAlreadyBlocked,    /*!< If ::Block() is called on a task that is already
                              blocked */
    TaskAlreadyStarted, /*!< If ::Unblock() is called on a task that is already
                           running */
    InvalidQueue, /*!< When any function is given a ThreadId which does not
                     point to a running RuntimeQueue */

    SameThread, /*!< Function has no effect on same thread */

    QueueCreationFailed, /*!< Failed to create queue */

    ShuttingDown /*!< Queues are shutting down, killing threads */
};

using RuntimeQueueVerboseError = std::pair<RuntimeQueueError, std::string>;

std::string_view to_string(RuntimeQueueError error);

enum class task_flags
{
    parallel    = 0x1,
    single_shot = 0x2,
    periodic    = 0x4,
    immediate   = 0x8,
};

C_FLAGS(task_flags, u32);

struct runtime_task
{
    STATICINLINE runtime_task CreateTask(
        std::function<void()>&& fun,
        task_flags              flags          = task_flags::single_shot,
        detail::time_point      scheduled_time = {})
    {
        return runtime_task{
            .task  = std::move(fun),
            .time  = scheduled_time,
            .flags = flags,
        };
    }

    template<task_flags Flags, typename T>
    STATICINLINE std::pair<runtime_task, std::future<T>> CreateTask(
        std::function<T()>&& fun, detail::time_point scheduled_time = {})
    {
        static_assert(
            static_cast<int>(Flags) & static_cast<int>(task_flags::single_shot),
            "only single-shot tasks supported");

        std::promise<T> result;
        auto            handle = result.get_future();
        return {
            runtime_task{
                .task
                = [fun    = std::move(fun),
                   result = std::move(result)] { result.set_value(fun()); },
                .time  = scheduled_time,
                .flags = Flags,
            },
            handle,
        };
    }

    template<typename T1, typename T2>
    STATICINLINE runtime_task
    CreateTask(std::future<T2>&& dependency, std::function<T1()>&& fun);

    STATICINLINE runtime_task CreateTask(
        std::function<void()>&& fun,
        task_flags              flags      = task_flags::single_shot,
        detail::duration        time_until = {})
    {
        return runtime_task{
            .task     = std::move(fun),
            .interval = time_until,
            .flags    = flags,
        };
    }

    std::function<void()> task{};
    detail::time_point    time{};
    detail::duration      interval{};
    task_flags            flags{task_flags::single_shot};
    u32                   _pad{};

    FORCEDINLINE bool operator<(runtime_task const& other) const
    {
        return time < other.time;
    }
};

struct dependent_task_invoker
{
    virtual ~dependent_task_invoker();
    virtual bool ready()   = 0;
    virtual void execute() = 0;
};

template<typename Dependency, typename Out>
struct dependent_task : public dependent_task_invoker
{
    virtual ~dependent_task() = default;

    STATICINLINE auto CreateProcessor(
        std::future<Dependency>&&         dependency,
        std::function<Out(Dependency*)>&& task)
    {
        if(!dependency.valid())
            Throw(std::runtime_error("dependent_task: invalid dependency"));
        auto out        = std::make_unique<dependent_task>();
        out->task       = std::move(task);
        out->dependency = std::move(dependency);
        return out;
    }

    STATICINLINE auto CreateSource(std::function<Out()>&& task)
    {
        auto out        = std::make_unique<dependent_task>();
        out->task       = [task = std::move(task)](void*) { return task(); };
        out->dependency = {};
        return out;
    }

    STATICINLINE auto CreateSink(
        std::future<Dependency>&&          dependency,
        std::function<void(Dependency*)>&& task)
    {
        auto out        = std::make_unique<dependent_task>();
        out->task       = std::move(task);
        out->dependency = std::move(dependency);
        return out;
    }

    virtual bool ready() override final
    {
        if(!dependency.valid())
            return true;
        using namespace std::chrono_literals;
        return dependency.wait_for(0ms) == std::future_status::ready;
    }

    virtual void execute() override final
    {
        (*this)();
    }

    template<typename Dummy = void>
    requires(!std::is_same_v<Dependency, void>) void operator()()
    {
        Dependency input = dependency.get();
        detail::set_promise_value<Out, Dependency*>(output, task, &input);
    }

    template<typename Dummy = void>
    requires std::is_same_v<Dependency, void>
    void operator()()
    {
        if(dependency.valid())
            dependency.get();
        detail::set_promise_value<Out, void*>(output, task, nullptr);
    }

    std::function<Out(Dependency*)> task{};
    std::future<Dependency>         dependency;
    std::promise<Out>               output;
};

class runtime_queue
{
  public:
    struct semaphore_t
    {
        std::condition_variable condition;
        std::mutex              mutex;
        std::atomic_bool        running;
    };

    struct QueueContext
    {
        QueueContext()
        {
            shutdown_flag.store(false);
        }
        ~QueueContext()
        {
            if(auto error = runtime_queue::TerminateThreads())
                (void)error;
        }

        std::mutex       global_lock;
        std::atomic_bool shutdown_flag;

        std::map<u64, runtime_queue>  queues;
        std::map<u64, detail::thread> queue_threads;
        /*!
         * \brief Contains all data necessary to manage a worker thread.
         * Needs to be a std::shared_ptr<T> in order to avoid early destruction.
         */
        std::map<u64, std::shared_ptr<semaphore_t>> queue_flags;
    };

    using queue_context_ptr = std::shared_ptr<QueueContext>;

    static detail::result<runtime_queue*, RuntimeQueueError> CreateNewQueue(
        std::string_view name);
    static detail::result<runtime_queue*, RuntimeQueueVerboseError>
    CreateNewThreadQueue(std::string_view name);

    static detail::result<runtime_queue*, RuntimeQueueError> GetCurrentQueue();

    /*!
     * \brief From inside a task, get a reference to itself
     * \return
     */
    static detail::result<runtime_task*, RuntimeQueueError> GetSelf();

    /*!
     * \brief From inside a task, get its ID. Calling from outside will return
     * 0. \return
     */
    static detail::result<u64, RuntimeQueueError> GetSelfId();

    /*!
     * \brief Enqueue a task into the current thread's runtime queue
     * For SingleShot tasks, if the time in the future is in the past
     *  upon validation, the task will be killed before queueing.
     * This behavior is so to keep deadlines.
     * Periodic tasks will simply be re-scheduled.
     * By adding ::Immediate flag to a task, deadlines will be "ignored",
     *  allowing a task to be executed even if delayed.
     * \param task Task to be run with its internal parameters
     * \return 0 on failure, non-zero value handle otherwise.
     */
    static detail::result<u64, RuntimeQueueError> Queue(runtime_task&& task);
    /*!
     * \brief Like Queue(RuntimeTask&&), but targets a thread.
     * Queue(RuntimeTask&&) uses this function behind the scenes.
     * Same semantics apply.
     * \param targetThread
     * \param task
     * \return
     */
    static detail::result<u64, RuntimeQueueError> Queue(
        detail::thread_id targetThread, runtime_task&& task);

    /*!
     * \brief Queue
     * \param queue
     * \param task
     * \param ec
     * \return
     */
    static detail::result<u64, RuntimeQueueError> Queue(
        runtime_queue* queue, runtime_task&& task);

    static detail::result<u64, RuntimeQueueError> Queue(
        runtime_queue* queue, std::unique_ptr<dependent_task_invoker>&& task);

    static detail::result<u64, RuntimeQueueError> Queue(
        std::unique_ptr<dependent_task_invoker>&& task);

    /*!
     * \brief Queue a single-shot task, without the effort
     * \param q Target queue, must be a valid, non-null queue
     * \param time Targeted time in the future
     * \param task Task to be run
     * \return
     */
    STATICINLINE detail::result<u64, RuntimeQueueError> QueueShot(
        runtime_queue* q, detail::duration time, std::function<void()>&& task)
    {
        if(!q)
            return 0;
        return Queue(
            q->thread_id(),
            runtime_task::CreateTask(
                std::move(task),
                task_flags::single_shot,
                detail::clock::now() + time));
    }

    /*!
     * \brief For running a task on a thread, running it
     *  immediately if possible.
     * This is very handy for thread-bound contexts.
     * \param q
     * \param task
     * \return
     */
    STATICINLINE detail::result<u64, RuntimeQueueError> QueueEnsureThread(
        runtime_queue* q, std::function<void()>&& task, bool await = false)
    {
        if(!q)
        {
            return detail::failure(RuntimeQueueError::InvalidQueue);
        }

        /* If we are on the desired thread, run it now */
        if(detail::current_thread_id() == q->thread_id())
        {
            task();
            return detail::success(0);
        }

        /* Otherwise queue it */
        if(auto res = Queue(
               q->thread_id(),
               runtime_task::CreateTask(
                   std::move(task),
                   task_flags::single_shot | task_flags::immediate,
                   detail::clock::now()));
           res.has_error())
            return detail::failure(res.error());
        else if(await)
        {
            if(auto error = AwaitTask(q->thread_id(), res.value()))
                return detail::failure(*error);
            else
                return detail::success(res.value());
        } else
            return detail::success(res.value());
    }

    /*!
     * \brief Like QueueShot, but will run as soon as possible. The difference
     * is that this one ignores any possible deadlines. (QueueShot will kill the
     * task if the deadline is not met)
     * \param q
     * \param task
     * \return
     */
    STATICINLINE detail::result<u64, RuntimeQueueError> QueueImmediate(
        runtime_queue* q, detail::duration time, std::function<void()>&& task)
    {
        if(!q)
            return detail::failure(RuntimeQueueError::InvalidQueue);
        return Queue(
            q->thread_id(),
            runtime_task::CreateTask(
                std::move(task),
                task_flags::single_shot | task_flags::immediate,
                detail::clock::now() + time));
    }

    [[nodiscard]] STATICINLINE detail::result<u64, RuntimeQueueError>
                               QueuePeriodic(
                                   runtime_queue* q, detail::duration time, std::function<void()>&& task)
    {
        if(!q)
            return detail::failure(RuntimeQueueError::InvalidQueue);
        return Queue(
            q->thread_id(),
            {
                task,
                {},
                time,
                task_flags::periodic,
                0,
            });
    }

    template<typename... Args>
    STATICINLINE std::function<void(Args...)> BindToQueue(
        std::function<void(Args...)> const& func)
    {
        using namespace std::chrono_literals;

        auto                         queue = GetCurrentQueue().value();
        std::function<void(Args...)> out
            = [queue, func = std::move(func)](Args... args) {
                  std::function<void()> task = [func, args...]() mutable {
                      func(std::forward<Args>(args)...);
                  };
                  auto res = Queue(
                      queue,
                      runtime_task::CreateTask(
                          std::move(task), task_flags::single_shot, 0ms));
                  if(res.has_error())
                      throw rq::runtime_queue_error(
                          "failed to bind function to thread");
              };
        return out;
    }

    struct task_data_t
    {
        runtime_task task;
        u64          index;
        union
        {
            struct
            {
                bool alive : 1;
                bool to_dispose : 1;
            };
        };

        char _pad[7];

        FORCEDINLINE bool operator<(task_data_t const& other) const
        {
            return task < other.task;
        }
    };

    struct dependent_task_data_t
    {
        std::unique_ptr<dependent_task_invoker> task;
        bool                                    alive{true};
    };

    STATICINLINE std::optional<RuntimeQueueError> Block(u64 taskId)
    {
        return Block(detail::current_thread_id(), taskId);
    }
    static std::optional<RuntimeQueueError> Block(
        detail::thread_id targetThread, u64 taskId);

    STATICINLINE std::optional<RuntimeQueueError> Unblock(u64 taskId)
    {
        return Unblock(detail::current_thread_id(), taskId);
    }
    static std::optional<RuntimeQueueError> Unblock(
        detail::thread_id targetThread, u64 taskId);

    STATICINLINE std::optional<RuntimeQueueError> CancelTask(u64 taskId)
    {
        return CancelTask(detail::current_thread_id(), taskId);
    }
    static std::optional<RuntimeQueueError> CancelTask(
        detail::thread_id targetThread, u64 taskId);

    static std::optional<RuntimeQueueError> AwaitTask(
        detail::thread_id targetThread, u64 taskId);

    static detail::result<bool, RuntimeQueueError> IsRunning(
        runtime_queue* thread);

    static std::optional<RuntimeQueueError> TerminateThread(
        runtime_queue* thread);
    static std::optional<RuntimeQueueError> TerminateThreads();

    void              execute_tasks();
    detail::duration  time_till_next() const;
    detail::duration  time_till_next(detail::time_point clock) const;
    std::string_view  name();
    detail::thread_id thread_id() const;

    /*!
     * \brief Create a new thread context, do nothing else.
     * \return
     */
    static queue_context_ptr CreateContext();
    /*!
     * \brief Set queue context if and only if there is none from before.
     * If one already exists, we cannot control it.
     * \param i
     */
    static bool SetQueueContext(queue_context_ptr i);
    /*!
     * \brief Return the current context, regardless of whether it exists.
     * \return
     */
    static queue_context_ptr GetQueueContext();

    detail::time_point   m_next_wakeup{};
    std::recursive_mutex m_tasks_lock;

  private:
    u64  enqueue(runtime_task&& task);
    u64  enqueue(std::unique_ptr<dependent_task_invoker>&& task);
    void sortTasks();

    std::vector<task_data_t>           m_tasks;
    std::vector<dependent_task_data_t> m_dependent_tasks;
    detail::thread_id                  m_thread_id{0};
    u64                                m_task_index{0};
    u64                                m_current_task_id{0};

    static std::shared_ptr<QueueContext> context;
};

struct scoped_task
{
    scoped_task() : m_id(0)
    {
    }

    template<typename... Args>
    scoped_task(
        detail::thread_id tid, std::function<void()>&& fun, Args... args)
    {
        if(auto res = runtime_queue::Queue(
               tid, runtime_task::CreateTask(std::move(fun), args...));
           res.has_error())
            Throw(runtime_queue_error(to_string(res.error())));
        else
            m_id = res.value();
        m_thread_id = tid;
    }

    scoped_task(detail::thread_id tid, runtime_task&& task)
    {
        if(auto res = runtime_queue::Queue(tid, std::move(task));
           res.has_error())
            Throw(runtime_queue_error(to_string(res.error())));
        else
            m_id = res.value();

        m_thread_id = tid;
    }

    scoped_task(scoped_task&& other) :
        m_id(other.m_id), m_thread_id(other.m_thread_id)
    {
        other.m_id = 0;
    }

    C_DELETE_COPY_CONSTRUCTOR(scoped_task);

    ~scoped_task()
    {
        if(m_id != 0)
            runtime_queue::CancelTask(m_thread_id, m_id);
    }

    scoped_task& operator=(scoped_task&& other)
    {
        m_id        = other.m_id;
        m_thread_id = other.m_thread_id;

        other.m_id        = 0;
        other.m_thread_id = {};

        return *this;
    }

    u64 id() const
    {
        return m_id;
    }
    detail::thread_id threadId() const
    {
        return m_thread_id;
    }

  private:
    u64               m_id;
    detail::thread_id m_thread_id;
};

} // namespace rq
