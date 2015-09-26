#ifndef COFFEE_CDATASTORE_H
#define COFFEE_CDATASTORE_H

#include "coffee.h"
#include <mutex>

namespace Coffee {

struct CDataStore;

template<typename T>
struct CPointer
{
    T* dataPtr();


private:
    //To allow guaranteed access
    void lock();
    void unlock();

    CDataStore* m_store = nullptr;
    uint64      m_id    = 0;
};

struct CDataStore
{
    const void* dataPtr() const{
        return data;
    }
    uint64 size() const{
        return m_size;
    }

    void freeStore(){
        m_mut.lock();

        free(data);
        data = nullptr;
        m_size = 0;

        m_mut.unlock();
    }
    void* allocBlock(uint64 asize){
        m_mut.lock();

        uint64 offset = m_size;
        m_size += asize;
        data = realloc(data,m_size);
        char* ptr = reinterpret_cast<char*>(data);
        void* optr = &ptr[offset];

        m_mut.unlock();
        return optr;
    }
    template<typename T>
    T* allocType(){
        return reinterpret_cast<T*>(allocBlock(sizeof(T)));
    }
    template<typename T,typename... Args>
    T* constructType(Args... args){
        T* trg = allocType<T>();
        return new(trg) T(args...);
    }
private:
    struct CDataChunk
    {
        uint64 offset = 0;
        uint64 size   = 0;
    };

    void*       data        = nullptr;
    uint64    m_size        = 0;
    std::map<uint64,CDataChunk> m_chunks;
    std::mutex m_mut;
};

} // namespace Coffee

#endif // COFFEE_CDATASTORE_H
