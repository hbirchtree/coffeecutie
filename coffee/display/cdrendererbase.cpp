#include "cdrendererbase.h"

namespace Coffee {
namespace CDisplay {

CDRendererBase::CDRendererBase(CObject *parent) : CObject(parent)
{
}

CDRendererBase::~CDRendererBase()
{
}
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

} // namespace CDisplay
} // namespace Coffee
