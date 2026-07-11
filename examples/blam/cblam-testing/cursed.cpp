#include "cursed.h"

#include <coffee/components/proxy.h>
#include <coffee/components/restricted_subsystem.h>
#include <coffee/components/subsystem.h>

#include "coffee/graphics/apis/gleam/rhi.h"
#include "coffee/graphics/apis/gleam/rhi_buffer.h"
#include "coffee/graphics/apis/gleam/rhi_compat.h"
#include "coffee/graphics/apis/gleam/rhi_draw_command.h"
#include "coffee/graphics/apis/gleam/rhi_program.h"
#include "coffee/graphics/apis/gleam/rhi_submit.h"
#include "coffee/graphics/apis/gleam/rhi_system.h"
#include "coffee/graphics/apis/gleam/rhi_texture.h"
#include "coffee/graphics/apis/gleam/rhi_vertex.h"
#include "components.h"
#include "../../rs/terrain/rs2_geometry.h"
#include <coffee/core/task_queue/task.h>
#include "data.h"
#include "entity_container.h"
#include "peripherals/concepts/graphics_api.h"
#include "peripherals/semantic/chunk.h"
#include "peripherals/semantic/enum/data_types.h"
#include "peripherals/typing/enum/graphics/shader_stage.h"
#include "physics.h"
#include "resource_creation.h"
#include "types.h"

#if defined(FEATURE_ENABLE_BULLET3)
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#endif

#include <imgui.h>

#include <map>
#include <set>

namespace cursed {

using RS2CacheLoaderManifest = compo::SubsystemManifest<
    compo::type_list_t<
        PlayerCamera,
        PlayerInfo
    >,
    compo::type_list_t<
        BlamResources,
        GameEventBus
    >,
    compo::empty_list_t
>;

struct RS2CacheLoader : public compo::RestrictedSubsystem<RS2CacheLoader, RS2CacheLoaderManifest>
{
    using Proxy = compo::proxy_of<RS2CacheLoaderManifest>;
    using type = RS2CacheLoader;

