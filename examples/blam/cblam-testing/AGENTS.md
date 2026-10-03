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

### Threading

`SoundSystem` (`sounds.cpp`) is the one subsystem that opts in to running on a worker thread (`parallel_safe()`), so it is the reference for what that costs:

- Its manifest names everything it touches, including `SoundCache<Ver>` — which it writes, and which keeps `SoundUISystem` out of the same stage.
- `inject()` copies the event and hands it to the audio thread at the next frame hook; `process()` remains the same-thread path. Raise sound events with `inject()`.
- The `ClusterChanged` subscription is a queued one (`addQueuedEventFunction`), polled at the top of `start_restricted`, because the occluder raises it from the main thread.

`Occluder` is the second one, and it is the interesting case: its window reaches back to the batch holding `sdl2::GLFramebuffer`, so culling runs while the main thread is blocked in `SDL_GL_SwapWindow`. Two things make that legal:

- `parallel_safe()` returns `!m_markers_scheduled`. The only GPU work it does is mapping the debug marker buffer, and the flag it answers from is the one the *schedule* was built from — turning markers on takes effect the next frame, once the occluder is back on the main thread.
- `ClusterChanged` is raised from that worker, so `MeshRenderer` takes it through a queue (`m_cluster_events`) instead of having its `m_pending_changes` written from another thread.

Enable it with `COFFEE_ECS_THREADS=2` (see the root `AGENTS.md` for the rules); without it everything runs serially as before. `COFFEE_ECS_SCHEDULE=300` prints the resulting batches and offload windows.

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
- `PlayerSync` — full player roster broadcast (with each player's spawned state)

**Message structs** are POD types with `static constexpr auto message_type`. `Message<T>` wraps `MessageBase` + `T data`. Multi-value messages use `MessageBase::Multiple` flag and `num_values` count.

**Wire format**: `sizeof()` assertions enforce struct sizes. Little-endian integers and IEEE-754 floats assumed. `blam::bl_string` is a 32-byte fixed-size string type used in network messages.

### Sending

- `send_all(Message<T>&&, flags, connections, lane)` — broadcast to all/selected connections
- `send_all(MessageBase&&, span<T>, ...)` — multi-value broadcast (header + array payload)
- `send_single(connection, Message<T>&&, flags)` — send to one connection
- Lane 0 = frame updates (unreliable), Lane 1 = events (reliable)

### Player index space

`player_idx` is a player's network identity; `seat_idx` is the local split-screen seat. On the server, local seats use indices 0-3 and remote players 4+ via a monotonic counter `m_next_remote_idx`. On a client, seat 0 takes the index the server assigns (`PlayerJoinConfirm`), and every other local seat moves to `PlayerInfo::local_only_idx_base + seat` (0x10000+), which the server never hands out — otherwise the server's own split-screen players (idx 1-3) would land on the client's seats. `BlamCamera` has 8 viewports; `num_players()` only counts active ones so split-screen layout is unaffected.

A client only ever moves its own player: the server applies a client's `CameraSync` to that connection's player regardless of the index it names, ignores it while the player is held, and relays it to the other verified clients.

### Bipeds

A player has a biped — a mounted model plus a collision capsule — exactly while `biped_in_play()` (`components.h`) says so: a local seat that someone sits in (keyboard or controller), or a remote player that is connected and loaded; neither while held before spawning (`PlayerInfo::spawned`, false between `player_init()` and `release_held_players()`). Two reconcilers keep that true every frame, whatever order joins, leaves, spawns and map loads arrive in:

- `ResourceLoader::reconcile_player_bipeds()` (`loading.cpp`) mounts the map's player model (from globals → unit) and unmounts it, removing the part entities; it remounts after each map load.
- `PhysicsSystem::reconcile_player_bodies()` (`physics.cpp`) creates and removes capsules. A local seat in physics mode gets a dynamic body that drives its camera; everyone else a kinematic one that follows their camera, so collisions happen against where the biped is drawn. Bodies are only made once the world mesh exists.

Server-authoritative moves of our own player (spawn, birds-eye) go through `place_local_player()`, which also translates the body in physics mode — otherwise the next physics step would pull the camera back.

### Server flow

1. `create_server()` — binds listen socket, generates host name via `get_random_name()`
2. `start_restricted()` — every frame: picks up local `PlayerInfo` entities that became active (created by map loading, idx 0-3), assigns random names, stores refs in `m_local_player_info`
3. On client connect: accepts connection, assigns idx from `m_next_remote_idx++`
4. On `PlayerJoin`: creates `PlayerInfo` entity for remote player, sends `PlayerJoinConfirm` with assigned idx, the full roster (`send_player_roster()` — always complete, clients drop players it leaves out) and every other player's position (`send_positions()`)
5. On disconnect: removes player entity, erases connection, calls `send_player_roster()`
6. Per-frame: adds newly active local seats to the roster, receives messages on poll group, relays client camera syncs, and sends server-side camera/permission changes of networked players (local seats outside the roster stay local)

### Client flow

1. `connect_server()` — connects to server IP
2. On `GameJoin`: loads requested map, sends `PlayerJoin` with random name
3. On `PlayerJoinConfirm`: stores assigned remote index in `NetworkState`, gives it to seat 0 and moves the other seats to local-only indices
4. On `PlayerSync`: creates/updates/removes remote player entities (never a local seat), including whether they are spawned
5. On `CameraSync`: our own index (or `self_id`) is the server placing us; anything else moves that remote player
6. Per-frame: receives messages, sends own camera sync to server once the join is confirmed
7. On disconnect: removes remote players and restores local seats (`leave_server()`)

### NetworkState (`networking.h`)

Public state exposed to UI systems:
- `client_state` / `server_state` — connection lifecycle enums
- `remote_player_idx` — this client's server-assigned index
- `player_roster` — vector of `RosterEntry{name, remote_idx, loading_progress, is_self}`

### Testing

The dummy plug drives multiplayer scenarios: `.github/tests/net/dummy_plug_net_host_scenario.json` (and `dummy_plug_net_host_splitscreen.json`, with a second local seat on each side) makes the process a server and spawns a client child. Each side's `dump_state` event writes the roster and each player's biped (in play, model, body) to its journal, and `.github/tests/net/compare_journals.py <server journal> <client journal>` checks that every in-play player has a model and body under its camera, that client seats stay out of the server's index space, and that both sides agree where each player is (`MIN_IN_PLAY`, `POSITION_TOLERANCE`). CI runs both scenarios in `Test_x86_64_mesa`.

## Map loading (`map_loading.cpp`)

1. `open_map()` — clears `ObjectGC` entities, parses map file asynchronously via `FileMapper`
2. `init_map()` — loads caches (bitmaps, BSPs, models, shaders, sounds), creates scenario entities, creates the local player entities on first load (idx 0-3, tagged `PlayerBiped`; they persist across map loads), and puts local cameras on the spawn points (`create_camera()`); models and bodies follow via the biped reconcilers
3. `setup_load_eventhandlers()` — registers `MapLoadEvent` and `MapLoadFinished` handlers on `GameEventBus`

Local players are created from `shared_recipes::player_recipe`, one per seat for the map type (4 for multiplayer); only seats in play get a biped.

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
