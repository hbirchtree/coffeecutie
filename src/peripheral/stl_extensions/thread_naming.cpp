#include <peripherals/stl/thread_types.h>

#include <platforms/pimpl_state.h>
#include <peripherals/stl/functional_types.h>

using ::type_safety::remove_cvref_t;

namespace stl_types {

struct ThreadNames : platform::GlobalState
{
    virtual ~ThreadNames();

    Map<ThreadId::Hash, CString> names;
};

ThreadNames::~ThreadNames()
{
}

STATICINLINE ThreadNames& GetContext(platform::GlobalState* context = nullptr)
{
    using namespace ::platform;

    platform::GlobalState* castablePtr = context;

    ShPtr<GlobalState> ptr;

    if(!castablePtr)
    {
        ptr = state->PeekState("threadNames");
        if(!ptr)
        {
            ptr = MkShared<ThreadNames>();
            state->SwapState("threadNames", ptr);
        }
        castablePtr = ptr.get();
    }

    C_PTR_CHECK(castablePtr);

    auto threadNames = C_DCAST<ThreadNames>(castablePtr);

    C_PTR_CHECK(threadNames);

    return *threadNames;
}

STATICINLINE void SaveThreadName(ThreadId::Hash hs, CString const& name)
{
    C_UNUSED(auto state) = platform::state->LockState("threadNames");
    auto& context = GetContext();

    if(context.names.find(hs) != context.names.end())
        return;

    context.names[hs] = name;
}

STATICINLINE CString LoadThreadName(ThreadId::Hash hs)
{
    C_UNUSED(auto state) = platform::state->LockState("threadNames");
    auto& context = GetContext();

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

CString GetName(ThreadId::Hash const& t)
{
    return LoadThreadName(t);
}

Map<ThreadId::Hash, CString> GetNames(platform::GlobalState* context)
{
    C_UNUSED(auto state) = platform::state->LockState(*context);
    return GetContext(context).names;
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
