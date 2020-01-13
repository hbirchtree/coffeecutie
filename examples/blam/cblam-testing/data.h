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
        bitmap_file(Resource(MkUrl(
            Path(GetInitArgs().arguments().at(0)).dirname() +
                (std::is_same<Version, blam::pc_version_t>::value
                     ? "bitmaps.map"
                     : "bitmaps_custom.map"),
            RSCA::SystemFile))),
#endif
        map_container(map_file, Version()),
        bitm_cache(
            map_container,
            blam::magic_data_t(C_OCAST<Bytes>(bitmap_file)),
            nullptr),
        shader_cache(map_container, bitm_cache),
        bsp_cache(map_container, bitm_cache, shader_cache),
        model_cache(map_container, bitm_cache, shader_cache),
        std_camera(MkShared<std_camera_t>(&camera, &camera_opts)),
        controller_camera(&camera, &controller_opts)
    {
    }

    /* Blam! data */
    Resource                                      map_file;
    Resource                                      bitmap_file;
    blam::map_container                           map_container;
    blam::scn::scenario<blam::hsc::bc::v2> const* scenario;

    BitmapCache bitm_cache;
    ShaderCache shader_cache;

    BSPCache            bsp_cache;
    Vector<GFX::D_DATA> bsps;
    ShPtr<GFX::BUF_A>   bsp_buf;
    ShPtr<GFX::BUF_E>   bsp_index;
    ShPtr<GFX::V_DESC>  bsp_attr;

    PIP_PARAM* bsp_pipeline;
    PIP_PARAM* model_pipeline;
    PIP_PARAM* senv_micro_pipeline;

    ModelCache<Version> model_cache;

    ShPtr<GFX::BUF_A>  model_buf;
    ShPtr<GFX::BUF_E>  model_index;
    ShPtr<GFX::V_DESC> model_attr;

    Vector<Matf4>     model_mats;
    ShPtr<GFX::BUF_S> model_matrix_store;
    ShPtr<GFX::BUF_S> material_store;

    camera_t            camera;
    Matf4               camera_matrix;
    StandardCameraOpts  camera_opts;
    ShPtr<std_camera_t> std_camera;

    ControllerOpts                               controller_opts;
    ControllerCamera<camera_t*, ControllerOpts*> controller_camera;
};
