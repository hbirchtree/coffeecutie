#pragma once

#include "caching.h"

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
    Pass_Opaque,
    Pass_Alphatest,
    Pass_LastOpaque = Pass_Alphatest,
    Pass_Sky,
    Pass_Additive,
    Pass_Multiply,
    Pass_Glass,

    //    Pass_Wireframe,

    Pass_Count,

};

struct BspReference
{
    using value_type = BspReference;
    using type       = compo::alloc::VectorContainer<value_type>;

    generation_idx_t bsp;
    generation_idx_t shader;
    generation_idx_t lightmap;

    u32    draw_idx;
    Passes current_pass;

    gfx::draw_command draw;
    bool              visible{true};
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

    /* Data used at runtime */
    u32    draw_idx;
    Passes current_pass;

    gfx::draw_command draw;

    template<typename V>
    void initialize(
        generation_idx_t                       model_idx,
        typename ModelItem<V>::SubModel const& model_)
    {
        draw.call = {
            .indexed   = true,
            .instanced = true,
            .mode      = gfx::drawing::primitive::triangle_strip,
        };
        draw.data = {
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
    return Quatf(Vecf3(0, 0, 1) * spawn->rot.x)
           * Quatf(Vecf3(0, 1, 0) * spawn->rot.y)
           * Quatf(Vecf3(1, 0, 0) * -spawn->rot.z);
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
    blam::tag_t const*                            tag = nullptr;

    bool visible{true};

    template<typename T>
    void initialize(T const* spawn)
    {
        position  = spawn->pos;
        rotation = spawn_rotation_to_quat(spawn);
        update_matrix();
    }
    void update_matrix()
    {
        transform = glm::translate(Matf4(1), position)
                    * glm::mat4_cast(rotation);
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
    inline Passes get_render_pass(ShaderCache<V>& cache) const
    {
        using tc = blam::tag_class_t;
        using namespace enum_helpers;
        using namespace blam::shader;

        [[maybe_unused]] auto name
            = shader_tag->to_name().to_string(cache.magic);

        switch(shader_tag->tagclass_e[0])
        {
        case tc::soso: {
            [[maybe_unused]] auto info = shader_data<shader_model>();
            //            auto flags = info->flags;
            //            bool transparent
            //                = !feval(flags,
            //                shader_model::model_flags::no_alpha_test);
            //            return transparent ? Pass_Alphatest : Pass_Opaque;
            return Pass_Glass;
        }
        case tc::schi: {
            using blam::shader::chicago::flags_t;
            shader_chicago<V> const* info   = shader_data<shader_chicago<V>>();
            auto                     maps   = info->maps.data(cache.magic);
            auto                     layers = info->layers.data(cache.magic);

            auto is_multiplied = info->transparent.blend_function
                                 == chicago::framebuffer_blending::multiply;
            auto is_add = info->transparent.blend_function
                          == chicago::framebuffer_blending::add;
            //            auto is_alpha = info->transparent.blend_function
            //                            ==
            //                            chicago::framebuffer_blending::alpha_blend;

            if(is_multiplied)
                return Pass_Multiply;
            if(is_add)
                return Pass_Additive;
            return Pass_Glass;
        }
        case tc::scex: {
            shader_chicago_extended<V> const* info
                = shader_data<shader_chicago_extended<V>>();
            auto maps_2 = info->maps_2stage.data(cache.magic);
            auto maps_4 = info->maps_4stage.data(cache.magic);
            auto layers = info->layers.data(cache.magic);

            auto is_multiplied = info->transparent.blend_function
                                 == chicago::framebuffer_blending::multiply;
            auto is_add = info->transparent.blend_function
                          == chicago::framebuffer_blending::add;
            //            auto is_alpha = info->transparent.blend_function
            //                            ==
            //                            chicago::framebuffer_blending::alpha_blend;
            if(is_multiplied)
                return Pass_Multiply;
            if(is_add)
                return Pass_Additive;
            return Pass_Glass;
        }
        case tc::swat:
        case tc::sotr:
        case tc::sgla:
            return Pass_Glass;
        case tc::senv: {
            shader_env const* info = shader_data<shader_env>();
            return feval(info->flags & shader_env::flags_t::alpha_tested)
                       ? Pass_Glass
                       : Pass_Opaque;
        }
        case tc::smet:
            return Pass_Opaque;
        default:
            return Pass_Opaque;
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

    bool contains(Matf4 const& transform, blam::magic_data_t const& magic) const
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
    using type = compo::alloc::VectorContainer<PhysicsData>;

    Vecf3 velocity;
    Vecf3 acceleration;

    bool enabled{false};
};

struct NetworkInfo
{
    using value_type = NetworkInfo;
    using type = compo::alloc::VectorContainer<value_type>;

    std::string object_name;

    struct
    {
        bool transform:1;
    } changes;
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

    SubObjectMask      = 0xFFFFF,
};
