#pragma once

#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/windowing/renderer/renderer.h>

namespace Coffee {

template<
    typename R,
    typename D,
    void (*SetupFunction)(R&, D*),
    void (*LoopFunction)(R&, D*),
    void (*CleanupFunction)(R&, D*)>
struct EventLoopSpawner
{
    using ELD = Display::EventLoopData<R, D>;

    STATICINLINE UqPtr<ELD> Spawn()
    {
        return MkUqWrap<ELD>(new ELD{Display::CreateRendererUq(),
                                     MkUq<D>(),
                                     SetupFunction,
                                     LoopFunction,
                                     CleanupFunction,
                                     0,
                                     {},
                                     {}});
    }
};

template<typename GAPI, typename R, typename D>
i32 AutoExec(
    Function<void(ShPtr<R>, ShPtr<D>, Display::Properties&)>&& presetup,
    Display::LoopFunction<R, D>&&                  setup,
    Display::LoopFunction<R, D>&&                  loop,
    Display::LoopFunction<R, D>&&                  cleanup)
{
    using namespace Display;

    using ELD = EventLoopData<R, D>;

    auto eld_ptr = MkSharedMove<ELD>({Display::CreateRendererSh(),
                                         MkShared<D>(),
                                         std::move(setup),
                                         std::move(loop),
                                         std::move(cleanup),
                                         {}});

    auto visual = Display::GetDefaultVisual<GAPI>();

    if(presetup)
        presetup(eld_ptr->renderer, eld_ptr->data, visual);

    CString error;

    auto res =
        EventApplication::execEventLoop(std::move(eld_ptr), visual, error);

    /* TODO: Error handling */

    return res;
}

template<typename GAPI, typename R, typename D>
i32 AutoExec(
    Display::LoopFunction<R, D>&& setup,
    Display::LoopFunction<R, D>&& loop,
    Display::LoopFunction<R, D>&& cleanup)
{
    return AutoExec<GAPI, R, D>(
        [](ShPtr<R>, ShPtr<D>, Display::Properties&) {},
        std::move(setup),
        std::move(loop),
        std::move(cleanup));
}

} // namespace Coffee
