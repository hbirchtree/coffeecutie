#pragma once

#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/tdef/stlfunctypes.h>

namespace Coffee{
namespace RHI{

struct GraphicsAPI_Threading
{
    using GraphicsTask = Function<void()>;

    struct GraphicsQueue
    {
        GraphicsQueue(RuntimeQueue* q = nullptr):
            m_queue(q)
        {
        }

        bool queue(GraphicsTask&& task)
        {
            C_THIS_CHECK

            if(!m_queue)
                return false;

            if(isLocal())
            {
                task();
                return true;
            }

            createTask(std::move(task));
            return true;
        }
        bool await(GraphicsTask&& task)
        {
            C_THIS_CHECK

            if(!m_queue)
                return false;

            if(isLocal())
            {
                task();
                return true;
            }

            RuntimeQueue::AwaitTask(
                        m_queue->threadId(),
                        createTask(std::move(task))
                        );
            return true;
        }

    private:
        RuntimeQueue* m_queue;
        u64 createTask(GraphicsTask&& task)
        {
            return RuntimeQueue::QueueImmediate(
                        m_queue,
                        Chrono::milliseconds(0),
                        std::move(task)
                        );
        }
        bool isLocal()
        {
            return ThreadId() == m_queue->threadId();
        }
    };

    static u32 GraphicsThreads()
    {
        return 1;
    }

    static GraphicsQueue& Queue(u32 idx = 0)
    {
        Throw(implementation_error("not implemented"));
    }
};

}
}
