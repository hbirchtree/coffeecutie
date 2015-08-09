#ifndef COFFEEFUNCTIONQUEUERUNNER_H
#define COFFEEFUNCTIONQUEUERUNNER_H

#include <QVector>
#include <QMutexLocker>

class CoffeeFunctionQueueRunner
{
protected:
    void executeRunQueue();
    void queueFunction(std::function<void ()> func);

private:
    QMutex m_mutex;
    std::vector<std::function<void()> > runqueue;
    std::vector<std::function<void()> > runqueue_pending;
};

#endif // COFFEEFUNCTIONQUEUERUNNER_H
