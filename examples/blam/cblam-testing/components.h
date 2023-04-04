#pragma once

#include "caching.h"

using ERef = Components::EntityRef<Components::EntityContainer>;

using comp_app::detail::duration;
using comp_app::detail::time_point;

using vertex_type      = blam::vert::vertex<blam::vert::uncompressed>;
using xbox_vertex_type = blam::vert::vertex<blam::vert::compressed>;

using light_vertex_type = blam::vert::light_vertex<blam::vert::uncompressed>;
using light_xbox_vertex_type = blam::vert::light_vertex<blam::vert::compressed>;

using Components::ComponentRef;
using Components::EntityContainer;

enum Passes
{
    Pass_Opaque,
    Pass_Alphatest,
    Pass_LastOpaque = Pass_Alphatest,
    Pass_Lights,
    Pass_Glass,

    Pass_Wireframe,

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
    u16 draw;
    u16 instance;
};
static_assert(sizeof(model_tracker_t) == 4);

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

template<
    typename SpawnType,
    typename std::enable_if<
        !std::is_same<SpawnType, blam::scn::multiplayer_equipment>::value>::
        type* = nullptr>
static Vecf3 spawn_rotation_to_euler(SpawnType const* spawn)
{
    return spawn->rot;
}
template<
    typename SpawnType,
    typename std::enable_if<
        std::is_same<SpawnType, blam::scn::multiplayer_equipment>::value>::
        type* = nullptr>
static Vecf3 spawn_rotation_to_euler(SpawnType const* spawn)
{
    return Vecf3(spawn->facing, 0, 0);
}

struct Model
{
    using value_type = Model;
    using type       = compo::alloc::VectorContainer<value_type>;
    using tag_type   = value_type;

    Matf4 transform;

    Vector<ERef>                        models;
    mem_chunk<blam::mod2::region const> regions;
    blam::tag_t const*                  tag = nullptr;

    template<typename T>
    void initialize(T const* spawn)
    {
        transform = typing::vectors::translation(Matf4(), spawn->pos)
                    * typing::vectors::matrixify(
                        typing::vectors::euler(spawn_rotation_to_euler(spawn)));
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

        auto const& shader_ = get_shader(cache);
        auto        name    = shader_tag->to_name().to_string(cache.magic);

        switch(shader_tag->tagclass_e[0])
        {
        case tc::soso: {
//            auto info  = shader_data<shader_model>();
//            auto flags = info->flags;
//            bool transparent
//                = !feval(flags, shader_model::model_flags::no_alpha_test);
//            return transparent ? Pass_Alphatest : Pass_Opaque;
            return Pass_Glass;
        }
        case tc::schi: {
            using blam::shader::chicago::flags_t;
            shader_chicago<V> const* info   = shader_data<shader_chicago<V>>();
            auto                     maps   = info->maps.data(cache.magic);
            auto                     layers = info->layers.data(cache.magic);
            auto is_decal = feval(info->transparent.flags & flags_t::decal);
            auto is_alpha
                = feval(info->transparent.flags & flags_t::alpha_testing);

//            return is_decal ? Pass_Lights : is_alpha ? Pass_Glass : Pass_Opaque;
            return Pass_Lights;
        }
        case tc::scex: {
            shader_chicago_extended<V> const* info
                = shader_data<shader_chicago_extended<V>>();
            auto maps_2 = info->maps_2stage.data(cache.magic);
            auto maps_4 = info->maps_4stage.data(cache.magic);
            auto layers = info->layers.data(cache.magic);
            return Pass_Glass;
        }
        case tc::swat:
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

enum ObjectTags : u32
{
    ObjectScenery      = 0x1,
    ObjectEquipment    = 0x2,
    ObjectVehicle      = 0x4,
    ObjectBiped        = 0x8,
    ObjectDevice       = 0x10,
    ObjectLightFixture = 0x20,
    ObjectControl      = 0x40,
    ObjectSkybox       = 0x80,

    ObjectObject = 0x1000,
    ObjectUnit   = ObjectObject << 1,

    ObjectMod2 = 0x100000,
    ObjectBsp  = ObjectMod2 << 1,

    /* For objects that should be removed after loading a new map */
    ObjectGC = 0x800000,
};
