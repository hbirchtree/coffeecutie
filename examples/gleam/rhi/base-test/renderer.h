#include <coffee/asio/asio_worker.h>
#include <coffee/components/components.h>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/coffee_saving.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/chunk.h>
#include <coffee/core/types/graphics_types.h>
#include <coffee/core/url.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/graphics/common/query/gpu_query.h>
#include <coffee/image/cimage.h>
#include <coffee/interfaces/cgraphics_util.h>
#include <coffee/windowing/renderer/renderer.h>

#if defined(FEATURE_ENABLE_CoffeeASIO)
#include <coffee/asio/net_resource.h>
#endif

#if defined(FEATURE_ENABLE_DiscordLatte)
#include <coffee/discord/discord_binding.h>
#endif

#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

//#define USE_NULL_RENDERER

using namespace Coffee;
using namespace Display;
using namespace SceneGraph;
using namespace Coffee::RHI::Datatypes;

using CDRenderer = CSDL2Renderer;
using GLM        = GLEAMAPI;

struct RuntimeState
{
    CGCamera camera;

    i64  time_base     = 0.;
    bool debug_enabled = false;
    u8   padding[7];
};

static const constexpr szptr num_textures = 5;

struct TransformPair
{
    Transform first;
    Matf4     second;
    Vecf3     mask;
};

using TransformTag = Components::TagType<TransformPair, i32>;
using CameraTag    = Components::TagType<CGCamera, i32>;
using TimeTag      = Components::TagType<Chrono::seconds_float, i32>;

class TransformContainer : public Components::ComponentContainer<TransformTag>
{
    Map<u64, TransformPair> m_objects;
    Span<Matf4>             m_matrices;

  public:
    TransformContainer(Span<Matf4>&& matrices) : m_matrices(std::move(matrices))
    {
    }

    virtual void register_entity(u64 id) override
    {
        m_objects[id] = {};
    }
    virtual void unregister_entity(u64 id) override
    {
        m_objects.erase(id);
    }
    virtual TransformPair* get(u64 id) override
    {
        return &m_objects[id];
    }
    virtual bool visit(
        Components::ContainerProxy& container,
        Components::Entity&         entity) override
    {
        auto camera = container.subsystem<CameraTag>().get();

        camera.position.x() -= 0.1f;

        auto  projection    = GenPerspective<scalar>(camera);
        auto& object_matrix = m_objects[entity.id].second;

        auto left_idx  = (entity.id - 1) * 2;
        auto right_idx = left_idx + 1;

        auto output_matrix =
            projection * GenTransform<scalar>(camera) * object_matrix;

        m_matrices[left_idx] = output_matrix;

        camera.position.x() += 0.2f;

        m_matrices[right_idx] =
            projection * GenTransform<scalar>(camera) * object_matrix;

        return true;
    }

    virtual void prealloc(szptr) override
    {
    }
    virtual bool contains_entity(u64 id) const override
    {
        return m_objects.find(id) != m_objects.end();
    }
};

class CameraContainer : public Components::Subsystem<CameraTag>
{
    CGCamera* camera;

  public:
    CameraContainer(CGCamera* camera_ptr) : camera(camera_ptr)
    {
        camera->aspect      = 1.6f;
        camera->fieldOfView = 90.f;
        camera->zVals.far_  = 100.f;

        camera->position = {0, 0, -10};
    }

    virtual CGCamera const& get() const
    {
        return *camera;
    }
    virtual CGCamera& get()
    {
        return *camera;
    }
};

class TimeSystem : public Components::Subsystem<TimeTag>
{
  public:
    using system_clock = Chrono::high_resolution_clock;

  private:
    Chrono::seconds_float    loop_time;
    system_clock::time_point start_time;

    system_clock::time_point prev_time;

  public:
    TimeSystem() :
        loop_time(), start_time(system_clock::now()), prev_time(start_time)
    {
    }

    virtual system_clock::time_point& get_start()
    {
        return start_time;
    }

    virtual void start_frame() override
    {
        auto current = system_clock::now();
        loop_time += current - prev_time;
        prev_time = current;
    }
    virtual Chrono::seconds_float const& get() const override
    {
        return loop_time;
    }
    virtual Chrono::seconds_float& get() override
    {
        return loop_time;
    }
};

