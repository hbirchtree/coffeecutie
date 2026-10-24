#include "h2_scene.h"

#include "data.h"
#include "physics.h"
#include "world/static_collision.h"

#include <blam/common/portal_walk.h>
#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/core/CDebug>

#include <cstdlib>
#include <limits>

namespace h2 {

using H2OccluderManifest = compo::SubsystemManifest<
    compo::type_list_t<const PlayerCamera, const PlayerInfo>,
    compo::type_list_t<
        const H2Scene,
        H2Visibility,
        const RenderingParameters,
        PhysicsBus>,
    compo::empty_list_t>;

/* Picks the active section from the camera and walks its portals for the
 * visible clusters; the active section's collision is the static body */
struct H2Occluder : compo::RestrictedSubsystem<H2Occluder, H2OccluderManifest>
{
    using Proxy = compo::proxy_of<H2OccluderManifest>;
    using type  = H2Occluder;

    H2Occluder(compo::EntityContainer& e)
        : m_container(e)
    {
        /* Before H2Renderer */
        priority = 3070;
    }

    bool main_thread_only() const override
    {
        return true;
    }

    void start_restricted(Proxy& p, time_point const&)
    {
        auto const& scene = p.template subsystem<H2Scene>();
        auto&       vis   = p.template subsystem<H2Visibility>();
        if(scene.generation != m_generation)
        {
            /* A new map, or none: the old body goes, the start section
             * comes in */
            if(vis.active >= 0)
                remove_collision();
            vis          = {};
            m_generation = scene.generation;
            if(scene.loaded)
                set_active(scene, vis, scene.start_section);
        }
        if(!scene.loaded)
            return;
        PlayerCamera const* camera{};
        for(auto e : p.select<PlayerCamera, PlayerInfo>())
        {
            auto const& [cam, info] = e.components();
            if(info.seat_idx != 0)
                continue;
            camera = &cam;
            break;
        }
        if(!camera)
            return;
        /* The debug UI's "Update occlusion" freezes the section and the
         * visible set, so what was culled can be looked at from elsewhere */
        if(!p.template subsystem<RenderingParameters>().occluder_update)
            return;
        update_section(scene, vis, camera->camera.position);
        update_visibility(scene, vis, camera->camera.position, camera->matrix);
    }

  private:
    /* The camera's cluster and the clusters its portals open onto. Solid
     * space (noclip through rock) keeps the last set. */
    void update_visibility(
        H2Scene const& scene, H2Visibility& vis, Vecf3 const& eye, Matf4 const& mvp)
    {
        if(vis.active < 0 || !m_visibility_enabled)
        {
            vis.visible.clear();
            return;
        }
        auto const& sec     = scene.sections[vis.active];
        auto        cluster = sec.cluster_of(eye);
        if(!cluster)
        {
            if(vis.visible.size() != sec.clusters.size())
                vis.visible.clear();
            return;
        }
        if(*cluster != vis.camera_cluster || mvp != m_last_mvp ||
           vis.visible.size() != sec.clusters.size())
        {
            vis.visible = blam::portal_walk::visible_set(
                sec.clusters, *cluster, mvp, m_portal_scratch);
            m_last_mvp = mvp;
        }
        if(*cluster != vis.camera_cluster)
        {
            u32 count = 0;
            for(bool v : vis.visible)
                count += v;
            vis.fog       = sec.clusters[*cluster].fog;
            vis.fog_range = sec.clusters[*cluster].fog_range;
            cDebug(
                "H2: cluster {} of {}, {} visible, fog ({:.2f}, {:.2f}, {:.2f}) x{} over [{}, {}]",
                *cluster,
                sec.clusters.size(),
                count,
                vis.fog.r,
                vis.fog.g,
                vis.fog.b,
                vis.fog.a,
                vis.fog_range.x,
                vis.fog_range.y);
            vis.camera_cluster = *cluster;
        }
    }

    /* Switch sections when the camera enters a trigger for the active one.
     * A camera that left the active section without crossing one (freecam,
     * teleport) gets the smallest section whose bounds hold it. */
    void update_section(H2Scene const& scene, H2Visibility& vis, Vecf3 const& eye)
    {
        auto inside = [&](Vecf3 const& lo, Vecf3 const& hi) {
            return glm::all(glm::greaterThanEqual(eye, lo)) &&
                   glm::all(glm::lessThanEqual(eye, hi));
        };
        for(auto const& sw : scene.switches)
            if(sw.source == vis.active && sw.destination != vis.active &&
               sw.destination >= 0 &&
               static_cast<size_t>(sw.destination) < scene.sections.size() &&
               inside(sw.lo, sw.hi))
            {
                set_active(scene, vis, sw.destination);
                return;
            }
        if(vis.active >= 0 && inside(scene.sections[vis.active].lo, scene.sections[vis.active].hi))
            return;
        /* Bounds overlap between sections (a cinematic BSP sits over 03a's
         * landing pad); being in one of a section's clusters is decisive,
         * the smallest containing bounds only a last resort */
        i32 best = -1;
        f32 best_volume = 0.f;
        for(size_t i = 0; i < scene.sections.size(); i++)
        {
            auto const& sec = scene.sections[i];
            if(!inside(sec.lo, sec.hi))
                continue;
            if(sec.cluster_of(eye))
            {
                best = static_cast<i32>(i);
                break;
            }
            Vecf3 size   = sec.hi - sec.lo;
            f32   volume = size.x * size.y * size.z;
            if(best < 0 || volume < best_volume)
                best = static_cast<i32>(i), best_volume = volume;
        }
        if(best >= 0)
            set_active(scene, vis, best);
    }

    /* The active section's collision is the one static body */
    void set_active(H2Scene const& scene, H2Visibility& vis, i32 section)
    {
        if(section == vis.active || section < 0 ||
           static_cast<size_t>(section) >= scene.sections.size())
            return;
        if(vis.active >= 0)
            remove_collision();
        vis.active         = section;
        vis.camera_cluster = std::numeric_limits<u32>::max();
        vis.visible.clear();
        vis.fog        = {};
        vis.planar_fog = scene.sections[section].planar_fog;
        cDebug("H2: section {} ({}) active", section, scene.sections[section].name);
        submit_collision(scene.sections[section].collision);
    }

    /* One static body for the active section, like the Halo 1 world mesh */
    void submit_collision(std::shared_ptr<CollisionMesh> mesh)
    {
        world::submit_static_collision(m_container.subsystem_cast<PhysicsBus>(), std::move(mesh), collision_body_id);
    }

    void remove_collision()
    {
        Physics::Event       event{.type = Physics::Event::BodyRemoval};
        Physics::BodyRemoval removal{.entity_id = collision_body_id};
        m_container.subsystem_cast<PhysicsBus>().process(event, &removal);
    }

    compo::EntityContainer& m_container;
    u32                     m_generation{0};
    blam::portal_walk::scratch_t m_portal_scratch;
    Matf4                        m_last_mvp{};
    bool m_visibility_enabled{!std::getenv("COFFEE_H2_NO_VISIBILITY")};
};

void alloc_h2_occluder(compo::EntityContainer& e)
{
    e.register_subsystem_inplace<H2Occluder>(std::ref(e));
}

} // namespace h2
