#include "impact_sounds.h"

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

#include <vector>

using ImpactSoundsManifest = compo::SubsystemManifest<
    type_list_t<const Model>,
    type_list_t<const BlamFiles<halo_version>>,
    type_list_t<comp_app::EventBus<SoundEvent>>>;

struct ImpactSounds
    : compo::RestrictedSubsystem<ImpactSounds, ImpactSoundsManifest>
{
    using type  = ImpactSounds;
    using Proxy = compo::proxy_of<ImpactSoundsManifest>;

    ImpactSounds()
    {
        this->priority = 901;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        for(auto const& impact : std::exchange(m_impacts, {}))
            hear(p, impact);
    }

    /* Items: the impact entry of their material effects, else their own
     * collision sound. Vehicles: the chassis entry, or the crash sound when
     * hit hard. */
    void hear(Proxy& p, Physics::Impact const& impact)
    {
        auto const* model = p.template get<Model>(impact.entity_id);
        if(!model || !model->origin_object)
            return;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const& magic = files.container.magic;
        auto const& tag   = *model->origin_object;
        using blam::tag_class_t;

        blam::tagref_t const* sound = nullptr;
        if(tag.matches(tag_class_t::vehi))
        {
            auto vehicle = tag.template data<blam::scn::vehicle>(magic);
            if(!vehicle.has_value())
                return;
            auto const& v = vehicle.value()[0];
            if(impact.speed > crash_speed && v.crash_sound.valid())
                sound = &static_cast<blam::tagref_t const&>(v.crash_sound);
            else
                sound = material_sound(
                    p, v.material_effects, vehicle_chassis, impact.shader);
        } else if(
            tag.matches(tag_class_t::weap) || tag.matches(tag_class_t::eqip) ||
            tag.matches(tag_class_t::garb))
        {
            auto item = tag.template data<blam::scn::item>(magic);
            if(!item.has_value())
                return;
            auto const& i = item.value()[0];
            sound = material_sound(p, i.material_effect, impact_effect, impact.shader);
            if(!sound && i.collision_sound.valid())
                sound = &static_cast<blam::tagref_t const&>(i.collision_sound);
        }
        if(!sound)
            return;
        auto* sounds = p.template service<comp_app::EventBus<SoundEvent>>();
        if(!sounds)
            return;
        SoundEvent     ev{.type = SoundEvent::play_sound};
        PlaySoundEvent play{.sound = sound, .position = impact.point};
        sounds->inject(ev, &play);
    }

    /* foot tag: effects[effect].materials[the shader's physics material] */
    blam::tagref_t const* material_sound(
        Proxy&                p,
        blam::tagref_t const& foot,
        u32                   effect,
        blam::tagref_t const* shader_ref)
    {
        if(!foot.valid() || !shader_ref || !shader_ref->valid())
            return nullptr;
        auto const& files = p.template subsystem<BlamFiles<halo_version>>();
        auto const& magic = files.container.magic;
        blam::tag_index_view<halo_version> index(files.container);
        auto shader = index.find(*shader_ref);
        auto feet   = index.find(foot);
        if(shader == index.end() || feet == index.end())
            return nullptr;
        auto surface =
            (*shader).template data<blam::shader::radiosity_properties>(magic);
        auto effects_tag =
            (*feet).template data<blam::scn::material_effects>(magic);
        if(!surface.has_value() || !effects_tag.has_value())
            return nullptr;
        auto effects = effects_tag.value()[0].effects.data(magic);
        if(!effects.has_value() || effect >= effects.value().size())
            return nullptr;
        auto materials = effects.value()[effect].materials.data(magic);
        auto material  = static_cast<size_t>(surface.value()[0].physics);
        if(!materials.has_value() || material >= materials.value().size())
            return nullptr;
        blam::tagref_t const& sound = materials.value()[material].sound;
        return sound.valid() ? &sound : nullptr;
    }

    /* material_effects entries: 8 is an object's impact, 10 a vehicle's
     * chassis (its 9 is tire slip) */
    static constexpr u32 impact_effect   = 8;
    static constexpr u32 vehicle_chassis = 10;
    static constexpr f32 crash_speed     = 6.f; /* wu/s */

    std::vector<Physics::Impact> m_impacts;
};

void alloc_impact_sounds(compo::EntityContainer& e)
{
    auto& sounds = e.register_subsystem_inplace<ImpactSounds>();
    e.subsystem_cast<PhysicsBus>().addEventFunction<Physics::Impact>(
        0, [&sounds](Physics::Event&, Physics::Impact* impact) {
            sounds.m_impacts.push_back(*impact);
        });
}
