#pragma once

#include <blam/volta/blam_stl.h>
#include <coffee/comp_app/services.h>
#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/vector_types.h>

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

    blam::map_container<Version> map_container;
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
    Matf4                         camera_matrix{};
    Matf4                         rotation_matrix{};
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

    rq::runtime_queue* background_worker{nullptr};

    std::shared_ptr<gfx::buffer_t>       bsp_buf;
    std::shared_ptr<gfx::buffer_t>       bsp_index;
    std::shared_ptr<gfx::vertex_array_t> bsp_attr;
    std::shared_ptr<gfx::buffer_t>       bsp_light_buf;

    std::shared_ptr<gfx::program_t> bsp_pipeline;
    std::shared_ptr<gfx::program_t> model_pipeline;
    std::shared_ptr<gfx::program_t> senv_micro_pipeline;
    std::shared_ptr<gfx::program_t> wireframe_pipeline;

    std::shared_ptr<gfx::buffer_t>       model_buf;
    std::shared_ptr<gfx::buffer_t>       model_index;
    std::shared_ptr<gfx::vertex_array_t> model_attr;

    std::shared_ptr<gfx::buffer_t>       debug_lines;
    std::shared_ptr<gfx::buffer_t>       debug_line_colors;
    std::shared_ptr<gfx::vertex_array_t> debug_attr;
    std::shared_ptr<gfx::program_t>      debug_lines_pipeline;

    std::shared_ptr<gfx::buffer_t> model_matrix_store;
    std::shared_ptr<gfx::buffer_t> material_store;
    std::shared_ptr<gfx::buffer_t> transparent_store;
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

    libc_types::u32 mipmap_bias{0};

    bool render_scenery{true};
    bool render_ui{true};
    bool debug_clear{true};

    bool debug_portals{false};
    bool debug_clusters{false};
    bool debug_triggers{false};
};

struct LoadingStatus : compo::SubsystemBase
{
    using type = LoadingStatus;

    std::string     status;
    libc_types::i16 progress;
    bool            loading{false};
};

struct GameEvent
{
    enum EventType
    {
        None,
        MapLoadStart,
        MapListing,
        MapDataLoad,
        MapLoadFinished,
        MapChanged,

        ServerConnect,
        ServerDisconnect,
    };
    EventType type{None};
};

struct MapLoadEvent
{
    static constexpr auto event_type = GameEvent::MapLoadStart;

    std::optional<platform::url::Url> directory;
    std::optional<platform::url::Url> file;
};

struct MapListingEvent
{
    static constexpr auto event_type = GameEvent::MapListing;

    platform::url::Url              directory;
    std::vector<platform::url::Url> maps;
    platform::url::Url              bitmap_file;
};

struct MapDataLoadEvent
{
    static constexpr auto event_type = GameEvent::MapDataLoad;

    semantic::BytesConst map;
    semantic::BytesConst bitmap;
};

template<typename V>
struct MapLoadFinishedEvent
{
    static constexpr auto event_type = GameEvent::MapLoadFinished;

    blam::map_container<V> container;
    semantic::BytesConst   bitmap_file;
};

template<typename V>
struct MapChangedEvent
{
    static constexpr auto event_type = GameEvent::MapChanged;

    blam::map_container<V>&       container;
    blam::magic_data_t            bitmap_magic;
    blam::scn::scenario<V> const* scenario{nullptr};
};

struct ServerConnectEvent
{
    static constexpr auto event_type = GameEvent::ServerConnect;

    enum ConnectType
    {
        Peer,
        Server,
    };
    ConnectType type{Server};
    std::string remote;
};

struct ServerDisconnectEvent
{
    static constexpr auto event_type = GameEvent::ServerDisconnect;
};

using GameEventBus = comp_app::BasicEventBus<GameEvent>;
