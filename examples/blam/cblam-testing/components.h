#pragma once

#include "caching.h"

#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/concepts/graphics_api.h> 
#include <blam/volta/blam_scenario.h>
#include <chrono>

using ERef = compo::EntityRef<compo::EntityContainer>;

using comp_app::detail::duration;
using comp_app::detail::time_point;

using vertex_type      = blam::vert::vertex<blam::vert::uncompressed>;
using xbox_vertex_type = blam::vert::vertex<blam::vert::compressed>;

using light_vertex_type = blam::vert::light_vertex<blam::vert::uncompressed>;
using light_xbox_vertex_type = blam::vert::light_vertex<blam::vert::compressed>;

using compo::ComponentRef;
using compo::EntityContainer;

enum Passes
{
    // Skybox passes — drawn before everything, no depth write
    Pass_SkyOpaque,
    Pass_SkyAdditive,
    Pass_SkyMultiply,
    Pass_SkyGlass,
    Pass_LastSky = Pass_SkyGlass,

    // Opaque world geometry — depth write on, stencil write
    Pass_Opaque,
    Pass_Alphatest,
    Pass_LastOpaque = Pass_Alphatest,

    // Transparent world geometry — depth write off
    Pass_Additive,
    Pass_Multiply,
    Pass_Glass,
    Pass_Max, // component_max blend (GL_MAX), e.g. stacked holograms

    Pass_Count,
};

struct Visibility
{
    using value_type = Visibility;
    using type = compo::alloc::BufferedContainer<value_type, 2>;
    bool visible{true};
};

/*! Per-frame draw bookkeeping, kept apart from the geometry it draws */
struct DrawState
{
    using value_type = DrawState;
    using type = compo::alloc::VectorContainer<value_type>;

    Passes            current_pass{Pass_Opaque};
    gfx::draw_command draw;
};

struct BspReference
{
    using value_type = BspReference;
    using type       = compo::alloc::VectorContainer<value_type>;

    generation_idx_t bsp;
    generation_idx_t shader;
    generation_idx_t lightmap;

    Vecf3  sort_center{};

    u32 cluster_idx{std::numeric_limits<u32>::max()};
    u32 subcluster_idx{std::numeric_limits<u32>::max()};
    /* All clusters owning this chunk's faces (boundary-straddling chunks have
     * several); the occluder hides the chunk when none is PVS-visible.
     * Empty = no cluster info, never PVS-culled. */
    std::vector<u16> clusters{};
    /* Chunk AABB (BSP space) for frustum culling; valid when has_bounds. */
    Vecf3 bmin{};
    Vecf3 bmax{};
    bool  has_bounds{false};
};

struct model_tracker_t
{
    u16  bucket;
    u16  draw;
    u16  instance;
    bool enabled{false};
};

static_assert(sizeof(model_tracker_t) == 8);

struct MeshTrackingData
{
    using value_type = MeshTrackingData;
    using type       = compo::alloc::VectorContainer<value_type>;

    model_tracker_t model_id;
};

struct SubModel
{
    using value_type = SubModel;
    using type       = compo::alloc::VectorContainer<value_type>;

    u64 parent;
    u16 cluster;

    generation_idx_t shader;
    generation_idx_t model;

    template<typename V>
    void initialize(
        generation_idx_t                       model_idx,
        typename ModelItem<V>::SubModel const& model_,
        DrawState&                             state)
    {
        state.draw.call = {
            .indexed   = true,
            .instanced = true,
            .mode      = gfx::drawing::primitive::triangle_strip,
        };
        state.draw.data = {
            model_.draw,
        };
        model  = model_idx;
        shader = model_.shader;
    }
};

template<typename SpawnType>
requires(!std::is_same_v<SpawnType, blam::scn::multiplayer_equipment>)
//
static Quatf spawn_rotation_to_quat(SpawnType const* spawn)
{
    return Quatf(Vecf3(0, 0, 1) * spawn->rot.x) *
           Quatf(Vecf3(0, 1, 0) * spawn->rot.y) *
           Quatf(Vecf3(1, 0, 0) * spawn->rot.z);
}

template<typename SpawnType>
requires std::is_same_v<SpawnType, blam::scn::multiplayer_equipment>
//
static Quatf spawn_rotation_to_quat(SpawnType const* spawn)
{
    return Quatf(Vecf3(0, 0, spawn->facing));
}

struct Model
{
    using value_type = Model;
    using type       = compo::alloc::VectorContainer<value_type>;
    using tag_type   = value_type;

