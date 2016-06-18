#ifndef CBLAM_SCENARIO
#define CBLAM_SCENARIO

#include "cblam_structures.h"
#include "cblam_mod2.h"

namespace Coffee{
namespace CBlam{

using blam_scn_chunk = byte_t[100];

/*!
 * \brief The blam_bounding_box struct
 */
struct blam_bounding_box
{
    blam_bounding_box():
        min(0),
        max(0)
    {
    }

    CVec3 min;
    CVec3 max;
};

template<typename T>
/*!
 * \brief Encapsulates a base and reference reflexive group
 */
struct blam_reflex_group
{
    reflexive_t<T> base;
    reflexive_t<T> ref;
};

struct blam_scn_biped
{
    int16 BipedType;
    int16 unk2;
    uint32 unk3;
    CVec3 pos;
    scalar rot;
    uint32 unk[24];
};

struct blam_scn_equip
{
    int16 numid;
    int16 unk2;
    uint32  unk3;
    CVec3 pos;
    uint32 unk[5];
};

struct blam_scn_vehicle_spawn
{
    int16 numid;
    uint16 flag;
    uint32 unknown1;
    CVec3 pos;
    scalar rot;
    uint32 unknown2[24];
};

struct blam_scn_palette
{
    bl_tag tag;
    uint32 NamePtr;
    uint32 unk1;
    uint32 ObjectTagId;
    uint32 unk[8];
};

struct blam_scn_weapon_spawn
{
    int16 numid;
    uint16 flag;
    uint32   unknown1;
    CVec3 pos;
    scalar  rot;
    uint32   unknown2[17];
};

struct blam_scn_weapon_ref
{
    int16 numid;
    uint16 flag;
    uint32   unknown1;
    CVec3 pos;
    scalar  rot;
    uint32   unknown2[17];
};


struct blam_scn_scenery_spawn
{
    int16 numid;
    uint16 flag;
    uint32 unknown1;
    CVec3 pos;
    scalar  rot;
    scalar  unk1[2];
    uint32   unknown2[10];
};

struct blam_scn_machine
{
    int16 MachineType;
    int16 unk2;
    uint32  unk3;
    CVec3 pos;
    scalar rot;
    uint32 unk[10];
};

struct blam_scn_sound_scenery
{
    int16 SoundType;
    int16 unk2;
    uint32  unk3;
    CVec3 pos;
    uint32 unk4[5];
};

struct blam_scn_player_spawn
{
    CVec3 pos;
    scalar  rot;
    scalar  unknown2[9];
};

struct blam_scn_profile_placement
{
    bl_string name;
    scalar health;
    scalar shields;
    bl_tag primarytag;
    int32 primaryrawfilename;
    byte_t zero1[4];
    int32 primaryidentifier;
    int16 primaryclip;
    int16 primarytotal;
    bl_tag secondarytag;
    int32 secondaryfilename;
    byte_t zero2[4];
    int32 secondaryidentifier;
    int16 secondaryclip;
    int16 secondarytotal;
    int16 fraggrencount;
    int16 plasmagrencount;
    byte_t zero3[20];
};


struct blam_scn_multiplayer_flag
{
    CVec3 pos;
    scalar yaw;
    uint16 index1;
    uint16 index2;
    bl_tag  tag;
    uint32  unk2[31];
};

struct blam_scn_multiplayer_equipment
{
    uint32 unk[16];
    CVec3 pos;
    scalar yaw;
    scalar unk1;
    uint32 NamePtr;
    uint32 unk2;
    uint32 ObjectTagId;
    uint32 unk3[12];
};

struct blam_scn_player_starting_profile
{
    uint32 unk1_offset;
    byte_t name[28];
    uint16 padding1;
    uint32 unk2_offset;
    tagref_t weapon1;
    uint32 unk3;
    tagref_t weapon2;
    uint32 unk4;
    uint32 unk5;
    uint32 padding2[5];
};

struct blam_scn_device_group
{
    byte_t unk[52];
};

struct blam_scn_bsp_trigger
{
    uint32 unk[2];
};

struct blam_scn_move_positions
{
    byte_t unk1[32];
    byte_t unk2[4];
    uint32 Offset;
    uint32 unk[5];
};

struct blam_scn_object_name
{
    bl_string name;
    uint32 unknown;
};

struct blam_scn_trigger_volume
{
    uint32 unk;
    bl_string name;
    scalar unk2[9];
    blam_bounding_box box;
};

struct blam_scn_actor_variant_ref
{
    bl_tag tag;
    uint32 NamePtr;
    uint32 unk;
    scalar unk2;
};

struct blam_scn_ai_animation_ref
{
    bl_string name;
    int16 unk1;
    int16 unk2;
    uint32 unk[14];
};

struct blam_scn_ai_script_ref
{
    bl_string name;
    uint32 unk[15];
};

struct blam_scn_ai_recording_ref
{
    uint32 unk1[6];
    bl_tag tag;
    uint32 NamePtr;
    uint32 unk[2];
};

struct blam_scn_script_triggers
{
    uint32 unk1;
    bl_string name;
    CVec3 pos;
    uint32 unk[11];
};

struct blam_scn_globals
{
    uint32 unk1;
    bl_string name;
    uint32 unk2;
    CVec3 pos;
    scalar unk3[4];
    uint32 unk[9];
};

struct blam_scn_references
{
    uint32 unk1;
    bl_string name;
    uint32 unk[15];
};

struct blam_scn_encounter
{
    byte_t text[16];
    uint32 unk[28];
    reflexive_t<byte_t> squads;
    reflexive_t<byte_t> platoons;
    reflexive_t<byte_t> firingPositions;
    reflexive_t<byte_t> playerStartLocations;
};

struct blam_scn_encounter_squad
{
    bl_string name;
    int16 ActorType;
    int16 Platoon;
    int16 InitialState;
    int16 ReturnState;
    uint32  unk1[11];
    uint32  Ai_Attacking;
    uint32  Ai_AttackingSearch;
    uint32  Ai_AttackingGuard;
    uint32  Ai_Defending;
    uint32  Ai_DefendingSearch;
    uint32  Ai_DefendingGuard;
    uint32  Ai_Pursuing;
    uint32  unk2[3];
    int16 NormalDiffCount;
    int16 InsaneDiffCount;
    uint32  unk3[20];
    reflexive_t<byte_t> StartLocations;
    uint32  unk4[3];
};

struct blam_scn_encounter_squad_spawn
{
    CVec3 pos;
    scalar yaw;
    int16 unk2[5];
    int16 CommandList;
};

struct blam_scn_encounter_info
{
    byte_t text[16];
    blam_scn_encounter_squad *pSquads;
    blam_scn_encounter_squad **ppSquadSpawns;
    reflexive_t<byte_t> Platoons;
    reflexive_t<byte_t> FiringPositions;
    reflexive_t<byte_t> PlayerStartLocations;
};

struct blam_scn_decal
{
    int16 unk1;
    int16 unk2;
    CVec3 pos;
};

struct blam_scn_decal_ref
{
    bl_tag tag;
    uint32 NamePtr;
    uint32 reserved;
    uint32 TagId;
};

struct blam_scn_shader_index
{
    uint32 ShaderType;
    uint32 ShaderIndex;
};

struct blam_scn_skybox
{
    bl_tag tag;
    uint32 NameRef;
    uint32 unk1;
    uint32 TagId;
};

struct blam_scn_starting_equip
{
    uint32 unknown1; /*!< Sometimes 1? */
    uint32 index;
    byte_t padding2[51];
    tagref_t items[6];
    byte_t padding3[45];
};

struct blam_scn_control
{
    int16 unk1;
    int16 unk2;
    byte_t unk3[4];
    CVec3 pos;
    uint32 tag_id;
    byte_t unk[40];
};

struct blam_scn_light_fixture
{
    bl_rgba_t ambient;
    byte_t unk1[4];
    CVec3 pos;
    byte_t unk2[12];
    bl_rgba_t specular;
    int32 zero1;
    bl_rgba_t color;
    byte_t unk3[40];
};

struct blam_scn_sbsp
{
    uint32 header_offset;
    uint32 xbox_reflexive_count;
    uint32 xbox_reflexive_offset;
    uint32 xbox_lightmap_reflexive_count;
    uint32 xbox_lightmap_reflexive_offset;
    bl_tag tag;
};

/*!
 * \brief Is not the same as a blam_mod2_bsp_header, this is extracted from the scenario and references the blam_mod2_bsp_header structure it belongs to.
 */
struct blam_scn_bsp_header
{
    uint32 offset;
    uint32 size;
    uint32 magic;
    uint32 zero;
    bl_tag tag;
    uint32 name_ptr;
    uint32 unknown1;
    uint32 tag_id;
};

/*!
 * \brief A Blam! scenario descriptor
 */
struct blam_scenario
{
    byte_t unk_str1[16];
    byte_t unk_str2[16];
    byte_t unk_str3[16];
    reflexive_t<blam_scn_skybox> skybox;
    uint32 zero1;
    reflexive_t<tagref_t> child_scenarios;

