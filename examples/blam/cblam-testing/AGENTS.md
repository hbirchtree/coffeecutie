# cblam-testing — Agent Guide

A Halo CE map viewer/engine built on the Coffee engine, with split-screen rendering and networked multiplayer.

## Architecture

### Entity Component System (ECS)

The app uses `compo::EntityContainer` as the central ECS registry. Components and subsystems are registered in `graphics.cpp` during startup.

- **Components** are value types stored in `VectorContainer`. Each declares `using value_type` and `using type = compo::alloc::VectorContainer<value_type>`.
- **Subsystems** are singletons (extend `compo::SubsystemBase`), accessed via `e.subsystem_cast<T>()`.
- **RestrictedSubsystems** declare a `SubsystemManifest<Components, Subsystems, Services>` that controls which types they can access through their `Proxy`. They run `start_restricted`/`end_restricted` each frame.
- **Entities** are created with `e.create_entity(EntityRecipe{components, tags})` and can be tagged with `ObjectTags` bitmask flags (defined in `components.h`).
- **Entity iteration**: `e.select<ComponentType>()` iterates entities with a given component. `e.select(tag_mask)` iterates by tag.
- **ComponentRef<Container, T>**: a persistent handle to a component on an entity. Check `.m_ref` for validity, dereference with `(*ref)`.

### Key subsystems

| Subsystem        | File              | Purpose                                      |
|------------------|-------------------|----------------------------------------------|
| `GameEventBus`   | `data.h`          | Central event bus for game events             |
| `BlamCamera`     | `data.h`          | 8 viewports (0-3 split-screen, 4-7 remote)   |
| `NetworkState`   | `networking.h`    | Client/server connection state + player roster|
| `Networking`     | `networking.cpp`  | GameNetworkingSockets-based multiplayer       |
| `BlamMapBrowser` | `map_loader.h`    | ImGui UI for map selection and server browser |
| `LoadingStatus`  | `loading.h`       | Tracks async map/bitmap/sound loading progress|
| `BlamFiles`      | `blam_files.h`    | Holds loaded map container and file resources |
| Various caches   | `*_cache.h`       | Bitmap, shader, model, BSP, sound, UI caches  |

### Key components

| Component       | File            | Purpose                                  |
|-----------------|-----------------|------------------------------------------|
| `PlayerInfo`    | `components.h`  | Player name, remote address, index, loading progress |
| `NetworkInfo`   | `components.h`  | Network replication state for entities    |
| `Model`         | `components.h`  | 3D model transform, mesh data, visibility|
| `SubModel`      | `components.h`  | Individual sub-mesh within a model        |
| `BspReference`  | `components.h`  | BSP geometry draw commands                |
| `ShaderData`    | `components.h`  | Shader/material assignment for rendering  |
| `DebugDraw`     | `components.h`  | Debug visualization draw commands         |

### Entity tags (`ObjectTags` in `components.h`)

Tags are bitmask flags on entities used for filtering and lifecycle:
- `ObjectGC` — erased on map load (used for per-map entities)
- `PlayerBiped` — marks local player biped entities
- `ObjectScenery`, `ObjectVehicle`, `ObjectBiped`, etc. — object type classification
- `PositioningStatic/Dynamic/Background` — spatial positioning categories

## Game events (`data.h`)

Events flow through `GameEventBus` (a `BasicEventBus<GameEvent>`). Key events:

| Event                      | Data struct                | Purpose                           |
|----------------------------|----------------------------|-----------------------------------|
| `MapLoadStart`             | `MapLoadEvent`             | Triggers map loading (local/remote)|
| `MapDataLoad`              | `MapDataLoadEvent`         | Map file bytes ready              |
| `MapLoadFinished`          | `MapLoadFinishedEvent<V>`  | Map fully parsed, init systems    |
| `MapChanged`               | `MapChangedEvent<V>`       | Scenario data ready for caches    |
| `ServerConnect`            | `ServerConnectEvent`       | Initiate peer/server/listen       |
| `ServerConnected`          | `ServerConnectedEvent`     | Connection established            |
| `ServerCameraControl`      | `ServerCameraControl`      | Request camera focus on a player  |
| `ServerStateUpdate`        | `ServerStateUpdate`        | Player count, server name updates |
| `ServerPlayerStateUpdate`  | `ServerPlayerStateUpdate`  | Per-player state changes          |

Event handlers are registered with `gbus.addEventFunction<DataType>(priority, lambda)` or `gbus.addEventData({priority, lambda})` for raw forwarding.

## Networking (`networking.cpp` + `networking.h`)

### Protocol

Uses Valve's GameNetworkingSockets library. Communication is message-based with `MessageBase` header + typed payload.

**Message types** (`MessageBase::Type` enum):
- `GameJoin` — server sends map name + RNG seed to client
- `GameLoadState` — client reports loading progress to server
- `PlayerJoin` / `PlayerJoinConfirm` — client sends name, server confirms with assigned index
- `GameEvent` — wrapped game events forwarded over network
- `CameraSync` — per-frame camera position/rotation sync
- `EntitySpawn` — server requests client to spawn entities
- `Screenshot` — debug screenshot request
- `PlayerSync` — full player roster broadcast