    RS2CacheLoader(
        gfx::api& api,
        PhysicsBus& physics,
        compo::EntityRef<compo::EntityContainer> const& entity,
        std::string const& cache_path)
        : api(api)
        , physics(physics)
        , world_entity(entity)
        , loader(cache_path)
    {
        this->priority = 3080;
        region_vao = api.alloc_vertex_array();
        region_vao->alloc();
        region_vao->add(gfx::vertex_attribute{
            .index = 0,
            .value = {
                .offset = 0,
                .stride = sizeof(rs2::Vertex),
                .count = 3,
            },
        });
        region_vao->add(gfx::vertex_attribute{
            .index = 1,
            .value = {
                .offset = sizeof(f32) * 3 + sizeof(u8) * 4,
                .stride = sizeof(rs2::Vertex),
                .count = 2,
            },
        });
        region_vao->add(gfx::vertex_attribute{
            .index = 2,
            .value = {
                .offset = sizeof(f32) * 3,
                .stride = sizeof(rs2::Vertex),
                .count = 4,
                .type = semantic::type_t::u8,
                .flags = gfx::vertex_attribute::attribute_flags::packed |
                    gfx::vertex_attribute::attribute_flags::normalized,
            },
        });
        region_vao->set_attribute_names({
            {"position", 0},
            {"texcoord", 1},
            {"color", 2},
        });

        vertex_buf = api.alloc_buffer(
            gfx::buffers::vertex,
            semantic::RSCA::ReadWrite | semantic::RSCA::Persistent);
        element_buf = api.alloc_buffer(
            gfx::buffers::element,
            semantic::RSCA::ReadWrite | semantic::RSCA::Persistent);

        vertex_buf->alloc();
        element_buf->alloc();
        vertex_buf->commit(64 * 1024 * 1024);
        element_buf->commit(32 * 1024 * 1024);

        region_vao->set_buffer(gfx::buffers::vertex, vertex_buf, 0);
        region_vao->set_buffer(gfx::buffers::element, element_buf);

        region_tex = std::make_shared<gfx::compat::texture_2da_t>(
            &api, PixDesc(pix_fmt::RGBA8), 1);
        region_tex->alloc(size_3d<u32>{128, 128, 64});
        region_sampler = region_tex->sampler();
        region_sampler->alloc();

        // Region building happens on a dedicated worker thread; the worker
        // owns all rs2::RegionLoader access (it is not thread-safe).
        // Results are marshalled back to this (the GPU) thread's queue.
        worker = rq::runtime_queue::CreateNewThreadQueue("RS2::Loader")
                     .assume_value();
        on_built = rq::runtime_queue::BindToQueue(
            std::function<void(std::shared_ptr<RegionData>)>(
                [this](std::shared_ptr<RegionData> data) {
                    integrate_region(std::move(data));
                }));
#if defined(FEATURE_ENABLE_BULLET3)
        // Worker built a ready collision shape; hand it to the physics
        // system on the main thread if the region is still wanted.
        on_physics_built = rq::runtime_queue::BindToQueue(
            std::function<void(std::shared_ptr<PhysicsRegion>)>(
                [this](std::shared_ptr<PhysicsRegion> region) {
                    if(!in_ring(region->coord, wanted_center))
                        return; // moved on while building — drop it
                    u64 k = region_key(region->coord.x, region->coord.y);
                    physics_bodies.insert(k);
                    Physics::Event event{
                        .type = Physics::Event::BodyCreationPrebuilt};
                    this->physics.process(event, region->body.get());
                }));
#endif

        region_program = api.alloc_program();
        {
            create_program(api, shader_pair_t{
                .vertex_file = "rs2",
                .fragment_file = "rs2",
                .shader = region_program,
            });
            // auto vert_shader = api.alloc_shader(std::string_view());
            // auto frag_shader = api.alloc_shader(std::string_view());
            // region_program->add(gfx::program_t::stage_t::Vertex, vert_shader);
            // region_program->add(gfx::program_t::stage_t::Fragment, frag_shader);
            // if(auto compile = region_program->compile(); compile.has_error())
            //     cWarning("Failed to compile cursed program: {}", compile.error());
            // else
            //     cDebug("Cursed program state: {}", compile.value());
        }
    }

    struct RegionData
    {
        Veci2 coord{};
        // one mesh per material (rs2::repack_by_material over all planes,
        // terrain + loc chunks) → one draw per material per region
        std::vector<rs2::Mesh> meshes;
    };

#if defined(FEATURE_ENABLE_BULLET3)
    // A collision shape built on the worker thread (BVH build included),
    // ready for the physics system to adopt on the main thread.
    struct PhysicsRegion
    {
        Veci2 coord{};
        std::shared_ptr<Physics::BodyCreationPrebuilt> body;
    };
#endif

    // First-fit range allocator with coalescing free — buffers become a
    // pool of per-region slots so a new region only writes its own range.
    struct Range
    {
        u32 offset{}, size{};
    };

    struct RangeAllocator
    {
        explicit RangeAllocator(u32 capacity) : free_list{{0, capacity}}
        {
        }

        std::optional<u32> alloc(u32 size)
        {
            for(auto it = free_list.begin(); it != free_list.end(); ++it)
                if(it->size >= size)
                {
                    u32 offset = it->offset;
                    it->offset += size;
                    it->size -= size;
                    if(it->size == 0)
                        free_list.erase(it);
                    return offset;
                }
            return std::nullopt;
        }

        void free(Range r)
        {
            // insert sorted by offset, merge with neighbours
            auto it = std::lower_bound(
                free_list.begin(), free_list.end(), r.offset,
                [](Range const& a, u32 off) { return a.offset < off; });
            it = free_list.insert(it, r);
            if(auto next = it + 1;
               next != free_list.end() &&
               it->offset + it->size == next->offset)
            {
                it->size += next->size;
                free_list.erase(next);
            }
            if(it != free_list.begin())
            {
                auto prev = it - 1;
                if(prev->offset + prev->size == it->offset)
                {
                    prev->size += it->size;
                    free_list.erase(it);
                }
            }
        }