    Matf4 transform;
    Vecf3 position;
    Quatf rotation;

    generation_idx_t                              model;
    std::vector<ERef>                             parts;
    semantic::mem_chunk<blam::mod2::region const> regions;

    blam::tag_t const* tag{nullptr};
    blam::tag_t const* origin_object{nullptr};

    template<typename T>
    void initialize(T const* spawn)
    {
        position = spawn->pos;
        rotation = spawn_rotation_to_quat(spawn);
        update_matrix();
    }

    void update_matrix()
    {
        transform =
            glm::translate(Matf4(1), position) * glm::mat4_cast(rotation);
    }
};

struct ObjectSpawn
{
    using value_type = ObjectSpawn;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::object_spawn const* header = nullptr;
    blam::tag_t const*             tag    = nullptr;
};

struct MultiplayerSpawn
{
    using value_type = MultiplayerSpawn;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::multiplayer_equipment const* spawn      = nullptr;
    blam::scn::item_collection const*       collection = nullptr;
    blam::scn::item const*                  item       = nullptr;
};

template<blam::tag_class_t TC, typename DataType, typename SourceType>
struct ShaderTemplate
{
    static constexpr blam::tag_class_t tag_class = TC;

    using data_type   = DataType;
    using source_type = SourceType;
};

struct ShaderData
{
    using value_type = ShaderData;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::tag_t const*                        shader_tag;
    blam::shader::radiosity_properties const* shader;

    generation_idx_t shader_id;

    template<typename T>
    inline T const* shader_data() const
    {
        return C_RCAST<T const*>(shader);
    }

    template<typename V>
    inline Passes get_render_pass(
        ShaderCache<V>& cache, bool skybox = false) const
    {
        using tc = blam::tag_class_t;
        using namespace enum_helpers;
        using namespace blam::shader;

        [[maybe_unused]] auto name =
            shader_tag->to_name().to_string(cache.magic);

        auto sky_pass = [skybox](Passes p) -> Passes {
            if(!skybox)
                return p;
            switch(p)
            {
            case Pass_Opaque:
            case Pass_Alphatest:
                return Pass_SkyOpaque;
            case Pass_Additive:
                return Pass_SkyAdditive;
            case Pass_Multiply:
                return Pass_SkyMultiply;
            default:
                return Pass_SkyGlass;
            }
        };

        switch(shader_tag->tagclass_e[0])
        {
        case tc::soso: {
            auto info = shader_data<shader_model>();
            bool alpha_test =
                !feval(info->flags & shader_model::model_flags::no_alpha_test);
            return sky_pass(alpha_test ? Pass_Alphatest : Pass_Opaque);
        }
        case tc::schi: {
            shader_chicago<blam::pc_version_t> const* info =
                shader_data<shader_chicago<blam::pc_version_t>>();
            using fb = chicago::framebuffer_blending;
            switch(info->transparent.blend_function)
            {
            case fb::multiply:
            case fb::double_multiply:
                return sky_pass(Pass_Multiply);
            case fb::add:
            case fb::alpha_multiply_add:
                return sky_pass(Pass_Additive);
            default:
                return sky_pass(Pass_Glass);
            }
        }
        case tc::scex: {
            shader_chicago_extended<blam::pc_version_t> const* info =
                shader_data<shader_chicago_extended<blam::pc_version_t>>();
            using fb = chicago::framebuffer_blending;
            switch(info->transparent.blend_function)
            {
            case fb::multiply:
            case fb::double_multiply:
                return sky_pass(Pass_Multiply);
            case fb::add:
            case fb::alpha_multiply_add:
                return sky_pass(Pass_Additive);
            default:
                return sky_pass(Pass_Glass);
            }
        }
        case tc::sotr: {
            shader_transparent const* info = shader_data<shader_transparent>();
            using fb                       = chicago::framebuffer_blending;
            switch(info->transparent.blend_function)
            {
            case fb::multiply:
            case fb::double_multiply:
                return sky_pass(Pass_Multiply);
            case fb::add:
            case fb::alpha_multiply_add:
            case fb::component_min:
                /* min has no dedicated state; additive is the closest match. */
                return sky_pass(Pass_Additive);
            case fb::component_max:
                /* true GL_MAX so stacked hologram layers stay bounded
                 * (additive would sum overlaps to white). */
                return sky_pass(Pass_Max);
            default:
                return sky_pass(Pass_Glass);
            }
        }
        case tc::swat:
        case tc::sgla:
            return sky_pass(Pass_Glass);
        case tc::senv: {
            shader_env const* info = shader_data<shader_env>();
            Passes p = feval(info->flags & shader_env::flags_t::alpha_tested)
                           ? Pass_Alphatest
                           : Pass_Opaque;
            return sky_pass(p);
        }
        case tc::smet:
            return sky_pass(Pass_Opaque);
        case tc::spla:
            /* Plasma is glowing energy — rendered additively. */
            return sky_pass(Pass_Additive);
        default:
            return sky_pass(Pass_Opaque);
        }
    }