struct RendererState
{
    RendererState() : camera_cnt(&r_state.camera)
    {
    }

    // State that can be loaded from disk
    RuntimeState          r_state;
    ShPtr<Store::SaveApi> save_api;

    RuntimeQueue*            rt_queue;
    ShPtr<ASIO::ASIO_Worker> net_worker;

    ScopedTask component_task;

    Components::EntityContainer entities;
    CameraContainer             camera_cnt;
    TimeSystem                  timing_sys;

    u32 frameCount;

    struct RGraphicsData
    {
        RGraphicsData() :
            transform_cnt(Span<Matf4>::From(object_matrices, 128)),
            params(eye_pip)
        {
        }

        void reset()
        {
            loader     = nullptr;
            vertbuf    = nullptr;
            vertdesc   = {};
            v_shader   = {};
            f_shader   = {};
            eye_pip    = {};
            eyetex     = nullptr;
            eyesamp    = {};
            time_value = 0;
            blendstate = {};
            deptstate  = {};
            rpass_s    = {};
            rpass      = {};
        }

        TransformContainer transform_cnt;

        // GLEAM data
        GLM::API_CONTEXT loader = nullptr;

        GLM::BUF_A* vertbuf;
        GLM::V_DESC vertdesc = {};

        GLM::SHD                    v_shader = {};
        GLM::SHD                    f_shader = {};
        GLM::PIP                    eye_pip  = {};
        RHI::shader_param_view<GLM> params;

        GLM::S_2DA*  eyetex;
        GLM::SM_2DA  eyesamp        = {};
        const scalar eyetexGridSize = 2.f;

        // Graphics data
        Matf4  object_matrices[128] = {};
        scalar time_value;

        Vecf4 clear_col = {.267f, .267f, .267f, 1.f};

        GLM::D_DATA instdata = {6, 0, 4};

        // View information
        GLM::BLNDSTATE blendstate = {};
        GLM::DEPTSTATE deptstate  = {};
        GLM::D_CALL    call       = {};

        GLM::RenderPass rpass_s = {};
        GLM::OPT_DRAW   rpass   = {};
    } g_data;
};

