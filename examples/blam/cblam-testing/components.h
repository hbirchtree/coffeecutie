#pragma once

#include "access.h"
#include "allocators.h"
#include "caching.h"
#include "types.h"

#include <blam/volta/blam_antr.h>
#include <blam/volta/blam_collision.h>
#include <blam/volta/blam_scenario.h>
#include <chrono>
#include <memory>
#include <optional>
#include <coffee/core/input/standard_input_handlers.h>
#include <peripherals/concepts/graphics_api.h>
#include <utility>

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
    Pass_Water,
    Pass_Glass,
    Pass_Max, // component_max blend (GL_MAX), e.g. stacked holograms

    // Effects that rely on framebuffer data, read from a copy of the world
    // taken before first-person is drawn
    Pass_Postprocess,

    // First-person models — drawn after the world into a depth slice nearest
    // the camera, only for the owning seat. Models only, so no water
    Pass_FP_Opaque,
    Pass_FP_Additive,
    Pass_FP_Multiply,
    Pass_FP_Max,
    Pass_FP_Glass,
    Pass_LastFP = Pass_FP_Glass,
    Pass_FP_Postprocess, // reads the same pre-first-person copy

    Pass_Count,
};

inline bool pass_is_first_person(Passes pass)
{
    return pass >= Pass_FP_Opaque && pass <= Pass_FP_Postprocess;
}

inline bool pass_is_postprocess(Passes pass)
{
    return pass == Pass_Postprocess || pass == Pass_FP_Postprocess;
}

/* Blended passes, sorted by depth and inserted one instance per draw */
inline bool pass_is_sorted(Passes pass)
{
    return pass > Pass_LastOpaque && pass != Pass_FP_Opaque;
}

/* Which set of passes a shader is placed in */
enum class render_layer
{
    world,
    skybox,
    first_person,
};

struct Visibility
{
    using value_type = Visibility;
    using type       = compo::alloc::BufferedContainer<value_type, 2>;

    // Viewport ID: player ID/mirror
    // Only player 1 and 2 have a mirror as per the old campaign limitations
    using viewport_id = std::pair<u32, bool>;

    std::map<viewport_id, bool> visible{};
    // Detail level the occluder picked, per viewport, in mod2_lod order
    std::map<viewport_id, u8> lod{};
    // Interior is invariant to viewports
    bool interior{false};

    u8 lod_for(u32 player_id = 0, bool mirror = false) const
    {
        auto it = lod.find(std::make_pair(player_id, mirror));
        return it == lod.end() ? 0 : it->second;
    }
    // Set for player biped to prevent rendering for self
    std::optional<u32> skip_render_for;
    // Set for first-person models, only drawn for their owner and never culled
    std::optional<u32> only_render_for;
    bool _dummy{};

    /* `show_self`: draw own biped, for third-person cameras */
    bool visible_for(
        u32 player_id = 0, bool mirror = false, bool show_self = false) const
    {
        if(only_render_for.has_value())
            return !mirror && *only_render_for == player_id;
        if(skip_render_for.has_value() && !mirror && !show_self)
            if(*skip_render_for == player_id)
                return false;
        if(auto vis = visible.find(std::make_pair(player_id, mirror));
           vis != visible.end())
            return vis->second;
        return true;
    }

    bool visible_any() const
    {
        if(visible.empty())
            return true;
        for(auto const& [id, v] : visible)
            if(v)
                return true;
        return false;
    }

    void hide_all()
    {
        for(auto& [id, v] : visible)
            v = false;
    }

    void set_visibility(bool visible, u32 player_id = 0, bool mirror = false)
    {
        if(skip_render_for.has_value())
            if(*skip_render_for == player_id)
                return;
        this->visible[std::make_pair(player_id, mirror)] = visible;
    }

    bool& visibility(u32 player_id = 0, bool mirror = false)
    {
        if(skip_render_for.has_value())
            if(*skip_render_for == player_id)
                return _dummy;
        return visible[std::make_pair(player_id, mirror)];
    }

    // TODO: We need to decide at some point if visibility extends from server
    // to client data
};

/*! Per-frame draw bookkeeping, kept apart from the geometry it draws */
struct DrawState
{
    using value_type = DrawState;
    using type       = compo::alloc::VectorContainer<value_type>;

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

    Vecf3 sort_center{};

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
    u32  epoch{0};
};

