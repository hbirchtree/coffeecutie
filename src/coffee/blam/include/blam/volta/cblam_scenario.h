#pragma once

#include "cblam_bsp_structures.h"
#include "cblam_bytecode.h"
#include "cblam_mod2.h"
#include "cblam_structures.h"

namespace blam {
namespace scn {

using angle_t = scalar;

/* Data which is not part of the structures proper */

union local_actor_type
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

struct unit
{
    reflexive_t<byte_t>               unknown_r;
    u32                               padding[4];
    Vecf3                             unknown_offset;
    tagref_typed_t<tag_class_t::mod2> model;
    tagref_typed_t<tag_class_t::antr> anim_trigger;
    u32                               padding2[10];
    tagref_typed_t<tag_class_t::coll> collider;
    tagref_typed_t<tag_class_t::pphy> physics;
    tagref_typed_t<tag_class_t::shdr> shader;
    tagref_typed_t<tag_class_t::effe> effect;
};

using vehicle   = unit;
using biped     = unit;
using equipment = unit;

template<size_t Padding>
struct object_spawn
{
    i16   ref;
    i16   flag;
    u32   unknown_2;
    Vecf3 pos;
    Vecf3 rot;
    u32   unknown_3[Padding];
};

using biped_spawn   = object_spawn<22>;
using vehicle_spawn = object_spawn<22>;
using equip_spawn   = object_spawn<2>;
using scenery_spawn = object_spawn<10>;
using weapon_spawn  = object_spawn<15>;

struct palette
{
    tagref_t tag;
    byte_t   unk[32];
};

struct weapon_instance
{
    u32      reserved1[8];
    u32      unknown;
    tagref_t something;
};

using weapon_tagref = reflexive_t<weapon_instance>;

struct scenery
{
    u32                               count;
    string_ref                        name;
    u32                               padding[8];
    tagref_typed_t<tag_class_t::mod2> model;
};

struct machine
{
    i16    type;
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
    struct perception_t /* perception */
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
    } perception;
    struct movement_t
    {
    } movement;
    struct looking_t
    {
    } looking;
    struct unopposable_t
    {
    } unopposable;
    struct panic_t
    {
    } panic;
    struct defensive_t
    {
    } defensive;
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

    struct movement_t
    {
        actor_movement_t movement;
        scalar           crouch_chance;
        Vecf2            crouch_time;
        Vecf2            run_time;
    } movement;

    struct combat_t
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
    } combat;
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
    reflexive_t<byte_t>       firing_positions;
    reflexive_t<player_spawn> start_locations;
};

struct squad
{
    bl_string           name;
    local_actor_type    actor_type;
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

    struct /* 324-byte block, object spawns */
    {
        reflexive_t<object_name>    object_names;
        reflex_group<scenery_spawn> scenery;
        reflex_group<biped_spawn>   bipeds;
        reflex_group<vehicle_spawn> vehicles;
        reflex_group<equip_spawn>   equips;
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
        u32                               script_function_table_offset;
        u32                               script_function_table_size;
        reflexive_t<char>                 script_string_segment;
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

    inline semantic::mem_chunk<hsc::opcode_layout const> bytecode(
        magic_data_t const& magic) const
    {
        auto num_opcodes = (script_bytecode_size - sizeof(hsc::script_ref)) /
                           sizeof(hsc::opcode_layout);
        auto const& script_base = scripts.data(magic)[0].opcode_first();
        return semantic::mem_chunk<hsc::opcode_layout const>::From(
            &script_base, num_opcodes);
    }
    inline string_segment_ref string_segment(magic_data_t const& magic) const
    {
        constexpr Array<char, 4> terminator = {{0, 0, 0, 0}};

        auto string_base = script_string_segment.data(magic);
        auto end         = C_RCAST<const char*>(::memmem(
            &string_base[0],
            string_base.size,
            terminator.data(),
            terminator.size()));

        u32  num_strings = 0;
        auto start_ptr   = string_base.data;

        while(start_ptr < end && start_ptr >= string_base.data)
        {
            if(start_ptr[0] != 0)
                num_strings++;

            start_ptr =
                C_RCAST<const char*>(::memchr(start_ptr, 0, string_base.size));

            if(start_ptr[1] == 0)
                break;

            start_ptr++; // Jump over null-terminator
        }

        return {semantic::mem_chunk<const char>::From(
                    &string_base[0], (end - &string_base[0]) + 1),
                num_strings};
    }
    inline semantic::mem_chunk<hsc::function_declaration const> function_table(
        magic_data_t const& magic) const
    {
        auto string_seg = string_segment(magic);

        auto init_ptr = string_seg.data.data + script_function_table_offset;
        hsc::function_declaration const* func_seg_start = nullptr;

        /* TODO: Find out what the true offset is, for now just look for the
         * padding */
        for(auto i : stl_types::Range<u32>(4))
        {
            func_seg_start = C_RCAST<decltype(func_seg_start)>(init_ptr + i);
            if(func_seg_start->padding[0] == 0 &&
               func_seg_start->padding[1] == 0)
                break;
        }

        auto num_functions =
            (script_string_segment.count - string_seg.data.size) /
            sizeof(hsc::function_declaration);

        for(auto i : stl_types::Range<>(num_functions))
        {
            num_functions = i;
            if(func_seg_start[i].padding[0] != 0)
                break;
        }

        if(!num_functions)
            return {};

        return semantic::mem_chunk<hsc::function_declaration const>::From(
            func_seg_start, num_functions);
    }

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
