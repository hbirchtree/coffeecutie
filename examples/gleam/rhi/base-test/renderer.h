#include <coffee/components/proxy.h>

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
#include <coffee/image/image_coder_system.h>
#include <coffee/interfaces/cgraphics_util.h>
#include <coffee/windowing/renderer/renderer.h>

#include <coffee/components/entity_selectors.h>

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/asio_system.h>
#include <coffee/asio/net_resource.h>
#endif

#if defined(FEATURE_ENABLE_DiscordLatte)
#include <coffee/discord/discord_system.h>
#endif

#include <coffee/interfaces/graphics_subsystem.h>

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

using GfxTag = RHI::Components::AllocatorTag<GLM>;
using GfxSys = RHI::Components::GraphicsAllocator<GLM>;

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

struct CameraData
{
    CGCamera camera_source;

    Matf4 projection;
    Matf4 transform;

    const scalar eye_distance = 0.1f;
};

using TransformTag = Components::TagType<TransformPair>;
using MatrixTag    = Components::TagType<Pair<Matf4*, Matf4*>>;
using CameraTag    = Components::TagType<CameraData>;
using TimeTag      = Components::TagType<Chrono::seconds_float>;
using FrameTag     = Components::TagType<u32>;
using StateTag     = Components::TagType<RuntimeState>;

static constexpr u32 FloorTag    = 0x1;
static constexpr u32 BaseItemTag = 0x2;

using TransformContainer =
    Components::Allocators::VectorContainer<TransformTag>;

struct MatrixContainer
    : Components::Allocators::VectorBaseContainer<MatrixTag, Matf4>
{
    Map<u64, type> m_pairs;

    virtual void register_entity(u64 id)
    {
        VectorBaseContainer::register_entity(id);

        m_data.push_back({});
        m_pairs.insert(
            {id,
             {&m_data.at(m_data.size() - 1), &m_data.at(m_data.size() - 2)}});
    }

    virtual MatrixTag::type* get(u64 id)
    {
        auto it = m_mapping.find(id);

        if(it == m_mapping.end())
            return nullptr;

        auto& out = m_pairs.at(id);

        out = {&m_data.at((*it).second), &m_data.at((*it).second + 1)};

        return &out;
    }
};

class TransformVisitor : public Components::EntityVisitor<
                             Components::TypeList<TransformTag, MatrixTag>,
                             Components::TypeList<CameraTag>>
{
  public:
    TransformVisitor()
    {
    }

    virtual bool visit(
        Proxy& c,
        const Components::Entity&,
        const Components::time_point&) override
    {
        auto camera        = c.subsystem<CameraTag>().get();
        auto camera_source = camera.camera_source;

        camera_source.position.x() -= camera.eye_distance;

        auto& object_matrix = c.get<TransformTag>().second;

        auto output_matrix = camera.projection *
                             GenTransform<scalar>(camera_source) *
                             object_matrix;

        auto& mats = c.get<MatrixTag>();

        *mats.first = output_matrix;

        camera_source.position.x() += camera.eye_distance * 2;

        *mats.second = camera.projection * GenTransform<scalar>(camera_source) *
                       object_matrix;

        return true;
    }
};

class FloorVisitor : public Components::EntityVisitor<
                         Components::TypeList<TransformTag>,
                         Components::TypeList<TimeTag>>
{
  public:
    FloorVisitor() : VisitorType(FloorTag)
    {
    }

    virtual bool visit(
        Proxy& c,
        const Components::Entity&,
        const Components::time_point&) override
    {
        auto& xf = c.get<TransformTag>();

        auto time = c.subsystem<TimeTag>().get();

        xf.first.position.x() = CMath::sin(time.count() * 2.f) * xf.mask.x();
        xf.first.position.y() = CMath::cos(time.count() * 2.f) * xf.mask.y();

        xf.first.rotation.y() = CMath::sin(time.count());
        xf.first.rotation     = normalize_quat(xf.first.rotation);

        xf.second = GenTransform<scalar>(xf.first);

        return true;
    }
};

