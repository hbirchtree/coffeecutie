#pragma once

#include <chrono>
#include <string>
#include <thread>

// Requires #define PROFILER_CUSTOM_THREAD_NS ...
#define PROFILER_CUSTOM_THREADLIB

//#define PROFILER_STD_TYPES

#if !defined(PROFILER_STD_TYPES)
#include <coffee/core/base.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>
#include <peripherals/stl/types.h>
#else
#include <stdint.h>
#ifndef STATICINLINE
#define STATICINLINE static inline
#endif
#endif

namespace profiler {

#if defined(PROFILER_STD_TYPES)
template<typename T>
using ptr                   = std::shared_ptr<T>;
using clock                 = std::chrono::steady_clock;
using thread                = std::thread;
using thread_id_constructor = std::hash<thread::id>;
using thread_id             = thread_id_constructor::result_type;

namespace this_thread {
using namespace std::this_thread;
}

using cstring = const char*;
using u16     = ::uint16_t;
using u32     = ::uint32_t;

#else
#ifndef PROFILER_CUSTOM_THREAD_NS
#define PROFILER_CUSTOM_THREAD_NS ::Coffee::CurrentThread
#endif

template<typename T>
using ptr                   = ::Coffee::ShPtr<T>;
using clock                 = ::Coffee::Chrono::steady_clock;
using thread                = ::Coffee::Thread;
using thread_id             = ::Coffee::ThreadId::Hash;
using thread_id_constructor = ::Coffee::ThreadId;

namespace this_thread {
using namespace PROFILER_CUSTOM_THREAD_NS;
}

using namespace ::Coffee::Primitives;
#endif

template<typename Clock>
struct datapoint_t
{
    enum Type
    {
        Profile,
        Push,
        Pop,
        Complete,
    };

    enum Attr
    {
        AttrNone,
    };

    thread_id                tid;
    std::string              name;
    cstring                  component;
    typename Clock::duration ts;
    typename Clock::duration dur;

    union
    {
        Type type : 3;

        u16 _extra : 14;
        u16 _extra2;
        u32 _extra3;

    } flags;
};

namespace option_concepts {

template<typename Context, typename Clock, typename Types>
struct runtime
{
    void        push_stack(std::string const& frame);
    std::string pop_stack();

    cstring component();

    void (*push)(Context&, datapoint_t<Clock> const&);

    static bool                            enabled();
    static bool                            deep_enabled();
    static runtime<Context, Clock, Types>& get_properties();
};

} // namespace option_concepts

namespace options {

template<bool ProfileEnable = false, bool DeepEnable = false>
struct compile
{
    static constexpr bool profile_enable = ProfileEnable;
    static constexpr bool deep_enable    = DeepEnable;
};

using compile_default = compile<>;

} // namespace options

namespace detail {

template<typename Clock, typename Datapoint, typename RuntimeOptions>
struct prof_types
{
    using clock           = Clock;
    using datapoint       = Datapoint;
    using runtime_options = RuntimeOptions;
};

template<typename prof_types>
struct prof_common
{
    STATICINLINE void basic_init(
        typename prof_types::datapoint&       data,
        typename prof_types::runtime_options& opt)
    {
        data           = {};
        data.ts        = prof_types::clock::now().time_since_epoch();
        data.tid       = thread_id_constructor()(this_thread::get_id());
        data.component = opt.component();
    }
};

template<typename prof_types, bool Enabled>
struct prof_component
{
    using clock           = typename prof_types::clock;
    using datapoint       = typename prof_types::datapoint;
    using runtime_options = typename prof_types::runtime_options;

    template<
        typename Dummy                                 = void,
        typename std::enable_if<Enabled, Dummy>::type* = nullptr>
    STATICINLINE void push(runtime_options& opt, std::string&& name)
    {
        datapoint data;
        prof_common<prof_types>::basic_init(data, opt);

        opt.push_stack(name);
        data.flags.type = datapoint::Push;
        data.name       = name.c_str();
        data.component  = opt.component();

        opt.push(*opt.context, data);
    }

