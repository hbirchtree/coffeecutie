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
    reflexive_t<T> base;
    reflexive_t<T> ref;
};

namespace scn {

using scn_chunk = byte_t[100];

struct biped
{
    i16    BipedType;
    i16    unk2;
    u32    unk3;
    Vecf3  pos;
    scalar rot;
    u32    unk[24];
};

struct equip
{
    i16   numid;
    i16   unk2;
    u32   unk3;
    Vecf3 pos;
    u32   unk[5];
};

struct vehicle_spawn
{
    i16    numid;
    uint16 flag;
    u32    unknown1;
    Vecf3  pos;
    scalar rot;
    u32    unknown2[24];
};

struct palette
{
    bl_tag tag;
    u32    NamePtr;
    u32    unk1;
    u32    ObjectTagId;
    u32    unk[8];
};

struct weapon_spawn
{
    i16    numid;
    uint16 flag;
    u32    unknown1;
    Vecf3  pos;
    scalar rot;
    u32    unknown2[17];
};

struct weapon_ref
{
    i16    numid;
    uint16 flag;
    u32    unknown1;
    Vecf3  pos;
    scalar rot;
    u32    unknown2[17];
};

struct scenery_spawn
{
    i16    numid;
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
    u32    unk[16];
    Vecf3  pos;
    scalar yaw;
    scalar unk1;
    u32    NamePtr;
    u32    unk2;
    u32    ObjectTagId;
    u32    unk3[12];
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
    byte_t unk[52];
};

struct bsp_trigger
{
    u32 unk[2];
};

struct move_positions
{
    byte_t unk1[32];
    byte_t unk2[4];
    u32    Offset;
    u32    unk[5];
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
    bl_tag tag;
    u32    NamePtr;
    u32    unk;
    scalar unk2;
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
    u32    unk1[6];
    bl_tag tag;
    u32    NamePtr;
    u32    unk[2];
};

} // namespace ai

struct script_trigger
{
    u32       unk1;
    bl_string name;
    Vecf3     pos;
    u32       unk[11];
};

struct globals
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
    bl_tag tag;
    u32    NamePtr;
    u32    reserved;
    u32    TagId;
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
    u32 header_offset;

    /* Below values are only valid on Xbox */
    reflexive_t<byte_t, xbox_reflexive>        xbox_vertices;
    reflexive_t<light_fixture, xbox_reflexive> xbox_lightmaps;
    bl_tag                                     tag;

    inline reflexive_t<header> to_header(info const& info_data) const;
};

struct info
{
    u32      offset;
    u32      size;
    u32      magic;
    u32      zero;
    tagref_t tag;

    inline reflexive_t<section> to_bsp() const
    {
        return {1, C_FCAST<i32>(magic - offset)};
    }
};

struct header
{
    tagref_t            lightmaps;
    u32                 unknown1[37];
    reflexive_t<byte_t> shaders;
    reflexive_t<byte_t> collision_header;
    reflexive_t<byte_t> nodes;
    bounding_box        world_bounds;
    reflexive_t<byte_t> leaves;
    reflexive_t<byte_t> leaf_surfaces;
    reflexive_t<byte_t> submesh_tris;
    reflexive_t<byte_t> submesh_header;
    reflexive_t<byte_t> chunk_10;
    reflexive_t<byte_t> chunk_11;
    reflexive_t<byte_t> chunk_12;
    reflexive_t<byte_t> clusters;
    i32                 cluster_data_size;
    u32                 unknown2;
    reflexive_t<byte_t> cluster_data;
    reflexive_t<byte_t> cluster_portals;
    reflexive_t<byte_t> chunk_16a;
    reflexive_t<byte_t> breakables_surfaces;
    reflexive_t<byte_t> fog_planes;
    reflexive_t<byte_t> fog_regions;
    reflexive_t<byte_t> fog_weather_pallette;
    reflexive_t<byte_t> chunk_16f;
    reflexive_t<byte_t> chunk_16g;
    reflexive_t<byte_t> weather;
    reflexive_t<byte_t> weather_polyhedra;
    reflexive_t<byte_t> chunk_19;
    reflexive_t<byte_t> chunk_20;
    reflexive_t<byte_t> pathfinding_surface;
    reflexive_t<byte_t> chunk_24;
    reflexive_t<byte_t> background_sound;
    reflexive_t<byte_t> sound_env;
    reflexive_t<byte_t> sound_pas_data_size;
    u32                 unknown3;
    reflexive_t<byte_t> sound_pas_data;
    reflexive_t<byte_t> chunk_26;
    reflexive_t<byte_t> chunk_27;
    reflexive_t<byte_t> markers;
    reflexive_t<byte_t> clutter_objects; /*!< aka detail_objects */
    reflexive_t<byte_t> runtime_decals;
    u32                 unkown4[9];
};

