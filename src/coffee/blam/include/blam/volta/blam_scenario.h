#pragma once

#include "blam_base_types.h"
#include "blam_bsp_structures.h"
#include "blam_file_header.h"
#include "blam_magic_data.h"
#include "blam_mod2.h"
#include "blam_reflexive.h"
#include "blam_tag_index.h"
#include "blam_vertex.h"
#include "hsc/blam_bytecode.h"

#include <peripherals/stl/range.h>

namespace blam::scn {

template<typename T>
/*!
 * \brief Encapsulates a base and reference reflexive group
 */
struct reflex_group
{
    reflexive_t<T>                       instances;
    reflexive_t<std::array<tagref_t, 3>> palette;
};

using angle_t = f32;

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

using scn_chunk = byte_t[100];

enum class object_type : u16
{
    biped,
    vehicle,
    weapon,
    equipment,
    garbage,
    projectile,
    scenery,
    machine,
    control,
    light_fixture,
    placeholder,
    sound_scenery,
};

enum class object_flags : u16
{
    no_shadow                  = 0x1,
    transparent_self_occlusion = 0x2,
    bright = 0x4, /* "Brighter than it should be", as in unshaded? */
    not_pathfinding_obstacle = 0x8,
};

struct object
{
    object_type                       type;
    object_flags                      flags;
    f32                               bound_radius;
    Vecf3                             bound_offset;
    Vecf3                             origin_offset;
    f32                               acceleration_scale;
    u32                               padding_;
    tagref_typed_t<tag_class_t::mod2> model;
    tagref_typed_t<tag_class_t::antr> anim_graph;
    u32                               padding2[10];
    tagref_typed_t<tag_class_t::coll> collider;
    tagref_typed_t<tag_class_t::pphy> physics;
    tagref_typed_t<tag_class_t::shdr> shader;
    tagref_typed_t<tag_class_t::effe> effect;
    f32                               render_bound_radius;

    struct
    {
        u32 inputs[4];
        i16 hud_msg;
        i16 shader_perm;
    } export_;
};

struct item : object
{
    struct attachment_t
    {
        tagref_t type;
        i16      marker;
        u32      primary_scale;
        u32      second_scale;
        u32      change_color;
    };

    reflexive_t<attachment_t> attachments;
};

using vehicle = object;
using biped   = object;

struct weapon : item
{
};

struct equipment : item
{
};

struct alignas(4) object_spawn
{
    enum class spawn_flags : u16
    {
        none      = 0x0,
        automatic = 0x1,
        on_easy   = 0x2,
        on_normal = 0x4,
        on_hard   = 0x8,
    };

    enum bsp_flags_t : u16
    {
        none = 0x0,
    };

    i16         ref;
    i16         name;
    spawn_flags flags;
    u16         permutation;
    Vecf3       pos;
    Vecf3       rot;
    bsp_flags_t bsp_flags;
    u16         padding;
};

struct biped_spawn : object_spawn
{
    u32 padding[9];

    enum biped_flags_t : u16
    {
        none = 0x0,
        dead = 0x1,
    };

    f32           vitality;
    biped_flags_t biped_flags;
    u32           padding2[10];
};

static_assert(sizeof(biped_spawn) == 120);

struct vehicle_spawn : object_spawn
{
    u32 unknown_[21];
};

static_assert(sizeof(vehicle_spawn) == 120);

struct equip_spawn : object_spawn
{
    enum equip_flags_t : u16
    {
        none              = 0x0,
        initially_at_rest = 0x1, /* Not affected by gravity */
        obsolete          = 0x2, /* Unused? */
        does_accelerate   = 0x4, /* Not affected by explosions */
    };

    equip_flags_t equip_flags;
    u16           padding;
};

static_assert(sizeof(equip_spawn) == 40);

struct scenery_spawn : object_spawn
{
    u32 unknown_[9];
};

static_assert(sizeof(scenery_spawn) == 72);

struct weapon_spawn : object_spawn
{
    u32 unknown_[14];
};

static_assert(sizeof(weapon_spawn) == 92);

enum class machine_spawn_flags : u16
{
    none              = 0x0,
    initially_on      = 0x1,
    initially_off     = 0x2,
    changes_once      = 0x4,
    position_reversed = 0x8,
    not_usable        = 0x10,
};

struct device_machine_spawn : object_spawn
{
    enum class machine_spawn_flags2 : u16
    {
        none                 = 0x0,
        one_sided            = 0x1,
        never_appears_locked = 0x2,
        opened_by_melee      = 0x4,
    };

