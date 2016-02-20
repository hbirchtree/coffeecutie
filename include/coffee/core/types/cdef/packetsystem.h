#pragma once

#include "../../coffee_mem_macros.h"
#include "../tdef/integertypes.h"
#include "../tdef/stltypes.h"

namespace Coffee{

template<typename PT>
/*!
 * \brief Wrapper around std::queue to make it easier to recycle packets in a fast loop
 */
class PacketPool
{
public:
    ~PacketPool()
    {

    }

    FORCEDINLINE void expand(size_t n)
    {
        /* Place lock on data */
        Lock l(m_lock);

        for(size_t i=0;i<n;i++)
        {
            PT* p = new PT;
            initPacket(p);
            m_free.push(p);
        }
    }
    FORCEDINLINE PT* grab()
    {
        autoRecycle();

        m_lock.lock();
        size_t numfree = m_free.size();
        m_lock.unlock();

        if(numfree<1)
            expand(1);

        /* Place lock on data */
        Lock l(m_lock);

        PT* p = m_free.front();
        m_free.pop();
        return p;
    }
    FORCEDINLINE void recycle(PT* p)
    {
        /* Place lock on data */
        Lock l(m_lock);
        C_UNUSED(l);

        cleanPacket(p);
        m_occupied.remove(p);
        m_free.push(p);
    }
    FORCEDINLINE void autoRecycle()
    {
        /* Place lock on data */
        Lock l(m_lock);

        for(PT* p : m_occupied)
            if(isAvailable(p))
                m_tmp_store.push_back(p);

        for(PT* p : m_tmp_store)
            recycle(p);
        m_tmp_store.clear();
    }
protected:
    virtual void initPacket(PT*)
    {
    }
    virtual bool isAvailable(PT*)
    {
        return false;
    }
    virtual void cleanPacket(PT*)
    {
    }
    virtual void freePacket(PT*)
    {
    }

    Mutex m_lock;
    LinkList<PT*> m_occupied;
    Queue<PT*> m_free;

    Vector<PT*> m_tmp_store;
};

template<typename PT>
class PacketConsumer
{
public:
    FORCEDINLINE PacketConsumer()
    {
    }
    virtual void processPacket(PT* packet) = 0;
};

template<typename PT>
class PacketProducer
{
public:
    FORCEDINLINE PacketProducer(PacketPool<PT>& pool):
        m_pool(&pool)
    {
    }

    FORCEDINLINE void registerConsumer(PacketConsumer<PT>* c)
    {
        Lock l(m_consumer_mutex);
        m_consumers.push_back(c);
    }
    FORCEDINLINE void removeConsumer(PacketConsumer<PT>* c)
    {
        Lock l(m_consumer_mutex);
        m_consumers.remove(c);
    }

    FORCEDINLINE PT* getPacket()
    {
        return m_pool->grab();
    }
    FORCEDINLINE void usePacket(PT* p)
    {
        Lock l(m_consumer_mutex);
        for(PacketConsumer<PT>* pc : m_consumers)
            pc->processPacket(p);
        m_pool->recycle(p);
    }

protected:
    PacketPool<PT>* m_pool;

    Mutex m_consumer_mutex;
    LinkList<PacketConsumer<PT>*> m_consumers;
};

}