        std::vector<Range> free_list;
    };

    struct ResidentRegion
    {
        std::shared_ptr<RegionData> data;
        Range                       vrange{}, erange{};
        std::vector<gfx::draw_command::data_t> draws;
        std::vector<i32> draw_textures; // per draw: texture id, -1 = flat
    };

    static u64 region_key(i32 rx, i32 ry)
    {
        return u64(u32(rx)) << 32 | u32(ry);
    }

    static bool in_ring(Veci2 coord, Veci2 center)
    {
        return std::abs(coord.x - center.x) <= 1 &&
               std::abs(coord.y - center.y) <= 1;
    }

    // Main thread: evict regions outside the new 3×3, request missing ones
    // from the worker. Already-resident regions are untouched — a border
    // cross only builds the new row/column.
    void request_surrounding(i32 rx, i32 ry)
    {
        wanted_center = {rx, ry};

        bool dirty = false;
        for(auto it = resident.begin(); it != resident.end();)
            if(!in_ring(it->second.data->coord, wanted_center))
            {
                // free the GPU slots; no data movement
                vertex_alloc.free(it->second.vrange);
                element_alloc.free(it->second.erange);
                it    = resident.erase(it);
                dirty = true;
            }
            else
                ++it;

#if defined(FEATURE_ENABLE_BULLET3)
        // evict physics bodies of regions leaving the ring
        for(auto it = physics_bodies.begin(); it != physics_bodies.end();)
        {
            Veci2 coord{i32(*it >> 32), i32(u32(*it))};
            if(!in_ring(coord, wanted_center))
            {
                Physics::Event       event{.type = Physics::Event::BodyRemoval};
                Physics::BodyRemoval removal{.entity_id = *it};
                physics.process(event, &removal);
                it = physics_bodies.erase(it);
            }
            else
                ++it;
        }
#endif

        for(i32 x = rx - 1; x <= rx + 1; ++x)
            for(i32 y = ry - 1; y <= ry + 1; ++y)
            {
                u64 k = region_key(x, y);
                if(resident.contains(k) || pending.contains(k))
                    continue;
                pending.insert(k);
                auto queued = rq::runtime_queue::QueueImmediate(
                    worker, rq::detail::duration(), [this, x, y] {
                        // worker thread: sole owner of `loader`. Build all
                        // planes, then regroup into one mesh per material
                        // so the GPU thread gets batch-ready geometry.
                        std::vector<rs2::RegionGeometry> planes;
                        for(auto plane : stl_types::range<>(4))
                            if(auto region = loader.load(x, y, plane))
                                planes.push_back(std::move(region.value()));

                        std::vector<rs2::Mesh const*> parts;
                        for(auto const& plane : planes)
                        {
                            parts.push_back(&plane.terrain);
                            for(auto const& chunk : plane.locs)
                                parts.push_back(&chunk);
                        }

                        auto data   = std::make_shared<RegionData>();
                        data->coord = {x, y};
                        data->meshes = rs2::repack_by_material(parts);
                        on_built(std::move(data));

#if defined(FEATURE_ENABLE_BULLET3)
                        // Collision set: collidable triangles only, chunked
                        // for u16, scaled into engine space. The BVH build
                        // (the expensive part) happens HERE on the worker;
                        // the main thread only wraps it in a rigid body.
                        auto physics_data = std::make_shared<RegionData>();
                        physics_data->coord = {x, y};
                        physics_data->meshes = rs2::repack_by_material<size_t>(
                            parts,
                            rs2::sorting_method::by_u16_chunk,
                            rs2::filter_method::collidable_only);
                        for(auto& mesh : physics_data->meshes)
                        {
                            for(auto& vert : mesh.vertices)
                            {
                                vert.x /= world_scale;
                                vert.y /= world_scale;
                                vert.z /= world_scale;
                            }
                        }

                        u32 phys_tris = 0;
                        for(auto const& mesh : physics_data->meshes)
                            phys_tris += mesh.indices.size() / 3;
                        if(phys_tris > 0)
                        {
                            auto body = std::make_shared<
                                Physics::BodyCreationPrebuilt>();
                            body->entity_id = region_key(x, y);
                            body->mesh_iface =
                                std::make_unique<btTriangleIndexVertexArray>();
                            for(auto const& mesh : physics_data->meshes)
                            {
                                btIndexedMesh part;
                                part.m_numTriangles = mesh.indices.size() / 3;
                                part.m_triangleIndexBase =
                                    reinterpret_cast<const unsigned char*>(
                                        mesh.indices.data());
                                part.m_triangleIndexStride = sizeof(u16) * 3;
                                part.m_numVertices = mesh.vertices.size();
                                part.m_vertexBase =
                                    reinterpret_cast<const unsigned char*>(
                                        mesh.vertices.data());
                                part.m_vertexStride = sizeof(rs2::Vertex);
                                part.m_vertexType   = PHY_FLOAT;
                                part.m_indexType    = PHY_SHORT;
                                body->mesh_iface->addIndexedMesh(
                                    part, PHY_SHORT);
                            }
                            body->shape =
                                std::make_unique<btBvhTriangleMeshShape>(
                                    body->mesh_iface.get(), true);
                            // spans above point into these meshes
                            body->keep_alive = physics_data;

                            auto payload    = std::make_shared<PhysicsRegion>();
                            payload->coord  = {x, y};
                            payload->body   = std::move(body);
                            on_physics_built(std::move(payload));
                        }
#endif
                    });
                if(queued.has_error())
                    pending.erase(k);
            }

        if(dirty)
            refresh_draws();
    }

