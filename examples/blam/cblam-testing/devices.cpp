#include "devices.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "game_events.h"
#include "network/networking.h"
#include "object_functions.h"
#include "selected_version.h"
#include "sounds.h"
#include "ticks.h"

#include <blam/volta/blam_tag_index.h>

#include <set>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>

using DevicesManifest = compo::SubsystemManifest<
    type_list_t<
        DeviceState,
        AnimationPlayback,
        ObjectSpawn,
        Model,
        const ObjectPhysics,
        const PlayerInfo,
        const PlayerCamera,
        const NetworkInfo>,
    type_list_t<
        DeviceGroups,
        GameEventBus,
        const NetworkState,
        const LoadingStatus,
        const BlamFiles<halo_version>>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

/* Machines and controls move toward where their groups, scripts, doors and
 * players' use buttons put them. What they are told is decided on the
 * authority; every peer moves, poses and sounds them alike */
struct DeviceSystem : compo::RestrictedSubsystem<DeviceSystem, DevicesManifest>
{
    using type  = DeviceSystem;
    using Proxy = compo::proxy_of<DevicesManifest>;

    DeviceSystem()
    {
        this->priority = 902;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        f32 dt = 0.f;
        for(auto const& tick : m_ticks->take())
            dt += tick.seconds;
        m_input_queue->poll();
        m_sync_queue->poll();
        m_save_queue->poll();
        m_restore_queue->poll();
        bool const save    = std::exchange(m_save, false);
        bool const restore = std::exchange(m_restore, false);
        if(p.template subsystem<LoadingStatus>().loaded_map != LoadingStatus::loaded)
        {
            m_uses.clear();
            m_syncs.clear();
            return;
        }
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto&       groups = p.template subsystem<DeviceGroups>();
        m_magic            = &files.container.magic;
        if(groups.load_generation != files.load_generation)
        {
            reset_groups(files, groups);
            m_saved.reset();
        }
        for(auto const& sync : std::exchange(m_syncs, {}))
            take(p, groups, sync);
        if(save)
            keep(p, groups);
        if(restore && m_saved)
            put_back(p, groups);

        bool const authority = !p.template subsystem<NetworkState>().replica;
        if(authority)
        {
            for(u64 player : std::exchange(m_uses, {}))
                use(p, groups, player);
            if((m_settling -= dt) <= 0.f)
                open_doors(p, dt);
        }
        m_uses.clear();
        move(p, groups, dt);
    }

    void reset_groups(BlamFiles<halo_version> const& files, DeviceGroups& groups)
    {
        groups                 = {};
        groups.load_generation = files.load_generation;
        auto const* scenario   = files.container.scenario().value_or(nullptr);
        if(!scenario)
            return;
        auto list = scenario->objects.device_groups.data(*m_magic);
        if(!list.has_value())
            return;
        using flags_t = blam::scn::device_group::device_group_flags;
        for(auto const& group : list.value())
        {
            groups.values.push_back(group.initial_value);
            groups.changes_once.push_back(
                (group.flags & flags_t::changes_once) != flags_t::none);
        }
        groups.spent.assign(groups.values.size(), false);
    }

    /* ---- Where devices are told to go ---- */

    /* Doors open for units close by and stay open a while after */
    void open_doors(Proxy& p, f32 dt)
    {
        std::vector<Vecf3> units;
        for(auto player : p.template select<PlayerInfo, PlayerCamera, NetworkInfo>())
        {
            auto [info, cam, net] = player.components();
            if(biped_in_play(info, cam, net))
                units.push_back(cam.camera.position - Vecf3{0.f, 0.f, info.eye_offset()});
        }
        for(auto object : p.template select<Model, ObjectPhysics>())
        {
            auto [model, physics] = object.components();
            if(physics.upright)
                units.push_back(model.position + Vecf3{0.f, 0.f, unit_middle});
        }

        using type_t = blam::scn::machine::machine_type_t;
        for(auto entity : p.template select<DeviceState, Model>())
        {
            auto [device, model] = entity.components();
            auto const* physics  = p.template get<ObjectPhysics>(entity.id());
            if(!device.machine || !device.tag || !device.automatic ||
               device.machine->machine_type != type_t::door || device.power <= 0.f)
                continue;
            f32 const reach = device.tag->automatic_activation_radius > 0.f
                                  ? device.tag->automatic_activation_radius
                                  : default_activation_radius;
            Vecf3 const ahead  = model.rotation * Vecf3{1.f, 0.f, 0.f};
            Vecf3 const centre = model.position + model.rotation * middle_of(physics);
            bool const  one_sided = device.kind_flags & DeviceState::one_sided;
            bool        near      = false;
            for(Vecf3 const& unit : units)
            {
                Vecf3 const to = unit - centre;
                if(glm::length(to) <= reach + unit_radius &&
                   (!one_sided || glm::dot(to, ahead) >= 0.f))
                    near = true;
            }
            if(near)
            {
                device.open_left       = device.machine->door_open_time;
                device.position_target = 1.f;
            } else if(device.position_target > 0.f && (device.open_left -= dt) <= 0.f)
                device.position_target = 0.f;
        }
    }