class BaseItemVisitor : public Components::EntityVisitor<
                            Components::TypeList<TransformTag>,
                            Components::TypeList<TimeTag>>
{
  public:
    BaseItemVisitor() : VisitorType(BaseItemTag)
    {
    }

    virtual bool visit(
        Proxy& c,
        const Components::Entity&,
        const Components::time_point&) override
    {
        auto& xf   = c.get<TransformTag>();
        auto  time = c.subsystem<TimeTag>().get().count();

        xf.first.position.x() = CMath::sin(time * 4.f);

        xf.second = GenTransform<scalar>(xf.first);

        return true;
    }
};

class BasicVisitor : public Components::EntityVisitor<
                         Components::TypeList<TimeTag>,
                         Components::TypeList<TimeTag>>
{
    virtual bool visit(
        Proxy&, Components::Entity const&, Components::time_point const&)
    {
        return true;
    }

  public:
    BasicVisitor(VisitorFlags flags) : VisitorType(0, flags)
    {
    }
};

class Basic2Visitor : public Components::EntityVisitor<
                          Components::TypeList<CameraTag>,
                          Components::TypeList<TimeTag>>
{
    virtual bool visit(
        Proxy&, Components::Entity const&, Components::time_point const&)
    {
        return true;
    }

  public:
    Basic2Visitor(VisitorFlags flags) : VisitorType(0, flags)
    {
    }
};

class CameraContainer : public Components::Globals::ValueSubsystem<CameraTag>
{
  public:
    CameraContainer()
    {
        reset();
    }

    void reset()
    {
        get().camera_source.aspect      = 1.6f;
        get().camera_source.fieldOfView = 90.f;
        get().camera_source.zVals.far_  = 100.f;

        get().camera_source.position = {0, 0, -10};
    }

    virtual void start_frame(ContainerProxy&, time_point const&)
    {
        get().projection = GenPerspective<scalar>(get().camera_source);
        get().transform  = GenTransform<scalar>(get().camera_source);
    }
};

class TimeSystem : public Components::Globals::ValueSubsystem<TimeTag>
{
  public:
    using system_clock = Chrono::high_resolution_clock;

    TimeSystem(system_clock::time_point const& start) :
        start_time(system_clock::now()), prev_time(start_time)
    {
        get_start() = start;
    }

    virtual system_clock::time_point& get_start()
    {
        return start_time;
    }

    virtual void start_frame(
        Components::ContainerProxy&, Components::time_point const&) override
    {
        auto current = system_clock::now();
        get() += current - prev_time;
        prev_time = current;

        current_time = get().count();
    }

    scalar& get_time()
    {
        return current_time;
    }

  private:
    scalar                   current_time;
    system_clock::time_point start_time;
    system_clock::time_point prev_time;
};

struct FrameCounter : public Components::Globals::ValueSubsystem<FrameTag>
{
    time_point next_print;

  public:
    virtual void start_frame(ContainerProxy&, time_point const& current)
    {
        get()++;

        if(next_print < current)
        {
            next_print = current + Chrono::seconds(1);

            cDebug("FPS: {0}", get());
            get() = 0;
        }
    }
};

using RuntimeStateSystem = Components::Globals::ValueSubsystem<StateTag>;

struct RendererState
{
    RendererState()
    {
    }

    // State that can be loaded from disk
    ShPtr<Store::SaveApi>       saving;
    ScopedTask                  component_task;
    Components::EntityContainer entities;
    RuntimeQueue*               component_queue;

    struct RGraphicsData
    {
        RGraphicsData()
        {
        }

        void reset()
        {
            blendstate = {};
            deptstate  = {};
        }

        Vecf4           clear_col  = {.267f, .267f, .267f, 1.f};
        GLM::D_DATA     instdata   = {6, 0, 4};
        GLM::BLNDSTATE  blendstate = {};
        GLM::DEPTSTATE  deptstate  = {};
        GLM::D_CALL     call       = {};
        GLM::OPT_DRAW   draws      = {};
        WkPtr<GLM::PIP> pipeline;
    } g_data;
};

