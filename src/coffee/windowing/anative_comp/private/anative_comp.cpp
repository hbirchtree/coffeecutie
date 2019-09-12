#include <coffee/anative/anative_comp.h>

#include <coffee/android/android_main.h>
#include <coffee/foreign/foreign.h>

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

} // namespace anative