inline reflexive_t<header> section::to_header(info const& info_data) const
{
    return {1,
            C_FCAST<i32>(header_offset - (info_data.magic - info_data.offset))};
}

} // namespace bsp

/*!
 * \brief A Blam! scenario descriptor
 */
struct scenario
{
    byte_t                unk_str1[16];
    byte_t                unk_str2[16];
    byte_t                unk_str3[16];
    reflexive_t<skybox>   skyboxes;
    u32                   zero1;
    reflexive_t<tagref_t> child_scenarios;

    u32 reserved1[46];

    i32 editor_scenario_size;
    u32 unknown_2;
    u32 unknown_3;
    u32 ptr_to_index;
    u32 reserved2[2];
    u32 ptr_to_index_end;
    u32 reserved3[57];

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

    reflexive_t<scn_chunk> unknown_4[7];

    reflexive_t<player_starting_profile> player_start_profile;
    reflexive_t<player_spawn>            player_spawn;
    reflexive_t<trigger_volume>          trigger_volume;
    reflexive_t<scn_chunk>               animation;
    reflexive_t<multiplayer_flag>        multiplayer_flags;
    reflexive_t<multiplayer_equipment>   multiplayer_equipment;
    reflexive_t<starting_equip>          starting_equipment;
    reflexive_t<bsp_trigger>             bsp_switch_trigger;
    reflex_group<decal>                  decals;
    reflexive_t<scn_chunk>               detail_obj_collision_ref;
    reflexive_t<scn_chunk>               unknown_5[7];
    reflexive_t<actor_variant_ref>       actor_variant_ref;
    reflexive_t<encounter::encounter>    encounters;

    reflexive_t<scn_chunk>         command_lists;
    reflexive_t<scn_chunk>         unknown_6;
    reflexive_t<scn_chunk>         starting_locations;
    reflexive_t<scn_chunk>         platoons;
    reflexive_t<scn_chunk>         ai_conversations;
    u32                            script_syntax_data_size;
    u32                            unknown_7;
    reflexive_t<scn_chunk>         scripts;
    reflexive_t<scn_chunk>         commands;
    reflexive_t<scn_chunk>         points;
    reflexive_t<ai::animation_ref> ai_animation_refs;
    reflexive_t<globals>           globals;
    reflexive_t<ai::recording_ref> ai_recording_refs;
    reflexive_t<scn_chunk>         unknown_8;
    reflexive_t<scn_chunk>         participants;
    reflexive_t<scn_chunk>         lines;
    reflexive_t<script_trigger>    script_triggers;
    reflexive_t<scn_chunk>         cutscenes_verify;
    reflexive_t<scn_chunk>         cutscene_titles_verify;
    reflexive_t<scn_chunk>         source_files;
    reflexive_t<scn_chunk>         cutscene_flags;
    reflexive_t<scn_chunk>         cutscene_camera_poi;
    reflexive_t<scn_chunk>         cutscene_titles;
    reflexive_t<scn_chunk>         unknown_9[8];
    u32                            zero2;
    u32                            unknown_10;
    reflexive_t<bsp::info>         bsp_info;
};

} // namespace scn
} // namespace blam