    i16                  power_group;
    i16                  position_group;
    machine_spawn_flags  internal_flags;
    machine_spawn_flags2 device_flags;

    u32 padding[5];
};

static_assert(sizeof(device_machine_spawn) == 64);

struct light_fixture_spawn : object_spawn
{
    i16                 power_group;
    i16                 position_group;
    machine_spawn_flags flags;
    Vecf3               color;
    f32                 intensity;
    f32                 falloff_angle;
    f32                 cutoff_angle;

    u32 padding[5];
};

static_assert(offsetof(light_fixture_spawn, power_group) == 36);
static_assert(sizeof(light_fixture_spawn) == 88);

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

struct device_machine : object
{
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
    Vecf3 pos;
    f32   rot;
    f32   unknown2[9];
};

struct profile_placement
{
    bl_string name;
    f32       health;
    f32       shields;
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
    f32    yaw;
    u16    index1;
    u16    index2;
    bl_tag tag;
    u32    unk2[31];
};

struct item_permutation
{
    u32      padding_[8];
    f32      weight;
    tagref_t item;
};

struct item_collection
{
    reflexive_t<item_permutation> items;
    u32                           spawn_time;
};

enum class equipment_gamemode_flags : u16
{
    none,
    ctf,
    slayer,
    oddball,
    king_of_the_hill,
    race,
    terminator,
    stub,
    ignored1,
    ignored2,
    ignored3,
    ignored4,
    all_games,
    all_except_ctf,
    all_except_race_ctf,
};

struct multiplayer_equipment
{
    enum class equipment_flag : u32
    {
        none     = 0x0,
        levitate = 0x1,
    };

    equipment_flag flags;

    equipment_gamemode_flags type0;
    equipment_gamemode_flags type1;
    equipment_gamemode_flags type2;
    equipment_gamemode_flags type3;

    u32 team_idx;
    u32 spawn_time;

    u32 padding[11];

    Vecf3    pos;
    f32      facing;
    tagref_t item;
    u32      unk3[12];
};

struct player_starting_profile
{
    u32      unk1_offset;
    byte_t   name[28];
    u16      padding1;
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
    enum class device_group_flags : u32
    {
        none         = 0x0,
        changes_once = 0x1,
    };

    bl_string          name;
    f32                initial_value;
    device_group_flags flags;
    u32                unk[3];
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
    f32          unk2[9];
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
        f32     max_vision_dist;
        angle_t central_vision;
        angle_t max_vision;
        angle_t peripheral_vision;
        f32     peripheral_dist;
        Vecf3   stand_gun_offset;
        Vecf3   crouch_gun_offset;
        f32     hearing_dist;
        f32     projectile_awareness; /* [0,1] */
        f32     vehicle_awareness;    /* [0,1] */
        f32     combat_perception_time;
        f32     guard_perception_time;
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
        f32              crouch_chance;
        Vecf2            crouch_time;
        Vecf2            run_time;
    } movement;

    struct combat_t
    {
        tagref_typed_t<cls::weap> weapon;

        f32     max_fire_dist;
        f32     rate_of_fire;
        angle_t projectile_error;
        Vecf2   burst_delay;
        f32     retarget_fire_time;

        f32 surprise_delay;
        f32 surprise_wild_fire_time;

        f32 death_wild_fire_chance;
        f32 death_wild_fire_time;

        Vecf2 desired_combat_range;
        Vecf3 custom_stand_gun_offset;
        Vecf3 custom_crouch_gun_offset;

        Vecf2 target_track;
        Vecf2 target_lead;
        f32   dmg_modifier;
        f32   dmg_per_second;
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

struct platoon
{
    u32 unknown[8];
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
    Vecf3 pos;
    f32   yaw;
    i16   unk2[5];
    i16   CommandList;
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

using skybox_ref = tagref_t;

struct skybox
{
    struct shader_function
    {
        bl_tag global_func;
    };

    struct animation
    {
        i16 anim_idx;
        u32 period;
    };

    struct light
    {
        enum class light_flags : u32
        {
            none              = 0x0,
            affects_exteriors = 0x1,
            affects_interiors = 0x2,
        };

        tagref_typed_t<tag_class_t::lens> lens_flare;
        i8                                marker_id;
        bl_string                         marker_name;

        u32 padding_2[6];

