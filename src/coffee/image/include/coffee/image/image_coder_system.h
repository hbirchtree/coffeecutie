#pragma once

#include <coffee/components/components.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/image/cimage.h>

namespace Coffee {
namespace IMG {

struct ImageCoderData
{
};

using ImageCoderTag = Components::TagType<ImageCoderData>;

struct ImageCoderSubsystem : Components::Globals::ValueSubsystem<ImageCoderTag>
{
  private:
    using img_trigger = Function<void(stb::image_rw&&)>;

    struct DecodeItem
    {
        DecodeItem(BytesConst&& data) : data(std::move(data))
        {
        }

        BytesConst                    data;
        ShPtr<stb::image_rw>          output;
        ShPtr<semantic::ByteProvider> source;

        img_trigger   trigger;
        RuntimeQueue* trigger_queue;

        Mutex taskMtx;
    };
    struct EncodeItem
    {
        BytesConst data;
        Size       size;
    };

  public:
    ImageCoderSubsystem(CString const& queueName) :
        m_queue(nullptr), m_queueName(queueName)
    {
    }

    virtual ~ImageCoderSubsystem()
    {
        shutdown();
    }

    void shutdown()
    {
        if(!m_queue)
            return;

        runtime_queue_error ec;
        RuntimeQueue::TerminateThread(m_queue, ec);

        C_ERROR_CHECK(ec);

        m_queue = nullptr;
    }

    struct CodingChain
    {
        CodingChain(DecodeItem& item) : item(item), taskLock(item.taskMtx)
        {
        }

        void then(RuntimeQueue* queue, img_trigger&& fun)
        {
            item.trigger       = std::move(fun);
            item.trigger_queue = queue;
        }

        DecodeItem& item;
        UqLock      taskLock;
    };

    CodingChain decode(
        BytesConst&& imgData, ShPtr<semantic::ByteProvider>&& source = {})
    {
        UqLock _(m_queueLock);
        m_decodeQueue.emplace_back(std::move(imgData));

        m_decodeQueue.back().source = std::move(source);

        start();

        return {m_decodeQueue.back()};
    }

  private:
    RuntimeQueue* m_queue;
    CString       m_queueName;
    /* TODO: Solve this without abusing the heap */
    LinkList<DecodeItem> m_decodeQueue;
    Mutex                m_queueLock;

    void start()
    {
        if(m_queue)
            return;

        runtime_queue_error ec;
        m_queue = RuntimeQueue::CreateNewThreadQueue(m_queueName, ec);

        C_ERROR_CHECK(ec);

        RuntimeQueue::QueuePeriodic(
            m_queue,
            Chrono::milliseconds(10),
            [&]() {
                LinkList<DecodeItem> workQueue;
                {
                    UqLock _(m_queueLock, std::try_to_lock);

                    if(!_.owns_lock())
                        return;

                    workQueue = std::move(m_decodeQueue);
                    m_decodeQueue.clear();
                }

                runtime_queue_error ec;
                for(auto& item : workQueue)
                {
                    stb_error sec;
                    item.output = MkShared<stb::image_rw>();
                    stb::LoadData(item.output.get(), item.data, sec);
                    item.data = {};

                    /* TODO: Use move-semantics for these */
                    auto outputPtr = std::move(item.output);
                    auto trigger   = std::move(item.trigger);

                    if(!trigger)
                        Throw(undefined_behavior("no consumer for data"));

                    auto triggerTask = [outputPtr, trigger]() {
                        trigger(std::move(*outputPtr));
                    };

                    if(item.trigger_queue == m_queue)
                        triggerTask();
                    else
                        RuntimeQueue::QueueImmediate(
                            item.trigger_queue,
                            Chrono::milliseconds(0),
                            std::move(triggerTask),
                            ec);

                    C_ERROR_CHECK(ec);
                }
            },
            ec);

        C_ERROR_CHECK(ec);
    }
};

} // namespace IMG
} // namespace Coffee
