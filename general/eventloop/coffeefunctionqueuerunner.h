#ifndef COFFEEFUNCTIONQUEUERUNNER_H
#define COFFEEFUNCTIONQUEUERUNNER_H

#include <QVector>

class CoffeeFunctionQueueRunner
{
protected:
    void executeRunQueue();
    void queueFunction(std::function<void ()> *func);

private:
    QVector<std::function<void()>*> runqueue;
};

#endif // COFFEEFUNCTIONQUEUERUNNER_H
