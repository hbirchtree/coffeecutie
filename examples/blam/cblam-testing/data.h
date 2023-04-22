#pragma once

#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/vector_types.h>

#include "blam_files.h"
#include "graphics_api.h"

using namespace Coffee::StandardInput;

using libc_types::f32;
using semantic::Span;
using typing::vector_types::Matf4;
using camera_t     = typing::vectors::scene::camera<f32>;
using std_camera_t = StandardCamera<camera_t*, StandardCameraOpts*>;

template<typename Version>
struct BlamData
{
    using type = BlamData<Version>;

    BlamData(/*Url map_filename*/)
    //#if defined(COFFEE_ANDROID)
    //        map_file(map_filename), bitmap_file("bitmaps.map"_rsc),
    //#else
    //        map_file(map_filename),
    //        bitmap_file(Resource((map_filename.path().dirname()
    //                              / (std::is_same_v<Version,
    //                              blam::pc_version_t>
    //                                     ? "bitmaps.map"
    //                                     : "bitmaps_custom.map"))
    //                                 .url(RSCA::SystemFile)))
    //#endif
    {
        //        auto container
        //            = blam::map_container<Version>::from_bytes(map_file,
        //            Version());
        //        if(container.has_error())
        //        {
        //            auto error = magic_enum::enum_name(container.error());
        //            Throw(undefined_behavior(
        //                Strings::fmt("failed to load map: {0}", error)));
        //        }
        //        map_container = std::move(container.value());
    }

    /* Blam! data */
    blam::map_container<Version> map_container;

    std::unique_ptr<blam::tag_index_view<Version>> tags_view;
    blam::scn::scenario<Version> const*            scenario{nullptr};
};

struct BlamCamera : compo::SubsystemBase
{
    using type = BlamCamera;

    BlamCamera() :
        std_camera(std::make_shared<std_camera_t>(&camera, &camera_opts)),
        controller_camera(std_camera, &controller_opts)
    {
    }

    camera_t                      camera;
    Matf4                         camera_matrix;
    Matf4                         rotation_matrix;
    StandardCameraOpts            camera_opts;
    std::shared_ptr<std_camera_t> std_camera;

    f32 wireframe_distance{100.f};

    ControllerOpts controller_opts;
    ControllerCamera<std::shared_ptr<std_camera_t>, ControllerOpts*>
        controller_camera;
};

struct BlamResources : compo::SubsystemBase
{
    using type = BlamResources;

    std::vector<gfx::draw_command::data_t> bsps;
    std::shared_ptr<gfx::buffer_t>         bsp_buf;
    std::shared_ptr<gfx::buffer_t>         bsp_index;
    std::shared_ptr<gfx::vertex_array_t>   bsp_attr;
    std::shared_ptr<gfx::buffer_t>         bsp_light_buf;

    std::shared_ptr<gfx::program_t> bsp_pipeline;
    std::shared_ptr<gfx::program_t> model_pipeline;
    std::shared_ptr<gfx::program_t> senv_micro_pipeline;
    std::shared_ptr<gfx::program_t> wireframe_pipeline;

    std::shared_ptr<gfx::buffer_t>       model_buf;
    std::shared_ptr<gfx::buffer_t>       model_index;
    std::shared_ptr<gfx::vertex_array_t> model_attr;

    std::shared_ptr<gfx::buffer_t>       debug_lines;
    std::shared_ptr<gfx::vertex_array_t> debug_attr;
    std::shared_ptr<gfx::program_t>      debug_lines_pipeline;

    std::vector<Matf4>             model_mats;
    std::shared_ptr<gfx::buffer_t> model_matrix_store;
    std::shared_ptr<gfx::buffer_t> material_store;
    std::shared_ptr<gfx::buffer_t> world_store;

    typing::vector_types::Veci2          offscreen_size{1920, 1080};
    std::shared_ptr<gfx::rendertarget_t> offscreen;
    std::shared_ptr<gfx::texture_2d_t>   color;
    std::shared_ptr<gfx::texture_2d_t>   depth;
};

struct PostProcessParameters : compo::SubsystemBase
{
    using type = PostProcessParameters;

    f32  exposure{1.5f};
    f32  gamma{1.6f};
    f32  scale{1.f};
    bool auto_expose{true};

    bool doom_mode{false};
};

struct RenderingParameters : compo::SubsystemBase
{
    using type = RenderingParameters;

    bool render_scenery{false};
    bool debug_clear{true};

    bool debug_portals{false};
    bool debug_triggers{false};
};