void SetupRendering(CDRenderer& renderer, RendererState* d)
{
    runtime_queue_error rqec;
    RuntimeQueue::CreateNewQueue("Main");
    d->rt_queue = RuntimeQueue::CreateNewThreadQueue("Component Worker", rqec);
    auto onlineWorker =
        RuntimeQueue::CreateNewThreadQueue("Online Worker", rqec);

    d->net_worker = ASIO::GenWorker();

    C_ERROR_CHECK(rqec);

    RendererState::RGraphicsData& g = d->g_data;

    d->g_data.reset();

    d->save_api = Store::CreateDefaultSave();
    d->save_api->restore(Bytes::Create(d->r_state));

    cVerbose("Entering run() function");

    ProfContext a("Renderer setup");

    const constexpr cstring textures[num_textures] = {"circle_red.png",
                                                      "circle_blue.png",
                                                      "circle_alpha.png",
                                                      "floor-tile.png"};

    const scalar vertexdata[] = {
        -1.f, -1.f, 0.f,  0.f,  0.f, 1.f, -1.f, 0.f,
        1.f,  0.f,  -1.f, 1.f,  0.f, 0.f, 1.f,

        -1.f, 1.f,  0.f,  0.f,  1.f, 1.f, 1.f,  0.f,
        1.f,  1.f,  1.f,  -1.f, 0.f, 1.f, 0.f,
    };

    cVerbose("Loading GLeam API");
    /*
     * Loading the GLeam API, chosen according to what is available at runtime
     */
    GLM::OPTS load_opts      = {};
    load_opts.crash_on_error = true;
    g.loader                 = GLM::GetLoadAPI(load_opts);
    if(!g.loader(PlatformData::IsDebug()))
    {
        cDebug("Failed to load GLEAM API");
        return;
    }

    g.vertbuf      = new GLM::BUF_A(RSCA::ReadOnly, sizeof(vertexdata));
    auto& vertbuf  = *g.vertbuf;
    auto& vertdesc = g.vertdesc;

    cVerbose("Entering GL initialization stage");
    /* Uploading vertex data and creating descriptors */
    {
        Profiler::PushContext("Vertex data upload");

        vertdesc.alloc();
        vertbuf.alloc();

        vertbuf.commit(sizeof(vertexdata), vertexdata);

        /*
         * Specifying a vertex format which is applied.
         * This is driver- and API-agnostic
         */
        GLM::V_ATTR pos = {};
        pos.m_size      = 3;
        pos.m_type      = TypeEnum::Scalar;
        pos.m_stride    = sizeof(Vecf3) + sizeof(Vecf2);

        GLM::V_ATTR tc = {};
        tc.m_idx       = 1;
        tc.m_size      = 2;
        tc.m_off       = sizeof(Vecf3);
        tc.m_type      = TypeEnum::Scalar;
        tc.m_stride    = sizeof(Vecf3) + sizeof(Vecf2);

        /* Finally we add these attributes to the descriptor */
        vertdesc.addAttribute(pos);
        vertdesc.addAttribute(tc);

        Profiler::PopContext();
    }

    cVerbose("Generated vertex buffers");

    auto& eye_pip = g.eye_pip;

    /* Compiling shaders and assemble a graphics pipeline */

    do
    {
        ProfContext             b("Shader compilation");
        const constexpr cstring shader_files[] = {"vr/vshader.glsl",
                                                  "vr/fshader.glsl",
                                                  "vr/vshader_es.glsl",
                                                  "vr/fshader_es.glsl",
                                                  "vr/vshader_es2.glsl",
                                                  "vr/fshader_es2.glsl"};

        bool isGles = (GLM::LevelIsOfClass(GLM::Level(), GLM::APIClass::GLES));
        bool isGles20 = GLM::Level() == RHI::GLEAM::GLES_2_0;

        Resource v_rsc(shader_files[isGles * 2 + isGles20 * 2]);
        Resource f_rsc(shader_files[isGles * 2 + isGles20 * 2 + 1]);

        if(!RHI::LoadPipeline<GLM>(eye_pip, v_rsc, f_rsc))
        {
            cFatal("Failed to load shaders");
        }

    } while(false);
    cVerbose("Compiled shaders");

    /* Uploading textures */
    g.eyetex = new GLM::S_2DA(
        PixFmt::RGBA8, 1, GLM::TextureDMABuffered | GLM::TextureAutoMipmapped);
    auto& eyetex = *g.eyetex;

    eyetex.allocate({1024, 1024, 4}, PixCmp::RGBA);
    cVerbose("Texture allocation");

    Profiler::PushContext("Texture loading");
    for(i32 i = 0; i < eyetex.m_size.depth; i++)
    {
        Resource rsc(textures[i], RSCA::AssetFile);

        if(!RHI::LoadTexture<GLM>(eyetex, std::move(rsc), i))
            return;
    }

#if defined(FEATURE_ENABLE_CoffeeASIO)
    /* We download a spicy meme and paste it into the texture */
    if(Net::Supported())
    {
        auto rsc = "http://i.imgur.com/nQdOmCJ.png"_http.rsc<Net::Resource>(
            ASIO::GetContext());

        if(rsc.fetch())
        {
            auto imdata = rsc.data();

            stb::image_rw img;
            stb::LoadData(
                &img, BytesConst(imdata.data, imdata.size, imdata.size));

            eyetex.upload(
                BitFmt::UByte,
                PixCmp::RGBA,
                {img.size.w, img.size.h, 1},
                img.data,
                {0, 0, 0});
        } else
        {
            cWarning("Failed to retrieve spicy meme");
        }
    }

#if defined(FEATURE_ENABLE_DiscordLatte)
    auto callbacks = MkShared<Discord::DiscordDelegate>();

    auto mainQueue   = RuntimeQueue::GetCurrentQueue(rqec);
    callbacks->ready = [mainQueue, &eyetex](Discord::PlayerInfo const& info) {
        cDebug("Identified as: {0}", info.userTag);

        auto rsc = Net::Resource(ASIO::GetContext(), info.avatarUrl);

        if(rsc.fetch())
        {
            auto imdata = rsc.data();

            /* TODO: Make this a move operation */
            ShPtr<stb::image_rw> img = MkShared<stb::image_rw>();
            stb::LoadData(
                img.get(), BytesConst(imdata.data, imdata.size, imdata.size));

            auto imageUpload = [img, &eyetex]() {
                cDebug("Uploading");
                eyetex.upload(
                    PixDesc(eyetex.m_pixfmt, BitFmt::UByte, PixCmp::RGBA),
                    {img->size.w, img->size.h, 1},
                    C_OCAST<Bytes>(*img),
                    {});
            };

            runtime_queue_error ec;
            RuntimeQueue::QueueShot(
                mainQueue, Chrono::seconds(1), imageUpload, ec);
        }
    };

    auto service =
        Discord::CreateService({"468164529617109002", 256}, callbacks);

    service->getPresence()->put({"", 1, 1, {}, {}});
    service->getGame()->put(platform::online::GameDelegate::Builder(
        {}, "Messing around", MkUrl("coffee_cup")));

    RuntimeQueue::QueuePeriodic(
        onlineWorker,
        Chrono::milliseconds(100),
        [service]() {
            ProfContext _("Discord poll");
            service->poll();
        },
        rqec);
#endif
#endif

    Profiler::PopContext();
    cVerbose("Uploading textures");

    /* Attaching the texture data to a sampler object */
    auto& eyesamp = g.eyesamp;
    eyesamp       = {};
    eyesamp.alloc();
    eyesamp.attach(&eyetex);
    eyesamp.setFiltering(Filtering::Linear, Filtering::Linear);
    cVerbose("Setting sampler properties");

    /* We create some pipeline state, such as blending and viewport state */
    auto& blendstate = g.blendstate;
    auto& deptstate  = g.deptstate;

    blendstate.m_doBlend = true;
    deptstate.m_test     = true;
    deptstate.m_func     = C_CAST<u32>(ValueComparison::Less);

    /* Applying state information */
    GLM::SetBlendState(g.blendstate);
    GLM::SetDepthState(deptstate);
    cVerbose("Set renderer state");

    /* We query the current pipeline for possible uniform/texture/buffer values
     */
    g.params.get_pipeline_params();

    for(auto& constant : g.params.constants())
    {
        if(constant.m_name == "transform")
            g.params.set_constant(constant, Bytes::From(g.object_matrices, 6));
        else if(constant.m_name == "texdata")
            g.params.set_sampler(constant, g.eyesamp.handle());
        else if(constant.m_name == "mx")
            g.params.set_constant(constant, Bytes::Create(g.time_value));
        else if(constant.m_name == "texdata_gridSize")
            g.params.set_constant(constant, Bytes::Create(g.eyetexGridSize));
    }

    g.params.build_state();

    cVerbose("Acquire and set shader uniforms");

    /* Vertex descriptors are based upon the ideas from GL4.3 */
    vertdesc.bind();
    vertdesc.bindBuffer(0, vertbuf);

    GLM::PreDrawCleanup();

    g.rpass_s.pipeline    = &g.eye_pip;
    g.rpass_s.blend       = &g.blendstate;
    g.rpass_s.depth       = &g.deptstate;
    g.rpass_s.framebuffer = &GLM::DefaultFramebuffer();

    g.call.m_inst = true;

    g.rpass_s.draws.push_back(
        {&g.vertdesc, &g.params.get_state(), g.call, g.instdata});

    GLM::OptimizeRenderPass(g.rpass_s, g.rpass);

    d->entities.register_component(g.transform_cnt);
    d->entities.register_subsystem(d->camera_cnt);
    d->entities.register_subsystem(d->timing_sys);

    d->timing_sys.get_start() = TimeSystem::system_clock::time_point(
        Chrono::milliseconds(d->r_state.time_base));

    Components::EntityRecipe floor_object;
    Components::EntityRecipe base_object;

    floor_object.components = {typeid(TransformTag).hash_code()};
    floor_object.interval   = Chrono::milliseconds(10);
    base_object             = floor_object;

    floor_object.loop = [](Components::ContainerProxy& c) {
        auto& xf = c.get<TransformTag>();

        auto time = c.subsystem<TimeTag>().get();

        xf.first.position.x() = CMath::sin(time.count() * 2.f) * xf.mask.x();
        xf.first.position.y() = CMath::cos(time.count() * 2.f) * xf.mask.y();

        xf.first.rotation.y() = CMath::sin(time.count());
        xf.first.rotation     = normalize_quat(xf.first.rotation);

        xf.second = GenTransform<scalar>(xf.first);
    };

    base_object.loop = [](Components::ContainerProxy& c) {
        auto& xf   = c.get<TransformTag>();
        auto  time = c.subsystem<TimeTag>().get().count();

        xf.first.position.x() = CMath::sin(time * 4.f);

        xf.second = GenTransform<scalar>(xf.first);
    };

    floor_object.tags = 0x1;
    base_object.tags  = 0x2;

    d->entities.create_entity(base_object);
    d->entities.create_entity(floor_object);

    for(auto& entity : d->entities.select(0x1))
    {
        auto& xf = *d->entities.get<TransformTag>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = {1.5};

        xf.mask.x() = 2;
        xf.mask.y() = 2;
    }

    for(auto& entity : d->entities.select(0x2))
    {
        auto& xf = *d->entities.get<TransformTag>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = {1};

        xf.mask.x() = 1;
        xf.mask.y() = 0.2f;
    }

    d->entities.exec();

    d->component_task = ScopedTask(
        d->rt_queue->threadId(),
        {[d]() {
             Profiler::PushContext("Components::exec()");
             d->entities.exec();
             Profiler::PopContext();
         },
         {},
         std::chrono::milliseconds(10),
         RuntimeTask::Periodic,
         0});

    GpuInfo::GpuQueryInterface interf;
    gpu_query_error            ec;
    GpuInfo::LoadDefaultGpuQuery(interf, ec);

    if(!ec)
    {
        for(auto dev : GpuInfo::GpuQueryView(interf))
            cDebug("Video memory: {0}:{1}", dev.mem().total, dev.mem().free);
    }
}