    // Main (GPU) thread, via on_built: adopt a finished region if it is
    // still wanted, upload it into its own slots, refresh the draw list.
    void integrate_region(std::shared_ptr<RegionData> data)
    {
        u64 k = region_key(data->coord.x, data->coord.y);
        pending.erase(k);
        if(!in_ring(data->coord, wanted_center))
            return; // moved on while this was building — drop it
        if(data->meshes.empty())
            return; // not in the map index (ocean)

        // total size over all material meshes of the region
        u32 vcount = 0, ecount = 0;
        for(auto const& mesh : data->meshes)
        {
            vcount += mesh.vertices.size();
            ecount += mesh.indices.size();
        }
        if(ecount == 0)
            return;

        auto voff = vertex_alloc.alloc(vcount);
        auto eoff = element_alloc.alloc(ecount);
        if(!voff || !eoff)
        {
            if(voff)
                vertex_alloc.free({*voff, vcount});
            if(eoff)
                element_alloc.free({*eoff, ecount});
            cWarning("RS2: out of buffer space for region {},{} "
                     "({} verts, {} indices)",
                data->coord.x, data->coord.y, vcount, ecount);
            return;
        }

        ResidentRegion region{
            .data   = std::move(data),
            .vrange = {*voff, vcount},
            .erange = {*eoff, ecount},
        };

        // upload ONLY this region's ranges — resident regions untouched.
        // Map JUST this region's byte ranges: a whole-buffer map/unmap is
        // not guaranteed to preserve unwritten contents (write-oriented
        // maps may hand back scratch memory), which progressively
        // corrupted previously-uploaded regions — geometry vanished as
        // later regions integrated. We fully overwrite the mapped ranges,
        // so scratch semantics are safe here.
        auto verts = vertex_buf->map<rs2::Vertex>(
            size_t(region.vrange.offset) * sizeof(rs2::Vertex),
            size_t(region.vrange.size) * sizeof(rs2::Vertex));
        auto elements = element_buf->map<u16>(
            size_t(region.erange.offset) * sizeof(u16),
            size_t(region.erange.size) * sizeof(u16));
        // vptr/eptr are GLOBAL buffer positions (for the draw records);
        // the mapped spans are range-local, so copies index from 0.
        u32 vptr = region.vrange.offset, eptr = region.erange.offset;
        u32 vloc = 0, eloc = 0;
        for(auto const& mesh : region.data->meshes)
        {
            if(mesh.indices.empty())
                continue;
            region.draws.push_back({
                .elements = {
                    .count  = static_cast<u32>(mesh.indices.size()),
                    .offset = eptr * sizeof(u16),
                    .vertex_offset = vptr,
                    .type   = semantic::type_t::u16,
                },
            });
            region.draw_textures.push_back(
                mesh.tri_texture.empty() ? -1 : mesh.tri_texture.front());
            std::copy(
                mesh.vertices.begin(), mesh.vertices.end(),
                verts.begin() + vloc);
            std::copy(
                mesh.indices.begin(), mesh.indices.end(),
                elements.begin() + eloc);
            vptr += mesh.vertices.size();
            eptr += mesh.indices.size();
            vloc += mesh.vertices.size();
            eloc += mesh.indices.size();
        }
        for(auto tex : region.draw_textures)
        {
            if(tex == -1)
                continue;
            auto tex_data_ = loader.texture_sprite(tex);
            if(!tex_data_)
                continue;
            auto tex_data = *tex_data_;
            region_tex->upload(
                tex_data.rgba,
                Veci3{0, 0, tex},
                size_3d<i32>{tex_data.width, tex_data.height, 1}
            );
        }
        vertex_buf->unmap();
        element_buf->unmap();

        resident.emplace(k, std::move(region));

        // A/B probe: re-upload EVERY resident region after integrating.
        // If the world stops vanishing with this on, buffer contents do
        // not survive map/unmap cycles and slot-writes are invalid.
        if(std::getenv("RS2_REWRITE_ALL"))
        {
            auto va = vertex_buf->map<rs2::Vertex>(0);
            auto ea = element_buf->map<u16>(0);
            for(auto const& [rk, rr] : resident)
            {
                u32 vp = rr.vrange.offset, ep = rr.erange.offset;
                for(auto const& mesh : rr.data->meshes)
                {
                    if(mesh.indices.empty())
                        continue;
                    std::copy(
                        mesh.vertices.begin(), mesh.vertices.end(),
                        va.begin() + vp);
                    std::copy(
                        mesh.indices.begin(), mesh.indices.end(),
                        ea.begin() + ep);
                    vp += mesh.vertices.size();
                    ep += mesh.indices.size();
                }
            }
            vertex_buf->unmap();
            element_buf->unmap();
        }

        refresh_draws();
    }

