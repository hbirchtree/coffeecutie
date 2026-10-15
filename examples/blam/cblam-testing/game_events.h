#pragma once

/* Events on the game bus (GameEventBus). Their types are appended, not
 * reordered: GameEvent::EventType values are sent over the wire. */

#include <array>
#include <chrono>
#include <limits>
#include <optional>
#include <string>
#include <vector>

#include <blam/volta/blam_mod2.h>
#include <blam/volta/blam_stl.h>
#include <blam/volta/blam_tag_classes.h>
#include <blam/volta/blam_tag_ref.h>
#include <coffee/comp_app/file_mapper.h>
#include <coffee/comp_app/services.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/typing/vectors/glm_vector_types.h>
#include <url/url.h>

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using libc_types::u64;
using typing::vector_types::Quatf;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;

struct BSPItem;

struct GameEvent
{
    enum EventType
    {
        None,
        MapLoadByName,
        MapLoadStart,
        MapRequestListing,
        MapListing,
        MapDataLoad,
        MapLoadFinished,
        MapDataReady,
        MapAllLoaded,
        MapChanged,

        ClusterChanged,
        SkyboxChanged,
        WeatherChanged,

        // Resource loading
        MountModel, // SpawnModel, but to an existing entity
        SpawnBiped,
        SpawnBSP,
        SpawnEquipment,
        SpawnModel,

        PlayModelAnimation,

        PlayerTeleport,
        PlayerCameraLerp,

        ServerConnect,
        ServerConnected,
        ServerCameraControl,
        ServerDisconnect,
        ServerJoinInfo,
        ServerStateUpdate,
        ServerPlayerStateUpdate,

        /* Appended rather than grouped with the Spawn* events: event types
         * are sent over the wire in GameEventWrapper */
        SpawnObject,
        DespawnObject,
        UnitEnterVehicle,
        UnitExitVehicle,
        ServerDisconnected,
        SpawnEffect,
        WeaponFire,
        ObjectHit,
    };

    EventType type{None};
};

struct MapLoadEvent
{
    static constexpr auto event_type = GameEvent::MapLoadStart;

    enum Origin
    {
        Local,
        Remote,
    } origin{Local};

    std::optional<platform::url::Url> directory{};
    std::optional<platform::url::Url> file{};
    /*! The file as already fetched by a loader that only sniffed it; the
     * next loader takes it instead of fetching again */
    std::shared_ptr<comp_app::FileMapper::Resource> file_data{};
};

struct MapLoadByNameEvent
{
    static constexpr auto event_type = GameEvent::MapLoadByName;

    MapLoadEvent::Origin origin{MapLoadEvent::Local};
    blam::bl_string      map_name;
};

struct MapRequestListingEvent
{
    static constexpr auto event_type = GameEvent::MapRequestListing;
};

struct MapListingEvent
{
    static constexpr auto event_type = GameEvent::MapListing;

    platform::url::Url              directory;
    std::vector<platform::url::Url> maps;
    platform::url::Url              bitmap_file;
    platform::url::Url              sound_file;
};

struct MapDataLoadEvent
{
    static constexpr auto event_type = GameEvent::MapDataLoad;

    semantic::BytesConst map;
};

template<typename V>
struct MapLoadFinishedEvent
{
    static constexpr auto event_type = GameEvent::MapLoadFinished;

    blam::map_container<V>*      container;
    std::string                  map_name{};
    std::string                  map_title{};
    std::optional<blam::map_ptr> bitmaps{};
};

struct MapDataReadyEvent
{
    static constexpr auto event_type = GameEvent::MapDataReady;
};

template<typename V>
struct MapChangedEvent
{
    static constexpr auto event_type = GameEvent::MapChanged;

    blam::map_container<V>&       container;
    blam::scn::scenario<V> const* scenario{nullptr};
};

struct ClusterChangedEvent
{
    static constexpr auto event_type = GameEvent::ClusterChanged;

    BSPItem const* bsp{nullptr};
    u32            cluster{std::numeric_limits<u32>::max()};
};

struct SkyboxChangedEvent
{
    static constexpr auto event_type = GameEvent::SkyboxChanged;
};

struct WeatherChangedEvent
{
    static constexpr auto event_type = GameEvent::WeatherChanged;
};

