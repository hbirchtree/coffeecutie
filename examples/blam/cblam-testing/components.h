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
    Pass_Env = Pass_Opaque,
    Pass_EnvMicro,
    Pass_Metal,
    Pass_Alphatest,
    Pass_LastOpaque,
    Pass_Lights = Pass_LastOpaque,
    Pass_Glass,

    Pass_Wireframe,

    Pass_Count,

};

struct BspReference
{
    generation_idx_t bsp;
    generation_idx_t shader;
    generation_idx_t lightmap;

    u32    draw_idx;
    Passes current_pass;

    struct
    {
        GFX::D_DATA draw;
        GFX::D_CALL call;
    } draw;
    bool visible = false;
};

struct SubModel
{
    ERef parent;

    generation_idx_t shader;
    generation_idx_t model;

    /* Data used at runtime */
    u32    draw_idx;
    Passes current_pass;

    struct
    {
        GFX::D_DATA draw;
        GFX::D_CALL call;
    } draw;

    void initialize(
        generation_idx_t model_idx, ModelItem::SubModel const& model_)
    {
        draw.call =
            GFX::D_CALL().withIndexing().withTriStrip().withInstancing();
        draw.draw = model_.draw;
        model     = model_idx;
        shader    = model_.shader;
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
    Matf4 transform;

    Vector<ERef>                        models;
    mem_chunk<blam::mod2::region const> regions;
    blam::tag_t const*                  tag = nullptr;

    template<typename T>
    void initialize(T const* spawn)
    {
        transform = typing::vectors::translation(Matf4(), spawn->pos) *
                    typing::vectors::matrixify(
                        typing::vectors::euler(spawn_rotation_to_euler(spawn)));
    }
};

struct ObjectSpawn
{
    blam::scn::object_spawn const* header = nullptr;
    blam::tag_t const*             tag    = nullptr;
};

struct MultiplayerSpawn
{
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

namespace materials {

struct alignas(32) lightmap_data
{
    Vecf2 atlas_scale;
    Vecf2 atlas_offset;
    u32   layer;
};

struct alignas(16) basic
{
    Vecf2 atlas_scale;
    Vecf2 uv_scale;
    Vecf2 atlas_offset;
    i32   source;
    u32   layer;
    f32   bias;
};

struct alignas(32) map_data
{
    Vecf2 atlas_scale;
    Vecf2 atlas_offset;
    Vecf2 uv_scale;
    u32   layer;
    f32   bias;
};

struct alignas(32) senv
{
    map_data      base;
    lightmap_data lightmap;
};

struct alignas(32) senv_micro
{
    map_data      base;
    map_data      micro;
    map_data      primary;
    map_data      secondary;
    lightmap_data lightmap;
};

} // namespace materials

struct ShaderData
{
    blam::tag_t const*       shader_tag;
    blam::shader_base const* shader;

    generation_idx_t shader_id;

    template<typename T>
    inline T const* shader_data() const
    {
        return C_RCAST<T const*>(shader);
    }

    inline Passes get_render_pass(ShaderCache& cache) const
    {
        using tc = blam::tag_class_t;
        using namespace enum_helpers;

        auto const& shader_ = get_shader(cache);
        auto        name    = shader_tag->to_name().to_string(cache.magic);

        switch(shader_tag->tagclass_e[0])
        {
        case tc::senv:
        {
            blam::shader_env const* shader = shader_data<blam::shader_env>();

            return Pass_EnvMicro;
            //            return shader->diffuse.micro.map.valid() ?
            //            Pass_EnvMicro : Pass_Env;
        }
        case tc::soso:
        {
            auto flags = shader_data<blam::shader_model>()->flags;
            bool transparent =
                feval(flags, blam::shader_model::model_flags::no_alpha_test);
            return transparent ? Pass_Glass : Pass_Env;
        }
        case tc::swat:
            return Pass_Glass;
        case tc::sgla:
            return Pass_Glass;
        case tc::smet:
            return Pass_Opaque;
        default:
            return Pass_Opaque;
        }
    }

    inline ShaderItem const& get_shader(ShaderCache& cache) const
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
};

using BspTag              = Components::TagType<BspReference>;
using SubModelTag         = Components::TagType<SubModel>;
using ModelTag            = Components::TagType<Model>;
using ObjectSpawnTag      = Components::TagType<ObjectSpawn>;
using MultiplayerSpawnTag = Components::TagType<MultiplayerSpawn>;
using ShaderTag           = Components::TagType<ShaderData>;

using BspStore         = Components::Allocators::VectorContainer<BspTag>;
using ModelStore       = Components::Allocators::VectorContainer<SubModelTag>;
using ModelParentStore = Components::Allocators::VectorContainer<ModelTag>;
using ObjectSpawnStore =
    Components::Allocators::VectorContainer<ObjectSpawnTag>;
using MultiplayerSpawnStore =
    Components::Allocators::VectorContainer<MultiplayerSpawnTag>;
using ShaderDataStore = Components::Allocators::VectorContainer<ShaderTag>;
