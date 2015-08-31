#ifndef COFFEE_CFUNCTIONAL_CQUEUERUNNER_H
#define COFFEE_CFUNCTIONAL_CQUEUERUNNER_H

#include "coffee.h"
#include <mutex>

namespace Coffee {
namespace CFunctional {

class CQueueRunner
{
public:
    void queueFunction(QueueFunction func);

protected:
    CQueueRunner();

    void executeRunQueue();

private:
    std::vector<QueueFunction> m_queue_pending;
    std::vector<QueueFunction> m_queue;
    std::mutex m_mutex;
};

} // namespace CFunctional
} // namespace Coffee

#endif // COFFEE_CFUNCTIONAL_CQUEUERUNNER_H
