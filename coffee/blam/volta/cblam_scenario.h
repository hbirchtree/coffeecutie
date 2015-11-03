#ifndef CBLAM_SCENARIO
#define CBLAM_SCENARIO

#include "cblam_structures.h"
#include "cblam_mod2.h"

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

template<typename T>
/*!
 * \brief Encapsulates a base and reference reflexive group
 */
struct blam_reflex_group
{
    blam_reflexive<T> base;
    blam_reflexive<T> ref;
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
    uint32 unk1_offset;
    byte name[28];
    uint16 padding1;
    uint32 unk2_offset;
    blam_tagref weapon1;
    uint32 unk3;
    blam_tagref weapon2;
    uint32 unk4;
    uint32 unk5;
    uint32 padding2[5];
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

struct blam_scn_object_name
{
    byte name[32];
    uint32 unknown;
};

struct blam_scn_trigger_volume
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
    blam_reflexive<byte> squads;
    blam_reflexive<byte> platoons;
    blam_reflexive<byte> firingPositions;
    blam_reflexive<byte> playerStartLocations;
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
    blam_reflexive<byte> StartLocations;
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
    byte text[16];
    blam_scn_encounter_squad *pSquads;
    blam_scn_encounter_squad **ppSquadSpawns;
    blam_reflexive<byte> Platoons;
    blam_reflexive<byte> FiringPositions;
    blam_reflexive<byte> PlayerStartLocations;
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
    uint32 unknown1; /*!< Sometimes 1? */
    uint32 index;
    ubyte padding2[51];
    blam_tagref items[6];
    ubyte padding3[45];
};

struct blam_scn_chunk
{
    byte unknown[100];
};

struct blam_scn_control
{
    int16 unk1;
    int16 unk2;
    ubyte unk3[4];
    CVec3 pos;
    uint32 tag_id;
    ubyte unk[40];
};

struct blam_scn_light_fixture
{
    blam_rgba ambient;
    ubyte unk1[4];
    CVec3 pos;
    ubyte unk2[12];
    blam_rgba specular;
    int32 zero1;
    blam_rgba color;
    ubyte unk3[40];
};

struct blam_scn_sbsp
{
    uint32 header_offset;
    uint32 xbox_reflexive_count;
    uint32 xbox_reflexive_offset;
    uint32 xbox_lightmap_reflexive_count;
    uint32 xbox_lightmap_reflexive_offset;
    byte tag[4];
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
    byte tag[4];
    uint32 name_ptr;
    uint32 unknown1;
    uint32 tag_id;
};

/*!
 * \brief A Blam! scenario descriptor
 */
struct blam_scenario
{
    byte unk_str1[16];
    byte unk_str2[16];
    byte unk_str3[16];
    blam_reflexive<blam_scn_skybox> skybox;
    uint32 zero1;
    blam_reflexive<blam_tagref> child_scenarios;

    uint32 reserved1[46];

    int32 editor_scenario_size;
    uint32 unknown_2;
    uint32 unknown_3;
    uint32 ptr_to_index;
    uint32 reserved2[2];
    uint32 ptr_to_index_end;
    uint32 reserved3[57];

    blam_reflexive<blam_scn_object_name> object_names;
    blam_reflex_group<blam_scn_scenery_spawn> scenery;
    blam_reflex_group<blam_scn_biped> biped;
    blam_reflex_group<blam_scn_vehicle_spawn> vehicle;
    blam_reflex_group<blam_scn_equip> equip;
    blam_reflex_group<blam_scn_weapon_spawn> weap;
    blam_reflexive<blam_scn_device_group> device_groups;
    blam_reflex_group<blam_scn_machine> machine;
    blam_reflex_group<blam_scn_control> control;
    blam_reflex_group<blam_scn_light_fixture> light_fixture;
    blam_reflex_group<blam_scn_sound_scenery> snd_scenery;

    blam_reflexive<blam_scn_chunk> unknown_4[7];

    blam_reflexive<blam_scn_player_starting_profile> player_start_profile;
    blam_reflexive<blam_scn_player_spawn> player_spawn;
    blam_reflexive<blam_scn_trigger_volume> trigger_volume;
    blam_reflexive<blam_scn_chunk> animation;
    blam_reflexive<blam_scn_multiplayer_flag> multiplayer_flags;
    blam_reflexive<blam_scn_multiplayer_equipment> multiplayer_equipment;
    blam_reflexive<blam_scn_starting_equip> starting_equipment;
    blam_reflexive<blam_scn_bsp_trigger> bsp_switch_trigger;
    blam_reflex_group<blam_scn_decal> decals;
    blam_reflexive<blam_scn_chunk> detail_obj_collision_ref;
    blam_reflexive<blam_scn_chunk> unknown_5[7];
    blam_reflexive<blam_scn_actor_variant_ref> actor_variant_ref;
    blam_reflexive<blam_scn_encounter> encounters;

    blam_reflexive<blam_scn_chunk> command_lists;
    blam_reflexive<blam_scn_chunk> unknown_6;
    blam_reflexive<blam_scn_chunk> starting_locations;
    blam_reflexive<blam_scn_chunk> platoons;
    blam_reflexive<blam_scn_chunk> ai_conversations;
    uint32 script_syntax_data_size;
    uint32 unknown_7;
    blam_reflexive<blam_scn_chunk> scripts;
    blam_reflexive<blam_scn_chunk> commands;
    blam_reflexive<blam_scn_chunk> points;
    blam_reflexive<blam_scn_ai_animation_ref> ai_animation_refs;
    blam_reflexive<blam_scn_globals> globals;
    blam_reflexive<blam_scn_ai_recording_ref> ai_recording_refs;
    blam_reflexive<blam_scn_chunk> unknown_8;
    blam_reflexive<blam_scn_chunk> participants;
    blam_reflexive<blam_scn_chunk> lines;
    blam_reflexive<blam_scn_script_triggers> script_triggers;
    blam_reflexive<blam_scn_chunk> cutscenes_verify;
    blam_reflexive<blam_scn_chunk> cutscene_titles_verify;
    blam_reflexive<blam_scn_chunk> source_files;
    blam_reflexive<blam_scn_chunk> cutscene_flags;
    blam_reflexive<blam_scn_chunk> cutscene_camera_poi;
    blam_reflexive<blam_scn_chunk> cutscene_titles;
    blam_reflexive<blam_scn_chunk> unknown_9[8];
    uint32 zero2;
    uint32 unknown_10;
    blam_reflexive<blam_scn_bsp_header> struct_bsp;
};

}
}

#endif
