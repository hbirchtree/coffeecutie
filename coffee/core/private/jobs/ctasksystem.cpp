#include <coffee/core/jobs/ctasksystem.h>

namespace Coffee{
namespace CFunctional{
namespace Threads{

CTask *coffee_jobs_create_task(CTaskFunction fun, CTask* parent)
{
    CTask* task = new CTask;
    task->unfinished_tasks.store(1);
    task->task_function = fun;

    if(parent)
        parent->unfinished_tasks++;

    task->parent = parent;

    return task;
}

void coffee_jobs_finish(CTaskContext *ctxt, CTask *task)
{
    task->unfinished_tasks--;
    if(task->unfinished_tasks.load() == 0)
    {
        if(task->parent)
            coffee_jobs_finish(ctxt,task->parent);
        ctxt->tasks_delete_queue.add(task);
    }
}

void coffee_jobs_garbage_collect(CTaskContext *ctxt)
{
    while(!ctxt->tasks_delete_queue.empty())
    {
        CTask* item = ctxt->tasks_delete_queue.pop();
        delete item;
    }
}

}
}
}
