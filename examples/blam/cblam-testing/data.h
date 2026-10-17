#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <future>
#include <limits>
#include <string>

#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_tag_classes.h>
#include <blam/volta/blam_tag_ref.h>
#include <coffee/comp_app/services.h>
#include <coffee/core/debug/formatting.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/identify/compiler/unreachable.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/glm_vector_types.h>
#include <peripherals/typing/vectors/vector_types.h>
#include <platforms/environment.h>

#include "materials.h"
#include "game_events.h"
#include "graphics_api.h"

using namespace Coffee::StandardInput;

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using libc_types::u64;
using semantic::Span;
struct BSPItem;
using typing::vector_types::Matf4;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

/* The only genuinely fixed reservation in the debug-line buffers: three
 * 2-vertex axis lines, drawn once at load (resource_creation.cpp) and
 * never resized. Everything else — physics body boxes, occluder eye
 * markers, map-load geometry — is allocated dynamically at runtime
 * (DebugMarkers::acquire_strip / create_marker / create_loop), bounded
 * only by the buffers' real capacity instead of hand-picked offsets. */
constexpr u32 debug_axes_verts  = 6;
constexpr u32 debug_axes_colors = 3;

template<typename Version>
struct BlamData
{
    using type = BlamData<Version>;
};

namespace const_config {
const bool supports_splitscreen = !compile_info::platform::is_emscripten;
}

struct BlamResources : compo::SubsystemBase
{
    using type = BlamResources;

    rq::runtime_queue* background_worker{nullptr};

    std::shared_ptr<gfx::buffer_t>       bsp_buf;
    std::shared_ptr<gfx::buffer_t>       bsp_index;
    std::shared_ptr<gfx::buffer_t>       bsp_light_buf;
    std::shared_ptr<gfx::vertex_array_t> bsp_attr;

    /* Modern uber shaders, so powerful it needs to be split */
    std::shared_ptr<gfx::program_t> bsp_pipeline_base;
    std::shared_ptr<gfx::program_t> bsp_pipeline_chicago;
    std::shared_ptr<gfx::program_t> bsp_pipeline_sotr;
    std::shared_ptr<gfx::program_t> model_pipeline_base;
    std::shared_ptr<gfx::program_t> model_pipeline_chicago;
    std::shared_ptr<gfx::program_t> model_pipeline_sotr;

    std::shared_ptr<gfx::program_t> camo;
    /* Depth-only object casters; only with the full uber shaders */
    std::shared_ptr<gfx::program_t> shadow_pipeline;
    /* Depth-only BSP, hiding receivers from the sun */
    std::shared_ptr<gfx::program_t> shadow_occluder_pipeline;

    /* ES2 shaders */
    std::shared_ptr<gfx::program_t> bsp_pipeline;
    std::shared_ptr<gfx::program_t> model_pipeline;
    std::shared_ptr<gfx::program_t> chicago_pipeline; /* ES2 schi/scex combiner */
    std::shared_ptr<gfx::program_t> water_pipeline; /* ES2 swat water */

    std::shared_ptr<gfx::program_t> wireframe_pipeline;

    std::shared_ptr<gfx::buffer_t>       model_buf;
    std::shared_ptr<gfx::buffer_t>       model_index;
    std::shared_ptr<gfx::vertex_array_t> model_attr;

    std::shared_ptr<gfx::buffer_t>       debug_lines;
    std::shared_ptr<gfx::buffer_t>       debug_line_colors;
    std::shared_ptr<gfx::vertex_array_t> debug_attr;
    std::shared_ptr<gfx::program_t>      debug_lines_pipeline;

    std::shared_ptr<gfx::revolving_buffer_t> model_matrix_store;
    std::shared_ptr<gfx::revolving_buffer_t> material_store;
    std::shared_ptr<gfx::revolving_buffer_t> transparent_store;
    std::shared_ptr<gfx::buffer_t>           world_store;
    /* What world_store holds; it is rewritten whole each frame */
    materials::world_data world{};
    std::shared_ptr<gfx::revolving_buffer_t>
        bone_matrix_buf; /* per-frame bone transforms, separate from static
                            vertex data */

    typing::vector_types::Veci2          offscreen_size{1920, 1080};
    std::shared_ptr<gfx::rendertarget_t> offscreen;
    std::shared_ptr<gfx::texture_2d_t>   color;
    std::shared_ptr<gfx::texture_2d_t>   depth;

    struct shader_status_t
    {
        u32 ready{0};
        u32 total{0};
    };

    stl_types::result<shader_status_t, gfx::program_t::compile_error_t>
    check_shaders_ready() const
    {
        shader_status_t status;
        for(gfx::program_t* program : {
                bsp_pipeline.get(),
                model_pipeline.get(),
                bsp_pipeline_base.get(),
                bsp_pipeline_chicago.get(),
                bsp_pipeline_sotr.get(),
                model_pipeline_base.get(),
                model_pipeline_chicago.get(),
                model_pipeline_sotr.get(),
                camo.get(),
                chicago_pipeline.get(),
                water_pipeline.get(),
                wireframe_pipeline.get(),
                debug_lines_pipeline.get(),
            })
        {
            if(!program)
                continue;
            status.total++;
            /* Never entered async compile (no extension, or already linked),
             * so check_async_ready() would hit its unreachable(). */
            if(!program->m_async_waiting)
            {
                status.ready++;
                continue;
            }
            /* No early exit: check_async_ready() is what advances each
             * program's state machine, so all of them need a call. */
            auto ready = program->check_async_ready();
            if(ready.has_error())
                return stl_types::failure(ready.error());
            if(ready.value())
                status.ready++;
        }
        return stl_types::success(status);
    }
};