    template<
        typename Dummy                                 = void,
        typename std::enable_if<Enabled, Dummy>::type* = nullptr>
    STATICINLINE void pop(runtime_options& opt)
    {
        datapoint data;
        prof_common<prof_types>::basic_init(data, opt);

        auto name = opt.pop_stack();

        if(!name.size())
            return;

        data.flags.type = datapoint::Pop;
        data.name       = std::move(name);

        opt.push(*opt.context, data);
    }

    template<
        typename Dummy                                 = void,
        typename std::enable_if<Enabled, Dummy>::type* = nullptr>
    STATICINLINE void profile(runtime_options& opt, std::string&& name)
    {
        datapoint data;
        prof_common<prof_types>::basic_init(data, opt);

        data.flags.type = datapoint::Profile;
        data.name       = std::move(name);

        opt.push(*opt.context, data);
    }

    template<
        typename Dummy                                 = void,
        typename std::enable_if<Enabled, Dummy>::type* = nullptr>
    STATICINLINE void complete(
        runtime_options&                opt,
        std::string&&                   name,
        typename clock::duration const& duration)
    {
        datapoint data;
        prof_common<prof_types>::basic_init(data, opt);

        data.flags.type = datapoint::Complete;
        data.name       = std::move(name);
        data.dur        = duration;
    }

    template<
        typename Dummy                                  = void,
        typename std::enable_if<!Enabled, Dummy>::type* = nullptr>
    STATICINLINE void push(runtime_options&, std::string const&)
    {
    }

    template<
        typename Dummy                                  = void,
        typename std::enable_if<!Enabled, Dummy>::type* = nullptr>
    STATICINLINE void pop(runtime_options&)
    {
    }

    template<
        typename Dummy                                  = void,
        typename std::enable_if<!Enabled, Dummy>::type* = nullptr>
    STATICINLINE void profile(runtime_options&, std::string const&)
    {
    }

    template<
        typename Dummy                                  = void,
        typename std::enable_if<!Enabled, Dummy>::type* = nullptr>
    STATICINLINE void complete(
        runtime_options&, std::string const&, typename clock::duration const&)
    {
    }
};

} // namespace detail

template<
    typename Context,
    typename Clock,

    template<
        typename RContext = Context,
        typename RClock   = Clock,
        typename Types    = detail::prof_types<Clock, datapoint_t<Clock>, void>>
    class RuntimeOptions,

    typename CompileOptions = options::compile_default>
struct prof
{
    using clock           = Clock;
    using datapoint       = datapoint_t<Clock>;
    using opts            = CompileOptions;
    using prof_ntypes     = detail::prof_types<Clock, datapoint, void>;
    using runtime_options = RuntimeOptions<Context, Clock, prof_ntypes>;

    using prof_types = detail::prof_types<Clock, datapoint, runtime_options>;

    static constexpr bool app_profiling = CompileOptions::profile_enable;
    static constexpr bool lib_profiling =
        app_profiling && CompileOptions::deep_enable;

    using app_component = detail::prof_component<prof_types, app_profiling>;
    using lib_component = detail::prof_component<prof_types, lib_profiling>;

    struct app
    {
        STATICINLINE void push(std::string&& name)
        {
            if(!runtime_options::enabled())
                return;

            auto& props = runtime_options::get_properties();
            app_component::push(props, std::move(name));
        }

        STATICINLINE void pop()
        {
            if(!runtime_options::enabled())
                return;

            auto& props = runtime_options::get_properties();
            app_component::pop(props);
        }

        STATICINLINE void profile(std::string&& name)
        {
            if(!runtime_options::enabled())
                return;

            auto& props = runtime_options::get_properties();
            app_component::profile(props, std::move(name));
        }
    };

    struct lib
    {
        STATICINLINE void push(std::string&& name)
        {
            if(!runtime_options::deep_enabled())
                return;

            auto& props = runtime_options::get_properties();
            lib_component::push(props, std::move(name));
        }

        STATICINLINE void pop()
        {
            if(!runtime_options::deep_enabled())
                return;

            auto& props = runtime_options::get_properties();
            lib_component::pop(props);
        }

        STATICINLINE void profile(std::string&& name)
        {
            if(!runtime_options::deep_enabled())
                return;

            auto& props = runtime_options::get_properties();
            lib_component::profile(props, std::move(name));
        }
    };
};

} // namespace profiler