    /* The middle of a device's collision, in its own space: its origin is
     * often at an edge */
    static Vecf3 middle_of(ObjectPhysics const* physics)
    {
        if(!physics || !physics->collision || physics->collision->nodes.empty())
            return {};
        Vecf3 lo(std::numeric_limits<f32>::max()), hi(std::numeric_limits<f32>::lowest());
        for(auto const& node : physics->collision->nodes)
            for(Vecf3 const& point : node.points)
            {
                lo = glm::min(lo, point);
                hi = glm::max(hi, point);
            }
        return lo.x <= hi.x ? (lo + hi) * .5f : Vecf3{};
    }

    /* A player's use button on the control it faces within reach */
    void use(Proxy& p, DeviceGroups& groups, u64 player)
    {
        auto const* info = p.template get<PlayerInfo>(player);
        auto const* cam  = p.template get<PlayerCamera>(player);
        if(!info || !cam || info->riding.vehicle != 0)
            return;
        Vecf3 const eye  = cam->camera.position;
        Vecf3 const look = glm::transpose(Matf3(cam->rotation)) * Vecf3{0.f, 0.f, -1.f};
        u64 best  = 0;
        f32 aimed = use_cone;
        for(auto entity : p.template select<DeviceState, Model>())
        {
            auto [device, model] = entity.components();
            if(!device.control)
                continue;
            Vecf3 const to = model.position - eye;
            f32 const   d  = glm::length(to);
            if(d > use_reach || d <= 0.f)
                continue;
            if(f32 const a = glm::dot(to / d, look); a > aimed)
            {
                aimed = a;
                best  = entity.id();
            }
        }
        if(best == 0)
            return;
        auto&       device = *p.template get<DeviceState>(best);
        auto const& model  = *p.template get<Model>(best);
        auto const& tag    = *device.control;
        bool const  both   = device.kind_flags & DeviceState::both_sides;
        bool const  facing = glm::dot(eye - model.position, model.rotation * Vecf3{1.f, 0.f, 0.f}) >= 0.f;
        bool const  usable = !(device.flags & DeviceState::not_usable) && device.power > 0.f &&
                            !device.used && (both || facing);
        if(!usable)
            return sound(p, tag.deny, model);
        using type_t = blam::scn::device_control::control_type_t;
        f32 const now = device.position_target;
        f32 const to  = tag.control_type == type_t::toggle_switch ? (now > .5f ? 0.f : 1.f)
                        : tag.control_type == type_t::on_button   ? 1.f
                        : tag.control_type == type_t::off_button  ? 0.f
                                                                  : tag.call_value;
        bool const reversed = device.flags & DeviceState::reversed;
        if(groups.valid(device.position_group) &&
           !groups.set(device.position_group, reversed ? 1.f - to : to))
            return sound(p, tag.deny, model);
        device.position_target = to;
        device.used            = device.flags & DeviceState::changes_once;
        sound(p, to > now ? tag.on : tag.off, model);
    }

    /* What the server says a device or group is told */
    void take(Proxy& p, DeviceGroups& groups, DeviceSyncEvent const& sync)
    {
        if(sync.group >= 0)
        {
            if(groups.valid(sync.group))
                groups.values[sync.group] = sync.value;
            return;
        }
        auto* device = sync.device ? p.template get<DeviceState>(sync.device) : nullptr;
        if(!device)
            return;
        device->position_target = sync.position_target;
        device->power_target    = sync.power_target;
        if(sync.snap != device->snaps)
        {
            device->position = sync.position_target;
            device->snaps    = sync.snap;
        }
    }