    // Main (GPU) thread: draw list = concatenation of per-region draws,
    // with a parallel per-draw texture id list. No buffer traffic.
    void refresh_draws()
    {
        region_draws.clear();
        region_draw_textures.clear();
        for(auto const& [k, region] : resident)
        {
            region_draws.insert(
                region_draws.end(), region.draws.begin(),
                region.draws.end());

            region_draw_textures.insert(
                region_draw_textures.end(), region.draw_textures.begin(),
                region.draw_textures.end());
        }
        u32 instance_offset = 0;
        for(auto& draw : region_draws)
        {
            draw.instances = {
                .count = 1,
                .offset = instance_offset++,
            };
        }
        cDebug("RS2: {} resident regions ({} pending), {} draws",
            resident.size(), pending.size(), region_draws.size());
    }

    Vecf2 to_rs2(Vecf2 world_pos)
    {
        return world_scale * world_pos;
    }

    Veci2 to_rs2_region(Vecf2 world_pos)
    {
        // RS2 world is +x east, +y north, z-up — same handedness as the
        // engine, no flip. floor() so slightly-negative positions don't
        // round toward region 0.
        auto rs_space = to_rs2(world_pos);
        return Veci2(
            i32(std::floor(rs_space.x / 8192.f)),
            i32(std::floor(rs_space.y / 8192.f)));
    }