    uint32 reserved1[46];

    int32 editor_scenario_size;
    uint32 unknown_2;
    uint32 unknown_3;
    uint32 ptr_to_index;
    uint32 reserved2[2];
    uint32 ptr_to_index_end;
    uint32 reserved3[57];

    reflexive_t<blam_scn_object_name> object_names;
    blam_reflex_group<blam_scn_scenery_spawn> scenery;
    blam_reflex_group<blam_scn_biped> biped;
    blam_reflex_group<blam_scn_vehicle_spawn> vehicle;
    blam_reflex_group<blam_scn_equip> equip;
    blam_reflex_group<blam_scn_weapon_spawn> weap;
    reflexive_t<blam_scn_device_group> device_groups;
    blam_reflex_group<blam_scn_machine> machine;
    blam_reflex_group<blam_scn_control> control;
    blam_reflex_group<blam_scn_light_fixture> light_fixture;
    blam_reflex_group<blam_scn_sound_scenery> snd_scenery;

    reflexive_t<blam_scn_chunk> unknown_4[7];

    reflexive_t<blam_scn_player_starting_profile> player_start_profile;
    reflexive_t<blam_scn_player_spawn> player_spawn;
    reflexive_t<blam_scn_trigger_volume> trigger_volume;
    reflexive_t<blam_scn_chunk> animation;
    reflexive_t<blam_scn_multiplayer_flag> multiplayer_flags;
    reflexive_t<blam_scn_multiplayer_equipment> multiplayer_equipment;
    reflexive_t<blam_scn_starting_equip> starting_equipment;
    reflexive_t<blam_scn_bsp_trigger> bsp_switch_trigger;
    blam_reflex_group<blam_scn_decal> decals;
    reflexive_t<blam_scn_chunk> detail_obj_collision_ref;
    reflexive_t<blam_scn_chunk> unknown_5[7];
    reflexive_t<blam_scn_actor_variant_ref> actor_variant_ref;
    reflexive_t<blam_scn_encounter> encounters;

