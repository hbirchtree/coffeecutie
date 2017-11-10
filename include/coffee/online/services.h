#pragma once

#include <coffee/core/types/tdef/stlfunctypes.h>
#include <coffee/core/types/cdef/memtypes.h>

namespace Coffee {
namespace Online{

struct ServiceDescription;
struct Achievement;

struct Service;

using IdentifyService = CString(*)();
using InitService = void(*)(void*);
using DeinitService = void(*)(void*);
using ServiceConstructor = void(*)(Service&);

template<typename RType, typename... Args>
using ServiceFn = Function<RType(Args...)>;

using ServiceLoader = Function<Service*()>;

struct ServiceDescription
{
    IdentifyService identity;

    InitService load;

    ServiceConstructor get;

    DeinitService unload;

    void* service_data;
};

struct Achievement
{
    u64 id;
    cstring name;
};

struct Friend
{
    u64 id;
    cstring name;
    Bytes* image;
};

struct Service {
    ServiceFn<void, u64> Achieved;
    ServiceFn<Vector<Achievement> const*> GetAchievementList;
};

extern ServiceLoader GetLoader();

}
}
