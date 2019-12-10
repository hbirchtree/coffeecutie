#pragma once

#include <coffee/core/types/vector_types.h>

#include "cblam_mod2.h"
#include "cblam_structures.h"

namespace blam {

namespace scn {

/* Data which is not part of the structures proper */

union actor_type
{
    i16 data;

    inline operator i16() const
    {
        return data;
    }
    inline bool valid() const
    {
        return data != -1;
    }
};

enum class ai_state : i16
{
    unknown_1 = 0,
    unknown_2 = 2,
    unknown_3 = 6,
    unknown_4 = 7,
    unknown_5 = 9,
    unknown_6 = 10,
    unknown_7 = 11,
};

} // namespace scn

/*!
 * \brief The blam_bounding_box struct
 */
struct bounding_box
{
    bounding_box() : min(0), max(0)
    {
    }

    Vecf3 min;
    Vecf3 max;
};

template<typename T>
/*!
 * \brief Encapsulates a base and reference reflexive group
 */
struct reflex_group
{
    reflexive_t<T>                  base;
    reflexive_t<Array<tagref_t, 3>> ref;
};

namespace scn {

using scn_chunk = byte_t[100];

struct biped
{
    i16    biped_type;
    i16    unk2;
    u32    unk3;
    Vecf3  pos;
    scalar rot;
    u32    unk[24];
};

struct equip
{
    i16   equip_id;
    i16   unk2;
    u32   unk3;
    Vecf3 pos;
    u32   unk[5];
};

struct vehicle_spawn
{
    i16    vehicle_id;
    uint16 flag;
    u32    unknown1;
    Vecf3  pos;
    scalar rot;
    u32    unknown2[24];
};

struct palette
{
    tagref_t tag;
    byte_t   unk[32];
};

struct weapon_spawn
{
    i16    weapon_id;
    uint16 flag;
    u32    unknown1;
    Vecf3  pos;
    scalar rot;
    u32    unknown2[17];
};

struct weapon_instance
{
    u32      reserved1[8];
    u32      unknown;
    tagref_t something;
};

using weapon_tagref = reflexive_t<weapon_instance>;

struct weapon_ref
{
    i16    weapon_ref_id;
    uint16 flag;
    u32    unknown1;
    Vecf3  pos;
    scalar rot;
    u32    unknown2[17];
};

struct scenery_spawn
{
    i16    scenery_id;
    uint16 flag;
    u32    unknown1;
    Vecf3  pos;
    scalar rot;
    scalar unk1[2];
    u32    unknown2[10];
};

struct machine
{
    i16    MachineType;
    i16    unk2;
    u32    unk3;
    Vecf3  pos;
    scalar rot;
    u32    unk[10];
};

struct sound_scenery
{
    i16   SoundType;
    i16   unk2;
    u32   unk3;
    Vecf3 pos;
    u32   unk4[5];
};

struct player_spawn
{
    Vecf3  pos;
    scalar rot;
    scalar unknown2[9];
};

struct profile_placement
{
    bl_string name;
    scalar    health;
    scalar    shields;
    bl_tag    primarytag;
    i32       primaryrawfilename;
    byte_t    zero1[4];
    i32       primaryidentifier;
    i16       primaryclip;
    i16       primarytotal;
    bl_tag    secondarytag;
    i32       secondaryfilename;
    byte_t    zero2[4];
    i32       secondaryidentifier;
    i16       secondaryclip;
    i16       secondarytotal;
    i16       fraggrencount;
    i16       plasmagrencount;
    byte_t    zero3[20];
};

struct multiplayer_flag
{
    Vecf3  pos;
    scalar yaw;
    uint16 index1;
    uint16 index2;
    bl_tag tag;
    u32    unk2[31];
};

struct multiplayer_equipment
{
    u32      unk1[4];
    u32      unknown_tag;
    u32      unk2[11];
    Vecf3    pos;
    scalar   yaw;
    tagref_t item;
    u32      unk3[12];
};

struct player_starting_profile
{
    u32      unk1_offset;
    byte_t   name[28];
    uint16   padding1;
    u32      unk2_offset;
    tagref_t weapon1;
    u32      unk3;
    tagref_t weapon2;
    u32      unk4;
    u32      unk5;
    u32      padding2[5];
};

struct device_group
{
    bl_string name;
    u32       offset;
    u32       unk[4];
};

struct bsp_trigger
{
    u32 unk[2];
};

struct move_positions
{
    bl_string unk1[32];
    byte_t    unk2[4];
    u32       offset;
    u32       unk[5];
};

struct object_name
{
    bl_string name;
    u32       unknown;
};

struct trigger_volume
{
    u32          unk;
    bl_string    name;
    scalar       unk2[9];
    bounding_box box;
};

struct actor_variant_ref
{
    bl_tag     tag;
    string_ref name;
    u32        unk;
    scalar     unk2;
};

namespace ai {

struct animation_ref
{
    bl_string name;
    i16       unk1;
    i16       unk2;
    u32       unk[14];
};

struct script_ref
{
    bl_string name;
    u32       unk[15];
};

struct recording_ref
{
    u32        unk1[6];
    bl_tag     tag;
    string_ref name;
    u32        unk[2];
};

} // namespace ai

struct script_trigger
{
    u32       unk1;
    bl_string name;
    Vecf3     pos;
    u32       unk[11];
};

struct global
{
    u32       unk1;
    bl_string name;
    u32       unk2;
    Vecf3     pos;
    scalar    unk3[4];
    u32       unk[9];
};

struct references
{
    u32       unk1;
    bl_string name;
    u32       unk[15];
};

namespace encounter {

struct squad;
struct squad_spawn;

struct encounter
{
    bl_string_var<16>         text;
    u32                       unk[28];
    reflexive_t<squad>        squads;
    reflexive_t<byte_t>       platoons;
    reflexive_t<byte_t>       firingPositions;
    reflexive_t<player_spawn> playerStartLocations;
};

struct squad
{
    bl_string           name;
    actor_type          ActorType;
    i16                 Platoon;
    ai_state            InitialState;
    ai_state            ReturnState;
    u32                 unk1[11];
    u32                 Ai_Attacking;
    u32                 Ai_AttackingSearch;
    u32                 Ai_AttackingGuard;
    u32                 Ai_Defending;
    u32                 Ai_DefendingSearch;
    u32                 Ai_DefendingGuard;
    u32                 Ai_Pursuing;
    u32                 unk2[3];
    i16                 NormalDiffCount;
    i16                 InsaneDiffCount;
    u32                 unk3[20];
    reflexive_t<byte_t> StartLocations;
    u32                 unk4[3];
};

struct squad_spawn
{
    Vecf3  pos;
    scalar yaw;
    i16    unk2[5];
    i16    CommandList;
};

struct info
{
    bl_string_var<16>   text;
    squad*              pSquads;
    squad**             ppSquadSpawns;
    reflexive_t<byte_t> Platoons;
    reflexive_t<byte_t> FiringPositions;
    reflexive_t<byte_t> PlayerStartLocations;
};

} // namespace encounter

struct decal
{
    i16   unk1;
    i16   unk2;
    Vecf3 pos;
};

struct decal_ref
{
    bl_tag     tag;
    string_ref name;
    u32        reserved;
    u32        TagId;
};

struct shader_index
{
    u32 ShaderType;
    u32 ShaderIndex;
};

using skybox = tagref_t;

struct starting_equip
{
    u32                unknown1; /*!< Sometimes 1? */
    u32                index;
    byte_t             padding2[51];
    Array<tagref_t, 6> items;
    byte_t             padding3[45];
};

struct control
{
    i16    unk1;
    i16    unk2;
    byte_t unk3[4];
    Vecf3  pos;
    u32    tag_id;
    byte_t unk[40];
};

struct light_fixture
{
    bl_rgba_t ambient;
    byte_t    unk1[4];
    Vecf3     pos;
    byte_t    unk2[12];
    bl_rgba_t specular;
    i32       zero1;
    bl_rgba_t color;
    byte_t    unk3[40];
};

namespace bsp {

struct compressed_variant
{
};
struct uncompressed_variant
{
};

template<
    typename RType = uncompressed_variant,
    bool           = std::is_same<RType, uncompressed_variant>::value>
struct alignas(4) vertex
{
    Vecf3 position;
    Vecf3 normal;
    Vecf3 binorm;
    Vecf3 tangent;
    Vecf2 texcoord;

