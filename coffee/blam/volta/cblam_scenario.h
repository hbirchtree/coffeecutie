#ifndef CBLAM_SCENARIO
#define CBLAM_SCENARIO

#include "cblam_structures.h"

namespace Coffee{
namespace CBlam{

/*!
 * \brief The blam_bounding_box struct
 */
struct blam_bounding_box
{
    CVec3 min;
    CVec3 max;
};

/*!
 * \brief Encapsulates a base and reference reflexive group
 */
struct blam_reflex_group
{
    blam_reflexive base;
    blam_reflexive ref;
};

/*!
 * \brief A Blam! scenario descriptor
 */
struct blam_scenario
{
    byte unk_str1[16];
    byte unk_str2[16];
    byte unk_str3[16];
    blam_reflexive skybox;
    uint32 unknown_1;
    blam_reflexive child_scenarios;

    uint32 reserved1[46];

    int32 editor_scenario_size;
    uint32 unknown_2;
    uint32 unknown_3;
    uint32 ptr_to_index;
    uint32 reserved2[2];
    uint32 ptr_to_index_end;
    uint32 reserved3[57];

    blam_reflexive object_names;
    blam_reflex_group scenery;
    blam_reflex_group biped;
    blam_reflex_group vehicle;
    blam_reflex_group equip;
    blam_reflex_group weap;
    blam_reflexive device_groups;
    blam_reflex_group machine;
    blam_reflex_group control;
    blam_reflex_group light_fixture;
    blam_reflex_group snd_scenery;
    blam_reflexive unknown_4[7];
    blam_reflexive player_start_profile;
    blam_reflexive player_spawn;
    blam_reflexive trigger_volume;
    blam_reflexive animation;
    blam_reflexive multiplayer_flags;
    blam_reflexive multiplayer_equipment;
    blam_reflexive starting_equipment;
    blam_reflexive bsp_switch_trigger;
    blam_reflex_group decals;
    blam_reflexive detail_obj_collision_ref;
    blam_reflexive unknown_5[7];
    blam_reflexive actor_variant_ref;
    blam_reflexive encounters;

    blam_reflexive command_lists;
    blam_reflexive unknown_6;
    blam_reflexive starting_locations;
    blam_reflexive platoons;
    blam_reflexive ai_conversations;
    uint32 script_syntax_data_size;
    uint32 unknown_7;
    blam_reflexive scripts;
    blam_reflexive commands;
    blam_reflexive points;
    blam_reflexive ai_animation_refs;
    blam_reflexive globals;
    blam_reflexive ai_recording_refs;
    blam_reflexive unknown_8;
    blam_reflexive participants;
    blam_reflexive lines;
    blam_reflexive script_triggers;
    blam_reflexive cutscenes_verify;
    blam_reflexive cutscene_titles_verify;
    blam_reflexive source_files;
    blam_reflexive cutscene_flags;
    blam_reflexive cutscene_camera_poi;
    blam_reflexive cutscene_titles;
    blam_reflexive unknown_9[8];
    uint32 unknown_10[2];
    blam_reflexive struct_bsp;
};

struct blam_scn_biped
{
    int16 BipedType;
    int16 unk2;
    uint32  unk3;
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
    byte tag[4];
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
    uint32   unknown1;
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
    byte name[32];
    scalar health;
    scalar shields;
    byte primarytag[4];
    int32 primaryrawfilename;
    byte zero1[4];
    int32 primaryidentifier;
    int16 primaryclip;
    int16 primarytotal;
    byte secondarytag[4];
    int32 secondaryfilename;
    byte zero2[4];
    int32 secondaryidentifier;
    int16 secondaryclip;
    int16 secondarytotal;
    int16 fraggrencount;
    int16 plasmagrencount;
    byte zero3[20];
};


struct blam_scn_multiplayer_flag
{
    CVec3 pos;
    scalar yaw;
    uint16 index1;
    uint16 index2;
    byte  tag[4];
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
    uint32 unk[26];
};

struct blam_scn_device_group
{
    byte unk[52];
};

struct blam_scn_bsp_trigger
{
    uint32 unk[2];
};

struct blam_scn_move_positions
{
    byte unk1[32];
    byte unk2[4];
    uint32 Offset;
    uint32 unk[5];
};

struct blam_scn_object_names
{
    byte name[32];
    uint16 unk1;
    uint16 unk2;
};

struct blam_scn_trigger_volumes
{
    uint32 unk;
    byte name[32];
    scalar unk2[9];
    blam_bounding_box box;
};

struct blam_scn_actor_variant_ref
{
    byte tag[4];
    uint32 NamePtr;
    uint32 unk;
    scalar unk2;
};

struct blam_scn_ai_animation_ref
{
    byte name[32];
    int16 unk1;
    int16 unk2;
    uint32 unk[14];
};

struct blam_scn_ai_script_ref
{
    byte name[32];
    uint32 unk[15];
};

struct blam_scn_ai_recording_ref
{
    uint32 unk1[6];
    byte tag[4];
    uint32 NamePtr;
    uint32 unk[2];
};

struct blam_scn_script_triggers
{
    uint32 unk1;
    byte name[32];
    CVec3 pos;
    uint32 unk[11];
};

struct blam_scn_globals
{
    uint32 unk1;
    byte name[32];
    uint32 unk2;
    CVec3 pos;
    scalar unk3[4];
    uint32 unk[9];
};

struct blam_scn_references
{
    uint32 unk1;
    byte name[32];
    uint32 unk[15];
};

struct blam_scn_encounter
{
    byte text[16];
    uint32 unk[28];
    blam_reflexive Squads;
    blam_reflexive Platoons;
    blam_reflexive FiringPositions;
    blam_reflexive PlayerStartLocations;
};

struct blam_scn_encounter_squad
{
    byte name[32];
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
    blam_reflexive StartLocations;
    uint32  unk4[3];
};

struct blam_scn_encounter_squad
{
    CVec3 pos;
    scalar yaw;
    int16 unk2[5];
    int16 CommandList;
};

struct blam_scn_encounter_info
{
    byte text[16];
    blam_scn_encounter_squad *pSquads;
    blam_scn_encounter_squad **ppSquadSpawns;
    blam_reflexive Platoons;
    blam_reflexive FiringPositions;
    blam_reflexive PlayerStartLocations;
};

struct blam_scn_decal
{
    int16 unk1;
    int16 unk2;
    CVec3 pos;
};

struct blam_scn_decal_ref
{
    byte tag[4];
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
    byte tag[4];
    uint32 NameRef;
    uint32 unk1;
    uint32 TagId;
};

struct blam_scn_starting_equip
{
    uint32 unk[51];
};

struct blam_scn_control
{
    int16 unk1;
    int16 unk2;
    uint32  unk3;
    CVec3 pos;
    uint32  Tag_Id;
    uint32 unk[10];
};

struct blam_scn_light_fixture
{
    uint32 unk[22];
};

}
}

#endif