    template<typename V>
    inline ShaderItem const& get_shader(ShaderCache<V>& cache) const
    {
        return cache.find(shader_id)->second;
    }

    template<typename T>
    inline bool match_data() const
    {
        return T::tag_class == shader_tag->tagclass_e[0];
    }

    void initialize(ShaderItem const& shader_, SubModel const& submod)
    {
        shader     = shader_.header;
        shader_tag = shader_.tag;
        shader_id  = submod.shader;
    }
};

struct DebugDraw
{
    using value_type = DebugDraw;
    using type       = compo::alloc::VectorContainer<value_type>;

    gfx::draw_command::data_t data{};
    u32                       color_ptr;
    bool                      selected{false};
};

struct TriggerVolume
{
    using value_type = TriggerVolume;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::trigger_volume const* trigger_volume{nullptr};
};

struct Light
{
    using value_type = Light;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::skybox::light const* light{nullptr};
    blam::mod2::marker const*       positioning{nullptr};

    Matf4 transform;
};

/**
 * Cluster association info
 * Attaches an object to a cluster, for occlusion purposes
 */
struct Cluster
{
    using value_type = Cluster;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::bsp::cluster const* cluster;
    u32                       id;

    bool contains(Matf4 const& transform, blam::map_ptr const& magic) const
    {
        auto subclusters_ = cluster->sub_clusters.data(magic);
        if(!subclusters_.has_value())
            return false;
        Span<blam::bsp::subcluster const> subclusters = subclusters_.value();
        Vecf3 probe = transform * Vecf4(Vecf3(0), 1);
        for(blam::bsp::subcluster const& cluster : subclusters)
        {
            if(!cluster.bounds.contains(probe))
                continue;
            return true;
        }
        return false;
    }
};

/**
 * Depth info for sorting objects in space relative to the camera
 * Contains
 * 1) a position in world-space and
 * 2) the relative distance to the camera (updated per-frame)
 * This can help drawing opaque objects more efficiently
 * based on the depth buffer
 */
struct DepthInfo
{
    using value_type = DepthInfo;
    using type       = compo::alloc::VectorContainer<value_type>;

    Vecf3 position;
    Vecf3 distance;

    bool masked{
        false}; /*! Objects such as transparent ones need to be drawn anyway */
};

struct PhysicsData
{
    using value_type = PhysicsData;
    using type       = compo::alloc::VectorContainer<PhysicsData>;

    u64   physics_id{0};
    Vecf3 velocity;
    Vecf3 acceleration;

    bool enabled{false};
};

struct NetworkInfo
{
    using value_type = NetworkInfo;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::tagref_t object;
    u32            instance_id{0};
    bool           connected{};

    struct
    {
        bool transform : 1 {false};
        bool viewport : 1 {false};
        bool permissions : 1 {false};
    } changes;

    f32                       priority{0.f};
    f32                       relevance{0.f};
    std::chrono::milliseconds update_period{300};
};

struct PlayerInfo
{
    using value_type = PlayerInfo;
    using type       = compo::alloc::VectorContainer<value_type>;

    std::string name;
    std::string remote;
    u32         player_idx{0};
    u32         seat_idx{0};
    u32         loading_progress{100};

    struct permissions_t
    {
        bool move{true};
        bool camera{true};
    } permissions;

    bool is_remote() const
    {
        return !remote.empty();
    }
};

struct PlayerInput
{
    using value_type = PlayerInput;
    using type       = compo::alloc::VectorContainer<value_type>;

    StandardCamera::Reg keys;         /*!< held keys, from KeyboardInput */
    Vecf2               look_delta{}; /*!< accumulated look; zeroed once applied */
    Vecf3 movement{};
    f32   accel{1.f};           /*!< speed modifier chosen by the source */
    bool  jump{false};

    std::optional<Vecf3> position; /*!< teleport target */
    std::optional<Quatf> rotation; /*!< absolute orientation */

