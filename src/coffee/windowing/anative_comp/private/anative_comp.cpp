#include <coffee/anative/anative_comp.h>

#include <coffee/android/android_main.h>
#include <coffee/foreign/foreign.h>

#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

namespace anative {

void Windowing::load(entity_container& e, comp_app::app_error& ec)
{
    using namespace Coffee;

    AndroidForeignCommand cmd;
    cmd.type = Android_QueryNativeWindow;

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_RequestPlatformData, &cmd, nullptr, nullptr);

    //    if(!cmd.data.ptr)
    //    {
    //        ec = "ANativeWindow not available";
    //        ec = comp_app::AppError::NoWindow;
    //        return;
    //    }

    auto windowInfo = e.service<comp_app::PtrNativeWindowInfo>();
    if(!windowInfo)
    {
        ec = "Missing comp_app::PtrNativeWindowInfo service";
        ec = comp_app::AppError::ServiceMissing;
        return;
    }

    windowInfo->window = cmd.data.ptr;
}

comp_app::size_2d_t Windowing::size() const
{
    using namespace Coffee;

    int winSize[2] = {};

    CoffeeForeignSignalHandleNA(
        CoffeeForeign_GetWinSize, winSize, nullptr, nullptr);

    return {winSize[0], winSize[1]};
}

comp_app::detail::WindowState Windowing::state() const
{
    return comp_app::detail::WindowState::Normal;
}

void Windowing::setState(comp_app::detail::WindowState state)
{
}

libc_types::u32 ControllerInput::count() const
{
    return m_cache.size();
}
ControllerInput::controller_map ControllerInput::state(
    libc_types::u32 idx) const
{
    return m_cache.at(idx);
}
comp_app::text_type_t ControllerInput::name(libc_types::u32 idx) const
{
    return "Generic Controller";
}

void AndroidEventBus::load(entity_container& e, comp_app::app_error&)
{
    m_container = &e;
}

void AndroidEventBus::handleInputEvent(AInputEvent* event)
{
    Coffee::DebugFun::cDebug("Input event: {0}", event);
}

void AndroidEventBus::handleWindowEvent(android_app* app, libc_types::i32 event)
{
    Coffee::DebugFun::cDebug("App event: {0}", event);
}

} // namespace anative
