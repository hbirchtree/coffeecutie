#pragma once

//#include <coffee/core/task_queue/task.h>
//#include <peripherals/stl/functional_types.h>

//namespace Coffee{
//namespace RHI{

//struct GraphicsAPI_Threading
//{
//    using GraphicsTask = Function<void()>;

//    struct GraphicsQueue
//    {
//        GraphicsQueue(RuntimeQueue* q = nullptr):
//            m_queue(q)
//        {
//        }

//        bool queue(GraphicsTask&& task)
//        {
//            C_THIS_CHECK

//            if(!m_queue)
//                return false;

//            if(isLocal())
//            {
//                task();
//                return true;
//            }

//            createTask(std::move(task));
//            return true;
//        }
//        bool await(GraphicsTask&& task)
//        {
//            C_THIS_CHECK

//            if(!m_queue)
//                return false;

//            if(isLocal())
//            {
//                task();
//                return true;
//            }

//            runtime_queue_error ec;

//            RuntimeQueue::AwaitTask(
//                        m_queue->threadId(),
//                        createTask(std::move(task)),
//                        ec);

//            C_ERROR_CHECK(ec);

//            return true;
//        }

//    private:
//        RuntimeQueue* m_queue;
//        u64 createTask(GraphicsTask&& task)
//        {
//            runtime_queue_error ec;
//            auto id = RuntimeQueue::QueueImmediate(
//                        m_queue,
//                        Chrono::milliseconds(0),
//                        std::move(task),
//                        ec);

//            C_ERROR_CHECK(ec);

//            return id;
//        }
//        bool isLocal()
//        {
//            return ThreadId() == m_queue->threadId();
//        }
//    };

//    static u32 GraphicsThreads()
//    {
//        return 1;
//    }

//    static GraphicsQueue& Queue(u32 idx = 0)
//    {
//        Throw(implementation_error("not implemented"));
//    }
//};

//}
//}
