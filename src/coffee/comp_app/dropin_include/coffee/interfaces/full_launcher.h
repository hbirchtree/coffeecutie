#pragma once

#include <coffee/comp_app/bundle.h>
#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/windowing/renderer/renderer.h>
#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/core/types/display/properties.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>

namespace Coffee {

template<typename R, typename D>
using PresetupFunc =
    stl_types::Function<void(ShPtr<R>, ShPtr<D>, Display::Properties&)>;

template<typename R, typename D>
using LoopFunc = stl_types::Function<void(R&, D*)>;

template<typename GAPI, typename R, typename D>
libc_types::i32 AutoExec(
    PresetupFunc<R, D>&& presetup,
    LoopFunc<R, D>&&     setup,
    LoopFunc<R, D>&&     loop,
    LoopFunc<R, D>&&     cleanup)
{
    using EventAppType =
        comp_app::AppService<comp_app::EventapplicationWrapper<R, D>>;

    comp_app::app_error ec;

    auto& container = comp_app::createContainer();
    auto& loader =
        comp_app::AppLoader::register_service<comp_app::AppLoader>(container);

    comp_app::configureDefaults(loader);
    comp_app::addDefaults(container, loader, ec);

    comp_app::AutoExec<R, D>::addTo(
        container,
        [ presetup, container = &container ](R & renderer, D * data) {
            renderer.m_container = container;

            auto eventApp = container->service<EventAppType>();

            Display::Properties props;
            presetup(
                eventApp->m_config.m_renderer,
                eventApp->m_config.m_data,
                props);
        },
        std::move(setup),
        std::move(loop),
        std::move(cleanup));

    return comp_app::ExecLoop<comp_app::BundleData>::exec(container);
}

template<typename GAPI, typename R, typename D>
libc_types::i32 AutoExec(
    LoopFunc<R, D>&& setup, LoopFunc<R, D>&& loop, LoopFunc<R, D>&& cleanup)
{
    return AutoExec<GAPI, R, D>(
        [](ShPtr<R>, ShPtr<D>, Display::Properties&) {},
        std::move(setup),
        std::move(loop),
        std::move(cleanup));
}

} // namespace Coffee