struct MountModelEvent
{
    static constexpr auto event_type = GameEvent::MountModel;
    blam::tagref_typed_t<blam::tag_class_t::mod2> model;
    u64                                           entity_id{0};
    bool                                          first_person{false};
};

struct SpawnBipedEvent
{
    static constexpr auto event_type = GameEvent::SpawnBiped;
    blam::tagref_typed_t<blam::tag_class_t::bipd> biped;
    u32                                           player_idx{0};
    u32                                           tags{};
};

struct SpawnBSPEvent
{
    static constexpr auto event_type = GameEvent::SpawnBSP;
    u32                   section_id{};
};

struct SpawnEquipmentEvent
{
    static constexpr auto event_type = GameEvent::SpawnEquipment;
};

struct SpawnModelEvent
{
    static constexpr auto event_type = GameEvent::SpawnModel;
    blam::tagref_typed_t<blam::tag_class_t::mod2> model;

    // TODO: Add param for which variant, eg. marine variant
    blam::mod2::mod2_lod max_lod_level{};
};

/*! Where a scenario-placed object sits in the scenario */
enum class ScenarioGroup : libc_types::u8
{
    None,
    /* Static, loaded by every peer */
    Scenery,
    LightFixture,
    Machine,
    Control,
    /* Dynamic, spawned by the server */
    Vehicle,
    Biped,
    Equipment,
    Weapon,
    NetgameEquipment,
};

/*!
 * \brief Spawn any object tag (obje subclass: bipd, vehi, weap, eqip, scen,
 * mach, ctrl, lifi, garb, ...) into the world.
 *
 * Same event for local and replicated spawns. On a server, Networking stamps
 * net_id on Local spawns and replicates them; Remote spawns came from the
 * server and are never sent back. net_id == 0 means local-only.
 */
struct SpawnObjectEvent
{
    static constexpr auto event_type = GameEvent::SpawnObject;

    /* Server-assigned ids live above this. Below it, the id addresses a
     * scenario instance, which every (verified) peer resolves from its own
     * map: group << 24 | index << 8 | sub */
    static constexpr libc_types::u32 dynamic_net_id_base = 0x80000000;

    static constexpr libc_types::u32 scenario_net_id(
        ScenarioGroup group, libc_types::u32 index, libc_types::u32 sub = 0)
    {
        return (static_cast<libc_types::u32>(group) << 24) |
               ((index & 0xFFFF) << 8) | (sub & 0xFF);
    }

    /* A client's stand-in for an object it asked the server to spawn,
     * removed once the server answers: impostor base | request id */
    static constexpr libc_types::u32 impostor_net_id_base = 0xF0000000;

    static constexpr bool is_impostor(libc_types::u32 net_id)
    {
        return net_id >= impostor_net_id_base;
    }

    /* What a client may ask the server for; anything else is rejected */
    static constexpr std::array<blam::tag_class_t, 4> client_requestable = {{
        blam::tag_class_t::proj,
        blam::tag_class_t::weap,
        blam::tag_class_t::eqip,
        blam::tag_class_t::garb,
    }};

    static bool is_client_requestable(blam::tag_t const& tag)
    {
        for(auto cls : client_requestable)
            if(tag.matches(cls))
                return true;
        return false;
    }

    static constexpr ScenarioGroup scenario_group(libc_types::u32 net_id)
    {
        if(net_id >= dynamic_net_id_base)
            return ScenarioGroup::None;
        return static_cast<ScenarioGroup>(net_id >> 24);
    }

    MapLoadEvent::Origin origin{MapLoadEvent::Local};
    blam::tagref_t       object{};
    Vecf3                position{};
    Quatf                rotation{1.f, 0.f, 0.f, 0.f};
    libc_types::u32      net_id{0};

    /* Only the server creates these. A replica defers scenario objects, and
     * for anything else spawns an impostor and asks the server for it */
    bool server_owned{false};
    /* Set on a replica for server_owned requests, which are then dropped */
    bool deferred{false};

    /* Created by an effect, which every peer plays itself: never replicated */
    bool            local_only{false};
    libc_types::u64 owner{0};   /*!< Projectiles: the body they leave through */
    libc_types::u64 shooter{0}; /*!< Projectiles: the player who fired them */
    Vecf3           velocity{};         /*!< wu/s, bodies with mass */
    Vecf3           angular_velocity{}; /*!< rad/s */
    libc_types::f32 lifetime{0.f};      /*!< seconds, 0 = until despawned */
};