  private:
    constexpr void size_check()
    {
        static_assert(
            sizeof(vertex<uncompressed_variant>) == 56, "Invalid size");
    }
};

template<typename RType>
struct vertex<RType, false>
{
    // Compressed Xbox variant
    Vecf3 position;
    u32   normal;   /*!< PixFmt::R11G11B10F */
    u32   binormal; /*!< Same as binormal */
    u32   tangent;  /*!< Same as normal */
    Vecf2 texcoord;

  private:
    constexpr void size_check()
    {
        static_assert(sizeof(vertex<compressed_variant>) == 32, "Invalid size");
    }
};

using vertex_face = Array<u16, 3>;

template<
    typename RType = uncompressed_variant,
    bool           = std::is_same<RType, uncompressed_variant>::value>
struct light_vertex
{
    Vecf3 normal;
    Vecf2 texcoord;
};

template<typename RType>
struct light_vertex<RType, false>
{
    // Compressed Xbox variant
    u32 normal;   /*!< PixFmt::R11G11B10F */
    u16 texcoord; /*!< PixFmt::R16, Normalized */
};

struct header;
struct info;

struct section
{
    using comp_vertex   = vertex<compressed_variant>;
    using comp_lightmap = light_vertex<compressed_variant>;

    u32 header_offset;

