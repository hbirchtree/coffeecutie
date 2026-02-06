#pragma once

#include <blam/volta/blam_stl.h>
#include <coffee/comp_app/services.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/identify/compiler/unreachable.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/vector_types.h>

#include "graphics_api.h"

using namespace Coffee::StandardInput;

constexpr libc_types::u32 reserved_debug_points = 24 + 16 * 7;
constexpr libc_types::u32 reserved_debug_colors = 6 + 16;

using libc_types::f32;
using semantic::Span;
using typing::vector_types::Matf4;
using camera_t     = typing::vectors::scene::camera<f32>;
using std_camera_t = StandardCamera<camera_t*, StandardCameraOpts*>;

template<typename Version>
struct BlamData
{
    using type = BlamData<Version>;
};

struct BlamCamera : compo::SubsystemBase
{
    using type = BlamCamera;

    BlamCamera()
    {
        viewports[0].active = true;
    }

    struct viewport_t
    {
        viewport_t()
            : camera_(std::make_shared<std_camera_t>(&camera, &camera_opts))
        {
        }

        viewport_t(const viewport_t&) = delete;

        StandardCameraOpts            camera_opts;
        camera_t                      camera;
        std::shared_ptr<std_camera_t> camera_;
        Matf4                         matrix{};
        Matf4                         rotation{};
        ControllerOpts                controller_opts;
        bool                          active{false};
    };

    std::array<viewport_t, 8> viewports;
    libc_types::u32           focused_player{0};

    viewport_t& player(libc_types::u32 idx = 0)
    {
        if(idx >= viewports.size())
            Throw(implementation_error("player idx > 3 does not exist"));
        return viewports[idx];
    }

    libc_types::u32 num_players() const
    {
        libc_types::u32 sum = 0;
        for(auto const& viewport : viewports)
            sum += viewport.active ? 1 : 0;
        return sum;
    }
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

    libc_types::u32 mipmap_bias{compile_info::platform::is_32bit ? 2 : 3};

    bool render_scenery{true};
    bool render_ui{false};
    bool debug_clear{true};

    bool debug_markers{
        compile_info::debug_mode && !compile_info::platform::is_mobile};
    bool debug_portals{false};
    bool debug_clusters{false};
    bool debug_triggers{false};

    bool color_changing{true};

    f32 tex_res{0.f};
    f32 draw_distance{500.f};
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
    bool                           loading{false};
    loading_t                      loaded_map{none};
    loading_t                      loaded_bitmaps{none};
    loading_t                      loaded_sounds{none};

    void check_all_loaded()
    {
        if(loaded_map == loaded && loaded_bitmaps == loaded &&
           loaded_sounds == loaded)
        {
            loading  = false;
            progress = -1;
            app_info->setState(comp_app::interfaces::AppInfo::loaded);
        }
    }
};

struct GameEvent
{
    enum EventType
    {
        None,
        MapLoadByName,
        MapLoadStart,
        MapListing,
        MapDataLoad,
        MapLoadFinished,
        MapChanged,

        ServerConnect,
        ServerConnected,
        ServerCameraControl,
        ServerDisconnect,
        ServerStateUpdate,
        ServerPlayerStateUpdate,
    };

    EventType type{None};
};

struct MapLoadByName
{
    static constexpr auto event_type = GameEvent::MapLoadByName;

    blam::bl_string map_name;
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

template<typename V>
struct MapChangedEvent
{
    static constexpr auto event_type = GameEvent::MapChanged;

    blam::map_container<V>&       container;
    blam::scn::scenario<V> const* scenario{nullptr};
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
    } request{None};

    libc_types::u32 target_player{0xFFFF};
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