    /* ---- Checkpoints ---- */

    struct saved_t
    {
        std::vector<f32>         values;
        std::vector<bool>        spent;
        std::map<u32, DeviceState> devices; /*!< By net id */
    };

    void keep(Proxy& p, DeviceGroups const& groups)
    {
        saved_t out{.values = groups.values, .spent = groups.spent};
        for(auto entity : p.template select<DeviceState, NetworkInfo>())
        {
            auto [device, net] = entity.components();
            if(net.instance_id != 0)
                out.devices[net.instance_id] = device;
        }
        m_saved = std::move(out);
    }

    /* Straight to where they were headed, without moving or sounding; replicas
     * follow through the groups and a snap */
    void put_back(Proxy& p, DeviceGroups& groups)
    {
        if(m_saved->values.size() == groups.values.size())
        {
            groups.values = m_saved->values;
            groups.spent  = m_saved->spent;
        }
        for(auto entity : p.template select<DeviceState, NetworkInfo>())
        {
            auto [device, net] = entity.components();
            auto it = m_saved->devices.find(net.instance_id);
            if(it == m_saved->devices.end())
                continue;
            auto const& saved      = it->second;
            device.power           = saved.power;
            device.power_target    = saved.power_target;
            device.position        = saved.position_target;
            device.position_target = saved.position_target;
            device.open_left       = saved.open_left;
            device.position_seen   = saved.position_seen;
            device.power_seen      = saved.power_seen;
            device.used            = saved.used;
            device.automatic       = saved.automatic;
            device.snaps++;
        }
        m_settling = settle_time;
    }

    /* ---- Every peer: moving, posing, sounding ---- */

    void move(Proxy& p, DeviceGroups& groups, f32 dt)
    {
        for(auto entity : p.template select<DeviceState, Model>())
        {
            auto [device, model] = entity.components();
            if(!device.tag)
                continue;
            auto const* net = p.template get<NetworkInfo>(entity.id());
            if(net && net->instance_id != 0 && m_known.insert(entity.id()).second)
                recall(device, groups.remembered[net->instance_id]);
            bool const reversed = device.flags & DeviceState::reversed;
            if(f32 const v = groups.get(device.position_group);
               groups.valid(device.position_group) && v != device.position_seen)
            {
                device.position_target = reversed ? 1.f - v : v;
                device.position_seen   = v;
            }
            if(f32 const v = groups.get(device.power_group);
               groups.valid(device.power_group) && v != device.power_seen)
            {
                device.power_target = v;
                device.power_seen   = v;
            }

            f32 const was_power = device.power;
            device.power = approach(device.power, device.power_target, device.tag->power_transition_time, dt);
            /* Unpowered, it moves at its depowered pace, if at all */
            f32 const was     = device.position;
            bool const powered = device.power > 0.f;
            f32 const pace     = powered ? device.tag->position_transition_time
                                         : device.tag->depowered_position_transition_time;
            using flags_t    = blam::scn::device::device_flags_t;
            bool const loops = (device.tag->device_flags & flags_t::position_loops) != flags_t::none;
            /* A looping one turns round and round while powered */
            if(loops && powered && pace > 0.f)
                device.position = std::fmod(device.position + device.power * dt / pace, 1.f);
            else if(!loops && (powered || pace > 0.f))
                device.position = approach(device.position, device.position_target, pace, dt);
            if(device.position != was)
            {
                bool const opening = device.position > was;
                if(was == 0.f || was == 1.f)
                    sound(p, opening ? device.tag->open : device.tag->close, model);
                if(device.position == 1.f)
                    sound(p, device.tag->opened, model);
                else if(device.position == 0.f)
                    sound(p, device.tag->closed, model);
            }

            if(auto* spawn = p.template get<ObjectSpawn>(entity.id());
               spawn && groups.valid(device.power_group))
                spawn->power = device.power;
            if(auto* anim = p.template get<AnimationPlayback>(entity.id()))
                pose(*anim, device);
            export_functions(p, device, model, was_power, was, dt);
            if(net && net->instance_id != 0)
                groups.remembered[net->instance_id] = {
                    .position        = device.position,
                    .position_target = device.position_target,
                    .power           = device.power,
                    .power_target    = device.power_target,
                    .automatic       = device.automatic,
                    .one_sided       = (device.kind_flags & DeviceState::one_sided) != 0,
                    .used            = device.used,
                };
        }
        std::erase_if(m_known, [&p](u64 id) { return !p.template get<DeviceState>(id); });
    }

