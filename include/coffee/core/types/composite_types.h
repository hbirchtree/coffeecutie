#ifndef COFFEE_COMPOSITE_TYPES_H
#define COFFEE_COMPOSITE_TYPES_H

#include "../coffee_macros.h"
#include "basetypes.h"
#include "map.h"
#include "translatable_string.h"

#include "cdef/debugtypes.h"
#include "cdef/funtypes.h"
#include "cdef/geometry.h"
#include "cdef/infotypes.h"
#include "cdef/memtypes.h"
#include "cdef/packetsystem.h"
#include "cdef/pixtypes.h"
#include "cdef/timetypes.h"

namespace Coffee{

struct Subsystem
{
    static void Init();
    static void Deinit();

    static void Process();
};

template<typename SSys>
FORCEDINLINE void StartSubsystem()
{
    SSys::Init();
}

template<typename SSys>
FORCEDINLINE void StopSubsystem()
{
    SSys::Deinit();
}

template<typename Sys>
class SubsystemWrapper
{
public:
    template<typename... Args>
    SubsystemWrapper(Args... arg)
    {
        Sys::Init(arg...);
    }
    ~SubsystemWrapper()
    {
        Sys::Deinit();
    }
    template<typename... Args>
    void process(Args... arg)
    {
        Sys::Process(arg...);
    }
};

}

#endif