struct PostProcessParameters : compo::SubsystemBase
{
    using type = PostProcessParameters;

    f32   exposure{2.f};
    f32   gamma{.7f};
    f32   scale{1.f};
    bool  auto_expose{true};
    bool  tonemap{true};
    f32   blur{0.f};
    Vecf4 rgb_comp{};
    f32   camo_scale{0.05f};

    bool forge_overlay{false};

    bool doom_mode{false};
};

struct RenderingParameters : compo::SubsystemBase
{
    using type = RenderingParameters;

    libc_types::u32 mipmap_bias{compile_info::platform::is_32bit ? 2 : 3};

    // Common render flags
    bool only_diffuse{false};
    bool only_normals{false};
    bool only_reflections{false};
    bool render_fog{true};
    bool render_reflection{true};

    // MOD-exclusive
    bool color_changing{true};
    bool only_detail{false};
    bool only_multipurpose{false};
    bool only_multipurpose2{false};
    bool render_model_bones{true};
    bool render_scenery{true};

    // ES2 renderer: closer to the main renderer, at a cost
    bool legacy_enhanced{
        platform::env::var("COFFEE_LEGACY_ENHANCED").has_value()};

    // BSP-exclusive
    bool only_aux_channels{false};
    bool only_micro{false};
    bool only_lightmaps{false};
    bool only_normalmaps{false};
    bool render_lightmaps{true};

    std::array<libc_types::f32, 4> object_function{{0.f, 0.f, 0.f, 0.f}};
    libc_types::f32                meter_value{0.f};

    bool  render_ui{false};
    bool  render_hud{true}; /* hidden while a menu is up */
    bool  debug_clear{false}; // sets a different clear color for visibility
    Vecf3 clear_color{0, 0.2f, 0.5f};

    bool structural_change_pending{false};
    bool occluder_update{true};

    bool debug_markers{
        compile_info::debug_mode && !compile_info::platform::is_mobile};
    bool debug_portals{false};
    bool debug_clusters{false};
    bool debug_triggers{false};

    f32 tex_res{0.f};
    f32 draw_distance{500.f};
    f32 lod_scale{1.f};
    libc_types::u32 lod_cap{0};

    /* Cascaded shadows: near cascades per seat, plus one shared */
    bool            shadows{true};
    libc_types::u32 shadow_resolution{2048}; /* Per tile, halved in splitscreen */
    libc_types::u32 shadow_cascades{3};      /* Per seat, 1-3 */
    f32             shadow_distance{60.f};   /* What the seat's cascades cover */
    f32             shadow_split{.85f};      /* 0 = even splits, 1 = logarithmic */

    libc_types::u32 current_bsp_cluster{
        std::numeric_limits<libc_types::u32>::max()};
    bool interior{false};
};

struct LoadingStatus : compo::SubsystemBase
{
    using type = LoadingStatus;

    enum loading_t
    {
        none,
        in_progress,
        loaded,
    };

    comp_app::interfaces::AppInfo* app_info{};
    std::string                    status;
    libc_types::i16                progress;
    std::atomic<bool>              loading{false};
    loading_t                      loaded_map{none};
    loading_t                      loaded_bitmaps{none};
    loading_t                      loaded_sounds{none};
    loading_t                      loaded_shaders{none};
    bool                           init_started{false};
    std::promise<void>             finished{};

    void set_shader_progress(libc_types::u32 ready, libc_types::u32 total)
    {
        using namespace Coffee::Logging;
        loading_t const previous = loaded_shaders;
        loaded_shaders           = ready == total ? loaded : in_progress;
        if(previous != loaded_shaders)
            cDebug("Shader programs linked: {}/{}", ready, total);
        if(loaded_shaders == loaded || loaded_map != loaded)
            return;
        /* Only once the map loader is done with the status line. */
        status = fmt::format("Compiling shaders ({}/{})", ready, total);
        progress =
            static_cast<libc_types::i16>(total ? ready * 100 / total : 0);
    }

    void check_all_loaded(bool quiet = false)
    {
        using namespace Coffee::Logging;
        bool            old_loading  = loading;
        libc_types::i16 old_progress = progress;
        if(loaded_map == loaded && loaded_bitmaps == loaded &&
           loaded_sounds == loaded && loaded_shaders == loaded)
        {
            bool was_loading = true;
            if(loading.compare_exchange_strong(was_loading, false))
                finished.set_value();
            progress = -1;
            app_info->setState(comp_app::interfaces::AppInfo::loaded);
        }
        if(!quiet)
            cDebug(
                "Re-evaluating loading state: loading={} -> {} progress={} -> "
                "{}",
                old_loading,
                loading,
                old_progress,
                progress);
    }
};

struct GameState : compo::SubsystemBase
{
    // TODO: Find pointer to game rules data
    // It might be filled in from ui.map?
};