    /* Its power and position, through its object functions, onto its
     * model: what starts and scales its attachments */
    void export_functions(
        Proxy& p, DeviceState const& device, Model& model, f32 was_power, f32 was_position, f32 dt)
    {
        auto const& magic     = p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto        functions = device.tag->functions.data(magic);
        if(!functions.has_value() || functions.value().empty())
            return;
        auto change = [dt](f32 from, f32 to, f32 seconds) {
            return dt > 0.f ? std::clamp(std::abs(to - from) / dt * std::max(seconds, dt), 0.f, 1.f) : 0.f;
        };
        auto const&        tag = *device.tag;
        std::array<f32, 4> in{};
        std::array<u16, 4> const sources{tag.a_in, tag.b_in, tag.c_in, tag.d_in};
        for(size_t i = 0; i < in.size(); i++)
            switch(sources[i])
            {
            case device_in::power:
                in[i] = device.power;
                break;
            case device_in::change_in_power:
                in[i] = change(was_power, device.power, tag.power_transition_time);
                break;
            case device_in::position:
                in[i] = device.position;
                break;
            case device_in::change_in_position:
                in[i] = change(was_position, device.position, tag.position_transition_time);
                break;
            default:
                break;
            }
        model.object_function = blam_functions::evaluate_all(functions.value(), in);
    }

    /* What a device's A..D in read */
    struct device_in
    {
        static constexpr u16 power              = 1;
        static constexpr u16 change_in_power    = 2;
        static constexpr u16 position           = 3;
        static constexpr u16 change_in_position = 4;
    };

    /* Built again as its section loads, it is as it was left */
    static void recall(DeviceState& device, DeviceGroups::remembered_t const& r)
    {
        device.position        = r.position.value_or(device.position);
        device.position_target = r.position_target.value_or(device.position_target);
        device.power           = r.power.value_or(device.power);
        device.power_target    = r.power_target.value_or(device.power_target);
        device.automatic       = r.automatic.value_or(device.automatic);
        device.used            = r.used.value_or(device.used);
        if(r.one_sided)
            device.kind_flags = *r.one_sided ? device.kind_flags | DeviceState::one_sided
                                             : device.kind_flags & ~DeviceState::one_sided;
        device.snaps++;
    }

    static f32 approach(f32 from, f32 to, f32 seconds, f32 dt)
    {
        if(seconds <= 0.f)
            return to;
        f32 const step = dt / seconds;
        return from < to ? std::min(from + step, to) : std::max(from - step, to);
    }

    /* Held at its position along the graph's position animation; ones that
     * loop keep turning while powered */
    void pose(AnimationPlayback& anim, DeviceState const& device) const
    {
        auto& layer = anim.layers[AnimationPlayback::base_slot];
        if(!anim.graph)
            return;
        using flags_t   = blam::scn::device::device_flags_t;
        auto const clip  = position_clip(anim.graph);
        bool const loops = (device.tag->device_flags & flags_t::position_loops) != flags_t::none;
        if(!clip)
        {
            layer.paused = device.power <= 0.f;
            return;
        }
        auto clips       = anim.graph->animations.data(*m_magic);
        if(!clips.has_value())
            return;
        auto const& animation = clips.value()[*clip];
        f32 const   rate      = animation.frame_rate();
        layer.graph     = anim.graph;
        layer.animation = static_cast<u32>(*clip);
        layer.loop      = loops;
        layer.paused    = true;
        layer.finished  = false;
        layer.weight    = 1.f;
        /* Round and round, the last frame blends into the first */
        layer.time = device.position * std::max(animation.frame_count - (loops ? 0 : 1), 0) / rate;
    }

