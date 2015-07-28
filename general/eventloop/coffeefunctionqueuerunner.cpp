#include "coffeefunctionqueuerunner.h"

void CoffeeFunctionQueueRunner::executeRunQueue()
{
    if(runqueue.size()>0){
        QVector<std::function<void()>*> t_queue(runqueue);
        for(auto f : t_queue){
            if(f){
                (*f)();
                delete f;
            }
            runqueue.removeOne(f);
        }
    }
}

void CoffeeFunctionQueueRunner::queueFunction(std::function<void ()> *func)
{
    runqueue.append(func);
}