    // TODO: Add more inputs later
};

struct PlayerCamera
{
    using value_type = PlayerCamera;
    using type       = compo::alloc::VectorContainer<value_type>;

    using camera_t         = typing::vectors::scene::camera<f32>;

    StandardCameraOpts camera_opts{};
    StandardCamera     camera_{};
    camera_t           camera{};

    struct
    {
        ControllerOpts     opts{};
        std::optional<i32> index{};
    } controller;

    struct
    {
        bool enabled{false};
    } keyboard;

    struct
    {
        bool freecam{false};
        bool physics{false};
    } mode;

    struct
    {
        Vecf2 movement_plane; /*!< Physics impulse movement
                               * forward = normalized to ground level */
        Vecf3 movement;       /*!< Freecam instant movement
                               * forward = camera forward */
    } inputs;

    Matf4 matrix{};
    Matf4 rotation{};

    bool is_active() const
    {
        return keyboard.enabled || controller.index.has_value();
    }
};

struct SoundEffects
{
    using value_type = SoundEffects;
    using type       = compo::alloc::VectorContainer<value_type>;

    std::vector<generation_idx_t> tracks; /* sound effects/music to play */
};

enum ObjectTags : u64
{
    ObjectScenery      = 0x1,
    ObjectEquipment    = 0x2,
    ObjectVehicle      = 0x4,
    ObjectBiped        = 0x8,
    ObjectDevice       = 0x10,
    ObjectLightFixture = 0x20,
    ObjectControl      = 0x40,
    ObjectSkybox       = 0x80,
    ObjectObject       = 0x1000,
    ObjectUnit         = ObjectObject << 1,
    ObjectMod2         = 0x10000,
    ObjectBsp          = ObjectMod2 << 1,

    ObjectScriptObject    = 0x100000,
    ObjectTriggerVolume   = ObjectScriptObject << 1,
    ClusterNode           = ObjectScriptObject << 2,
    PositioningStatic     = 0x1000000,
    PositioningDynamic    = PositioningStatic << 1,
    PositioningBackground = PositioningStatic << 2,
    ObjectGC              = 0x8000000, /* Erased on map load */
    PlayerBiped           = 0x10000000,

    SubObjectMask   = 0x00FFFFF,
    PositioningMask = 0xF000000,
};

namespace shared_recipes {
static const auto player_recipe = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<PlayerInfo>(),
            compo::type_hash_v<NetworkInfo>(),
            compo::type_hash_v<PlayerCamera>(),
            compo::type_hash_v<PlayerInput>(),
            compo::type_hash_v<SoundEffects>(),
            compo::type_hash_v<PhysicsData>(),
            compo::type_hash_v<DebugDraw>(),
        },
    .tags = PlayerBiped,
};

static const auto skybox_model = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<Model>(),
            compo::type_hash_v<Visibility>(),
        },
    .tags = ObjectSkybox | ObjectGC,
};

static const auto skybox_submodel = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<SubModel>(),
            compo::type_hash_v<DrawState>(),
            compo::type_hash_v<ShaderData>(),
            compo::type_hash_v<MeshTrackingData>(),
        },
    .tags = ObjectSkybox | ObjectMod2 | ObjectGC,
};

static const auto bsp = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<BspReference>(),
            compo::type_hash_v<DrawState>(),
            compo::type_hash_v<DepthInfo>(),
            compo::type_hash_v<ShaderData>(),
            compo::type_hash_v<Visibility>(),
        },
    .tags = ObjectBsp | ObjectGC,
};

static const auto model = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<DepthInfo>(),
            compo::type_hash_v<Model>(),
            compo::type_hash_v<NetworkInfo>(),
            compo::type_hash_v<ObjectSpawn>(),
            compo::type_hash_v<Visibility>(),
        },
    .tags = ObjectGC,
};

static const auto multiplayer_spawn = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<DepthInfo>(),
            compo::type_hash_v<Model>(),
            compo::type_hash_v<MultiplayerSpawn>(),
            compo::type_hash_v<NetworkInfo>(),
            compo::type_hash_v<Visibility>(),
        },
    .tags = ObjectGC,
};

static const auto submodel = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<MeshTrackingData>(),
            compo::type_hash_v<ShaderData>(),
            compo::type_hash_v<SubModel>(),
            compo::type_hash_v<DrawState>(),
        },
    .tags = ObjectMod2 | ObjectGC,
};

static const auto gc_marker = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<DebugDraw>(),
        },
    .tags = ObjectGC,
};

}