        struct radiosity_t
        {
            light_flags flags;
            Vecf3       color;
            f32         power;
            f32         test_distance;
            f32         unknown;
            Vecf2       direction; /* yaw, pitch */
            f32         diameter;
        } radiosity;
    };

    struct radiosity_opts
    {
        Vecf3 color;
        f32   power;
    };

    struct fog_opts
    {
        Vecf3 color;
        u32   padding[2];
        f32   density;
        f32   start_distance;
        f32   opaque_distance;
    };

    tagref_typed_t<tag_class_t::mod2> model;
    tagref_typed_t<tag_class_t::antr> anim_graph;

    u32 padding_1[6];

    radiosity_opts indoor_ambient;
    radiosity_opts outdoor_ambient;
    fog_opts       outdoor_fog;
    fog_opts       indoor_fog;
    tagref_t       indoor_fog_screen;

    u32 padding_3[1];

    reflexive_t<shader_function> shader_functions;
    reflexive_t<animation>       animations;
    reflexive_t<light>           lights;
}; // namespace scn

static_assert(sizeof(skybox) == 208);
static_assert(sizeof(skybox::light) == 116);
static_assert(offsetof(skybox, indoor_ambient) == 56);
static_assert(offsetof(skybox, lights) == 196);
static_assert(offsetof(skybox::light, radiosity) == 76);

struct starting_equip
{
    u32                     unknown1; /*!< Sometimes 1? */
    u32                     index;
    byte_t                  padding2[51];
    std::array<tagref_t, 6> items;
    byte_t                  padding3[45];
};

struct control : object_spawn
{
    u32 padding[7];
};

static_assert(sizeof(control) == 64);

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
template<typename V>
requires is_game_version<V>
struct scenario
{
    enum class scenario_type : u16
    {
        solo,
        multiplayer,
        main_menu,
    };
    enum class scenario_flags : u16
    {
        none         = 0x0,
        cortana_hack = 0x1,
        demo_ui      = 0x2,
    };

    using bytecode_t = typename V::bytecode_type;

    struct scenario_info_t /* 260-byte block? */
    {
        tagref_t unk_bsp1; // Unused
        tagref_t unk_bsp2; // Unused
        tagref_t unk_sky;  // Unused

        reflexive_t<skybox_ref> skyboxes;

        scenario_type  type;
        scenario_flags flags;

        reflexive_t<tagref_t> child_scenarios;

        f32 local_north;

        reflexive_t<u32> predicted_resource;
        reflexive_t<u32> functions;

        u32 padding1[39];
    } info;

    struct editor_t /* 256-byte block */
    {
        i32                 scenario_size;
        u32                 unknown_2;
        reflexive_t<byte_t> comments;
        u32                 padding2[59];
    } editor;

    struct objects_t /* 324-byte block, object spawns */
    {
        reflexive_t<object_name>           object_names;
        reflex_group<scenery_spawn>        scenery;
        reflex_group<biped_spawn>          bipeds;
        reflex_group<vehicle_spawn>        vehicles;
        reflex_group<equip_spawn>          equips;
        reflex_group<weapon_spawn>         weapon_spawns;
        reflexive_t<device_group>          device_groups;
        reflex_group<device_machine_spawn> machines;
        reflex_group<control>              controls;
        reflex_group<light_fixture_spawn>  light_fixtures;
        reflex_group<sound_scenery>        snd_scenery;

        u32 padding3[21];
    } objects;

    struct multiplayer_t /* 216-byte block */
    {
        reflexive_t<player_starting_profile> player_start_profiles;
        reflexive_t<player_spawn>            player_spawns;
        reflexive_t<trigger_volume>          trigger_volumes;
        reflexive_t<scn_chunk>               animation;
        reflexive_t<multiplayer_flag>        flags;
        reflexive_t<multiplayer_equipment>   equipment;
        reflexive_t<starting_equip>          starting_equipment;
        reflexive_t<bsp_trigger>             bsp_switch_triggers;
        reflex_group<decal>                  decals;
        reflexive_t<scn_chunk>               detail_obj_collision_ref;
        u32                                  padding4[21];
    } mp;

