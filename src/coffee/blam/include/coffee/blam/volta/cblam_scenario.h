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

struct script_triggers
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
    byte_t              text[16];
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

using skybox_t = tagref_t;

struct starting_equip
{
    u32      unknown1; /*!< Sometimes 1? */
    u32      index;
    byte_t   padding2[51];
    tagref_t items[6];
    byte_t   padding3[45];
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

struct sbsp
{
    u32    header_offset;
    u32    xbox_reflexive_count;
    u32    xbox_reflexive_offset;
    u32    xbox_lightmap_reflexive_count;
    u32    xbox_lightmap_reflexive_offset;
    bl_tag tag;
};

/*!
 * \brief Is not the same as a blam_mod2_bsp_header, this is extracted from the
 * scenario and references the blam_mod2_bsp_header structure it belongs to.
 */
struct bsp_header
{
    u32    offset;
    u32    size;
    u32    magic;
    u32    zero;
    bl_tag tag;
    u32    name_ptr;
    u32    unknown1;
    u32    tag_id;
};

/*!
 * \brief A Blam! scenario descriptor
 */
struct scenario
{
    byte_t                unk_str1[16];
    byte_t                unk_str2[16];
    byte_t                unk_str3[16];
    reflexive_t<skybox_t> skybox_t;
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
    reflex_group<biped>         biped;
    reflex_group<vehicle_spawn> vehicle;
    reflex_group<equip>         equip;
    reflex_group<weapon_spawn>  weap;
    reflexive_t<device_group>   device_groups;
    reflex_group<machine>       machine;
    reflex_group<control>       control;
    reflex_group<light_fixture> light_fixture;
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
    reflexive_t<script_triggers>   script_triggers;
    reflexive_t<scn_chunk>         cutscenes_verify;
    reflexive_t<scn_chunk>         cutscene_titles_verify;
    reflexive_t<scn_chunk>         source_files;
    reflexive_t<scn_chunk>         cutscene_flags;
    reflexive_t<scn_chunk>         cutscene_camera_poi;
    reflexive_t<scn_chunk>         cutscene_titles;
    reflexive_t<scn_chunk>         unknown_9[8];
    u32                            zero2;
    u32                            unknown_10;
    reflexive_t<bsp_header>        struct_bsp;
};

} // namespace scn
} // namespace blam
