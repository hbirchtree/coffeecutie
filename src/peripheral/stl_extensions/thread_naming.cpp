#include <peripherals/stl/thread_types.h>

#include <coffee/core/base_state.h>

using namespace ::Coffee;

namespace stl_types {

struct ThreadNames : State::GlobalState
{
    virtual ~ThreadNames();

    Mutex                        access;
    Map<ThreadId::Hash, CString> names;
};

ThreadNames::~ThreadNames()
{
}

STATICINLINE ThreadNames& GetContext()
{
    auto ptr = State::PeekState("threadNames");

    if(!ptr)
    {
        State::SwapState("threadNames", MkShared<ThreadNames>());
        ptr = State::PeekState("threadNames");
    }

    C_PTR_CHECK(ptr);

    auto threadNames = C_DCAST<ThreadNames>(ptr.get());

    C_PTR_CHECK(threadNames);

    return *threadNames;
}

STATICINLINE void SaveThreadName(ThreadId::Hash hs, CString const& name)
{
    auto& context = GetContext();
    Lock  _(context.access);

    if(context.names.find(hs) != context.names.end())
        return;

    context.names[hs] = name;
}

STATICINLINE CString LoadThreadName(ThreadId::Hash hs)
{
    auto& context = GetContext();
    Lock  _(context.access);

    return context.names[hs];
}

namespace Threads {
bool SetName(Thread& t, CString const& name)
{
    SaveThreadName(ThreadId(t.get_id()).hash(), name);

#if defined(COFFEE_APPLE)
    //    pthread_setname_np(name.c_str());
    return false;
#elif defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_SETNAME_NP)
    pthread_setname_np(t.native_handle(), name.c_str());
    return true;
#else
    return false;
#endif
}

CString GetName(Thread& t)
{
    CString out;
    out.resize(17);
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_GETNAME_NP)
    int stat = pthread_getname_np(t.native_handle(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#else
    return LoadThreadName(ThreadId(t.get_id()).hash());
#endif
}

bool SetName(ThreadId::Hash t, CString const& name)
{
    SaveThreadName(t, name);
    return true;
}

CString GetName(ThreadId::Hash t)
{
    return LoadThreadName(t);
}

Map<ThreadId::Hash, CString> GetNames()
{
    return GetContext().names;
}
} // namespace Threads

namespace CurrentThread {

bool SetName(CString const& name)
{
    SaveThreadName(ThreadId().hash(), name);

    CString cpy = name;
    if(name.size() >= 16)
        cpy.resize(15);
#if defined(COFFEE_APPLE)
    pthread_setname_np(cpy.c_str());
    return true;
#elif defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_SETNAME_NP)
    pthread_setname_np(pthread_self(), cpy.c_str());
    return true;
#else
    return false;
#endif
}

CString GetName()
{
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_GETNAME_NP)
    CString out;
    out.resize(17);
    int stat = pthread_getname_np(pthread_self(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#elif defined(COFFEE_ANDROID)
    CString out;
    out.resize(17);
    int stat = prctl(PR_GET_NAME, &out[0], 0, 0, 0);
    out.resize(out.find('\0', 0));
    return out;
#else
    return LoadThreadName(ThreadId().hash());
#endif
}

} // namespace CurrentThread
} // namespace stl_types