void SetupRendering(CDRenderer& renderer, RendererState* d)
{
    runtime_queue_error rqec;
    RuntimeQueue::CreateNewQueue("Main");
    d->component_queue =
        RuntimeQueue::CreateNewThreadQueue("Component Worker", rqec);

#if defined(FEATURE_ENABLE_DiscordLatte)
    auto onlineWorker =
        RuntimeQueue::CreateNewThreadQueue("Online Worker", rqec);

    C_ERROR_CHECK(rqec);
#endif

    RendererState::RGraphicsData& g        = d->g_data;
    auto&                         entities = d->entities;

#if defined(FEATURE_ENABLE_DiscordLatte)
    entities.register_subsystem<Discord::Tag>(MkUq<Discord::Subsystem>(
        onlineWorker, Discord::DiscordOptions{"468164529617109002", 256}));
#endif
#if defined(FEATURE_ENABLE_ASIO)
    entities.register_subsystem<ASIO::Tag>(MkUq<ASIO::Subsystem>());
#endif
    entities.register_subsystem<IMG::ImageCoderTag>(
        MkUq<IMG::ImageCoderSubsystem>("stb::DecoderQueue"));
    d->g_data.reset();

    cVerbose("Entering run() function");

    ProfContext a("Renderer setup");

    const constexpr cstring textures[num_textures] = {"circle_red.png",
                                                      "circle_blue.png",
                                                      "circle_alpha.png",
                                                      "floor-tile.png"};

    const scalar vertexdata[] = {
        -1.f, -1.f, 0.f,  0.f, 0.f, 1.f, -1.f, 0.f,
        1.f,  0.f,  -1.f, 1.f, 0.f, 0.f, 1.f,

        -1.f, 1.f,  0.f,  0.f, 1.f, 1.f, -1.f, 0.f,
        1.f,  0.f,  1.f,  1.f, 0.f, 1.f, 1.f,
    };

    cVerbose("Loading GLeam API");
    /*
     * Loading the GLeam API, chosen according to what is available at runtime
     */
    GLM::OPTS load_opts      = {};
    load_opts.crash_on_error = true;

    auto& gfx = entities.register_subsystem_inplace<GfxTag, GfxSys>(
        load_opts, PlatformData::IsDebug());

    ShPtr<GLM::V_DESC> vao = nullptr;

    /* Uploading vertex data and creating descriptors */
    {
        ProfContext _("Vertex data upload");

        auto array_buf =
            gfx.alloc_buffer<GLM::BUF_A>(RSCA::ReadOnly, sizeof(vertexdata));

        array_buf->commit(sizeof(vertexdata), vertexdata);

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

        vao = gfx.alloc_desc<2>({{pos, tc}});
        vao->bindBuffer(0, *array_buf);
    }

    /* Compiling shaders and assemble a graphics pipeline */

    GfxSys::AllocData::PipelineParams* params = nullptr;

    {
        ProfContext b("Shader compilation");

        const bool isGles =
            (GLM::LevelIsOfClass(GLM::Level(), GLM::APIClass::GLES));
        const bool isGles20 = GLM::Level() == RHI::GLEAM::GLES_2_0;

        Resource v_rsc(
            (isGles20) ? "vr/vshader_es2.glsl"
                       : (isGles) ? "vr/vshader_es.glsl" : "vr/vshader.glsl");
        Resource f_rsc(
            (isGles20) ? "vr/fshader_es2.glsl"
                       : (isGles) ? "vr/fshader_es.glsl" : "vr/fshader.glsl");

        params = &gfx.alloc_standard_pipeline<2>(
            {{std::move(v_rsc), std::move(f_rsc)}});
    }

    auto mainTexture = gfx.alloc_surface_sampler<GLM::S_2DA>(
        PixFmt::RGBA8, 1, GLM::TextureDMABuffered | GLM::TextureAutoMipmapped);

    auto& mainSurface = *std::get<1>(mainTexture);
    {
        /* Uploading textures */
        {
            auto& mainSampler = *std::get<0>(mainTexture);
            mainSampler.setFiltering(Filtering::Linear, Filtering::Linear);
        }

        mainSurface.allocate({1024, 1024, 4}, PixCmp::RGBA);
        cVerbose("Texture allocation");

        ProfContext _("Texture loading");
        for(i32 i = 0; i < mainSurface.m_size.depth; i++)
        {
            Resource rsc(textures[i], RSCA::AssetFile);

            if(!RHI::LoadTexture<GLM>(mainSurface, std::move(rsc), i))
                return;
        }
    }

#if defined(FEATURE_ENABLE_ASIO)
    /* We download a spicy meme and paste it into the texture */
    if(Net::Supported())
    {
        ProfContext _("Downloading meme");
        auto rsc = "http://i.imgur.com/nQdOmCJ.png"_http.rsc<Net::Resource>(
            entities.subsystem_cast<ASIO::Subsystem>().context());

        if(rsc.fetch())
        {
            auto imdata = rsc.data();

            stb::image_rw img;
            stb::LoadData(
                &img, BytesConst(imdata.data, imdata.size, imdata.size));

            mainSurface.upload(
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
    {
        ProfContext _("Downloading Discord profile picture");

        runtime_queue_error rqec;
        auto& discord   = entities.subsystem_cast<Discord::Subsystem>();
        auto& imgCoder  = entities.subsystem_cast<IMG::ImageCoderSubsystem>();
        auto& network   = entities.subsystem_cast<ASIO::Subsystem>();
        auto  mainQueue = RuntimeQueue::GetCurrentQueue(rqec);

        C_ERROR_CHECK(rqec);

        RuntimeQueue::QueuePeriodic(
            onlineWorker,
            Chrono::milliseconds(100),
            [mainQueue, &discord, &network, &imgCoder, &mainSurface]() {
                ProfContext _("Awaiting Discord");
                cDebug("Waiting for discord...");
                if(discord.awaitReady(Chrono::milliseconds(10)))
                {
                    cDebug("User ID: {0}", discord.playerInfo().userTag);

                    auto rsc = MkShared<Net::Resource>(
                        network.context(), discord.playerInfo().avatarUrl);

                    if(rsc->fetch())
                    {
                        runtime_queue_error ec;
                        imgCoder.decode(rsc->data(), std::move(rsc))
                            .then(mainQueue, [&](stb::image_rw&& img) {
                                ProfContext _("Uploading texture");
                                mainSurface.upload(
                                    PixDesc(
                                        mainSurface.m_pixfmt,
                                        BitFmt::UByte,
                                        PixCmp::RGBA),
                                    {img.size.w, img.size.h, 1},
                                    C_OCAST<Bytes>(img),
                                    {});
                            });
                    }

                    discord.game().put(platform::online::GameDelegate::Builder(
                        "Cool Game", "Gaming", MkUrl("coffee_cup")));

                    runtime_queue_error ec;
                    RuntimeQueue::CancelTask(RuntimeQueue::GetSelfId(ec), ec);
                }
            },
            rqec);

        C_ERROR_CHECK(rqec);

        discord.start();
    }
#endif
#endif

    {
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
    }

    {
        using Components::VisitorFlags;

        entities.register_component<TransformTag>(MkUq<TransformContainer>());
        entities.register_component<MatrixTag>(MkUq<MatrixContainer>());

        entities.register_system(MkUq<TransformVisitor>());
        entities.register_system(MkUq<FloorVisitor>());
        entities.register_system(MkUq<BaseItemVisitor>());
        entities.register_system(MkUq<BasicVisitor>(VisitorFlags::MainThread));
        entities.register_system(MkUq<Basic2Visitor>(VisitorFlags::MainThread));

        entities.register_subsystem<StateTag>(MkUq<RuntimeStateSystem>());
        entities.register_subsystem<FrameTag>(MkUq<FrameCounter>());
        entities.register_subsystem<CameraTag>(MkUq<CameraContainer>());
    }

    {
        RuntimeState state = {};
        d->saving          = Store::CreateDefaultSave();

        d->saving->restore(Bytes::Create(state));
        entities.register_subsystem<TimeTag>(
            MkUq<TimeSystem>(TimeSystem::system_clock::time_point(
                Chrono::milliseconds(state.time_base))));

        entities.subsystem<StateTag>().get() = state;
    }

    {
        Components::EntityRecipe floor_object;
        Components::EntityRecipe base_object;

        floor_object.components = {typeid(TransformTag).hash_code(),
                                   typeid(MatrixTag).hash_code()};
        base_object             = floor_object;

        floor_object.tags = FloorTag;
        base_object.tags  = BaseItemTag;

        entities.create_entity(base_object);
        entities.create_entity(floor_object);
    }

    for(auto& entity : d->entities.select(FloorTag))
    {
        auto& xf = *d->entities.get<TransformTag>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = {1.5};

        xf.mask.x() = 2;
        xf.mask.y() = 2;
    }

    for(auto& entity : d->entities.select(BaseItemTag))
    {
        auto& xf = *d->entities.get<TransformTag>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = {1};

        xf.mask.x() = 1;
        xf.mask.y() = 0.2f;
    }

    entities.exec();

    GpuInfo::GpuQueryInterface interf;
    gpu_query_error            ec;
    GpuInfo::LoadDefaultGpuQuery(interf, ec);

    if(!ec)
    {
        for(auto dev : GpuInfo::GpuQueryView(interf))
            cDebug("Video memory: {0}:{1}", dev.mem().total, dev.mem().free);
    }

    /* We query the current pipeline for possible uniform/texture/buffer values
     */
    params->get_pipeline_params();

    for(auto& constant : params->constants())
    {
        if(constant.m_name == "transform")
            params->set_constant(
                constant,
                Bytes::CreateFrom(
                    entities.container_cast<MatrixContainer>().get_storage()));
        else if(constant.m_name == "texdata")
            params->set_sampler(constant, std::get<0>(mainTexture)->handle());
    }

    params->build_state();

    g.pipeline = params->pipeline_ptr();

    cVerbose("Acquire and set shader uniforms");

    GLM::PreDrawCleanup();

    {
        GLM::RenderPass pass;

        pass.pipeline    = params->pipeline_ptr();
        pass.blend       = g.blendstate;
        pass.depth       = g.deptstate;
        pass.framebuffer = GLM::DefaultFramebuffer();

        g.call.m_inst = true;

        pass.draws.push_back({vao, &params->get_state(), g.call, g.instdata});

        GLM::OptimizeRenderPass(pass, g.draws);
    }
}

void RendererLoop(CDRenderer& renderer, RendererState* d)
{
    DProfContext _("Render loop");

    runtime_queue_error ec;

    auto& g = d->g_data;

    renderer.pollEvents();

    {
        DProfContext _("Components");
        d->entities.exec();
    }

    auto const& component_task = d->component_task;
    {
        DProfContext __("Exclusive context");
        RuntimeQueue::Block(component_task.threadId(), component_task.id(), ec);

        GLM::DefaultFramebuffer()->use(FramebufferT::All);
        GLM::DefaultFramebuffer()->clear(0, g.clear_col, 1.);

        GLM::MultiDraw(*unwrap_ptr(g.pipeline), g.draws);

        RuntimeQueue::Unblock(
            component_task.threadId(), component_task.id(), ec);
    }

    renderer.swapBuffers();
}

void RendererCleanup(CDRenderer&, RendererState* d)
{
    Profiler::PushContext("Stopping workers");
    runtime_queue_error ec;
    RuntimeQueue::TerminateThread(d->component_queue, ec);

    auto& entities = d->entities;

#if defined(FEATURE_ENABLE_ASIO)
    entities.subsystem_cast<ASIO::Subsystem>().stop();
#endif

    Profiler::PopContext();

    auto& state = entities.subsystem<StateTag>().get();

    Profiler::PushContext("Saving time");
    cDebug("Saving time: {0}", state.time_base);

    state.time_base = Chrono::duration_cast<Chrono::milliseconds>(
                          (entities.subsystem_cast<TimeSystem>().get_start() +
                           entities.subsystem_cast<TimeSystem>().get())
                              .time_since_epoch())
                          .count();

    state.camera = entities.subsystem<CameraTag>().get().camera_source;
    d->saving->save(Bytes::Create(state));

    Profiler::PopContext();

    d->entities.reset();
}