struct DespawnObjectEvent
{
    static constexpr auto event_type = GameEvent::DespawnObject;

    libc_types::u32 net_id{0};
};

/*! Plays an effe (or a lone snd!) at a point, or on an entity's markers.
 *  Local only: every peer plays the effects of what it simulates. */
struct SpawnEffectEvent
{
    static constexpr auto event_type = GameEvent::SpawnEffect;

    blam::tagref_t  effect{};
    Vecf3           position{};
    Vecf3           direction{1.f, 0.f, 0.f}; /*!< Forward, e.g. a surface normal */
    Vecf3           velocity{};               /*!< Inherited by what it creates */
    libc_types::u64 parent{0};                /*!< Followed while it plays */
};

/*! A trigger pulled. Raised Local by the shooter's peer, which Networking
 *  relays; every peer plays the projectiles and the firing effect itself */
struct WeaponFireEvent
{
    static constexpr auto event_type = GameEvent::WeaponFire;

    MapLoadEvent::Origin origin{MapLoadEvent::Local};
    blam::tagref_t       weapon{};  /*!< weap */
    libc_types::u64      shooter{0}; /*!< Player entity */
    libc_types::u64      vehicle{0}; /*!< Whose weapon it is, 0 = held */
    libc_types::u32      player_idx{0};
    libc_types::u32      seed{0}; /*!< Spread, the same on every peer */
    libc_types::u16      trigger{0};
    Vecf3                position{};           /*!< Where projectiles leave */
    Vecf3                direction{1.f, 0.f, 0.f}; /*!< Aim, before spread */
    /*! When the shooter saw what it aimed at; the authority traces bipeds
     *  back there. Steady clock, server time on the server */
    std::optional<std::chrono::steady_clock::time_point> view_time{};
};

/*! A shot struck an object or a biped, as the authority (the server, or a
 *  peer on its own) decides. Every peer gets each hit once, replicas as
 *  Remote: what shields, health and damage go by */
struct ObjectHitEvent
{
    static constexpr auto event_type = GameEvent::ObjectHit;
    static constexpr libc_types::u32 no_player = 0xFFFFFFFF;

    MapLoadEvent::Origin origin{MapLoadEvent::Local};
    blam::tagref_t       projectile{}; /*!< proj */
    blam::tagref_t       damage{};     /*!< jpt!, its impact damage */
    libc_types::u64      target{0};    /*!< Entity struck */
    libc_types::u64      shooter{0};   /*!< Player entity, 0 if unknown */
    libc_types::u32      target_player{no_player};
    libc_types::u32      shooter_player{no_player};
    Vecf3                point{};
    Vecf3                normal{};
    Vecf3                direction{}; /*!< Of travel */
    libc_types::i16      part{-1};    /*!< Collision node, -1 if unknown */
    bool                 head{false};
    bool                 hitscan{false}; /*!< Resolved as the trigger was pulled */
};

/*! A unit takes a seat in a vehicle, as the unit_enter_vehicle script
 *  function and a player's use button do. Seats are looked up by label as
 *  scripts name them; an empty label takes the first driver's seat. */
struct UnitEnterVehicleEvent
{
    static constexpr auto event_type = GameEvent::UnitEnterVehicle;

    libc_types::u64 unit{0};    /*!< Player entity */
    libc_types::u64 vehicle{0}; /*!< Vehicle entity */
    blam::bl_string seat{};
};

/*! The unit_exit_vehicle script function, or a rider pressing use */
struct UnitExitVehicleEvent
{
    static constexpr auto event_type = GameEvent::UnitExitVehicle;

    libc_types::u64 unit{0};
};

/*! Plays an animation from an entity's own graph, as custom_animation does
 *  for scripts. Composes by the animation's own type: a base one crossfades
 *  in and holds off the unit's own movement until it ends, a replacement
 *  plays in the action slot, an overlay on top. An empty name with no index
 *  fades the entity's custom animations out. */
struct PlayModelAnimationEvent
{
    static constexpr auto event_type = GameEvent::PlayModelAnimation;