    reflexive_t<blam_scn_chunk> command_lists;
    reflexive_t<blam_scn_chunk> unknown_6;
    reflexive_t<blam_scn_chunk> starting_locations;
    reflexive_t<blam_scn_chunk> platoons;
    reflexive_t<blam_scn_chunk> ai_conversations;
    uint32 script_syntax_data_size;
    uint32 unknown_7;
    reflexive_t<blam_scn_chunk> scripts;
    reflexive_t<blam_scn_chunk> commands;
    reflexive_t<blam_scn_chunk> points;
    reflexive_t<blam_scn_ai_animation_ref> ai_animation_refs;
    reflexive_t<blam_scn_globals> globals;
    reflexive_t<blam_scn_ai_recording_ref> ai_recording_refs;
    reflexive_t<blam_scn_chunk> unknown_8;
    reflexive_t<blam_scn_chunk> participants;
    reflexive_t<blam_scn_chunk> lines;
    reflexive_t<blam_scn_script_triggers> script_triggers;
    reflexive_t<blam_scn_chunk> cutscenes_verify;
    reflexive_t<blam_scn_chunk> cutscene_titles_verify;
    reflexive_t<blam_scn_chunk> source_files;
    reflexive_t<blam_scn_chunk> cutscene_flags;
    reflexive_t<blam_scn_chunk> cutscene_camera_poi;
    reflexive_t<blam_scn_chunk> cutscene_titles;
    reflexive_t<blam_scn_chunk> unknown_9[8];
    uint32 zero2;
    uint32 unknown_10;
    reflexive_t<blam_scn_bsp_header> struct_bsp;
};

}
}

#endif
