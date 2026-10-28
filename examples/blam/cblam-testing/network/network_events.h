#pragma once

/* Messages between BlamGraphics peers: a MessageBase header followed by
 * num_values values of one type. Little-endian, IEEE-754, fixed padding. */

#include "game_events.h"

#include <blam/common/tag_index.h>
#include <blam/volta/blam_base_types.h>
#include <blam/volta/blam_tag_classes.h>
#include <gsl/span>
#include <peripherals/enum/helpers.h>
#include <peripherals/typing/enum/pixels/format.h>
#include <peripherals/typing/vectors/glm_vector_types.h>

#include <limits>

namespace net_messages {

using libc_types::i16;
using libc_types::i64;
using libc_types::u16;
using libc_types::u32;
using libc_types::u64;
using pix_fmt = typing::pixels::pix_fmt;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf4;

template<typename T>
struct Message;

struct MessageBase
{
    enum Type : u32
    {
        None,

        /* Control */
        GameJoin,
        GameLoadState,
        PlayerJoin,
        PlayerJoinConfirm,
        GameEvent,
        GameLeave,

        UpdatePermission,

        /* Replication */
        CameraSync,
        EntitySpawn,
        PlayerSpawn,
        ObjectSync,

        /* Client-requested spawns */
        SpawnRequest,

        /* Debug */
        Screenshot,

        /* Roster */
        PlayerSync,

        /* Extension data */
        NegotiateExtension,

        /* Join-time data check */
        MapVerify,

        ClockSync,

        /* Vehicle seats */
        SeatSync,
        DriveInput,

        /* Combat */
        WeaponFire,
        ObjectHit,

        /* Forge */
        GrabInput,
        GrabState,

        /* Simulation clock */
        TickSync,
        TickInput,

        /* Physics */
        ObjectImpact,

        /* What players hold */
        EquipmentSync,
        WeaponPickup,
        ObjectDespawn,

        /* What units have left */
        UnitVitality,

        /* The game mode: its state, and every player's score */
        GameStateSync,
        GameScoreSync,

        /* Machines, controls and their groups */
        DeviceSync,

        /* Scripted scenarios: a point every peer waits at */
        SyncPoint,
    } type{None};

    u32 request{};

    enum Flags : u32
    {
        NoFlags   = 0x0,
        RPC       = 0x1,
        Multiple  = 0x2,
        Replicate = 0x4,
        Enforce   = 0x8,
    } flags{NoFlags};

    u32 num_values{1};

    template<typename T>
    T const& value() const;

    template<typename T>
    gsl::span<const T> values() const;
};

C_FLAGS(MessageBase::Flags, u32)

template<typename T>
struct Message : MessageBase
{
    Message(T&& data)
        : MessageBase{T::message_type}
        , data(std::move(data))
    {
    }

    T data;
};

template<typename T>
T const& MessageBase::value() const
{
    Message<T> const* impl = static_cast<Message<T> const*>(this);
    return impl->data;
}

template<typename T>
gsl::span<const T> MessageBase::values() const
{
    Message<T> const* impl = static_cast<Message<T> const*>(this);
    return gsl::make_span(&impl->data, num_values);
}

/*! Identifies the tag data a peer has loaded. Tag ids and object classes
 *  are only meaningful across peers when these agree. */
struct MapFingerprint
{
    u32 tag_hash[2]{}; /*!< FNV-1a 64 over (classes, id, name) of every tag */
    u32 tag_count{0};
    u32 base_tag{0};

    bool operator==(MapFingerprint const&) const = default;
};

struct GameJoin
{
    static constexpr auto message_type = MessageBase::GameJoin;

    blam::bl_string map_name;
    u32             seed{0};
    MapFingerprint  fingerprint{};
};

/*! Client -> server once loaded; the server answers a mismatch with
 *  GameLeave, and a match with the dynamic objects spawned so far */
struct MapVerify
{
    static constexpr auto message_type = MessageBase::MapVerify;