    struct script_t /* 340-byte block */
    {
        reflexive_t<actor_variant_ref>           actor_variant_refs;
        reflexive_t<encounter::encounter>        encounters;
        reflexive_t<scn_chunk>                   command_lists;
        reflexive_t<scn_chunk>                   unknown_6;
        reflexive_t<scn_chunk>                   starting_locations;
        reflexive_t<scn_chunk>                   platoons;
        reflexive_t<scn_chunk>                   ai_conversations;
        u32                                      script_bytecode_size;
        u32                                      unknown_7;
        reflexive_t<hsc::script_ref<bytecode_t>> unknown_9;
        u32                                      script_function_table_offset;
        u32                                      script_function_table_size;
        reflexive_t<char>                        script_string_segment;
        reflexive_t<hsc::function_declaration>   scripts;
        reflexive_t<hsc::global>                 globals;
        reflexive_t<ai::recording_ref>           ai_recording_refs;
        reflexive_t<scn_chunk>                   unknown_8;
        reflexive_t<scn_chunk>                   participants;
        reflexive_t<scn_chunk>                   lines;
        reflexive_t<script_trigger>              script_triggers;
        reflexive_t<scn_chunk>                   cutscenes_verify;
        reflexive_t<scn_chunk>                   cutscene_titles_verify;
        reflexive_t<scn_chunk>                   source_files;
        reflexive_t<scn_chunk>                   cutscene_flags;
        reflexive_t<scn_chunk>                   cutscene_camera_poi;
        reflexive_t<scn_chunk>                   cutscene_titles_;
        u32                                      padding5[15];
    } script;

    inline Span<hsc::opcode_layout<bytecode_t> const> bytecode(
        magic_data_t const& magic) const
    {
        reflexive_t<hsc::opcode_layout<bytecode_t>> data{
            .count = static_cast<u32>(
                (script.script_bytecode_size -
                 sizeof(hsc::script_ref<bytecode_t>)) /
                sizeof(hsc::opcode_layout<bytecode_t>)),
            .offset = script.unknown_9.offset + 36,
        };
        return data.data(magic).value();
    }

    inline result<string_segment_ref, error_msg> string_segment(
        magic_data_t const& magic) const
    {
        if(auto string_base_chunk = script.script_string_segment.data(magic);
           string_base_chunk.has_error())
            return string_base_chunk.error();
        else if(auto chunk = string_base_chunk.value(); true)
        {
            return string_segment_ref{
                .data = std::string_view(chunk.data(), chunk.size()),
            };
        }
    }

    inline Span<hsc::function_declaration const> function_table(
        magic_data_t const& magic) const
    {
        return script.scripts.data(magic).value();
    }

    struct
    {
        tagref_t custom_object_names; /*!< Points to ui::unicode_ref */
        tagref_t cutscene_titles;     /*!< Points to ui::unicode_ref*/
        tagref_t hud_text;            /*!< Points to ui::hud_message */
    } ui_text;

    reflexive_t<bsp::info> bsp_info;
};

template<typename V>
requires is_game_version<V>
inline scenario<V> const& get_scenario(
    file_header_t const* header, magic_data_t const& magic)
{
    return tag_index_t<V>::from_header(header)
        .scenario(header)
        .template to_reflexive<scenario<V>>()
        .data(magic)[0];
}

} // namespace blam::scn

namespace blam::ui {

struct unicode_ref
{
    u32                         length;
    u32                         padding;
    reflexive_t<unicode_var<1>> data;

    inline result<ucs_string, error_msg> str(
        magic_data_t const& magic, u16 off = 0) const
    {
        if(auto str_data = data.data(magic); str_data.has_error())
            return str_data.error();
        else
            return str_data.value()[0].str(off);
    }
};

struct unicode_string
{
    reflexive_t<unicode_ref> sub_strings;
};

struct hud_symbol
{
    bl_string symbol;
    u16       offset;
    u16       unknown;
    u32       padding[7];
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
    inline std::optional<u16> str_offset(
        magic_data_t const& magic, u32 idx) const
    {
        if(auto offset_data = offsets.data(magic); offset_data.has_error())
            return std::nullopt;
        else
        {
            u16 out = 0;
            for(auto i : stl_types::Range<>(idx))
            {
                out += offset_data.value()[i].size;
            }
            return out;
        }
    }

    inline std::optional<ucs_string> symbol_find(
        magic_data_t const& magic, std::string_view const& sym) const
    {
        auto symbol_res = symbols.data(magic);
        if(symbol_res.has_error())
            return std::nullopt;
        auto symbol_data = std::move(symbol_res.value());
        for(hud_symbol const& s : symbol_data)
        {
            if(s.symbol != sym)
                continue;

            if(auto symbol = text.data.data(magic); symbol.has_error())
                continue;
            else
                return symbol.value()[0].view(text.length, s.offset);
        }

        return std::nullopt;
    }
};

} // namespace blam::ui
