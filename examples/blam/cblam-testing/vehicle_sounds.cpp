#include "vehicle_sounds.h"

#include "blam_files.h"
#include "components.h"
#include "data.h"
#include "physics.h"
#include "selected_version.h"
#include "sounds.h"

#include <blam/volta/blam_effects.h>
#include <blam/volta/blam_shaders.h>

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>

#include <map>
#include <set>
#include <vector>

using VehicleSoundsManifest = compo::SubsystemManifest<
    type_list_t<const Model, const ObjectPhysics, const PlayerInfo>,
    type_list_t<PhysicsBus, const BlamFiles<halo_version>>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

struct VehicleSounds
    : compo::RestrictedSubsystem<VehicleSounds, VehicleSoundsManifest>
{
    using type  = VehicleSounds;
    using Proxy = compo::proxy_of<VehicleSoundsManifest>;

    VehicleSounds()
    {
        this->priority = 902;
    }

    void start_restricted(Proxy& p, time_point const& t)
    {
        f32 const dt =
            m_last == time_point{}
                ? 0.f
                : std::min(std::chrono::duration<f32>(t - m_last).count(), .25f);
        m_last = t;
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        if(!sounds)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        if(files.load_generation != m_generation)
        {
            m_engines.clear();
            m_wheels.clear();
            m_generation = files.load_generation;
        }

        std::set<u64> driven;
        for(auto rider : p.template select<PlayerInfo>())
            if(auto const& riding = rider.template get<PlayerInfo>().riding;
               riding.vehicle != 0 && riding.driver)
                driven.insert(riding.vehicle);

        for(auto it = m_engines.begin(); it != m_engines.end();)
            if(!driven.contains(it->first) ||
               !p.template get<Model>(it->first))
            {
                for(auto id : it->second)
                {
                    SoundEvent ev{.type = SoundEvent::stop_sound, .entity_id = id};
                    sounds->inject(ev, nullptr);
                }
                it = m_engines.erase(it);
            } else
                ++it;

        for(auto ent : p.template select<Model, ObjectPhysics>())
        {
            auto [model, physics] = ent.components();
            if(!physics.drive || !model.origin_object)
                continue;
            bool const driving = driven.contains(ent.id());
            if(driving)
                engine(p, *sounds, ent.id(), model, physics);
            /* Parked ones settling at load are not driving over bumps */
            if(driving || glm::length(physics.linear_velocity) > moving_speed)
                knocks(p, *sounds, ent.id(), model, physics, t, dt);
            else
                m_wheels[ent.id()].depth = physics.vehicle.ground_depth;
        }

        for(auto const& hit : std::exchange(m_skids, {}))
            skid(p, *sounds, hit);
    }

    /* Looping attachments, scaled by throttle and speed */
    void engine(
        Proxy&                          p,
        comp_app::EventBus<SoundEvent>& sounds,
        u64                             id,
        Model const&                    model,
        ObjectPhysics const&            physics)
    {
        auto& loops = m_engines[id];
        if(loops.empty())
        {
            auto const& magic =
                p.template subsystem<BlamFiles<halo_version>>().container.magic;
            auto object =
                model.origin_object->template data<blam::scn::object>(magic);
            if(!object.has_value())
                return;
            auto attachments = object.value()[0].attachments.data(magic);
            for(size_t j = 0; attachments.has_value() &&
                              j < attachments.value().size();
                j++)
            {
                auto const& sound = static_cast<blam::tagref_t const&>(
                    attachments.value()[j].type);
                if(!sound.matches(blam::tag_class_t::lsnd))
                    continue;
                u64 const      loop = engine_base | (id << 4) | j;
                SoundEvent     ev{.type = SoundEvent::play_sound, .entity_id = loop};
                PlaySoundEvent play{
                    .sound    = &sound,
                    .position = model.position,
                    .looping  = true,
                };
                sounds.inject(ev, &play);
                loops.push_back(loop);
            }
            /* Remembered even when empty, so the tag isn't searched again */
            if(loops.empty())
                loops.push_back(0);
        }
        f32 const top   = physics.drive->forward_speed;
        f32 const speed = glm::length(physics.linear_velocity);
        f32 const power = std::clamp(
            std::max(
                std::abs(physics.vehicle.throttle),
                top > 0.f ? speed / top : 0.f),
            0.f,
            1.f);
        for(auto loop : loops)
        {
            if(loop == 0)
                continue;
            SoundEvent       ev{.type = SoundEvent::update_sound, .entity_id = loop};
            UpdateSoundEvent update{
                .position = model.position,
                .gain     = idle_gain + (1.f - idle_gain) * power,
                .pitch    = idle_pitch + pitch_range * power,
            };
            sounds.inject(ev, &update);
        }
    }

    /* Suspension knocks and tire skids */
    void knocks(
        Proxy&                          p,
        comp_app::EventBus<SoundEvent>& sounds,
        u64                             id,
        Model const&                    model,
        ObjectPhysics const&            physics,
        time_point const&               t,
        f32                             dt)
    {
        auto const& magic =
            p.template subsystem<BlamFiles<halo_version>>().container.magic;
        auto vehicle =
            model.origin_object->template data<blam::scn::vehicle>(magic);
        if(!vehicle.has_value())
            return;
        auto const& tag   = vehicle.value()[0];
        auto&       state = m_wheels[id];
        auto const& depth = physics.vehicle.ground_depth;

        f32 rise = 0.f;
        if(dt > 0.f && state.depth.size() == depth.size())
            for(size_t i = 0; i < depth.size(); i++)
                rise = std::max(rise, (depth[i] - state.depth[i]) / dt);
        state.depth = depth;
        blam::tagref_t const& knock = tag.suspension_sound;
        if(rise > knock_speed && knock.valid() && t >= state.next_knock)
        {
            SoundEvent     ev{.type = SoundEvent::play_sound};
            PlaySoundEvent play{.sound = &knock, .position = model.position};
            sounds.inject(ev, &play);
            state.next_knock = t + std::chrono::milliseconds(300);
        }

        if(physics.vehicle.slip > skid_speed && t >= state.next_skid &&
           static_cast<blam::tagref_t const&>(tag.material_effects).valid())
        {
            auto&                physics_bus = p.template subsystem<PhysicsBus>();
            Physics::Event       ev{Physics::Event::GroundProbe};
            Physics::GroundProbe probe{
                .entity_id = id,
                .from      = model.position + Vecf3{0.f, 0.f, .5f},
                .to        = model.position - Vecf3{0.f, 0.f, 1.f},
                .user      = tire_slip,
            };
            physics_bus.process(ev, &probe);
            state.next_skid = t + std::chrono::milliseconds(450);
        }
    }

    /* foot tag effects[tire slip].materials[the surface's material] */
    void skid(
        Proxy&                          p,
        comp_app::EventBus<SoundEvent>& sounds,
        Physics::GroundHit const&       hit)
    {
        auto const* model = p.template get<Model>(hit.entity_id);
        if(!model || !model->origin_object || !hit.shader ||
           !hit.shader->valid())
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const& magic = files.container.magic;
        auto vehicle =
            model->origin_object->template data<blam::scn::vehicle>(magic);
        if(!vehicle.has_value())
            return;
        blam::tag_index_view<halo_version> index(files.container);
        auto shader = index.find(*hit.shader);
        auto feet   = index.find(static_cast<blam::tagref_t const&>(
            vehicle.value()[0].material_effects));
        if(shader == index.end() || feet == index.end())
            return;
        auto surface =
            (*shader).template data<blam::shader::radiosity_properties>(magic);
        auto effects_tag =
            (*feet).template data<blam::scn::material_effects>(magic);
        if(!surface.has_value() || !effects_tag.has_value())
            return;
        auto effects = effects_tag.value()[0].effects.data(magic);
        if(!effects.has_value() || tire_slip >= effects.value().size())
            return;
        auto materials = effects.value()[tire_slip].materials.data(magic);
        auto material  = static_cast<size_t>(surface.value()[0].physics);
        if(!materials.has_value() || material >= materials.value().size())
            return;
        blam::tagref_t const& sound = materials.value()[material].sound;
        if(!sound.valid())
            return;
        SoundEvent     ev{.type = SoundEvent::play_sound};
        PlaySoundEvent play{.sound = &sound, .position = hit.point};
        sounds.inject(ev, &play);
    }

    /* 'VEHI' << 32, then the vehicle and its attachment */
    static constexpr u64 engine_base = 0x5645484900000000ULL;
    /* material_effects entry for tires sliding */
    static constexpr u32 tire_slip   = 9;
    static constexpr f32 idle_gain   = .35f;
    static constexpr f32 idle_pitch  = .8f;
    static constexpr f32 pitch_range = .5f;
    static constexpr f32 knock_speed = .6f; /* wu/s of wheel compression */
    static constexpr f32 skid_speed  = 1.5f; /* wu/s sideways */
    static constexpr f32 moving_speed = 1.f; /* wu/s */

    struct wheels_t
    {
        std::vector<f32> depth;
        time_point       next_knock{};
        time_point       next_skid{};
    };

    time_point                      m_last{};
    u32                             m_generation{0};
    std::map<u64, std::vector<u64>> m_engines;
    std::map<u64, wheels_t>         m_wheels;
    std::vector<Physics::GroundHit> m_skids;
};

void alloc_vehicle_sounds(compo::EntityContainer& e)
{
    auto& sounds = e.register_subsystem_inplace<VehicleSounds>();
    e.subsystem_cast<PhysicsBus>().addEventFunction<Physics::GroundHit>(
        0, [&sounds](Physics::Event&, Physics::GroundHit* hit) {
            if(hit->user == VehicleSounds::tire_slip)
                sounds.m_skids.push_back(*hit);
        });
}
