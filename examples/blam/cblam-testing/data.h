#pragma once

#include <atomic>
#include <future>
#include <limits>

#include <blam/volta/blam_stl.h>
#include <coffee/comp_app/services.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/identify/compiler/unreachable.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/vector_types.h>
#include <peripherals/typing/vectors/glm_vector_types.h>

#include "blam/volta/blam_versions.h"
#include "graphics_api.h"
#include "selected_version.h"

using namespace Coffee::StandardInput;

/* The only genuinely fixed reservation in the debug-line buffers: three
 * 2-vertex axis lines, drawn once at load (resource_creation.cpp) and
 * never resized. Everything else — physics body boxes, occluder eye
 * markers, map-load geometry — is allocated dynamically at runtime
 * (DebugMarkers::acquire_strip / create_marker / create_loop), bounded
 * only by the buffers' real capacity instead of hand-picked offsets. */
constexpr libc_types::u32 debug_axes_verts  = 6;
constexpr libc_types::u32 debug_axes_colors = 3;

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using semantic::Span;
struct BSPItem;
using typing::vector_types::Matf4;
using camera_t     = typing::vectors::scene::camera<f32>;
using std_camera_t = StandardCamera<camera_t*, StandardCameraOpts*>;

template<typename Version>
struct BlamData
{
    using type = BlamData<Version>;
};

struct BlamResources : compo::SubsystemBase
{
    using type = BlamResources;

    rq::runtime_queue* background_worker{nullptr};

    std::shared_ptr<gfx::buffer_t>       bsp_buf;
    std::shared_ptr<gfx::buffer_t>       bsp_index;
    std::shared_ptr<gfx::buffer_t>       bsp_light_buf;
    std::shared_ptr<gfx::vertex_array_t> bsp_attr;

    std::shared_ptr<gfx::program_t> bsp_pipeline;
    std::shared_ptr<gfx::program_t> model_pipeline;
    std::shared_ptr<gfx::program_t> chicago_pipeline; /* ES2 schi/scex combiner */
    std::shared_ptr<gfx::program_t> water_pipeline;    /* ES2 swat water */
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
    std::shared_ptr<gfx::revolving_buffer_t>
        bone_matrix_buf; /* per-frame bone transforms, separate from static
                            vertex data */

    typing::vector_types::Veci2          offscreen_size{1920, 1080};
    std::shared_ptr<gfx::rendertarget_t> offscreen;
    std::shared_ptr<gfx::texture_2d_t>   color;
    std::shared_ptr<gfx::texture_2d_t>   depth;
};

struct PostProcessParameters : compo::SubsystemBase
{
    using type = PostProcessParameters;

    f32  exposure{2.6f};
    f32  gamma{1.f};
    f32  scale{1.f};
    bool auto_expose{true};

    bool doom_mode{false};
};

struct RenderingParameters : compo::SubsystemBase
{
    using type = RenderingParameters;

    libc_types::u32 mipmap_bias{compile_info::platform::is_32bit ? 2 : 3};

    bool color_changing{true};
    bool render_fog{!std::is_same_v<blam::xbox_version_t, halo_version>};
    bool render_lightmaps{true};
    bool render_model_bones{true};
    bool render_reflection{true};
    bool render_scenery{true};
    bool only_normals{false};
    bool only_normalmaps{false};
    bool only_lightmaps{false};
    bool only_reflections{false};
    bool only_multipurpose{false};
    bool only_multipurpose2{false};
    bool only_diffuse{false};

    bool render_ui{false};
    bool debug_clear{false}; // sets a different clear color for visibility
    Vecf3 clear_color{0, 0.2f, 0.5f};

    bool occluder_update{true};

    bool debug_markers{
        compile_info::debug_mode && !compile_info::platform::is_mobile};
    bool debug_portals{false};
    bool debug_clusters{false};
    bool debug_triggers{false};


    f32 tex_res{0.f};
    f32 draw_distance{500.f};

    libc_types::u32 current_bsp_cluster{
        std::numeric_limits<libc_types::u32>::max()};
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
    std::atomic<bool> loading{false};
    loading_t                      loaded_map{none};
    loading_t                      loaded_bitmaps{none};
    loading_t                      loaded_sounds{none};
    loading_t                      loaded_shaders{none};
    bool                           init_started{false};
    std::promise<void>             finished{};

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
                "Re-evaluating loading state: loading={} -> {} progress={} -> {}",
                old_loading,
                loading,
                old_progress,
                progress);
    }
};

