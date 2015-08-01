#include "coffeefunctionqueuerunner.h"

#include <QDebug>

void CoffeeFunctionQueueRunner::executeRunQueue()
{
    if(runqueue.size()>0){
        QVector<std::function<void()>*> t_queue(runqueue);
        for(std::function<void()>* f : t_queue){
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