    virtual void start_restricted(Proxy& p, time_point const& t) final 
    {
        PlayerCamera* camera{};
        for(auto const& e : p.select<PlayerCamera>())
        {
            PlayerInfo* info = p.get<PlayerInfo>(e.id);
            if(info->seat_idx != 0)
                continue;
            camera = p.get<PlayerCamera>(e.id);
            break;
        }

        if(!camera)
            return;

        auto vert_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Vertex,
            gfx::uniform_pair<Matf4 const>{
                {"camera"}, semantic::SpanOne<const Matf4>(camera->matrix)
            },
            gfx::uniform_pair<const f32>{
                {"world_scale"}, semantic::SpanOne(world_scale)
            });
        auto frag_u = gfx::make_uniform_list(
            typing::graphics::ShaderStage::Fragment,
            gfx::uniform_pair<const i32>{
                {"f_texture_ids"},
                semantic::Span<const i32>(
                    region_draw_textures.data(),
                    std::min<size_t>(region_draw_textures.size(), 256)),
            });
        auto frag_sampler = gfx::make_sampler_list(
            gfx::sampler_definition_t{
                typing::graphics::ShaderStage::Fragment,
                {"f_textures"},
                region_sampler,
            });

        auto current_region = Veci2{last_region.x, last_region.y};
        if(ImGui::Begin("Cursed Control"))
        {
            // ImGui::InputFloat("World scale", &world_scale);
            ImGui::SliderInt2("Region coords", &current_region.x, 0, 100);
            {
                auto const& pos = camera->camera->position;
                ImGui::Text("World position: (%f, %f, %f)", pos.x, pos.y, pos.z);
            }
            {
                auto pos = camera->camera->position * world_scale;
                ImGui::Text("Runescape space: (%f, %f, %f)", pos.x, pos.y, pos.z);
            }
            ImGui::Text("Resident regions: %zu (%zu pending)",
                resident.size(), pending.size());
            if(!resident.empty() &&
               !resident.begin()->second.data->meshes.empty() &&
               !resident.begin()
                    ->second.data->meshes[0]
                    .vertices.empty())
            {
                auto const& v = resident.begin()
                                    ->second.data->meshes[0]
                                    .vertices[0];
                Vecf3 probe_pos = {v.x, v.y, v.z};
                ImGui::Text("Probe position: (%f, %f, %f)",
                    probe_pos.x, probe_pos.y, probe_pos.z);
                if(ImGui::Button("Warp to probe"))
                {
                    GameEvent ev{GameEvent::PlayerTeleport};
                    PlayerTeleportEvent teleport{
                        .seat_idx = 0,
                        .position = probe_pos / world_scale,
                    };
                    p.subsystem<GameEventBus>().process(ev, &teleport);
                }
            }
            if(ImGui::Button("Lumbridge teleport"))
            {
                GameEvent ev{GameEvent::PlayerTeleport};
                PlayerTeleportEvent teleport{
                    .seat_idx = 0,
                    .position = Vecf3{412000, 412000, 1000} / world_scale,
                };
                p.subsystem<GameEventBus>().process(ev, &teleport);
            }
            if(ImGui::Button("Varrock teleport"))
            {
                GameEvent ev{GameEvent::PlayerTeleport};
                PlayerTeleportEvent teleport{
                    .seat_idx = 0,
                    .position = Vecf3{411320, 438290, 1000} / world_scale,
                };
                p.subsystem<GameEventBus>().process(ev, &teleport);
            }
            if(ImGui::Button("Falador teleport"))
            {
                GameEvent ev{GameEvent::PlayerTeleport};
                PlayerTeleportEvent teleport{
                    .seat_idx = 0,
                    .position = Vecf3{379550, 432380, 1000} / world_scale,
                };
                p.subsystem<GameEventBus>().process(ev, &teleport);
            }
            if(ImGui::Button("Ardougne teleport"))
            {
                GameEvent ev{GameEvent::PlayerTeleport};
                PlayerTeleportEvent teleport{
                    .seat_idx = 0,
                    .position = Vecf3{340500, 423000, 1000} / world_scale,
                };
                p.subsystem<GameEventBus>().process(ev, &teleport);
            }
            if(ImGui::Button("Spawn physics probe"))
            {
                Physics::Event event{.type = Physics::Event::ProbeHere};
                physics.process(event, nullptr);
            }
        }
        ImGui::End();