static_assert(sizeof(model_tracker_t) == 12);

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
    /* Detail levels this part is drawn for, bit per mod2_lod */
    u8 lod_mask{0x1F};
    /* An overlay drawn only while this object function output is on */
    libc_types::i8 gate{-1};

    template<typename ModelT>
    bool shown(ModelT const& model) const
    {
        return gate < 0 || model.object_function[gate] > .01f;
    }

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

    Matf4 transform; /*!< As drawn, between the last two ticks if it moves */
    Vecf3 position;
    Quatf rotation;
    f32   scale{1.f}; /*!< A script's, applied after rotation */

    /*! The pose a tick ago, for drawing between ticks */
    Vecf3 tick_position{};
    Quatf tick_rotation{1.f, 0.f, 0.f, 0.f};
    bool  tick_posed{false};

    generation_idx_t                              model;
    std::vector<ERef>                             parts;
    semantic::mem_chunk<blam::mod2::region const> regions;

    blam::tag_t const* tag{nullptr};
    blam::tag_t const* origin_object{nullptr};
    /*! A weapon on the ground: the battery it was dropped with */
    libc_types::f32 item_age{0.f};

    /*! Active camouflage: drawn as a refraction of what is behind it */
    bool camouflaged{false};
    /* Region permutations, as mod2::header::model_at takes them */
    libc_types::u32 permutations{0};

    std::array<generation_idx_t, 5> lod_models{};
    std::array<libc_types::f32, 5>  lod_cutoff{};
    u8                              lod_built{0};
    libc_types::f32 render_radius{0.f};

    u8 lod_resolve(u8 wanted) const
    {
        for(int l = std::min<int>(wanted, 4); l >= 0; l--)
            if((lod_built >> l) & 1)
                return static_cast<u8>(l);
        for(int l = wanted + 1; l < 5; l++)
            if((lod_built >> l) & 1)
                return static_cast<u8>(l);
        return 0;
    }

    bool has_lods() const
    {
        for(auto cutoff : lod_cutoff)
            if(cutoff > 0.f)
                return true;
        return false;
    }

    std::array<libc_types::f32, 4> object_function{{-1.f, -1.f, -1.f, -1.f}};
    libc_types::f32                meter_value{-1.f};

    template<typename T>
    void initialize(T const* spawn)
    {
        position = spawn->pos;
        rotation = spawn_rotation_to_quat(spawn);
        update_matrix();
    }

    void update_matrix()
    {
        transform = glm::translate(Matf4(1), position) * glm::mat4_cast(rotation) *
                    glm::scale(Matf4(1), Vecf3(scale));
    }
};

struct ObjectSpawn
{
    using value_type = ObjectSpawn;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::object_spawn const* header = nullptr;
    blam::tag_t const*             tag    = nullptr;
    libc_types::f32                power{-1.f};
};

/*! A machine or control: how far along its position animation it is and
 *  how powered, and where it is going. Positions and powers in device
 *  groups follow their group's value */
struct DeviceState
{
    using value_type = DeviceState;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::device const*         tag{nullptr};
    blam::scn::machine const*        machine{nullptr}; /*!< Machines only */
    blam::scn::device_control const* control{nullptr}; /*!< Controls only */
    i16 power_group{-1};
    i16 position_group{-1};
    u16 flags{0};      /*!< Scenario device flags */
    u16 kind_flags{0}; /*!< Scenario machine or control flags */

    f32 power{1.f};
    f32 power_target{1.f};
    f32 position{0.f};
    f32 position_target{0.f};
    f32 open_left{0.f};  /*!< Doors: seconds held open after a unit leaves */
    /*! Group values last taken: a group moves its devices when it changes,
     *  so a door can open on its own without moving the rest */
    f32 position_seen{-1.f};
    f32 power_seen{-1.f};
    u32 snaps{0}; /*!< Times set without moving there, for replicas */
    bool used{false};    /*!< Changes once, and has */
    bool automatic{true}; /*!< Doors: open for units nearby */

    /* Scenario device flags */
    static constexpr u16 initially_open  = 0x1;
    static constexpr u16 initially_off   = 0x2;
    static constexpr u16 changes_once    = 0x4;
    static constexpr u16 reversed        = 0x8;
    static constexpr u16 not_usable      = 0x10;
    /* Scenario machine flags */
    static constexpr u16 not_automatic = 0x1;
    static constexpr u16 one_sided     = 0x2;
    /* Scenario control flags */
    static constexpr u16 both_sides = 0x1;
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