struct GameEvent
{
    enum EventType
    {
        None,
        MapLoadByName,
        MapLoadStart,
        MapRequestListing,
        MapListing,
        MapDataLoad,
        MapLoadFinished,
        MapDataReady,
        MapAllLoaded,
        MapChanged,
        ClusterChanged,

        PlayerTeleport,

        ServerConnect,
        ServerConnected,
        ServerCameraControl,
        ServerDisconnect,
        ServerStateUpdate,
        ServerPlayerStateUpdate,
    };

    EventType type{None};
};

struct MapLoadEvent
{
    static constexpr auto event_type = GameEvent::MapLoadStart;

    enum Origin
    {
        Local,
        Remote,
    } origin{Local};

    std::optional<platform::url::Url> directory{};
    std::optional<platform::url::Url> file{};
};

struct MapLoadByNameEvent
{
    static constexpr auto event_type = GameEvent::MapLoadByName;

    MapLoadEvent::Origin origin{MapLoadEvent::Local};
    blam::bl_string      map_name;
};

struct MapRequestListingEvent
{
    static constexpr auto event_type = GameEvent::MapRequestListing;
};

struct MapListingEvent
{
    static constexpr auto event_type = GameEvent::MapListing;

    platform::url::Url              directory;
    std::vector<platform::url::Url> maps;
    platform::url::Url              bitmap_file;
    platform::url::Url              sound_file;
};

struct MapDataLoadEvent
{
    static constexpr auto event_type = GameEvent::MapDataLoad;

    semantic::BytesConst map;
};

template<typename V>
struct MapLoadFinishedEvent
{
    static constexpr auto event_type = GameEvent::MapLoadFinished;

    blam::map_container<V>*      container;
    std::string                  map_name{};
    std::string                  map_title{};
    std::optional<blam::map_ptr> bitmaps{};
};

struct MapDataReadyEvent
{
    static constexpr auto event_type = GameEvent::MapDataReady;
};

template<typename V>
struct MapChangedEvent
{
    static constexpr auto event_type = GameEvent::MapChanged;

    blam::map_container<V>&       container;
    blam::scn::scenario<V> const* scenario{nullptr};
};

struct ClusterChangedEvent
{
    static constexpr auto event_type = GameEvent::ClusterChanged;

    BSPItem const* bsp{nullptr};
    u32            cluster{std::numeric_limits<u32>::max()};
};

struct PlayerTeleportEvent
{
    static constexpr auto event_type = GameEvent::PlayerTeleport;

    libc_types::u64 entity_id{0};
    libc_types::u32 seat_idx{0};
    Vecf3           position{};
};

struct ServerConnectEvent
{
    static constexpr auto event_type = GameEvent::ServerConnect;

    enum ConnectType : libc_types::u32
    {
        Peer,
        Server,
        Listen,
    } type{Server};

    std::string remote;
};

struct ServerConnectedEvent
{
    static constexpr auto event_type = GameEvent::ServerConnected;

    std::string     remote;
    libc_types::u32 seed{0}; /* RNG seed, to keep systems in sync */
};

struct ServerCameraControl
{
    static constexpr auto event_type = GameEvent::ServerCameraControl;

    enum RequestType : libc_types::u32
    {
        None,
        RequestCameraFocus,
        OverrideCamera,
    } request{None};

    libc_types::u32             target_player{0xFFFF};
    typing::vector_types::Vecf4 position{};
    Quatf                       rotation{};
};

struct ServerDisconnectEvent
{
    static constexpr auto event_type = GameEvent::ServerDisconnect;
};

struct ServerStateUpdate
{
    static constexpr auto event_type = GameEvent::ServerStateUpdate;

    enum StateField : libc_types::u32
    {
        None,
        ServerName,
        PlayerCount,
        PlayerMaxCount,
    } type{None};

    union
    {
        blam::bl_string string_field;
        libc_types::i32 num_field;
    };
};

struct ServerPlayerStateUpdate
{
    static constexpr auto event_type = GameEvent::ServerPlayerStateUpdate;

    enum StateField : libc_types::u32
    {
        None,
        Name,
    } type{None};

    libc_types::u32 index{0};

    union
    {
        blam::bl_string string_field;
        libc_types::i32 num_field;
    };
};

using GameEventBus = comp_app::BasicEventBus<GameEvent>;
