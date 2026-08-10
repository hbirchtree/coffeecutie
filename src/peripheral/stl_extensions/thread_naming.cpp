#include <peripherals/stl/thread_types.h>

#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/standard_exceptions.h>
#include <platforms/pimpl_state.h>

#if defined(COFFEE_UNIXPLAT)
#include <pthread.h>
#endif

using ::type_safety::remove_cvref_t;

namespace stl_types {

struct ThreadNames : platform::GlobalState
{
    virtual ~ThreadNames();

    std::map<stl_types::thread_id_t, std::string> names;
};

ThreadNames::~ThreadNames()
{
}

STATICINLINE ThreadNames& GetContext(platform::GlobalState* context = nullptr)
{
    using namespace ::platform;

    platform::GlobalState* castablePtr = context;

    std::shared_ptr<GlobalState> ptr;

    if(!castablePtr)
    {
        ptr = state->PeekState("threadNames");
        if(!ptr)
        {
            ptr = std::make_shared<ThreadNames>();
            state->SwapState("threadNames", ptr);
        }
        castablePtr = ptr.get();
    }

    C_PTR_CHECK(castablePtr)

    auto threadNames = C_DCAST<ThreadNames>(castablePtr);

    C_PTR_CHECK(threadNames)

    return *threadNames;
}

STATICINLINE void SaveThreadName(stl_types::thread_id_t hs, std::string const& name)
{
    C_UNUSED(auto state) = platform::state->LockState("threadNames");
    auto& context        = GetContext();

    if(context.names.find(hs) != context.names.end())
        return;

    context.names[hs] = name;
}

STATICINLINE std::string_view LoadThreadName(stl_types::thread_id_t hs)
{
    C_UNUSED(auto state) = platform::state->LockState("threadNames");
    auto& context        = GetContext();

    return context.names[hs];
}

namespace Threads {
bool SetName(std::thread& t, std::string const& name)
{
    SaveThreadName(stl_types::get_thread_id(t), name);

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

std::string_view GetName(std::thread& t)
{
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_GETNAME_NP) && 0
    std::string out;
    out.resize(17);
    int stat = pthread_getname_np(t.native_handle(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#else
    return LoadThreadName(std::hash<std::thread::id>{}(t.get_id()));
#endif
}

bool SetName(stl_types::thread_id_t t, std::string const& name)
{
    SaveThreadName(t, name);
    return true;
}

std::string_view GetName(stl_types::thread_id_t t)
{
    return LoadThreadName(t);
}

std::map<stl_types::thread_id_t, std::string> GetNames(platform::GlobalState* context)
{
    C_UNUSED(auto state) = platform::state->LockState(*context);
    return GetContext(context).names;
}
} // namespace Threads

namespace CurrentThread {

bool SetName(std::string const& name)
{
    SaveThreadName(stl_types::get_this_thread_id(), name);

    std::string cpy = name;
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

std::string_view GetName()
{
#if defined(COFFEE_UNIXPLAT) && !defined(COFFEE_NO_PTHREAD_GETNAME_NP) && 0
    std::string out;
    out.resize(17);
    int stat = pthread_getname_np(pthread_self(), &out[0], out.size());
    if(stat != 0)
        return out;
    out.resize(out.find('\0', 0));
    return out;
#elif defined(COFFEE_ANDROID) && 0
    std::string out;
    out.resize(17);
    int stat = prctl(PR_GET_NAME, &out[0], 0, 0, 0);
    out.resize(out.find('\0', 0));
    return out;
#else
    return LoadThreadName(stl_types::get_this_thread_id());
#endif
}

} // namespace CurrentThread
} // namespace stl_types
