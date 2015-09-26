#include "cqueuerunner.h"

#include <iterator>

namespace Coffee {
namespace CFunctional {

CQueueRunner::CQueueRunner()
{

}

void CQueueRunner::executeRunQueue()
{
    if(m_queue_pending.size()>0){
        m_mutex.lock();
        std::move(m_queue_pending.begin(),
                  m_queue_pending.end(),
                  std::back_inserter(m_queue));
        m_queue_pending.resize(0);
        m_mutex.unlock();
        for(auto it=m_queue.begin();it!=m_queue.end();it++){
            (*it)();
        }
        m_queue.clear();
    }
}

void CQueueRunner::queueFunction(QueueFunction func)
{
    m_mutex.lock();
    m_queue_pending.push_back(func);
    m_mutex.unlock();
}

} // namespace CFunctional

namespace CDisplay{

CDQueueRendererBase::CDQueueRendererBase(CObject *parent) : CDRendererBase(parent)
{
    m_queueRunner = new CQueueRunner();
}
CDQueueRendererBase::~CDQueueRendererBase()
{
    delete m_queueRunner;
}

void CDQueueRendererBase::executeRunQueue()
{
    m_queueRunner->executeRunQueue();
}

void CDQueueRendererBase::queueFunction(QueueFunction func)
{
    m_queueRunner->queueFunction(func);
}
}

} // namespace Coffee