    MapFingerprint fingerprint{};
};

struct GameLoadState
{
    static constexpr auto message_type = MessageBase::GameLoadState;

    u32 progress{0};
};

struct GameLeave
{
    static constexpr auto message_type = MessageBase::GameLeave;

    blam::bl_string_var<128> reason;
};

struct UpdatePermission
{
    static constexpr auto message_type = MessageBase::UpdatePermission;

    u32 player_idx{};

    enum Permission : libc_types::u16
    {
        Movement,
        Camera,
        Action,
    } permission;

    u16 mode{0};
};

struct PlayerJoin
{
    static constexpr auto message_type = MessageBase::PlayerJoin;

    blam::bl_string player_name;
    u32             seat{0}; /*!< Which of the client's own seats */
};

struct PlayerJoinConfirm
{
    static constexpr auto message_type = MessageBase::PlayerJoinConfirm;

    u32 player_idx{0};
    u32 seat{0};
};

template<typename T>
struct GameEventWrapper
{
    static constexpr auto message_type = MessageBase::GameEvent;

    GameEvent event;
    T         data;
};

struct alignas(8) CameraSync
{
    static constexpr auto message_type = MessageBase::CameraSync;
    static constexpr u32  self_id      = 0xFFFF;

    enum flags_t : u32
    {
        none      = 0x0,
        physics   = 0x1, /*!< PlayerInfo::mode.physics */
        simulated = 0x2, /*!< PlayerInfo::mode.simulated */
    };

    Vecf4   position; /*!< In the deck's frame when standing on one */
    Quatf   rotation;
    u32     target_player{0xFFFF};
    f32     fade{1.f};
    flags_t flags{none};
    u32     deck{0}; /*!< Net id of the vehicle stood on, 0 on the ground */
    u64     tick{0}; /*!< Server tick it was there at, 0 from clients */
};

/*! A player's vehicle seat; sent on change and to newly verified peers */
struct alignas(8) SeatSync
{
    static constexpr auto message_type = MessageBase::SeatSync;

    enum flags_t : u32
    {
        none    = 0x0,
        driver  = 0x1,
        exiting = 0x2, /*!< Playing the seat's exit */
    };

    u32     player_idx{0};
    u32     vehicle{0}; /*!< The vehicle's net id, 0 on foot */
    i16     seat{-1};
    u16     padding{0};
    flags_t flags{none};

    bool operator==(SeatSync const&) const = default;
};

/*! What a player holds; sent on change and to newly verified peers */
struct alignas(8) EquipmentSync
{
    static constexpr auto message_type = MessageBase::EquipmentSync;

    enum flags_t : u32
    {
        none      = 0x0,
        reloading = 0x1,
        throwing  = 0x2,
    };

    u32                player_idx{0};
    blam::tag_index<>  weapon{}; /*!< Tag ids, 0 for none */
    flags_t            flags{none};
    blam::tag_index<>  slung{};
    std::array<libc_types::u8, 4> grenades{}; /*!< Of each kind the globals list */

    bool operator==(EquipmentSync const&) const = default;
};

/*! The server gave a client's player a weapon off the ground */
struct alignas(8) WeaponPickup
{
    static constexpr auto message_type = MessageBase::WeaponPickup;

    u32               player_idx{0};
    blam::tag_index<> weapon{};
    f32               age{0.f}; /*!< Its battery, as it lay */
};

/*! A client reached a sync point, or the server released it for all */
struct SyncPoint
{
    static constexpr auto message_type = MessageBase::SyncPoint;

    std::array<char, 60> name{};
    u32                  released{0};
};

/*! An object the server removed */
struct ObjectDespawn
{
    static constexpr auto message_type = MessageBase::ObjectDespawn;

    u32 net_id{0};
};

/*! A driving client's input, sent to the server every frame */
struct alignas(8) DriveInput
{
    static constexpr auto message_type = MessageBase::DriveInput;

