#pragma once

#include <atomic>
#include <chrono>
#include <future>
#include <limits>

#include <blam/volta/blam_stl.h>
#include <coffee/comp_app/services.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/identify/compiler/unreachable.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/glm_vector_types.h>
#include <peripherals/typing/vectors/vector_types.h>

#include "blam/volta/blam_mod2.h"
#include "blam/volta/blam_tag_classes.h"
#include "blam/volta/blam_tag_ref.h"
#include "blam/volta/blam_versions.h"
#include "graphics_api.h"
#include "selected_version.h"

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

    std::shared_ptr<gfx::program_t> bsp_pipeline;
    std::shared_ptr<gfx::program_t> model_pipeline;
    /* Per-material-family builds of the uber shaders. A pass holding only one
     * family binds these instead, keeping sotr's register budget off every
     * other material. Null when the platform builds no uber shaders. */
    std::shared_ptr<gfx::program_t> bsp_pipeline_nosotr;
    std::shared_ptr<gfx::program_t> bsp_pipeline_base;
    std::shared_ptr<gfx::program_t> bsp_pipeline_chicago;
    std::shared_ptr<gfx::program_t> bsp_pipeline_sotr;
    std::shared_ptr<gfx::program_t> model_pipeline_nosotr;
    std::shared_ptr<gfx::program_t> model_pipeline_base;
    std::shared_ptr<gfx::program_t> model_pipeline_chicago;
    std::shared_ptr<gfx::program_t> model_pipeline_sotr;
    std::shared_ptr<gfx::program_t>
        chicago_pipeline;                           /* ES2 schi/scex combiner */
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

    f32   exposure{2.f};
    f32   gamma{.7f};
    f32   scale{1.f};
    bool  auto_expose{true};
    f32   blur{0.f};
    Vecf4 rgb_comp{};

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

    // BSP-exclusive
    bool only_aux_channels{false};
    bool only_micro{false};
    bool only_lightmaps{false};
    bool only_normalmaps{false};
    bool render_lightmaps{true};

    std::array<libc_types::f32, 4> object_function{{0.f, 0.f, 0.f, 0.f}};
    libc_types::f32                meter_value{0.f};

    bool  render_ui{false};
    bool  debug_clear{false}; // sets a different clear color for visibility
    Vecf3 clear_color{0, 0.2f, 0.5f};

    bool structural_change_pending{false};
    bool occluder_update{true};

    bool debug_markers{
        compile_info::debug_mode && !compile_info::platform::is_mobile};
    bool debug_portals{false};
    bool debug_clusters{false};
    bool debug_triggers{false};

    f32   tex_res{0.f};
    f32   draw_distance{500.f};

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
        SkyboxChanged,
        WeatherChanged,

        // Resource loading
        MountModel, // SpawnModel, but to an existing entity
        SpawnBiped,
        SpawnBSP,
        SpawnEquipment,
        SpawnModel,

        PlayModelAnimation,

        PlayerTeleport,
        PlayerCameraLerp,

        ServerConnect,
        ServerConnected,
        ServerCameraControl,
        ServerDisconnect,
        ServerJoinInfo,
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

struct SkyboxChangedEvent
{
    static constexpr auto event_type = GameEvent::SkyboxChanged;
};

struct WeatherChangedEvent
{
    static constexpr auto event_type = GameEvent::WeatherChanged;
};

struct MountModelEvent
{
    static constexpr auto event_type = GameEvent::MountModel;
    blam::tagref_typed_t<blam::tag_class_t::mod2> model;
    u64 entity_id{0};
};

struct SpawnBipedEvent
{
    static constexpr auto event_type = GameEvent::SpawnBiped;
    blam::tagref_typed_t<blam::tag_class_t::bipd> biped;
    u32 player_idx{0};
    u32 tags{};
};

struct SpawnBSPEvent
{
    static constexpr auto event_type = GameEvent::SpawnBSP;
    u32 section_id{};
};

struct SpawnEquipmentEvent
{
    static constexpr auto event_type = GameEvent::SpawnEquipment;
};

struct SpawnModelEvent
{
    static constexpr auto event_type = GameEvent::SpawnModel;
    blam::tagref_typed_t<blam::tag_class_t::mod2> model;

    // TODO: Add param for which variant, eg. marine variant
    blam::mod2::mod2_lod max_lod_level{};
};

struct PlayModelAnimationEvent
{
    static constexpr auto event_type = GameEvent::PlayModelAnimation;
};

struct PlayerTeleportEvent
{
    static constexpr auto event_type = GameEvent::PlayerTeleport;

    libc_types::u64      entity_id{0};
    libc_types::u32      seat_idx{0};
    Vecf3                position{};
    std::optional<Quatf> rotation{};
};

struct PlayerCameraLerpEvent
{
    static constexpr auto event_type = GameEvent::PlayerCameraLerp;

    libc_types::u32           seat_idx{0};
    Vecf3                     position{};
    std::optional<Quatf>      rotation{};
    std::chrono::milliseconds duration{};
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

    /* Only used with type == Listen: if set, additionally register this
     * server with a webrtc-gateway fleet registry so browser clients can
     * reach it (see gateway_fleet_registration.h). Empty = don't
     * register. */
    std::string gateway_register_url;
    std::string gateway_server_id;

    /* Only used with type == Listen and gateway_register_url non-empty:
     * enables signed metadata so clients can verify the server's identity
     * independently of the gateway. */
    std::string gateway_auth_secret; /* HMAC-SHA256 key, base64 */
    std::string gateway_auth_key;    /* Ed25519 private key PEM path */
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

struct ServerJoinInfo
{
    static constexpr auto event_type = GameEvent::ServerJoinInfo;

    blam::bl_string_var<128> server_id; /*!< ID part of URL passed to gateway */
    blam::bl_string_var<64>  secret;    /*!< Secret for connect, if needed */
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
        ServerJoinable,
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