    std::optional<i16> position_clip(blam::antr::header const* graph) const
    {
        auto devices = graph->devices.data(*m_magic);
        if(!devices.has_value() || devices.value().empty())
            return std::nullopt;
        auto refs = devices.value()[0].animations.data(*m_magic);
        if(!refs.has_value() || refs.value().size() <= blam::antr::device::position)
            return std::nullopt;
        i16 const clip = refs.value()[blam::antr::device::position].animation;
        if(clip < 0)
            return std::nullopt;
        return clip;
    }

    void sound(Proxy& p, blam::tagref_t const& ref, Model const& model)
    {
        if(!ref.valid())
            return;
        if(ref.tag_class == blam::tag_class_t::effe)
        {
            GameEvent        ev{.type = GameEvent::SpawnEffect};
            SpawnEffectEvent play{
                .effect    = ref,
                .position  = model.position,
                .direction = model.rotation * Vecf3{1.f, 0.f, 0.f},
            };
            p.template subsystem<GameEventBus>().inject(ev, &play);
            return;
        }
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        blam::tag_index_view<halo_version> index(
            p.template subsystem<BlamFiles<halo_version>>().container);
        auto tag = index.find(ref);
        if(!sounds || tag == index.end())
            return;
        SoundEvent     ev{.type = SoundEvent::play_sound, .entity_id = sound_base | m_next_sound++};
        PlaySoundEvent play{.sound_tag = &(*tag), .position = model.position};
        sounds->inject(ev, &play);
    }

    /* Units' middles above their feet and how wide they are, and how near
     * doors without an activation radius open */
    static constexpr f32 unit_middle               = .35f;
    static constexpr f32 unit_radius               = .3f;
    static constexpr f32 default_activation_radius = 1.5f;
    /* A control is used from this close, looked at within this cone */
    static constexpr f32 use_reach = 1.5f;
    static constexpr f32 use_cone  = .5f; /* cos 60 deg */
    static constexpr u64 sound_base = 0x4445564900000000ULL; /* 'DEVI' */

    std::unique_ptr<TickQueue>                                      m_ticks;
    std::shared_ptr<GameEventBus::queue_type<PlayerTickInputEvent>> m_input_queue;
    std::shared_ptr<GameEventBus::queue_type<DeviceSyncEvent>>      m_sync_queue;
    std::map<u64, bool>          m_using;
    std::vector<u64>             m_uses;
    std::vector<DeviceSyncEvent> m_syncs;
    blam::map_ptr const*         m_magic{nullptr};
    std::shared_ptr<GameEventBus::queue_type<WorldSaveEvent>>    m_save_queue;
    std::shared_ptr<GameEventBus::queue_type<WorldRestoreEvent>> m_restore_queue;
    bool                         m_save{false};
    bool                         m_restore{false};
    std::optional<saved_t>       m_saved;
    /* Bodies go back a frame or two after the doors; until then, doors
     * pay them no mind */
    static constexpr f32         settle_time = .25f; /* s */
    f32                          m_settling{0.f};
    std::set<u64>                m_known; /* devices recalled since they were built */
    u64                          m_next_sound{0};
};

void alloc_devices(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<DeviceGroups>();
    auto& devices   = e.register_subsystem_inplace<DeviceSystem>();
    auto& game      = e.subsystem_cast<GameEventBus>();
    devices.m_ticks = std::make_unique<TickQueue>(game);
    /* Use presses from every seat and client */
    devices.m_input_queue = game.addQueuedEventFunction<PlayerTickInputEvent>(
        0, [&devices](GameEvent&, PlayerTickInputEvent* input) {
            bool const use = (input->intents & InputIntent::use) != InputIntent::none;
            if(!std::exchange(devices.m_using[input->player], use) && use)
                devices.m_uses.push_back(input->player);
        });
    devices.m_sync_queue = game.addQueuedEventFunction<DeviceSyncEvent>(
        0, [&devices](GameEvent&, DeviceSyncEvent* sync) {
            if(sync->origin == MapLoadEvent::Remote)
                devices.m_syncs.push_back(*sync);
        });
    devices.m_save_queue = game.addQueuedEventFunction<WorldSaveEvent>(
        0, [&devices](GameEvent&, WorldSaveEvent*) { devices.m_save = true; });
    devices.m_restore_queue = game.addQueuedEventFunction<WorldRestoreEvent>(
        0, [&devices](GameEvent&, WorldRestoreEvent*) { devices.m_restore = true; });
}