    Vecf4 aim;
    u32   vehicle{0}; /*!< The vehicle's net id */
    f32   throttle{0.f};
    f32   steer{0.f};
    u32   padding{0};
};

/*! A trigger pulled (WeaponFireEvent): the client's own shots to the
 *  server, which relays them with the shooter's index to everyone else */
struct alignas(8) WeaponFire
{
    static constexpr auto message_type = MessageBase::WeaponFire;

    u64   view_tick{0}; /*!< Server tick the shooter saw others at */
    Vecf4 position;
    Vecf4 direction;
    blam::tag_index<> weapon{}; /*!< weap tag id */
    u32   player_idx{0};
    u32   vehicle{0}; /*!< The vehicle's net id, 0 = held */
    u32   seed{0};
    u16   trigger{0};
    enum flags_t : u16
    {
        none   = 0x0,
        thrown = 0x1, /*!< A grenade, `weapon` its proj */
        melee  = 0x2, /*!< A strike with `weapon` */
    } flags{none};
    u32 target{0};                 /*!< Net id of the unit aimed at, when not a player */
    u32 target_player{0xFFFFFFFF}; /*!< Its player index when it is */
    u32 shooter{0};                /*!< Net id of a shooter that is no player: an actor */
};

/*! A hit the server decided (ObjectHitEvent), to every peer */
struct alignas(8) ObjectHit
{
    static constexpr auto message_type = MessageBase::ObjectHit;

    Vecf4 point;
    Vecf4 normal;
    Vecf4 direction;
    blam::tag_index<> projectile{}; /*!< proj tag id, or the jpt! of a strike */
    u32   target{0};     /*!< Net id, when the target is not a player */
    u32   target_player{0};
    u32   shooter_player{0};
    i16   part{-1};
    libc_types::u8 head{0};
    libc_types::u8 hitscan{0};
    u32   padding{0};
};

/*! A unit's body and shield as the server has them (UnitVitalityEvent):
 *  each change, and every hurt or dead unit to a fresh peer */
struct alignas(8) UnitVitality
{
    static constexpr auto message_type = MessageBase::UnitVitality;

    u32 target{0}; /*!< Net id, when the unit is not a player */
    u32 target_player{0};
    f32 body{1.f};
    f32 shield{1.f};
    u32 dead{0};
    u32 respawned{0}; /*!< Brought back at `respawn`, facing its w; 2: with the respawn effect, 4: re-equipped */
    Vecf4 respawn{};
};

/*! The game being played, as the server keeps it (GameState) */
struct alignas(8) GameStateSync
{
    static constexpr auto message_type = MessageBase::GameStateSync;

    libc_types::u8 kind{0};
    libc_types::u8 phase{0};
    libc_types::u8 game_type{0};
    libc_types::u8 teams{0};
    libc_types::i32 winner{-1};
    f32             time_left{-1.f}; /*!< Whole seconds */
    i16             team_scores[2]{};
    u16             difficulty{1};
    u16             padding{0};
    u32             saves{0};
    u32             reverts{0};

    bool operator==(GameStateSync const&) const = default;
};

/*! One player's score; the whole table goes out when any of it changes */
struct alignas(8) GameScoreSync
{
    static constexpr auto message_type = MessageBase::GameScoreSync;

    u32 player_idx{0};
    i16 team{-1};
    i16 score{0};
    i16 kills{0};
    i16 deaths{0};

    bool operator==(GameScoreSync const&) const = default;
};

/*! Where the server tells a device (by net id) or a device group to be:
 *  each change, and all of them to a fresh peer */
struct alignas(8) DeviceSync
{
    static constexpr auto message_type = MessageBase::DeviceSync;

    u32 net_id{0};
    i16 group{-1}; /*!< A group's value, when not a device */
    u16 padding{0};
    f32 value{0.f};
    f32 position_target{0.f};
    f32 power_target{1.f};
    u32 snap{0};