**Message structs** are POD types with `static constexpr auto message_type`. `Message<T>` wraps `MessageBase` + `T data`. Multi-value messages use `MessageBase::Multiple` flag and `num_values` count.

**Wire format**: `sizeof()` assertions enforce struct sizes. Little-endian integers and IEEE-754 floats assumed. `blam::bl_string` is a 32-byte fixed-size string type used in network messages.

### Sending

- `send_all(Message<T>&&, flags, connections, lane)` — broadcast to all/selected connections
- `send_all(MessageBase&&, span<T>, ...)` — multi-value broadcast (header + array payload)
- `send_single(connection, Message<T>&&, flags)` — send to one connection
- Lane 0 = frame updates (unreliable), Lane 1 = events (reliable)

### Player index space

Local viewports (split-screen) use indices 0-3. Remote (network) players use indices 4+ via a monotonic counter `m_next_remote_idx`. This prevents overlap with split-screen rendering. `BlamCamera` has 8 viewports; `num_players()` only counts active ones so split-screen layout is unaffected.

### Server flow

1. `create_server()` — binds listen socket, generates host name via `get_random_name()`
2. `start_restricted()` — first frame: finds existing local `PlayerInfo` entities (created by map loading, idx 0-3), assigns random names, stores refs in `m_local_player_info`
3. On client connect: accepts connection, assigns idx from `m_next_remote_idx++`
4. On `PlayerJoin`: creates `PlayerInfo` entity for remote player, sends `PlayerJoinConfirm` with assigned idx, calls `send_player_roster()`
5. On disconnect: removes player entity, erases connection, calls `send_player_roster()`
6. Per-frame: receives messages on poll group, sends camera sync to all clients

### Client flow

1. `connect_server()` — connects to server IP
2. On `GameJoin`: loads requested map, sends `PlayerJoin` with random name
3. On `PlayerJoinConfirm`: stores assigned remote index in `NetworkState`
4. On `PlayerSync`: populates `NetworkState::player_roster` for UI display
5. On `CameraSync`: updates viewport 4 with server's camera data
6. Per-frame: receives messages, sends own camera sync to server

### NetworkState (`networking.h`)

Public state exposed to UI systems:
- `client_state` / `server_state` — connection lifecycle enums
- `remote_player_idx` — this client's server-assigned index
- `player_roster` — vector of `RosterEntry{name, remote_idx, loading_progress, is_self}`

## Map loading (`map_loading.cpp`)

1. `open_map()` — clears `ObjectGC` entities, parses map file asynchronously via `FileMapper`
2. `init_map()` — loads caches (bitmaps, BSPs, models, shaders, sounds), creates scenario entities, creates 4 local player biped entities (idx 0-3, tagged `ObjectGC | PlayerBiped`)
3. `setup_load_eventhandlers()` — registers `MapLoadEvent` and `MapLoadFinished` handlers on `GameEventBus`

Local player bipeds are always created as 4 entities with `PlayerInfo` + `NetworkInfo` + `SoundEffects` components.

## UI (`map_loader.h`)

`BlamMapBrowser` is a `RestrictedSubsystem` that renders the main ImGui game window with tabs:

- **Local** — map file browser, map info display, load button
- **Client** — server address input, connect button, connection state, player roster display, "Look at me!" camera focus button
- **Server** — listen address input, listen button, server state, connected player list with focus buttons per player

## File overview

| File                    | Role                                           |
|-------------------------|-------------------------------------------------|
| `main.cpp`              | Entry point, app setup                          |
| `graphics.cpp`          | Component/subsystem registration, graphics init |
| `data.h`                | Core data types: BlamCamera, GameEvent, event structs |
| `components.h`          | ECS component definitions                       |
| `selected_version.h`    | Compile-time Halo version selection (`pc_version_t`) |
| `networking.cpp`        | Networking implementation (server + client)      |
| `networking.h`          | NetworkState subsystem, `alloc_networking()`     |
| `map_loader.h`          | ImGui game browser UI                           |
| `map_loading.cpp`       | Map loading pipeline and entity creation         |
| `map_loading.h`         | `setup_load_eventhandlers()` declaration         |
| `rendering.cpp/h`       | Render pipeline and draw calls                   |
| `caching.cpp/h`         | Base cache infrastructure                        |
| `*_cache.h`             | Typed caches (bitmap, shader, model, BSP, etc.)  |
| `sounds.cpp/h`          | Sound system integration                         |
| `ui.cpp/h`              | Additional UI components                         |
| `graphics_api.h`        | Graphics API type aliases                        |

## Build

```sh
./cb build desktop:x86_64-buildroot-linux-gnu:multi
```

Networking support requires the `USE_NETWORKING` preprocessor define. Discord integration requires `FEATURE_ENABLE_DiscordLatte`.