        current_region = to_rs2_region(camera->camera->position);
        if(current_region != last_region)
        {
            request_surrounding(current_region.x, current_region.y);
            last_region = current_region;
        }

        auto res = api.submit({
                .program = region_program,
                .vertices = region_vao,
                .render_target = p.template subsystem<BlamResources>().offscreen,
                .call = {.indexed = true, .instanced = true},
                .data = region_draws,
            },
            vert_u,
            frag_u,
            frag_sampler,
            gfx::view_state{
                .depth = gfx::depth_state{.reversed = true},
            });
        if(res.has_value())
            cWarning("RS2: Failed to draw: {}", std::get<1>(*res));
    }

    virtual void end_restricted(Proxy&, time_point const& t) final {}

    gfx::api& api;
    PhysicsBus& physics;
    compo::EntityRef<compo::EntityContainer> world_entity;
    
    std::shared_ptr<gfx::vertex_array_t>        region_vao;
    std::vector<gfx::draw_command::data_t>      region_draws;
    // parallel to region_draws: texture id per draw, -1 = flat colour
    std::vector<i32>                            region_draw_textures;
    std::shared_ptr<gfx::buffer_t>              vertex_buf;
    std::shared_ptr<gfx::buffer_t>              element_buf;
    std::shared_ptr<gfx::compat::texture_2da_t> region_tex;
    std::shared_ptr<gfx::sampler_t>             region_sampler;
    std::shared_ptr<gfx::program_t>             region_program;

    static constexpr u32 vertex_capacity =
        64 * 1024 * 1024 / sizeof(rs2::Vertex);
    static constexpr u32 element_capacity = 32 * 1024 * 1024 / sizeof(u16);

    // `loader` is only touched from the worker queue after construction
    rs2::RegionLoader loader;
    rq::runtime_queue* worker{};
    std::function<void(std::shared_ptr<RegionData>)> on_built;
#if defined(FEATURE_ENABLE_BULLET3)
    std::function<void(std::shared_ptr<PhysicsRegion>)> on_physics_built;
    // region keys with a live physics body (entity_id = region_key)
    std::set<u64> physics_bodies;
#endif

    RangeAllocator vertex_alloc{vertex_capacity};
    RangeAllocator element_alloc{element_capacity};

    std::map<u64, ResidentRegion> resident;
    std::set<u64>                 pending;
    Veci2                         wanted_center{};

    const f32 world_scale{200.f};

    // sentinel: first frame always triggers a request
    Veci2 last_region{INT32_MIN, INT32_MIN};
};

void setup_cursed_loaders(compo::EntityContainer& e)
{
    if(auto cache = platform::env::var("RS2_CACHE"))
    {
        auto& physics_bus = e.subsystem_cast<PhysicsBus>();
        compo::EntityRecipe recipe = {
            .components = {
                compo::type_hash_v<PhysicsData>(),
                compo::type_hash_v<DebugDraw>(),
            },
        };
        auto world_entity = e.create_entity(recipe);
        e.register_subsystem_inplace<RS2CacheLoader>(
            std::ref(e.subsystem_cast<gfx::system>()),
            std::ref(physics_bus),
            world_entity,
            cache.value());
    }
}

}