    bool operator==(DeviceSync const&) const = default;
};

/*! A client's forge grab, carried out on the server: every frame while
 *  held, and once more as it lets go */
struct alignas(8) GrabInput
{
    static constexpr auto message_type = MessageBase::GrabInput;

    Vecf4 origin;
    Quatf to_world; /*!< Camera to world; the view is its -Z */
    u32   held{0};
    u32   player_idx{0}; /*!< One of the sender's own */
};

/*! What the server has a client carrying, as it changes */
struct alignas(8) GrabState
{
    static constexpr auto message_type = MessageBase::GrabState;

    u32 object{0}; /*!< Net id, 0 = nothing */
    u32 player_idx{0};
};

/*! The server's tick clock, which clients follow: tick `tick` began at
 *  server time `start_us`, and none follow while paused */
struct alignas(8) TickSync
{
    static constexpr auto message_type = MessageBase::TickSync;

    u64 tick{0};
    i64 start_us{0};
    i64 session_us{0}; /*!< When the server began listening */
    u64 pause_at{0};   /*!< Last tick before a pause, 0 = none */
    u32 paused{0};
    u32 speed_permille{1000}; /*!< A slowed server's ticks per tick of time */
};

/*! A client's input for one tick (PlayerTickInputEvent). Each packet
 *  repeats the last few, so a lost one costs nothing */
struct alignas(8) TickInput
{
    static constexpr auto message_type = MessageBase::TickInput;

    u64   tick{0};
    Quatf view;
    typing::vector_types::Vecf2 move;
    typing::vector_types::Vecf2 look;
    InputIntent intents{InputIntent::none};

    enum class flags_t : u32
    {
        none   = 0x0,
        resync = 0x1, /*!< Skipped ahead to here, the gap was never played */
    } flags{flags_t::none};
    u32 player_idx{0}; /*!< One of the sender's own */
    u32 padding{0};
};

C_FLAGS(TickInput::flags_t, u32)

/*! An object striking something hard enough to hear, on the server; a
 *  client's copies only follow, so they never collide themselves */
struct alignas(8) ObjectImpact
{
    static constexpr auto message_type = MessageBase::ObjectImpact;

    Vecf4 point;
    u32   object{0}; /*!< Net id */
    u32   other{0};  /*!< Net id, 0 = the world or an object not replicated */
    blam::tag_index<> shader{}; /*!< The world surface's shader tag id, 0 = none */
    f32   speed{0.f};
};

/*! A moving object at a server time. Sent while it is awake, once more as
 *  it falls asleep, and all of them to a peer once it is verified. */
struct alignas(8) ObjectSync
{
    static constexpr auto message_type = MessageBase::ObjectSync;

    enum flags_t : u32
    {
        none     = 0x0,
        sleeping = 0x1,
    };

    u64     tick{0}; /*!< Server tick this is the state after */
    Quatf   rotation;
    Vecf4   position; /*!< w unused, as are the velocities' */
    Vecf4   linear_velocity;
    Vecf4   angular_velocity;
    u32     net_id{0};
    flags_t flags{none};
};

struct alignas(8) Screenshot
{
    static constexpr auto message_type = MessageBase::Screenshot;

    pix_fmt format{pix_fmt::None};
};

/*! NTP-style exchange, in each side's GNS local timestamps. The server's
 *  hold time (send - recv) drops out of the round trip. server_base maps the
 *  server's GNS time onto its steady_clock. */
struct alignas(8) ClockSync
{
    static constexpr auto message_type = MessageBase::ClockSync;