    /* Below values are only valid on Xbox */
    reflexive_t<comp_vertex, xbox_variant>   xbox_vertices;
    reflexive_t<comp_lightmap, xbox_variant> xbox_lightmaps;
    bl_tag                                   tag;

    /*!
     * \brief to_header
     * Uses bsp_magic()
     * \return
     */
    inline reflexive_t<header> to_header() const
    {
        return {1, header_offset};
    }
};

struct info
{
    u32      offset;
    u32      size;
    u32      magic;
    u32      zero;
    tagref_t tag;

    inline magic_data_t bsp_magic(magic_data_t const& map_magic) const
    {
        return {map_magic.base_ptr, magic - offset, map_magic.max_size};
    }

    inline section const& to_bsp(magic_data_t const& magic) const
    {
        return *C_RCAST<section const*>(magic.base_ptr + offset);
    }
};

struct submesh_header
{
    using pc_vertex   = vertex<uncompressed_variant>;
    using xbox_vertex = vertex<compressed_variant>;

    struct dist_light
    {
        Vecf3 color;
        Vecf3 direction;
    };

    tagref_t                             shader;
    u32                                  unknown1;
    u32                                  mesh_index_offset; /*!< Mesh indices */
    u32                                  mesh_index_count;
    Vecf3                                centroid;
    Vecf3                                ambient_col;
    u32                                  dist_light_count;
    Array<dist_light, 2>                 dist_lights;
    Vecf3                                unknown2;
    Vecf4                                reflect_tint;
    Vecf3                                shadow_dir;
    Vecf3                                shadow_color;
    Vecf4                                plane;
    u32                                  breakable_surface;
    u32                                  unknown_count3;
    reflexive_t<pc_vertex, xbox_variant> pc_vertices_data;
    u32                                  memory_vertex_offset;
    u32                                  vert_reflexive;
    u32                                  unknown_always_3;
    reflexive_t<xbox_vertex, xbox_variant> xbox_vertices_data;
    u32                                    memory_lightmap_offset;
    u32                                    lightmap_vert_reflexive;
    u32                                    unknown_zero[2];
    u32                                    unknown_offset1;
    u32                                    pc_vertex_data_offset;
    u32                                    unkown_zero1;
    u32                                    comp_vertex_buffer_size;
    u32                                    unkown_zero2;
    u32                                    unknown_offset2;
    u32                                    vertex_data_offset;
    u32                                    unkown_zero3;

    inline reflexive_t<pc_vertex, xbox_variant> pc_vertices() const
    {
        auto base = pc_vertices_data;
        base.offset += pc_vertex_data_offset;
        return base;
    }

    inline reflexive_t<xbox_vertex, xbox_variant> xbox_vertices() const
    {
        auto base = xbox_vertices_data;
        base.offset += vertex_data_offset;
        return base;
    }

    inline reflexive_t<vertex_face> pc_indices(header const& head) const;

