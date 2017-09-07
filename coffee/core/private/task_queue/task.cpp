#include <coffee/core/task_queue/task.h>

namespace Coffee{

Mutex RuntimeQueue::globalMod;
Map<ThreadId::Hash, RuntimeQueue> RuntimeQueue::queues;
Map<ThreadId::Hash, Thread> RuntimeQueue::queueThreads;

RuntimeQueue* RuntimeQueue::CreateNewQueue(const CString &name)
{
    Lock _(globalMod);

    auto t_id = ThreadId().hash();

    auto q_it = queues.find(t_id);

    if(q_it == queues.end())
    {
        queues.insert({t_id, RuntimeQueue()});
        ThreadSetName(name);
        return &queues[t_id];
    }else
        return &(*q_it).second;
}

static void ImpCreateNewThreadQueue(CString const& name)
{

}

RuntimeQueue* RuntimeQueue::CreateNewThreadQueue(const CString &name)
{
    Lock _(globalMod);

    Thread t(ImpCreateNewThreadQueue, name);

    return nullptr;
}

static bool VerifyTask(RuntimeTask const& t)
{
    if(!(t.flags & (RuntimeTask::SingleShot|RuntimeTask::Periodic)))
        return false;
    if((t.flags & RuntimeTask::SingleShot) &&
            (t.flags & RuntimeTask::Periodic))
        return false;
    if((t.flags & RuntimeTask::SingleShot) &&
            t.time < RuntimeTask::clock::now())
        return false;
    return true;
}

u64 RuntimeQueue::Queue(RuntimeTask &&task)
{
    return Queue(ThreadId(), std::move(task));
}

u64 RuntimeQueue::Queue(ThreadId const& targetThread, RuntimeTask &&task)
{

    if(!VerifyTask(task))
        return 0;

    if(task.flags & RuntimeTask::Periodic)
    {
        task.time = RuntimeTask::clock::now() + task.interval;
    }

    Lock _(globalMod);

    auto q_it = queues.find(targetThread.hash());

    if(q_it == queues.end())
        return 0;
    else
    {
        auto& ref = (*q_it).second;
        return ref.enqueue(std::move(task));
    }
}

bool RuntimeQueue::CancelTask(u64 taskId)
{
    return CancelTask(ThreadId(), taskId);
}

bool RuntimeQueue::CancelTask(const ThreadId &targetThread, u64 taskId)
{
    Lock _(globalMod);
    return false;
}

void RuntimeQueue::executeTasks()
{
    Lock _(mTasksLock);

    auto currTime = RuntimeTask::clock::now();

    szptr i = 0;
    for(RuntimeTask& task : mTasks)
    {
        /* If this task has to be run in the future,
         *  all proceeding tasks will do as well */
        if(task.time > currTime)
            break;

        /* Skip dead tasks, clean it up later */
        if(!mTasksAlive[i])
            continue;

        /* In this case we will let it run */
        task.task();

        /* Now, if a task is single-shot, remove it */
        if(task.flags & RuntimeTask::SingleShot)
            mTasksAlive[i] = false;
        else if(task.flags & RuntimeTask::Periodic)
        {
            task.time += task.interval;
        }else{
            ABORTEVERYTHINGGOGOGO();
        }
        i++;
    }
}

CString RuntimeQueue::name()
{
    Lock _(mTasksLock);
    return {};
}

RuntimeQueue::RuntimeQueue(RuntimeQueue const& queue)
{
    mTasks = std::move(queue.mTasks);
    mTaskIndex = std::move(queue.mTaskIndex);
}

RuntimeQueue::RuntimeQueue():
    mTasks(),
    mTasksAlive(),
    mTasksLock(),
    mTaskIndex(0)
{
}

u64 RuntimeQueue::enqueue(RuntimeTask &&task)
{
    Lock _(mTasksLock);

    u64 output = ++mTaskIndex;

    mTasks.push_back(task);
    mTasksAlive.push_back(true);

    sortTasks();

    return output;
}

void RuntimeQueue::sortTasks()
{
    std::sort(mTasks.begin(), mTasks.end());
}

}
