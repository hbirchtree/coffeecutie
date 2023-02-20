#pragma once

#include <coffee/comp_app/bundle.h>
#include <coffee/comp_app/eventapp_wrapper.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/core/base/renderer/eventapplication.h>
#include <coffee/windowing/renderer/renderer.h>
#include <peripherals/libc/types.h>
#include <peripherals/stl/functional_types.h>

//namespace Coffee {

//template<typename R, typename D>
//using PresetupFunc =
//    stl_types::Function<void(ShPtr<R>, ShPtr<D>, Display::Properties&)>;

//template<typename R, typename D>
//using LoopFunc = stl_types::Function<void(R&, D*)>;

//template<typename GAPI, typename R, typename D>
//libc_types::i32 AutoExec(
//    PresetupFunc<R, D>&& presetup,
//    LoopFunc<R, D>&&     setup,
//    LoopFunc<R, D>&&     loop,
//    LoopFunc<R, D>&&     cleanup)
//{
//    comp_app::app_error ec;

//    auto& container = comp_app::createContainer();
//    auto& loader    = comp_app::AppLoader::register_service(container);


//    comp_app::configureDefaults(loader);
//#if defined(FEATURE_ENABLE_GLADComponent)
//    auto& glConfig = loader.config<comp_app::GLConfig>();
//    glConfig.profile |= comp_app::GLConfig::Debug;
//#endif
//    comp_app::addDefaults(container, loader, ec);

//    comp_app::AutoExec<R, D>::addTo(
//        container,
//        std::move(presetup),
//        std::move(setup),
//        std::move(loop),
//        std::move(cleanup));

//    return comp_app::ExecLoop<comp_app::BundleData>::exec(container);
//}

//template<typename GAPI, typename R, typename D>
//libc_types::i32 AutoExec(
//    LoopFunc<R, D>&& setup, LoopFunc<R, D>&& loop, LoopFunc<R, D>&& cleanup)
//{
//    return AutoExec<GAPI, R, D>(
//        [](ShPtr<R>, ShPtr<D>, Display::Properties&) {},
//        std::move(setup),
//        std::move(loop),
//        std::move(cleanup));
//}

//} // namespace Coffee