    inline u32 index_offset() const
    {
        return mesh_index_offset * sizeof(vertex_face);
    }
    inline u32 base_vertex() const
    {
        return mesh_index_offset;
    }
    inline u32 index_count() const
    {
        return mesh_index_count;
    }
};

struct alignas(4) submesh_group
{
    u16 lightmap_idx;
    /* Intentionally leave 2 bytes here for padding */
    u32                         unknown[4];
    reflexive_t<submesh_header> material;
};

struct subcluster
{
    bounding_box             bounds;
    reflexive_t<vertex_face> indices;
};

struct predicted_resource
{
    u16 unknown[2];
    u32 tag_id;
};

struct cluster_portal
{
    i16                front_cluster;
    i16                back_cluster;
    i32                plane_index;
    Vecf3              centroid;
    scalar             bound_radius;
    u32                unknown[7];
    reflexive_t<Vecf3> vertices;
};

struct cluster
{
    i16                             sky;
    i16                             fog;
    i16                             background_sound;
    i16                             sound_env;
    i16                             weather;
    i16                             transition_bsp;
    u32                             unknown1[7];
    reflexive_t<predicted_resource> predicted_resources;
    reflexive_t<subcluster>         sub_clusters;
    u32                             unknown2[7];
    reflexive_t<i16>                portals;
};

struct breakable_surface
{
    u32 _1;
    u32 _2;
    u32 offset;
    u32 offset2;
    u32 count;
    u32 padding[7];
};

struct header
{
    tagref_t                       lightmaps;
    u32                            unknown1[37];
    reflexive_t<byte_t>            shaders;
    reflexive_t<byte_t>            collision_header;
    reflexive_t<byte_t>            nodes;
    bounding_box                   world_bounds;
    reflexive_t<byte_t>            leaves;
    reflexive_t<byte_t>            leaf_surfaces;
    reflexive_t<vertex_face>       submesh_tri_indices;
    reflexive_t<submesh_group>     submesh_groups;
    reflexive_t<byte_t>            chunk_10;
    reflexive_t<byte_t>            chunk_11;
    reflexive_t<byte_t>            chunk_12;
    reflexive_t<cluster>           clusters;
    i32                            cluster_data_size;
    u32                            unknown2;
    reflexive_t<byte_t>            cluster_data;
    reflexive_t<cluster_portal>    cluster_portals;
    reflexive_t<byte_t>            chunk_16a;
    reflexive_t<breakable_surface> breakables_surfaces;
    reflexive_t<byte_t>            fog_planes;
    reflexive_t<byte_t>            fog_regions;
    reflexive_t<byte_t>            fog_weather_pallette;
    reflexive_t<byte_t>            chunk_16f;
    reflexive_t<byte_t>            chunk_16g;
    reflexive_t<byte_t>            weather;
    reflexive_t<byte_t>            weather_polyhedra;
    reflexive_t<byte_t>            chunk_19;
    reflexive_t<byte_t>            chunk_20;
    reflexive_t<byte_t>            pathfinding_surface;
    reflexive_t<byte_t>            chunk_24;
    reflexive_t<byte_t>            background_sound;
    reflexive_t<byte_t>            sound_env;
    i32                            sound_pas_data_size;
    u32                            unknown3;
    reflexive_t<byte_t>            sound_pas_data;
    reflexive_t<byte_t>            chunk_26;
    reflexive_t<byte_t>            chunk_27;
    reflexive_t<byte_t>            markers;
    reflexive_t<byte_t>            clutter_objects; /*!< aka detail_objects */
    reflexive_t<byte_t>            runtime_decals;
    u32                            unkown4[9];

    inline reflexive_t<vertex_face> all_indices() const
    {
        return submesh_tri_indices;
    }
};

inline reflexive_t<vertex_face> submesh_header::pc_indices(
    header const& head) const
{
    return {index_count(), index_offset() + head.submesh_tri_indices.offset, 0};
}

} // namespace bsp

/*!
 * \brief A Blam! scenario descriptor
 */
struct scenario
{
    struct /* 260-byte block? */
    {
        tagref_t              unk_bsp1; // 16
        tagref_t              unk_bsp2; // 16
        tagref_t              unk_sky;  // 16
        reflexive_t<skybox>   skyboxes; // 12
        u32                   padding;
        reflexive_t<tagref_t> child_scenarios; // 12

        u32 unknown_offset;

        u32 padding1[45];
    };

    struct /* 256-byte block */
    {
        i32                    editor_scenario_size;
        u32                    unknown_2;
        reflexive_t<scn_chunk> unknown_reflexive;
        u32                    padding2[59];
    };

