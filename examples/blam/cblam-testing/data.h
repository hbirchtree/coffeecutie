#pragma once

#include "components.h"

using namespace StandardInput;
using camera_t     = typing::vectors::scene::camera<scalar>;
using std_camera_t = StandardCamera<camera_t*, StandardCameraOpts*>;

template<typename Version>
struct BlamData
{
    BlamData() :
#if defined(COFFEE_ANDROID)
        map_file(MkUrl("bloodgulch.map")), bitmap_file("bitmaps.map"_rsc),
#else
        map_file(MkUrl(GetInitArgs().arguments().at(0), RSCA::SystemFile)),
        bitmap_file(
            Resource((Path(GetInitArgs().arguments().at(0)).dirname()
                      / (std::is_same<Version, blam::pc_version_t>::value
                             ? "bitmaps.map"
                             : "bitmaps_custom.map"))
                         .url(RSCA::SystemFile))),
#endif
        std_camera(MkShared<std_camera_t>(&camera, &camera_opts)),
        controller_camera(&camera, &controller_opts)
    {
        auto container
            = blam::map_container<Version>::from_bytes(map_file, Version());
        if(container.has_error())
        {
            auto error = magic_enum::enum_name(container.error());
            Throw(undefined_behavior(
                Strings::fmt("failed to load map: {0}", error)));
        }
        map_container = std::move(container.value());
    }

    /* Blam! data */
    Resource                            map_file;
    Resource                            bitmap_file;
    blam::map_container<Version>        map_container;
    blam::scn::scenario<Version> const* scenario;

    Vector<gfx::draw_command::data_t> bsps;
    ShPtr<gfx::buffer_t>              bsp_buf;
    ShPtr<gfx::buffer_t>              bsp_index;
    ShPtr<gfx::vertex_array_t>        bsp_attr;
    ShPtr<gfx::buffer_t>              bsp_light_buf;

    ShPtr<gfx::program_t> bsp_pipeline;
    ShPtr<gfx::program_t> model_pipeline;
    ShPtr<gfx::program_t> senv_micro_pipeline;
    ShPtr<gfx::program_t> wireframe_pipeline;

    ShPtr<gfx::buffer_t>       model_buf;
    ShPtr<gfx::buffer_t>       model_index;
    ShPtr<gfx::vertex_array_t> model_attr;

    Vector<Matf4>        model_mats;
    ShPtr<gfx::buffer_t> model_matrix_store;
    ShPtr<gfx::buffer_t> material_store;

    camera_t            camera;
    Matf4               camera_matrix;
    StandardCameraOpts  camera_opts;
    ShPtr<std_camera_t> std_camera;

    f32 wireframe_distance{100.f};

    ControllerOpts                               controller_opts;
    ControllerCamera<camera_t*, ControllerOpts*> controller_camera;
};