    i64 client_send{0};
    i64 server_recv{0};
    i64 server_send{0};
    i64 server_base{0};
};

/* Tag class + id only; a tagref_t's name is a pointer into the sender's
 * map. The receiver resolves the id against its own (verified) tag index. */
struct EntityTag
{
    blam::tag_class_t tag_class{blam::tag_class_t::none};
    blam::tag_index<> tag_id{blam::tag_index<>::none()};
    u32               instance_id{0}; /*!< Network id, same on all peers */
};

/* Server-side entity spawn, possibly as a response */
struct alignas(8) EntitySpawn
{
    static constexpr auto message_type = MessageBase::EntitySpawn;

    static constexpr auto no_request = std::numeric_limits<u32>::max();

    Vecf4     position{};
    Quatf     rotation{1.f, 0.f, 0.f, 0.f};
    EntityTag tag{};
    i32       permutation{-1};
    u32       response_id{no_request};
    u32       announce{0}; /*!< See SpawnObjectEvent::announce */
    blam::tag_index<> held{}; /*!< A unit's weapon, weap tag id */
};

struct alignas(8) PlayerSpawn
{
    static constexpr auto message_type = MessageBase::PlayerSpawn;

    Vecf4 position;
    u32   target_player{0xFFFF}; // Targets local players
    f32   facing;
};

/*! Client asks the server to spawn an object it has put up an impostor for.
 *  The server answers the requester with an EntitySpawn carrying
 *  response_id: instance_id 0 means rejected. */
struct alignas(8) SpawnRequest
{
    static constexpr auto message_type = MessageBase::SpawnRequest;

    Vecf4     position{};
    Quatf     direction{};
    EntityTag tag{};
    u32       request_id{EntitySpawn::no_request};
    u32       announce{0};
    f32       age{0.f}; /*!< See SpawnObjectEvent::age */
};

struct alignas(8) PlayerSyncEntry
{
    static constexpr auto message_type = MessageBase::PlayerSync;

    blam::bl_string name;
    u32             player_idx{0};
    u32             loading_progress{100};
    u32             connected{0x0};
    u32             spawned{0x0};
};

static_assert(sizeof(PlayerSyncEntry) == 48);

struct alignas(8) NegotiateExtension
{
    static constexpr auto message_type = MessageBase::NegotiateExtension;

    blam::bl_string_var<65> hash; /*!< Hash of extension data
                                   * Ensures we'll be in sync */

    enum extension_type_t
    {
        None = 0,
        RS2  = 1,
    } type;

    union
    {
        struct
        {
            /* dunno what to stuff here yet */
        } rs2;

        u64 raw[10];
    } data;
};

/*
 * Some ground rules for networking:
 *  - We expect little-endian integers
 *  - We expect IEEE-754 floats/doubles
 *  - Padding should be the same on all platforms
 */
static_assert(sizeof(Message<GameJoin>) == 68);
static_assert(sizeof(Message<MapVerify>) == 32);
static_assert(sizeof(EntityTag) == 12);
static_assert(sizeof(EntitySpawn) == 64);
static_assert(sizeof(SpawnRequest) == 56);
static_assert(sizeof(Message<ClockSync>) == 48);
static_assert(sizeof(Message<PlayerJoin>) == 52);
static_assert(sizeof(Message<CameraSync>) == 72);
static_assert(sizeof(ObjectSync) == 80);
static_assert(sizeof(SeatSync) == 16);
static_assert(sizeof(EquipmentSync) == 24);
static_assert(sizeof(WeaponPickup) == 16);
static_assert(sizeof(ObjectDespawn) == 4);
static_assert(sizeof(SyncPoint) == 64);
static_assert(sizeof(DriveInput) == 32);
static_assert(sizeof(WeaponFire) == 72);
static_assert(sizeof(ObjectHit) == 72);
static_assert(sizeof(GrabInput) == 40);
static_assert(sizeof(GrabState) == 8);
static_assert(sizeof(TickSync) == 40);
static_assert(sizeof(TickInput) == 56);
static_assert(sizeof(ObjectImpact) == 32);
static_assert(sizeof(Message<u32>) == 20);
static_assert(sizeof(Message<u64>) == 24);

} // namespace net_messages