    struct /* 324-byte block */
    {
        reflexive_t<object_name>    object_names;
        reflex_group<scenery_spawn> scenery;
        reflex_group<biped>         bipeds;
        reflex_group<vehicle_spawn> vehicles;
        reflex_group<equip>         equips;
        reflex_group<weapon_spawn>  weapon_spawns;
        reflexive_t<device_group>   device_groups;
        reflex_group<machine>       machines;
        reflex_group<control>       controls;
        reflex_group<light_fixture> light_fixtures;
        reflex_group<sound_scenery> snd_scenery;

        u32 padding3[21];
    };

    struct /* 216-byte block */
    {
        reflexive_t<player_starting_profile> player_start_profiles;
        reflexive_t<player_spawn>            player_spawns;
        reflexive_t<trigger_volume>          trigger_volumes;
        reflexive_t<scn_chunk>               animation;
        reflexive_t<multiplayer_flag>        mp_flags;
        reflexive_t<multiplayer_equipment>   mp_equipment;
        reflexive_t<starting_equip>          starting_equipment;
        reflexive_t<bsp_trigger>             bsp_switch_triggers;
        reflex_group<decal>                  decals;
        reflexive_t<scn_chunk>               detail_obj_collision_ref;
        u32                                  padding4[21];
    };

    struct /* 340-byte block */
    {
        reflexive_t<actor_variant_ref>    actor_variant_refs;
        reflexive_t<encounter::encounter> encounters;
        reflexive_t<scn_chunk>            command_lists;
        reflexive_t<scn_chunk>            unknown_6;
        reflexive_t<scn_chunk>            starting_locations;
        reflexive_t<scn_chunk>            platoons;
        reflexive_t<scn_chunk>            ai_conversations;
        u32                               script_syntax_data_size;
        u32                               unknown_7;
        reflexive_t<scn_chunk>            scripts;
        reflexive_t<scn_chunk>            commands;
        reflexive_t<scn_chunk>            points;
        reflexive_t<ai::animation_ref>    ai_animation_refs;
        reflexive_t<global>               globals;
        reflexive_t<ai::recording_ref>    ai_recording_refs;
        reflexive_t<scn_chunk>            unknown_8;
        reflexive_t<scn_chunk>            participants;
        reflexive_t<scn_chunk>            lines;
        reflexive_t<script_trigger>       script_triggers;
        reflexive_t<scn_chunk>            cutscenes_verify;
        reflexive_t<scn_chunk>            cutscene_titles_verify;
        reflexive_t<scn_chunk>            source_files;
        reflexive_t<scn_chunk>            cutscene_flags;
        reflexive_t<scn_chunk>            cutscene_camera_poi;
        reflexive_t<scn_chunk>            cutscene_titles_;
        u32                               padding5[14];
    };

    struct
    {
        tagref_t custom_object_names;
        tagref_t cutscene_titles;
        tagref_t hud_text;
    } ui_text;

    reflexive_t<bsp::info> bsp_info;
};

inline scenario const& get_scenario(
    file_header_t const* header, magic_data_t const& magic)
{
    return tag_index_t::from_header(header)
        .scenario(header)
        .to_reflexive<scenario>()
        .data(magic)[0];
}

} // namespace scn

namespace ui {

struct unicode_ref
{
    using wide_string = std::basic_string<u16>;

    u32              length;
    u32              padding;
    reflexive_t<u16> data;

    inline wide_string str(magic_data_t const& magic, u16 off = 0) const
    {
        using wide_char = wide_string::value_type;

        auto str_data = data.data(magic);
        return wide_string(
            C_RCAST<wide_char const*>(str_data.data) + off,
            length / sizeof(wide_char));
    }
};

struct unicode_string
{
    reflexive_t<unicode_ref> sub_strings;
};

struct hud_symbol
{
    bl_string symbol;
    u32       padding[8];
};

struct hud_message
{
    struct offset_pair
    {
        /* Could be a big-endian u16? */
        u8 pad;
        u8 size;
    };

    unicode_ref              text;
    reflexive_t<offset_pair> offsets;
    reflexive_t<hud_symbol>  symbols;

    /* Get offset into unicode_ref for a string */
    inline u16 str_offset(magic_data_t const& magic, u32 idx) const
    {
        u16  out         = 0;
        auto offset_data = offsets.data(magic);
        for(auto i : stl_types::Range<>(idx))
        {
            out += offset_data[i].size;
        }
        return out;
    }
};

} // namespace ui
} // namespace blam