    bool camouflaged{false};

    generation_idx_t shader_id;

    template<typename T>
    inline T const* shader_data() const
    {
        return C_RCAST<T const*>(shader);
    }

    template<typename V>
    inline Passes get_render_pass(
        ShaderCache<V>& cache, render_layer layer = render_layer::world) const
    {
        using tc = blam::tag_class_t;
        using namespace enum_helpers;
        using namespace blam::shader;

        [[maybe_unused]] auto name =
            shader_tag->to_name().to_string(cache.magic);

        auto sky_pass = [layer](Passes p) -> Passes {
            switch(layer)
            {
            case render_layer::world:
                return p;
            case render_layer::skybox:
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
            case render_layer::first_person:
                switch(p)
                {
                case Pass_Opaque:
                case Pass_Alphatest:
                    return Pass_FP_Opaque;
                case Pass_Additive:
                    return Pass_FP_Additive;
                case Pass_Multiply:
                    return Pass_FP_Multiply;
                case Pass_Max:
                    return Pass_FP_Max;
                default:
                    return Pass_FP_Glass;
                }
            }
            return p;
        };

        switch(shader_tag->tagclass_e[0])
        {
        case tc::soso: {
            auto info = shader_data<shader_model>();
            // TODO: Distinguish other postprocess shaders
            if(camouflaged)
                return layer == render_layer::first_person
                           ? Pass_FP_Postprocess
                           : Pass_Postprocess;
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
            return sky_pass(Pass_Water);
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
            return sky_pass(Pass_Glass);
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

    bool contains(Matf4 const& transform, blam::bsp_ptr const& magic) const
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

struct WorldInfo
{
    using value_type = WorldInfo;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::skybox const* skybox{nullptr};
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

/*! A coll tag in object space (bind pose) */
struct CollisionGeometry
{
    /*! One convex hull per collision node */
    struct node_t
    {
        std::vector<Vecf3> points;
        i32                bone{-1};
        bool               head{false};
        i16                material{-1}; /*!< Into the coll tag's materials */
        /*! Its material's kind, what projectiles respond to; none if unset */
        libc_types::u16 material_type{0xFFFF};
    };

    std::vector<node_t> nodes;
    std::vector<Vecf3>  triangles; /*!< Exact surfaces, 3 per triangle */
    /*! Each triangle's material kind, as node_t::material_type */
    std::vector<libc_types::u16> triangle_materials;
};

/*! A phys tag: the body's mass and the spheres (mass points) it meets the
 * world with, model space */
struct MassPoints
{
    struct powered_t
    {
        bool traction{false}; /*!< ground friction: drives the vehicle */
        bool antigrav{false};
        f32  strength{0.f};
        f32  offset{0.f};
        f32  height{0.f};
        f32  damp_fraction{0.f};
        f32  normal_k1{1.f};
        f32  normal_k0{0.f};
    };

    struct point_t
    {
        enum class friction_t : u16
        {
            point,
            forward,
            left,
            up,
        };

        Vecf3      position{};
        Vecf3      forward{1, 0, 0};
        Vecf3      up{0, 0, 1};
        f32        radius{0.f};
        f32        mass{0.f};
        i16        powered{-1};
        friction_t friction{friction_t::point};
        f32        parallel{1.f};
        f32        perpendicular{1.f};
    };

    f32   mass{0.f};
    f32   radius{0.f}; /*!< How far its torques reach */
    Vecf3 center_of_mass{};
    Vecf3 moments{}; /*!< Principal inertia, xx yy zz */
    f32   gravity_scale{1.f};
    f32   ground_friction{0.f};
    f32   ground_depth{0.f};
    f32   ground_damp_fraction{0.f};
    f32   ground_normal_k1{1.f};
    f32   ground_normal_k0{0.f};
    f32   air_friction{0.f};

    std::vector<powered_t> powered;
    std::vector<point_t>   points;
};

/*! How a vehicle tag drives, per second and in radians */
struct VehicleDrive
{
    enum type_t : u16
    {
        human_tank,
        human_jeep,
        human_boat,
        human_plane,
        alien_scout,
        alien_fighter,
        turret,
    };

    type_t type{human_jeep};
    f32    forward_speed{0.f};
    f32    reverse_speed{0.f};
    f32    acceleration{0.f};
    f32    deceleration{0.f};
    f32    max_turn{0.f};  /*!< Steered wheel angle */
    f32    turn_rate{0.f}; /*!< How fast the wheels or the body turn */
    f32    wheel_circumference{0.f};
    f32    slide_speed{0.f}; /*!< Sideways, either way */
    f32    slide_acceleration{0.f};
    /*! Seconds the driver's seat takes to power the engine up and down */
    f32 powerup{0.f};
    f32 powerdown{0.f};

    /*! Braking; tags without one slow down as fast as they speed up */
    f32 deceleration_or_accel() const
    {
        return deceleration > 0.f ? deceleration : acceleration;
    }
};

/*! A projectile in flight, moved by ProjectileSystem */
struct Projectile
{
    using value_type = Projectile;
    using type       = compo::alloc::VectorContainer<value_type>;

    blam::scn::projectile const* tag{nullptr};
    Vecf3                        velocity{}; /* wu/s */
    libc_types::f32              travelled{0.f};
    libc_types::f32              age{0.f};
    libc_types::f32              timer{-1.f}; /* seconds to detonation, <0 = not started */
    libc_types::u64              owner{0};    /* passes through its body */
    libc_types::u64              shooter{0};  /* credited with its hits */
    libc_types::u64              target{0};   /* the unit a guided one seeks */
    libc_types::u32              bounces{0};
    bool                         attached{false}; /* stuck where it hit */
    libc_types::u64              attached_to{0};  /* the unit it stuck to, if one */
    Vecf3                        attach_offset{}; /* in that unit's model space */
    bool                         resting{false};  /* settled where it fell */
    bool                         flew_by{false};  /* past a local ear, heard */
    bool                         combined{false}; /* gone into a super detonation */

    /*! Bullets fast enough to resolve as the trigger is pulled, along a
     *  ray of `reach` */
    static constexpr libc_types::f32 hitscan_speed = 8.f;   /* wu per tick */
    static constexpr libc_types::f32 hitscan_reach = 300.f; /* without a range */

    static bool hitscan(blam::scn::projectile const& tag)
    {
        return tag.initial_velocity >= hitscan_speed;
    }

    static libc_types::f32 reach(blam::scn::projectile const& tag)
    {
        return tag.maximum_range > 0.f ? tag.maximum_range : hitscan_reach;
    }
};

/*! Collision for a world object: static ones use the exact coll surfaces,
 * moving ones its hulls, or a box/sphere when there is no coll tag */
struct ObjectPhysics
{
    using value_type = ObjectPhysics;
    using type       = compo::alloc::VectorContainer<value_type>;

    std::shared_ptr<CollisionGeometry const> collision;
    /*! Vehicles: the body and what it touches the ground with */
    std::shared_ptr<MassPoints const> mass_points;
    std::optional<VehicleDrive>       drive;

    Vecf3 half_extents{}; /*!< Box, when there is no collision */
    Vecf3 center{};       /*!< Box/sphere centre in object space */
    Vecf3 center_of_mass{}; /*!< Hulls without a phys tag turn about this */
    f32   radius{0.f};    /*!< Sphere instead of a box */
    f32   mass{0.f};      /*!< 0 = static */
    bool  item{false};    /*!< Only collides with the world and other items */
    bool  upright{false}; /*!< Bipeds: never tip, hulls follow the pose */
    /*! The physics material; < 0 keeps the default */
    f32   friction{-1.f};
    f32   restitution{-1.f};
    bool  animated{false}; /*!< Devices: still, but hulls follow the pose */

    /*! Who moves a body with mass: this peer's simulation, or someone else
     * (the server) through Model, with the body following kinematically */
    enum authority_t : u8
    {
        Simulated,
        Follower,
    } authority{Simulated};

    /*! Simulated bodies, as of the last physics step */
    Vecf3 linear_velocity{};
    Vecf3 angular_velocity{};
    bool  sleeping{false};
    bool  grounded{false}; /*!< Upright bodies: world geometry just under it */
    Vecf3 walking{};       /*!< Upright bodies: its own stride, pushes left out */
    bool  teleported{false}; /*!< Moved by hand: the body starts over there */
    bool  scripted{false};   /*!< Moved by a script: carried, not simulated */
    bool  seated{false};     /*!< A script's rider: inside its vehicle, touching nothing */
    bool  recorded{false};   /*!< Driven by a script's recording: runs nobody down */

    /*! Vehicles, as of the last physics step */
    struct vehicle_t
    {
        std::vector<f32> ground_depth; /*!< Per mass point, 0 = off it */
        /*! Wheel centres above the ground along the suspension, negative
         *  when it is out of reach */
        std::vector<f32> ground_height;
        f32              steering{0.f}; /*!< Steered wheels, radians */
        f32              throttle{0.f}; /*!< The driver's, -1..1 */
        f32              slip{0.f};     /*!< Fastest sideways tire slide */
    } vehicle;
};

struct PhysicsData
{
    using value_type = PhysicsData;
    using type       = compo::alloc::VectorContainer<PhysicsData>;

    u64   physics_id{0};
    Vecf3 velocity;
    Vecf3 acceleration;
    Vecf3 position; /*!< Body origin */

    bool enabled{false};
    bool kinematic{false};
    bool grounded{false};
    Vecf3 walking{}; /*!< Its own stride, pushes left out */
    u64   deck{0};   /*!< The vehicle it stands on, 0 on the ground */

    u64 grabbed{0}; /*!< Entity this player carries (forge), 0 = none */

    /*! Initial velocity of the next body, e.g. exiting a vehicle; used once */
    std::optional<Vecf3> launch;
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
    bool sent_physics{false};

    /*! How much this object matters to each player who sees it, from
     *  NetworkRelevance on the server; Networking sends it to each by this */
    struct relevance_t
    {
        u32 viewer{0};      /*!< The viewing player's index */
        f32 relevance{0.f}; /*!< 0 out of sight and far, 1 close or in view */
        f32 priority{0.f};  /*!< Relevance weighed by what it is */
        std::chrono::milliseconds update_period{1000};
    };
    std::vector<relevance_t> relevance;

    relevance_t const* relevance_for(u32 viewer) const
    {
        for(auto const& r : relevance)
            if(r.viewer == viewer)
                return &r;
        return nullptr;
    }
};

struct PlayerInfo
{
    using value_type = PlayerInfo;
    using type       = compo::alloc::VectorContainer<value_type>;

    /*! For local seats the server does not know about */
    static constexpr u32 local_only_idx_base = 0x10000;

    /*! The biped's coll tag in biped space (feet at the origin, facing
     * +X) */
    using biped_collision_t = CollisionGeometry;

    /*! From the biped tag; the defaults stand in until one is loaded */
    struct biped_shape_t
    {
        f32 radius{0.1f};
        f32 height{0.7f};      /*!< Capsule, end to end */
        f32 eye_height{0.55f}; /*!< Camera above the feet */

        /*! Ground speeds, wu/s, from the graph's move clips */
        f32 run_speed{2.25f};
        f32 back_speed{2.25f};
        f32 strafe_speed{2.25f};
        f32 jump_velocity{2.3f}; /*!< wu/s, from the biped tag */
        /*! How far the camera looks down and up, radians; the cyborg's
         * tag leaves it empty */
        Vecf2 pitch_range{-1.5f, 1.5f};
        /*! The unit's camera marker in the bind pose, and its node */
        Matf4 camera_marker{1.f};
        i32   camera_node{-1};

        /*! Hit shape for bodies that follow a camera; the capsule above is
         * what a biped moves with */
        std::shared_ptr<biped_collision_t const> collision;

        /*! Its animations, and what its feet sound like on each material;
         * from the map loaded as `load_generation` */
        blam::antr::header const* anim_graph{nullptr};
        blam::tag_t const*        footsteps{nullptr};
        blam::tagref_t const*     respawn_effect{nullptr}; /*!< Played where it respawns */
        u32                       load_generation{0};

        f32 eye_offset() const
        {
            return eye_height - height / 2;
        }

        /*! Body origin relative to the camera (the eye) when spawning,
         * dropping it in from just above where it will stand */
        f32 spawn_lift() const
        {
            return 0.25f - eye_offset();
        }

        bool operator==(biped_shape_t const&) const = default;
    };

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

    biped_shape_t biped{};

    /*! False while held before spawning */
    bool spawned{true};

    /*! The vehicle seat this player sits in, while vehicle != 0 */
    struct
    {
        u64  vehicle{0};
        i16  seat{-1};
        bool driver{false};
        bool exiting{false};
        bool chase{false}; /*!< Seen from behind the vehicle, not the seat */
        bool armed{false}; /*!< The seat lets it fire what it holds */
        Vecf2 pitch_range{-1.5f, 1.5f}; /*!< How far the seat looks down and up */
    } riding;

    struct
    {
        bool physics{false};
        /*! Walked by the server from its TickInput, for clients without
         *  physics of their own */
        bool simulated{false};
    } mode;

    /*! Active camouflage, on the body and what it holds */
    bool camouflaged{false};

    /*! Seconds left of each timed powerup */
    struct
    {
        f32 camouflage{0.f};
        f32 speed{0.f};
        f32 vision{0.f};
    } powerups;

    /*! Double speed scales walking, firing and reloading */
    f32 speed_scale() const
    {
        return powerups.speed > 0.f ? 2.f : 1.f;
    }

    bool is_remote() const
    {
        return !remote.empty();
    }
};

struct debounced_button_t
{
    bool current{false};
    bool previous{false};

    debounced_button_t& operator|=(bool input)
    {
        current |= input;
        return *this;
    }
    debounced_button_t& operator=(bool input)
    {
        current = input;
        return *this;
    }

    operator bool() const
    {
        return current && !previous;
    }

    void frame_end()
    {
        previous = current;
        current  = false;
    }
};

struct PlayerInput
{
    using value_type = PlayerInput;
    using type       = compo::alloc::VectorContainer<value_type>;

    enum class input_mode_t
    {
        game,
        menu,
    } input_mode{input_mode_t::game};

    StandardCamera::Reg keys; /*!< held keys, from KeyboardInput */
    libc_types::u32 mouse_buttons{0};
    Vecf2 look_delta{};       /*!< accumulated look; zeroed once applied */
    Vecf3 movement{};
    f32   accel{1.f}; /*!< speed modifier chosen by the source */

    // Menu
    debounced_button_t start{};
    debounced_button_t accept{};
    debounced_button_t back{};
    debounced_button_t option{};
    debounced_button_t option_2{};
    debounced_button_t up{};
    debounced_button_t down{};
    debounced_button_t left{};
    debounced_button_t right{};
    debounced_button_t jump{}; /*!< Press edge; the controller keeps it */
    debounced_button_t swap{};   /*!< Switch weapons */
    debounced_button_t reload{};
    debounced_button_t grenade{};
    debounced_button_t grenade_switch{};
    debounced_button_t melee{};
    bool               scores{false}; /*!< Held: the scoreboard */

    /*! What the player wants, for whatever their input is routed to */
    struct intent_t
    {
        Vecf2 move{};       /*!< Forward and right, camera-relative, |move| <= 1 */
        f32   throttle{0.f}; /*!< Forward is positive */
        f32   steer{0.f};    /*!< Right is positive */
        bool  use{false};    /*!< Held: enter or leave a vehicle */
        bool  grab{false};   /*!< Held: forge grab */
        bool  fire{false};   /*!< Held: pull the trigger */
        bool  fire2{false};  /*!< Held: a mounted gun's second trigger */
        bool  jump{false};   /*!< Wanted; cleared once taken or stale */
        bool  swap{false};   /*!< Pressed: switch weapons; cleared once taken */
        bool  reload{false}; /*!< Pressed; cleared once taken */
        bool  grenade{false};        /*!< Pressed: throw; cleared once taken */
        bool  grenade_switch{false}; /*!< Pressed; cleared once taken */
        bool  melee{false};          /*!< Pressed; cleared once taken */
    } intent;

    std::optional<Vecf3> position; /*!< teleport target */
    std::optional<Quatf> rotation; /*!< absolute orientation */

    // TODO: Add more inputs later

    void frame_end()
    {
        start.frame_end();
        option.frame_end();
        option_2.frame_end();
        back.frame_end();
        accept.frame_end();
        left.frame_end();
        right.frame_end();
        up.frame_end();
        down.frame_end();
        jump.frame_end();
        swap.frame_end();
        reload.frame_end();
        grenade.frame_end();
        grenade_switch.frame_end();
        melee.frame_end();
    }
};

struct PlayerCamera
{
    using value_type = PlayerCamera;
    using type       = compo::alloc::VectorContainer<value_type>;

    using camera_t = typing::vectors::scene::camera<f32>;

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
        Vecf2 movement_plane; /*!< Physics impulse movement
                               * forward = normalized to ground level */
        Vecf3 movement;       /*!< Freecam instant movement
                               * forward = camera forward */
    } inputs;

    /*! Where the view is drawn from, off the eye: the dead look at their
     *  body from behind it */
    Vecf3 view_offset{};

    /*! A shot's kick and shake, and the first-person model's sway; off the
     *  look, so aim is untouched */
    struct
    {
        Vecf3 turn{};   /*!< Pitch, yaw, roll; radians */
        Vecf3 shift{};  /*!< Eye, in view space: right, up, back */
        Vecf3 weapon{}; /*!< First-person models, in their space */
    } kick;

    Matf4 matrix{};
    Matf4 rotation{};

    /*! Where the eye was a tick ago, for a body-driven camera */
    std::optional<Vecf3> tick_eye{};
    /*! Where the eye is drawn this frame, between ticks if it moves on them */
    Vecf3 drawn_eye{};

    /*! A cutscene's camera, drawn instead while set */
    struct cinematic_t
    {
        Vecf3 position{};
        Quatf rotation{1.f, 0.f, 0.f, 0.f};
        f32   fov{70.f}; /*!< degrees */
        u64   relative_to{0}; /*!< position and rotation in its space */
    };
    std::optional<cinematic_t> cinematic;
    /*! The whole window's aspect, which a cutscene is drawn at, and this
     *  seat's share of it in split screen */
    f32 screen_aspect{0.f};
    f32 split_aspect{0.f};

    bool is_active() const
    {
        return keyboard.enabled || controller.index.has_value();
    }
};

/*! The local seat a cutscene plays on, alone and full screen in place of
 *  the split: while one plays the first seat, else the first whose camera a
 *  cutscene holds */
template<typename Proxy>
std::optional<u32> cutscene_seat(Proxy& p, bool playing)
{
    std::optional<u32> seat;
    for(auto ent : p.template select<PlayerCamera, PlayerInfo>())
    {
        auto const& cam  = ent.template get<PlayerCamera>();
        auto const& info = ent.template get<PlayerInfo>();
        if(!info.is_remote() && cam.is_active() && (playing || cam.cinematic) &&
           (!seat || info.seat_idx < *seat))
            seat = info.seat_idx;
    }
    return seat;
}

/*! Whether a player's biped is drawn and animated, riders included */
inline bool biped_shown(
    PlayerInfo const& info, PlayerCamera const& cam, NetworkInfo const& net)
{
    if(!info.spawned)
        return false;
    if(info.is_remote())
        return net.connected && info.loading_progress >= 100;
    return cam.is_active();
}

/*! Whether a player's biped also has a collision body */
inline bool biped_in_play(
    PlayerInfo const& info, PlayerCamera const& cam, NetworkInfo const& net)
{
    return info.riding.vehicle == 0 && biped_shown(info, cam, net);
}

/*! A unit's body and shield, as fractions of its coll tag's maxima */
struct BipedStatus
{
    using value_type = BipedStatus;
    using type = compo::alloc::VectorContainer<BipedStatus>;

    f32  body{1.f};
    f32  shield{1.f};
    bool dead{false};
    /*! Seconds before the shield starts to come back */
    f32 stunned{0.f};
    /*! The "recent shield damage" export, 1 at a blow and fading */
    f32 recent_shield{0.f};
    /*! What it takes damage by; none and it can't be hurt */
    blam::coll::header const* coll{nullptr};
    /*! Scripts may change what it can have, in points (below 0, its coll
     *  tag's), or keep it from harm */
    f32  max_body{-1.f};
    f32  max_shield{-1.f};
    bool invulnerable{false};
    /*! An over shield: shield above 1 lost per second */
    f32 overcharge_decay{0.f};

    /*! What an over shield charges the shield to */
    static constexpr f32 over_shield = 3.f;

    f32 maximum_body() const
    {
        return max_body >= 0.f ? max_body : coll ? coll->maximum_body_vitality : 0.f;
    }

    f32 maximum_shield() const
    {
        return max_shield >= 0.f ? max_shield : coll ? coll->maximum_shield_vitality : 0.f;
    }
};

struct BipedEquipment
{
    using value_type = BipedEquipment;
    using type = compo::alloc::VectorContainer<BipedEquipment>;

    // We need the biped reference to check
    // certain params related to weapons and equipment
    blam::scn::biped const* biped{nullptr};
    struct weapon_t
    {
        blam::scn::weapon const* weapon{nullptr};
        blam::tag_t const*       tag{nullptr};
        /*! Its third-person model, in the biped's hand */
        u64 held{0};
        u16 rounds_loaded{0};
        u16 rounds_total{0};
        f32  heat{0.f};        /*!< 1 is the tag's overheat threshold's scale */
        f32  age{0.f};         /*!< Battery spent, 0 to 1 */
        bool overheated{false}; /*!< Venting until it cools to recovery */
        bool charged{false};    /*!< Its trigger held to a full charge */
    };
    struct equipment_t
    {
        blam::scn::equipment const* equip{nullptr};
        blam::tag_t const*          tag{nullptr};
        u16                         most{0}; /*!< What can be carried */
        u16                         left{0};
        std::chrono::seconds        time_left{};
    };
    std::array<weapon_t, 2>  weapons;
    std::vector<equipment_t> equipment; // grenades, powerups
    u16 grenade{0}; /*!< The equipment entry thrown, and counted on the HUD */
    /*! The map the pointers above point into */
    u32  load_generation{0};
    bool reloading{false}; /*!< The held weapon, as everyone sees it */
    bool throwing{false};  /*!< A grenade, from wind-up to recovery */

    weapon_t const* held(u32 generation) const
    {
        return generation == load_generation && weapons[0].weapon
                   ? &weapons[0]
                   : nullptr;
    }
};

/*! Rides a node of another entity's pose, e.g. a weapon in a hand */
struct HeldObject
{
    using value_type = HeldObject;
    using type       = compo::alloc::VectorContainer<value_type>;

    u64   holder{0};
    i32   node{-1};
    Matf4 offset{1}; /*!< Bind-pose marker on the holder */
    bool  shown{true};
};

/*! Follows another entity each frame (a player's eye) */
struct Attachment
{
    using value_type = Attachment;
    using type       = compo::alloc::VectorContainer<value_type>;

    u64   parent{0};
    Matf4 offset{1};
};

struct CameraLerp
{
    using value_type = CameraLerp;
    using type       = compo::alloc::VectorContainer<CameraLerp>;

    struct lerp_t
    {
        Vecf3 p1, p2; /*!< from, to */
        Quatf r1, r2; /*!< target rotation, blended with slerp */
        std::chrono::milliseconds remaining_time;
        std::chrono::milliseconds total_time;
        /*! Velocity it starts with, over the whole lerp: a lerp that cuts
         *  into another carries on its motion instead of turning sharply */
        Vecf3 v1{};
        f32   fov1{0.f}, fov2{0.f};
        bool  cinematic{false};
        u64   relative_to{0};
    };

    std::vector<lerp_t> lerps;
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
            compo::type_hash_v<CameraLerp>(),
            compo::type_hash_v<PlayerInput>(),
            compo::type_hash_v<SoundEffects>(),
            compo::type_hash_v<PhysicsData>(),
            compo::type_hash_v<DebugDraw>(),
            compo::type_hash_v<BipedStatus>(),
            compo::type_hash_v<BipedEquipment>(),

            compo::type_hash_v<Model>(),
            compo::type_hash_v<Visibility>(),
            compo::type_hash_v<AnimationPlayback>(),
        },
    .tags = PlayerBiped | PositioningDynamic,
};

/* Drawn in the first-person passes, for one seat only */
static const auto first_person_model = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<Attachment>(),
            compo::type_hash_v<Model>(),
            compo::type_hash_v<Visibility>(),
            compo::type_hash_v<AnimationPlayback>(),
        },
    .tags = ObjectGC,
};

static const auto skybox_model = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<WorldInfo>(),
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

/* An object held by a player biped */
static const auto held_model = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<DepthInfo>(),
            compo::type_hash_v<HeldObject>(),
            compo::type_hash_v<Model>(),
            compo::type_hash_v<Visibility>(),
        },
    .tags = PositioningDynamic | ObjectGC,
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

static const auto trigger_volume = compo::EntityRecipe{
    .components =
        {
            compo::type_hash_v<DebugDraw>(),
            compo::type_hash_v<TriggerVolume>(),
        },
    .tags = ObjectScriptObject | ObjectTriggerVolume | ObjectGC,
};

} // namespace shared_recipes
