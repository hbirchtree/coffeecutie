#ifndef COFFEE_CFUNCTIONAL_CQUEUERUNNER_H
#define COFFEE_CFUNCTIONAL_CQUEUERUNNER_H

#include "coffee/core/coffee.h"
#include "coffee/core/base/cdrendererbase.h"
#include <mutex>

namespace Coffee {
namespace CFunctional {

class CQueueRunner
{
public:
    CQueueRunner();
    virtual ~CQueueRunner();
    void queueFunction(QueueFunction func);
    virtual void executeRunQueue();

private:
    std::vector<QueueFunction> m_queue_pending;
    std::vector<QueueFunction> m_queue;
    std::mutex m_mutex;
};

} // namespace CFunctional

namespace CDisplay{

class CDQueueRendererBase : public CDRendererBase
{
public:
    virtual ~CDQueueRendererBase();
    virtual void queueFunction(QueueFunction func);

protected:
    CDQueueRendererBase(CObject* parent);
    CQueueRunner* m_queueRunner;
    void executeRunQueue();
};

}
} // namespace Coffee

#endif // COFFEE_CFUNCTIONAL_CQUEUERUNNER_H
