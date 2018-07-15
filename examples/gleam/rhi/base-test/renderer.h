#include <coffee/CGraphics>
#include <coffee/CSDL2>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/base/files/url.h>
#include <coffee/core/base/types/counter.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/image/cimage.h>

#include <coffee/components/components.h>

#include <coffee/interfaces/cgraphics_util.h>

#if defined(FEATURE_ENABLE_CoffeeASIO)
#include <coffee/asio/net_resource.h>
#endif

#include <coffee/core/coffee_saving.h>
#include <coffee/core/platform_data.h>

#include <coffee/graphics/common/query/gpu_query.h>

#include <coffee/core/task_queue/task.h>

//#define USE_NULL_RENDERER

using namespace Coffee;
using namespace Display;

using CDRenderer = CSDL2Renderer;
using GLM        = GLEAMAPI;

struct RuntimeState
{
    bigscalar time_base     = 0.;
    bool      debug_enabled = false;
    uint8     padding[7];
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
        auto camera = *container.get<CameraTag>(entity.id);

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

class CameraContainer : public Components::ComponentContainer<CameraTag>
{
    CGCamera camera;

  public:
    CameraContainer()
    {
        camera.aspect      = 1.6f;
        camera.fieldOfView = 90.f;
        camera.zVals.far_  = 100.f;

        camera.position = {0, 0, -10};
    }

    virtual void register_entity(u64) override
    {
    }
    virtual void unregister_entity(u64) override
    {
    }
    virtual CGCamera* get(u64) override
    {
        return &camera;
    }
    virtual void prealloc(szptr) override
    {
    }
    virtual bool contains_entity(u64) const override
    {
        return true;
    }
};

class TimeSystem : public Components::Subsystem<TimeTag>
{
    using system_clock = Chrono::high_resolution_clock;

    Chrono::seconds_float    loop_time;
    system_clock::time_point start_time;

  public:
    TimeSystem() : loop_time(), start_time(system_clock::now())
    {
    }

    virtual void start_frame() override
    {
        loop_time = system_clock::now() - start_time;
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
    // State that can be loaded from disk
    RuntimeState r_state;

    RuntimeQueue* rt_queue;

    ScopedTask component_task;

    Components::EntityContainer entities;
    CameraContainer             camera_cnt;
    TimeSystem                  timing_sys;

    uint32 frameCount;

    struct RGraphicsData
    {
        RGraphicsData() :
            transform_cnt(Span<Matf4>::From(object_matrices, 128)),
            params(eye_pip)
        {
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

        GLM::S_2DA* eyetex;
        GLM::SM_2DA eyesamp = {};

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

    C_ERROR_CHECK(rqec);

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

    RendererState::RGraphicsData& g = d->g_data;

    //    g = {};

    Store::RestoreMemory(Bytes::Create(d->r_state), 0);

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

        CResources::Resource v_rsc(shader_files[isGles * 2 + isGles20 * 2]);
        CResources::Resource f_rsc(shader_files[isGles * 2 + isGles20 * 2 + 1]);

        if(!RHI::LoadPipeline<GLM>(eye_pip, v_rsc, f_rsc))
        {
            cFatal("Failed to load shaders");
        }

    } while(false);
    cVerbose("Compiled shaders");

    /* Uploading textures */
    g.eyetex = new GLM::S_2DA(
        PixelFormat::RGBA8,
        1,
        GLM::TextureDMABuffered | GLM::TextureAutoMipmapped);
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
        ASIO::AsioContext ctx = ASIO::ASIO_Client::InitService();

        auto rsc =
            "http://i.imgur.com/nQdOmCJ.png"_http.rsc<Net::Resource>(ctx);

        if(rsc.fetch())
        {
            auto imdata = rsc.data();

            stb::image_rw img;
            stb::LoadData(
                &img, BytesConst(imdata.data, imdata.size, imdata.size));

            eyetex.upload(
                BitFormat::UByte,
                PixCmp::RGBA,
                {img.size.w, img.size.h, 1},
                img.data,
                {0, 0, 0});
        } else
        {
            cWarning("Failed to retrieve spicy meme");
        }
    }
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
    deptstate.m_func     = C_CAST<uint32>(ValueComparison::Less);

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

    d->entities.register_component(d->camera_cnt);
    d->entities.register_component(g.transform_cnt);
    d->entities.register_subsystem(d->timing_sys);

    Components::EntityRecipe floor_object;
    Components::EntityRecipe base_object;

    floor_object.components = {typeid(CameraTag).hash_code(),
                               typeid(TransformTag).hash_code()};
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
    ProfContext _("Render loop");

    runtime_queue_error ec;

    auto& g = d->g_data;

    renderer.pollEvents();

    auto const& component_task = d->component_task;
    RuntimeQueue::Block(component_task.threadId(), component_task.id(), ec);
    Profiler::PushContext("Exclusive context");

    g.time_value =
        (CMath::sin(d->entities.subsystem<TimeTag>().get().count()) / 2.f) +
        0.5f;

    GLM::DefaultFramebuffer().use(FramebufferT::All);
    GLM::DefaultFramebuffer().clear(0, g.clear_col, 1.);

    GLM::MultiDraw(g.eye_pip, g.rpass);

    Profiler::PopContext();
    RuntimeQueue::Unblock(component_task.threadId(), component_task.id(), ec);

    renderer.swapBuffers();
}

void RendererCleanup(CDRenderer&, RendererState* d)
{
    runtime_queue_error ec;
    RuntimeQueue::TerminateThread(d->rt_queue, ec);

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
    Store::SaveMemory(Bytes::Create(d->r_state), 0);
}
