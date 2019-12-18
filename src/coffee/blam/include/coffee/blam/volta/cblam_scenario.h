#pragma once

#include "cblam_bytecode.h"
#include "cblam_mod2.h"
#include "cblam_structures.h"

namespace blam {
namespace scn {

using angle_t = scalar;

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

enum class actor_flags_t : u32
{
    none                     = 0x0,
    see_in_darkness          = 0x1,
    sneak_uncovering_target  = 0x2,
    sneak_uncovering_pursuit = 0x4,
    /* unused */
    shoot_at_target_last_location = 0x10,
    stay_still_crouched           = 0x20,
    crouch_on_not_combat          = 0x40,
    crouch_on_guard               = 0x80,
    /* unused */
    crouch_shooter          = 0x100,
    panic_on_surprise       = 0x200,
    vehicle_with_player     = 0x400,
    fire_before_aim         = 0x800,
    no_strafe               = 0x1000,
    crouch_no_strafe        = 0x2000,
    defensive_crouch_charge = 0x4000,
    stalk                   = 0x8000,
    stalk_freeze_on_exposed = 0x10000,
    berserk_always          = 0x20000,
    berserk_panic_moving    = 0x40000,
    flying                  = 0x80000,
    panic_for_unstoppable   = 0x100000,
    crouch_on_hiding        = 0x200000,
    always_charge_attack    = 0x400000,
    dive_ledges             = 0x800000,
    swarm                   = 0x1000000,
    suicidal_melee          = 0x2000000,
    crouch_no_move          = 0x4000000,
    crouch_no_look          = 0x8000000,
    crouch_on_fire          = 0x10000000,
    avoid_friendly_fire     = 0x20000000,
};

enum class actor_ex_flags_t : u32
{
    none                       = 0x0,
    avoid_enemy_attack_vectors = 0x1,
    stand_to_fire              = 0x2,
    stop_to_fire               = 0x4,
    disallow_vehicle_combat    = 0x8,
    pathfind_ignore_danger     = 0x10,
    panic_in_group             = 0x20,
    no_corpse_shoot            = 0x40,
};

enum class actor_type_t : u32
{
    elite,
    jackal,
    grunt,
    hunter,
    engineer,
    assassin,
    player,
    marine,
    crew,
    combat_form,
    infection_form,
    carrier_form,
    monitor,
    sentinel,
    none,
    mounted_weapon,
};

C_FLAGS(actor_flags_t, u32)

struct actor
{
    actor_flags_t    flags;
    actor_ex_flags_t extra_flags;
    u32              padding[3];
    actor_type_t     type;
    struct /* perception */
    {
        scalar  max_vision_dist;
        angle_t central_vision;
        angle_t max_vision;
        angle_t peripheral_vision;
        scalar  peripheral_dist;
        Vecf3   stand_gun_offset;
        Vecf3   crouch_gun_offset;
        scalar  hearing_dist;
        scalar  projectile_awareness; /* [0,1] */
        scalar  vehicle_awareness;    /* [0,1] */
        scalar  combat_perception_time;
        scalar  guard_perception_time;
    };
    struct /* movement */
    {
    };
    struct /* looking */
    {
    };
    struct /* unopposable */
    {
    };
    struct /* panic */
    {
    };
    struct /* defensive */
    {
    };
};

enum class actor_variant_flags_t : u32
{
    none                      = 0x0,
    shoot_while_flying        = 0x1,
    interpolate_color_hsv     = 0x2,
    unlimited_grenades        = 0x4,
    moveswitch_stay_w_friends = 0x8,
    active_camo               = 0x10,
    super_active_camo         = 0x20,
    no_ranged_weapons         = 0x40,
    prefer_passenger_seat     = 0x80,
};

enum class actor_movement_t : u32
{
    always_run,
    always_crouch,
    switchable,
};

struct actor_variant
{
    using cls = tag_class_t;

    actor_variant_flags_t                           flags;
    tagref_typed_t<cls::actr>                       actor;
    tagref_typed_t<cls::bipd, cls::unit, cls::vehi> unit;
    tagref_typed_t<cls::actv>                       major;

    u32 padding[6];

    struct
    {
        actor_movement_t movement;
        scalar           crouch_chance;
        Vecf2            crouch_time;
        Vecf2            run_time;
    };

    struct
    {
        tagref_typed_t<cls::weap> weapon;

        scalar  max_fire_dist;
        scalar  rate_of_fire;
        angle_t projectile_error;
        Vecf2   burst_delay;
        scalar  retarget_fire_time;

        scalar surprise_delay;
        scalar surprise_wild_fire_time;

        scalar death_wild_fire_chance;
        scalar death_wild_fire_time;

        Vecf2 desired_combat_range;
        Vecf3 custom_stand_gun_offset;
        Vecf3 custom_crouch_gun_offset;

        Vecf2  target_track;
        Vecf2  target_lead;
        scalar dmg_modifier;
        scalar dmg_per_second;
    };
};

using actor_variant_ref = tagref_t;

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
    u32       padding_1;
    bl_string name;
    Vecf3     pos;
    u32       some_value;
    u32       padding[10];
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
    actor_type          actor_type;
    u16                 platoon;
    ai_state            initial_state;
    ai_state            return_state;
    u32                 unk1[11];
    u32                 attacking;
    u32                 attacking_search;
    u32                 attacking_guard;
    u32                 defending;
    u32                 defending_search;
    u32                 defending_guard;
    u32                 pursuing;
    u32                 unk2[3];
    u16                 normal_diff_count;
    u16                 insane_diff_count;
    u32                 unk3[20];
    reflexive_t<byte_t> start_locations;
    u32                 unk4[3];
};

struct squad_spawn
{
    Vecf3  pos;
    scalar yaw;
    i16    unk2[5];
    i16    CommandList;
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

struct header;
struct info;

struct section
{
    using comp_vertex   = vert::vertex<vert::compressed>;
    using comp_lightmap = vert::light_vertex<vert::compressed>;

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
    using pc_vertex   = vert::vertex<vert::uncompressed>;
    using xbox_vertex = vert::vertex<vert::compressed>;

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

    inline reflexive_t<vert::face> pc_indices(header const& head) const;

    inline u32 index_offset() const
    {
        return mesh_index_offset * sizeof(vert::face);
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
    bounding_box            bounds;
    reflexive_t<vert::face> indices;
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
    reflexive_t<shader_desc>       shaders;
    reflexive_t<byte_t>            collision_header;
    reflexive_t<byte_t>            nodes;
    bounding_box                   world_bounds;
    reflexive_t<byte_t>            leaves;
    reflexive_t<byte_t>            leaf_surfaces;
    reflexive_t<vert::face>        submesh_tri_indices;
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

    inline reflexive_t<vert::face> all_indices() const
    {
        return submesh_tri_indices;
    }
};

inline reflexive_t<vert::face> submesh_header::pc_indices(
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
        u32                               script_bytecode_size;
        u32                               unknown_7;
        reflexive_t<hsc::script_ref>      scripts;
        u32                               unknown_9[2];
        reflexive_t<string_segment>       script_string_segment;
        reflexive_t<ai::animation_ref>    ai_animation_refs;
        reflexive_t<hsc::global>          globals;
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
        u32                               padding5[15];
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

    u32                         length;
    u32                         padding;
    reflexive_t<unicode_var<1>> data;

    inline wide_string str(magic_data_t const& magic, u16 off = 0) const
    {
        auto str_data = data.data(magic);
        return str_data[0].str();
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
