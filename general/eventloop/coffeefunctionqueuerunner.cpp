#include "coffeefunctionqueuerunner.h"

#include <QDebug>
#include <QThread>

void CoffeeFunctionQueueRunner::executeRunQueue()
{
    if(runqueue_pending.size()>0){
        m_mutex.lock();
        qDebug() << "Moving item";
        std::move(runqueue_pending.begin(),runqueue_pending.end(),
                  std::back_inserter(runqueue));
        runqueue_pending.resize(0);
        qDebug() << "Moved item";
        m_mutex.unlock();
        qDebug() << runqueue.size();
        for(auto it=runqueue.begin();it!=runqueue.end();it++){
            (*it)();
        }
        runqueue.clear();
    }
}

void CoffeeFunctionQueueRunner::queueFunction(std::function<void()> func)
{
    m_mutex.lock();
    runqueue_pending.push_back(func);
    m_mutex.unlock();
}