void RendererLoop(CDRenderer& renderer, RendererState* d)
{
    DProfContext _("Render loop");

    runtime_queue_error ec;

    auto& g = d->g_data;

    renderer.pollEvents();

    auto const& component_task = d->component_task;
    {
        DProfContext __("Exclusive context");
        RuntimeQueue::Block(component_task.threadId(), component_task.id(), ec);

        g.time_value = 0.f
            /*(CMath::sin(d->entities.subsystem<TimeTag>().get().count()) / 2.f)
               + 0.5f*/
            ;

        GLM::DefaultFramebuffer().use(FramebufferT::All);
        GLM::DefaultFramebuffer().clear(0, g.clear_col, 1.);

        GLM::MultiDraw(g.eye_pip, g.rpass);

        RuntimeQueue::Unblock(
            component_task.threadId(), component_task.id(), ec);
    }

    renderer.swapBuffers();
}

void RendererCleanup(CDRenderer&, RendererState* d)
{
    runtime_queue_error ec;
    RuntimeQueue::TerminateThread(d->rt_queue, ec);

    d->net_worker->stop();

    d->entities.reset();

    auto& g = d->g_data;

    g.vertbuf->dealloc();
    g.eyetex->dealloc();
    g.vertdesc.dealloc();
    g.v_shader.dealloc();
    g.f_shader.dealloc();
    g.eye_pip.dealloc();
    g.eyesamp.dealloc();

    delete g.vertbuf;
    g.vertbuf = nullptr;

    delete g.eyetex;
    g.eyetex = nullptr;

    g.loader = nullptr;
    GLM::UnloadAPI();

    cDebug("Saving time: {0}", d->r_state.time_base);

    d->r_state.time_base = Chrono::duration_cast<Chrono::milliseconds>(
                               (d->timing_sys.get_start() + d->timing_sys.get())
                                   .time_since_epoch())
                               .count();
    d->save_api->save(Bytes::Create(d->r_state));
}
