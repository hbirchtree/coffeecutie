#pragma once

#include <coffee/core/debug/logging.h>
#include <peripherals/libc/types.h>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <pthread.h>
#include <string>
#include <thread>
#include <vector>

namespace compo::sched {

/*!
 * \brief Threads running frame work in lock-step with their owner.
 */
struct worker_pool
{
    using job_t  = std::function<void()>;
    using job_id = libc_types::szptr;

    explicit worker_pool(libc_types::szptr workers)
    {
        m_threads.reserve(workers);
        for(libc_types::szptr i = 0; i < workers; i++)
            m_threads.emplace_back([this]() { worker_main(); });
    }

    ~worker_pool()
    {
        {
            std::lock_guard _(m_lock);
            m_stopping = true;
        }
        m_wake.notify_all();

        for(auto& thread : m_threads)
            if(thread.joinable())
                thread.join();
    }

    worker_pool(worker_pool const&)            = delete;
    worker_pool& operator=(worker_pool const&) = delete;

    libc_types::szptr size() const
    {
        return m_threads.size();
    }

    /*! Hand a job to the pool. Runs inline when there are no workers. */
    job_id submit(job_t&& job)
    {
        if(m_threads.empty())
        {
            run_inline(job);

            m_jobs.push_back({{}, true});
            m_next = m_jobs.size();
            return m_jobs.size() - 1u;
        }

        job_id id{0};
        {
            std::lock_guard _(m_lock);
            m_jobs.push_back({std::move(job), false});
            id = m_jobs.size() - 1u;
        }
        m_wake.notify_one();
        return id;
    }

    /*! Return once `id` has finished, running pending jobs meanwhile. */
    void wait(job_id id)
    {
        if(m_threads.empty())
            return;

        std::unique_lock lock(m_lock);

        while(id < m_jobs.size() && !m_jobs.at(id).done)
        {
            if(run_one(lock))
                continue;
            m_finished.wait(lock);
        }
    }

    /*! Return once every submitted job has finished. */
    void wait_all()
    {
        if(m_threads.empty())
            return;

        std::unique_lock lock(m_lock);

        while(true)
        {
            if(run_one(lock))
                continue;
            if(m_running == 0)
                return;
            m_finished.wait(lock);
        }
    }

    /*! Drop bookkeeping for finished jobs. Call between frames. */
    void reset()
    {
        std::lock_guard _(m_lock);
        m_jobs.clear();
        m_next = 0;
    }

    /*! Jobs that ended in an exception, over the pool's lifetime. */
    libc_types::szptr failed() const
    {
        std::lock_guard _(m_lock);
        return m_failed;
    }

  private:
    struct job_slot
    {
        job_t fn;
        bool  done{false};
    };

    static void report_failure(std::string const& what)
    {
        Coffee::Logging::log(
            libc::io::io_handles::err,
            "Coffee::Components",
            "subsystem threw on a worker thread: " + what,
            semantic::debug::Severity::Critical);
    }

    void run_inline(job_t& job)
    {
        try
        {
            job();
        } catch(std::exception const& e)
        {
            m_failed++;
            report_failure(e.what());
        }
    }

    /*! Claim and run one job, with m_lock held on entry and exit.
     *  Returns false when there was nothing left to claim. */
    bool run_one(std::unique_lock<std::mutex>& lock)
    {
        if(m_next >= m_jobs.size())
            return false;

        auto const index = m_next++;
        auto       fn    = std::move(m_jobs.at(index).fn);
        m_running++;

        lock.unlock();

        bool        failed = false;
        std::string message;
        try
        {
            fn();
        } catch(std::exception const& e)
        {
            failed  = true;
            message = e.what();
        } catch(...)
        {
            failed  = true;
            message = "unknown exception";
        }

        /* Reported outside the lock: logging can be slow, and the frame is
         * already broken by the time we get here */
        if(failed)
            report_failure(message);

        lock.lock();

        m_running--;
        m_failed += failed ? 1u : 0u;
        m_jobs.at(index).done = true;

        m_finished.notify_all();

        return true;
    }

    void worker_main()
    {
#if defined(__linux__)
        sched_param sched{};
        pthread_setschedparam(pthread_self(), SCHED_IDLE, &sched);
#endif
        std::unique_lock lock(m_lock);
        while(true)
        {
            m_wake.wait(lock, [this]() {
                return m_stopping || m_next < m_jobs.size();
            });

            if(m_stopping)
                return;

            run_one(lock);
        }
    }

    mutable std::mutex       m_lock;
    std::condition_variable  m_wake;     /*!< a job became available */
    std::condition_variable  m_finished; /*!< a job completed */
    std::vector<job_slot>    m_jobs;
    libc_types::szptr        m_next{0};
    libc_types::szptr        m_running{0};
    libc_types::szptr        m_failed{0};
    bool                     m_stopping{false};
    std::vector<std::thread> m_threads;
};

} // namespace compo::sched
