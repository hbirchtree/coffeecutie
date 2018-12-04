#pragma once

#include <coffee/core/libc_types.h>

namespace Coffee {

struct Subsystem
{
    static const constexpr cstring SubsystemName = "Generic";

    static bool Init();
    static bool Deinit();

    static bool Process();
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

} // namespace Coffee