    libc_types::u64 entity{0};
    std::string     name{};       /*!< e.g. "stand rifle melee" */
    libc_types::i32 animation{-1}; /*!< Index into the graph, wins over name */

    bool            loop{false};
    libc_types::f32 fade{.2f}; /*!< Seconds to blend in, 0 = cut */
    libc_types::f32 rate{1.f};
    libc_types::f32 weight{1.f};
};

struct PlayerTeleportEvent
{
    static constexpr auto event_type = GameEvent::PlayerTeleport;

    libc_types::u64      entity_id{0};
    libc_types::u32      seat_idx{0};
    Vecf3                position{};
    std::optional<Quatf> rotation{};
};

struct PlayerCameraLerpEvent
{
    static constexpr auto event_type = GameEvent::PlayerCameraLerp;

    libc_types::u32           seat_idx{0};
    Vecf3                     position{};
    std::optional<Quatf>      rotation{};
    std::chrono::milliseconds duration{};
};

struct ServerConnectEvent
{
    static constexpr auto event_type = GameEvent::ServerConnect;

    enum ConnectType : libc_types::u32
    {
        Peer,
        Server,
        Listen,
    } type{Server};

    std::string remote;

    /* Only used with type == Listen: if set, additionally register this
     * server with a webrtc-gateway fleet registry so browser clients can
     * reach it (see gateway_fleet_registration.h). Empty = don't
     * register. */
    std::string gateway_register_url;
    std::string gateway_server_id;

    /* Only used with type == Listen: the server's Ed25519 private key PEM
     * path, which signs its certificate and gateway metadata. Empty = a key
     * generated in memory for this run. */
    std::string gateway_auth_key;

    /* Only used with type == Server, and only for a remote that is a plain
     * address rather than a gateway join URL (which carries the key in its
     * fragment): the server's Ed25519 public key, base64. When set, the
     * connection requires a certificate signed by it. */
    std::string server_public_key;

    /* Never use a direct route, in either role, both as server and client */
    bool relay_only{false};
};

struct ServerConnectedEvent
{
    static constexpr auto event_type = GameEvent::ServerConnected;

    std::string     remote;
    libc_types::u32 seed{0}; /* RNG seed, to keep systems in sync */
};

struct ServerCameraControl
{
    static constexpr auto event_type = GameEvent::ServerCameraControl;

    enum RequestType : libc_types::u32
    {
        None,
        RequestCameraFocus,
        OverrideCamera,
    } request{None};

    libc_types::u32             target_player{0xFFFF};
    typing::vector_types::Vecf4 position{};
    Quatf                       rotation{};
};

struct ServerDisconnectEvent
{
    static constexpr auto event_type = GameEvent::ServerDisconnect;
};

/* The server went away; only host_left is a clean close */
struct ServerDisconnectedEvent
{
    static constexpr auto event_type = GameEvent::ServerDisconnected;

    enum reason_t : libc_types::u8
    {
        host_left,
        join_failed,     /*!< Never got in */
        connection_lost, /*!< Timed out while connected */
        closed,          /*!< Closed by the peer for another reason */
    } reason{closed};
    std::string detail; /*!< GNS's end reason text */
};

struct ServerJoinInfo
{
    static constexpr auto event_type = GameEvent::ServerJoinInfo;

    blam::bl_string_var<128> server_id; /*!< ID part of URL passed to gateway */
    blam::bl_string_var<64>  secret;    /*!< Secret for connect, if needed */
};

struct ServerStateUpdate
{
    static constexpr auto event_type = GameEvent::ServerStateUpdate;

    enum StateField : libc_types::u32
    {
        None,
        ServerName,
        PlayerCount,
        PlayerMaxCount,
        ServerJoinable,
    } type{None};

    union
    {
        blam::bl_string string_field;
        libc_types::i32 num_field;
    };
};

struct ServerPlayerStateUpdate
{
    static constexpr auto event_type = GameEvent::ServerPlayerStateUpdate;

    enum StateField : libc_types::u32
    {
        None,
        Name,
    } type{None};

    libc_types::u32 index{0};

    union
    {
        blam::bl_string string_field;
        libc_types::i32 num_field;
    };
};

using GameEventBus = comp_app::BasicEventBus<GameEvent>;
