#include <coffee/components/components.h>
#include <coffee/components/proxy.h>

#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/coffee_saving.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/types/chunk.h>
#include <coffee/core/url.h>

#include <coffee/image/image_coder_system.h>
#include <coffee/interfaces/cgraphics_util.h>
#include <coffee/windowing/renderer/renderer.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/matrix_functions.h>
#include <peripherals/typing/vectors/transform.h>

//#if defined(FEATURE_ENABLE_G)
#include <coffee/graphics/apis/CGLeam>
//#endif

#include <coffee/components/entity_selectors.h>

#if defined(FEATURE_ENABLE_GpuQuery)
#include <coffee/graphics/common/query/gpu_query.h>
#endif

#if defined(FEATURE_ENABLE_ASIO)
#include <coffee/asio/asio_network_stats.h>
#include <coffee/asio/asio_system.h>
#include <coffee/asio/net_resource.h>
#endif

#if defined(FEATURE_ENABLE_DiscordLatte)
#include <coffee/discord/discord_system.h>
#endif

#include <coffee/comp_app/bundle.h>

#include <coffee/strings/info.h>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

//#define USE_NULL_RENDERER

using namespace Coffee;
using namespace Display;

using CDRenderer = CSDL2Renderer;

struct RuntimeState
{
    typing::vectors::scene::camera<f32> camera;

    i64  time_base     = 0.;
    bool debug_enabled = false;
    u8   padding[7];
};

struct TransformPair
{
    typing::vectors::scene::transform<f32> first;
    Matf4                                  second;
    Vecf3                                  mask;
};

struct CameraData
{
    typing::vectors::scene::camera<f32> camera_source;

    Matf4 projection;
    Matf4 transform;

    const f32 eye_distance = 0.1f;
};

struct TransformContainer;
struct MatrixContainer;
struct CameraContainer;
struct TimeSystem;
struct FrameCounter;
struct RuntimeStateSystem;

using TransformTag = Components::ValueTag<TransformContainer, TransformPair>;
using MatrixTag = Components::ValueTag<MatrixContainer, Pair<Matf4*, Matf4*>>;
using CameraTag = Components::ValueTag<CameraContainer, CameraData>;
using TimeTag   = Components::ValueTag<TimeSystem, Chrono::seconds_float>;
using FrameTag  = Components::ValueTag<FrameCounter, u32>;
using StateTag  = Components::ValueTag<RuntimeStateSystem, RuntimeState>;

static constexpr u32 FloorTag    = 0x1;
static constexpr u32 BaseItemTag = 0x2;

struct TransformContainer : Components::Allocators::VectorContainer<TransformTag>
{
};

struct MatrixContainer : Components::Allocators::VectorBaseContainer<MatrixTag>
{
    Vector<Matf4> m_matrices;

    virtual void register_entity(u64 id)
    {
        Components::Allocators::VectorBaseContainer<MatrixTag>::register_entity(
            id);

        m_matrices.push_back({});
        m_matrices.push_back({});

        *this->m_data.rbegin() = {
            &m_matrices.at(m_matrices.size() - 1),
            &m_matrices.at(m_matrices.size() - 2)};
    }

    virtual value_type* get(u64 id)
    {
        auto it = m_mapping.find(id);

        if(it == m_mapping.end())
            return nullptr;

        auto& out = m_data.at((*it).second);

        out = {
            &m_matrices.at((*it).second * 2),
            &m_matrices.at((*it).second * 2 + 1)};

        return &out;
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
        get().projection = GenPerspective<f32>(get().camera_source);
        get().transform  = GenTransform<f32>(get().camera_source);
    }
};

class TimeSystem : public Components::SubsystemBase
{
  public:
    using system_clock = Chrono::high_resolution_clock;

    TimeSystem(system_clock::time_point const& start) :
        start_time(start), prev_time(start)
    {
    }

    auto get_start() const
    {
        return start_time;
    }
    auto get_current() const
    {
        return current_time;
    }

    virtual void start_frame(
        Components::ContainerProxy&, Components::time_point const&) override
    {
        auto current = system_clock::now();
        prev_time    = start_time + current_time;
        current_time = current - start_time;

        current_value = current_time.count();
    }

    f32& get_time()
    {
        return current_value;
    }

  private:
    f32                      current_value;
    system_clock::time_point start_time{};
    system_clock::time_point prev_time{};
    system_clock::duration   current_time{};
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

struct RuntimeStateSystem : Components::SubsystemBase
{
    RuntimeState state;
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
        using typing::vectors::scene::GenTransform;

        auto camera        = c.subsystem_cast<CameraTag>().get();
        auto camera_source = camera.camera_source;

        camera_source.position.x() -= camera.eye_distance;

        auto& object_matrix = c.get<TransformTag>().second;

        auto output_matrix = camera.projection *
                             GenTransform<f32>(camera_source) * object_matrix;

        auto& mats = c.get<MatrixTag>();

        *mats.first = output_matrix;

        camera_source.position.x() += camera.eye_distance * 2;

        *mats.second = camera.projection * GenTransform<f32>(camera_source) *
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

        auto& time = c.subsystem_cast<TimeTag>().get_time();

        xf.first.position.x() = CMath::sin(time * 200.f) * xf.mask.x();
        xf.first.position.y() = CMath::cos(time * 200.f) * xf.mask.y();

        xf.first.rotation.y() = CMath::sin(time * 2000.f);
        xf.first.rotation     = normalize_quat(xf.first.rotation);

        xf.second = GenTransform<f32>(xf.first);

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
        auto  time = c.subsystem_cast<TimeTag>().get_time();

        xf.first.position.x() = CMath::sin(time * 400.f);

        xf.second = GenTransform<f32>(xf.first);

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

struct RendererState
{
    RendererState()
    {
    }

    // State that can be loaded from disk
    ShPtr<Store::SaveApi> saving;
    rq::scoped_task       component_task;
    rq::runtime_queue*    component_queue;
    rq::runtime_queue*    online_queue;

    gleam::api gfx;

    struct RGraphicsData
    {
        RGraphicsData()
        {
        }

        void reset()
        {
        }

        std::shared_ptr<gleam::vertex_array_t> vao;
        std::shared_ptr<gleam::buffer_t>       array;
        std::shared_ptr<gleam::program_t>      program;
        std::shared_ptr<gleam::texture_2da_t>  tex;
        std::shared_ptr<gleam::sampler_t>      sampler;

        Vecf4 clear_col = {.267f, .267f, .267f, 1.f};
    } g_data;
};

void SetupRendering(
    Components::EntityContainer& e,
    RendererState&               d,
    Components::time_point const&)
{
    rq::runtime_queue::CreateNewQueue("Main");
    if(auto q = rq::runtime_queue::CreateNewThreadQueue("Component Worker");
       q.has_value())
        d.component_queue = q.value();
    else
        Throw(rq::runtime_queue_error(q.error().second));

    RendererState::RGraphicsData& g = d.g_data;

    if(auto worker = rq::runtime_queue::CreateNewThreadQueue("Online Worker");
       worker.has_value())
    {
#if defined(FEATURE_ENABLE_DiscordLatte)
        e.register_subsystem_inplace<Discord::Subsystem>(
            worker.value(), Discord::DiscordOptions{"468164529617109002", 256});
#endif
        d.online_queue = worker.value();
    }
    else
        Throw(std::runtime_error("Failed to create queue for Discord"));
#if defined(FEATURE_ENABLE_ASIO)
    e.register_subsystem_inplace<ASIO::Subsystem>();
    auto asio_context = e.subsystem_cast<ASIO::Subsystem>().context();

    ASIO::NetStats::register_service<ASIO::NetStats>(
        comp_app::createContainer(), std::ref(*asio_context));
#endif
    d.g_data.reset();

    cVerbose(8, "Entering run() function");

    ProfContext a("Renderer setup");

    const constexpr Array<std::string_view, 4> textures = {{
        "circle_red.png",
        "circle_blue.png",
        "circle_alpha.png",
        "floor-tile.png",
    }};

    constexpr Array<f32, 30> vertexdata = {{
        -1.f, -1.f, 0.f, 0.f, 0.f,

        1.f,  -1.f, 0.f, 1.f, 0.f,

        -1.f, 1.f,  0.f, 0.f, 1.f,

        -1.f, 1.f,  0.f, 0.f, 1.f,

        1.f,  -1.f, 0.f, 1.f, 0.f,

        1.f,  1.f,  0.f, 1.f, 1.f,
    }};

    cVerbose(8, "Loading GLeam API");
    /*
     * Loading the GLeam API, chosen according to what is available at runtime
     */
    d.gfx.load({.api_version = 0x330});
    cDebug(
        "\nVersion: {0}\nShader language version: {1}",
        gleam::cmd::get_string(gleam::group::string_name::version),
        gleam::cmd::get_string(
            gleam::group::string_name::shading_language_version));

    /* Uploading vertex data and creating descriptors */
    {
        ProfContext _("Vertex data upload");

        auto array_vao = d.g_data.vao = d.gfx.alloc_vertex_array();
        array_vao->alloc();
        array_vao->add({
            .index = 0,
            .value =
                {
                    .stride = sizeof(Vecf3) + sizeof(Vecf2),
                    .count  = 3,
                },
        });
        array_vao->add({
            .index = 1,
            .value =
                {
                    .offset = sizeof(Vecf3),
                    .stride = sizeof(Vecf3) + sizeof(Vecf2),
                    .count  = 2,
                },
        });

        auto array_buf = d.g_data.array =
            d.gfx.alloc_buffer(gleam::buffers::vertex, RSCA::ReadOnly);
        array_buf->alloc();
        array_buf->commit(vertexdata);
        array_vao->set_buffer(gleam::buffers::vertex, array_buf, 0);
    }

    /* Compiling shaders and assemble a graphics pipeline */
    {
        ProfContext b("Shader compilation");

        bool isGLES = d.gfx.api_type() == gleam::api_type_t::es;
        bool isES20 = isGLES && d.gfx.api_version() == std::make_tuple(2u, 0u);

        Resource v_rsc(
            (isES20)   ? "vr/vshader_es2.glsl"
            : (isGLES) ? "vr/vshader_es.glsl"
                       : "vr/vshader.glsl");
        Resource f_rsc(
            (isES20)   ? "vr/fshader_es2.glsl"
            : (isGLES) ? "vr/fshader_es.glsl"
                       : "vr/fshader.glsl");

        auto program = d.g_data.program = d.gfx.alloc_program();
        program->add(
            gleam::program_t::stage_t::Vertex,
            d.gfx.alloc_shader(C_OCAST<Bytes>(v_rsc)));
        program->add(
            gleam::program_t::stage_t::Fragment,
            d.gfx.alloc_shader(C_OCAST<Bytes>(f_rsc)));
        if(auto res = program->compile(); res.has_error())
        {
            auto [error_msg] = res.error();
            cFatal("Failed to compile shaders: {0}", error_msg);
        } else
        {
            auto [info_msg, nothing] = res.value();
            cDebug("Shader compile info: {0}", info_msg);
        }
    }

    auto texture = d.g_data.tex = d.gfx.alloc_texture(
        gleam::textures::d2_array,
        typing::pixels::PixDesc(PixFmt::RGBA8, BitFmt::UByte, PixCmp::RGBA),
        1);
    {
        auto sampler = d.g_data.sampler = texture->sampler();
        sampler->setFiltering(
            typing::Filtering::Linear, typing::Filtering::Linear);

        /* Uploading textures */
        texture->alloc(size_3d<u32>{1024, 1024, 4}, true);
        cVerbose(8, "Texture allocation");

        ProfContext _("Texture loading");
        for(i32 i = 0; i < textures.size(); i++)
        {
            Resource      tex_src(textures.at(i), RSCA::AssetFile);
            stb::image_rw tex_img;
            stb::LoadData(&tex_img, C_OCAST<Bytes>(tex_src));
            texture->upload(
                C_OCAST<Bytes>(tex_img).view,
                Veci3{0, 0, i},
                size_3d<i32>{1024, 1024, 1});
        }
    }

#if defined(FEATURE_ENABLE_ASIO)
    /* We download a spicy meme and paste it into the texture */
    if(Net::Supported())
    {
        auto img_download = e.subsystem_cast<ASIO::Subsystem>().create_download(
            "http://i.imgur.com/nQdOmCJ.png"_http);
        auto img_decode = IMG::create_decoder(
            img_download->output.get_future(), PixCmp::RGBA);
        auto img_upload = rq::dependent_task<stb::image_rw, void>::CreateTask(
            img_decode->output.get_future(),
            [texture] (stb::image_rw* img) {
                auto isize = img->size.convert<i32>();
                texture->upload(
                    img->data_owner.view,
                    Veci3{512, 512, 1},
                    size_3d<i32>{isize.w, isize.h, 1});
            });

        rq::runtime_queue::Queue(d.online_queue, std::move(img_download));
        rq::runtime_queue::Queue(d.online_queue, std::move(img_decode));
        rq::runtime_queue::Queue(std::move(img_upload));
    }

#if defined(FEATURE_ENABLE_DiscordLatte)
    {
        ProfContext _("Downloading Discord profile picture");

        auto& discord   = e.subsystem_cast<Discord::Subsystem>();

        auto img_data = e.subsystem_cast<ASIO::Subsystem>().create_download(
            e.subsystem_cast<Discord::Subsystem>().on_started<Url>(
            [](Discord::Subsystem& system) {
                return system.playerInfo().avatarUrl;
            }));
        auto decoded_img = IMG::create_decoder(img_data->output.get_future());
        auto upload_img = rq::dependent_task<stb::image_rw, void>::CreateTask(
            decoded_img->output.get_future(),
            [texture](stb::image_rw* img) {
                ProfContext _("Uploading image to GPU");
                auto imsize = img->size.convert<i32>();
                texture->upload(
                    img->data_owner.view,
                    Veci3{0, 0, 0},
                    size_3d<i32>{imsize.w, imsize.h, 1});
            });

        auto& network = e.subsystem_cast<ASIO::Subsystem>();

        rq::runtime_queue::Queue(discord.queue(), std::move(img_data));
        rq::runtime_queue::Queue(discord.queue(), std::move(decoded_img));
        rq::runtime_queue::Queue(std::move(upload_img));

        ProfContext __("Starting Discord RPC");
        discord.start();
    }
#endif
#endif

    {
        using Components::VisitorFlags;

        e.register_component<TransformTag>(MkUq<TransformContainer>());
        e.register_component<MatrixTag>(MkUq<MatrixContainer>());

        e.register_system(MkUq<TransformVisitor>());
        e.register_system(MkUq<FloorVisitor>());
        e.register_system(MkUq<BaseItemVisitor>());
        e.register_system(MkUq<BasicVisitor>(VisitorFlags::MainThread));
        e.register_system(MkUq<Basic2Visitor>(VisitorFlags::MainThread));

        e.register_subsystem_inplace<StateTag>();
        e.register_subsystem_inplace<FrameTag>();
        e.register_subsystem_inplace<CameraTag>();
    }

    {
        RuntimeState state = {};
        d.saving           = Store::CreateDefaultSave();

        d.saving->restore(Bytes::ofBytes(state));
        e.register_subsystem_inplace<TimeTag>(
            TimeSystem::system_clock::time_point(
                Chrono::milliseconds(state.time_base)));

        e.subsystem_cast<StateTag>().state = state;
    }

    {
        Components::EntityRecipe floor_object;
        Components::EntityRecipe base_object;

        floor_object.components = {
            typeid(TransformTag).hash_code(), typeid(MatrixTag).hash_code()};
        base_object = floor_object;

        floor_object.tags = FloorTag;
        base_object.tags  = BaseItemTag;

        e.create_entity(base_object);
        e.create_entity(floor_object);
    }

    for(auto& entity : e.select(FloorTag))
    {
        auto& xf = *e.get<TransformTag>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = {1.5};

        xf.mask.x() = 2;
        xf.mask.y() = 2;
    }

    for(auto& entity : e.select(BaseItemTag))
    {
        auto& xf = *e.get<TransformTag>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = {1};

        xf.mask.x() = 1;
        xf.mask.y() = 0.2f;
    }

#if defined(FEATURE_ENABLE_GpuQuery)
    GpuInfo::GpuQueryInterface interf;
    gpu_query_error            ec;
    GpuInfo::LoadDefaultGpuQuery(interf, ec);

    if(!ec)
    {
        for(auto dev : GpuInfo::GpuQueryView(interf))
            cDebug("Video memory: {0}:{1}", dev.mem().total, dev.mem().free);
    }
#endif

    cVerbose(8, "Acquire and set shader uniforms");
}

void RendererLoop(
    Components::EntityContainer& e,
    RendererState&               d,
    Components::time_point const&,
    Components::duration const&)
{
    using typing::graphics::ShaderStage;

    DProfContext _("Render loop");

    auto& g = d.g_data;

    auto const& component_task = d.component_task;
    {
        DProfContext __("Exclusive context");
        rq::runtime_queue::Block(
            component_task.threadId(), component_task.id());

        d.gfx.default_rendertarget()->clear(Vecf4{.5f, 0.f, .5f, 1.f}, 1.f, 0);

        auto const& xf        = e.container_cast<MatrixTag>().m_matrices;
        i32         samplerId = 0;
        d.gfx.submit(
            {
                .program  = g.program,
                .vertices = g.vao,
                .call =
                    {
                        .instanced = true,
                    },
                .data =
                    {
                        .arrays =
                            {
                                .count = 6,
                            },
                        .instances =
                            {
                                .count = 4,
                            },
                    },
                .samplers =
                    {
                        {ShaderStage::Fragment,
                         {std::string_view("texdata"), 0},
                         g.sampler},
                    },
            },
            gleam::make_uniform_list<const Matf4>(
                ShaderStage::Vertex,
                {{std::string_view("transform"), gleam::invalid_uniform},
                 semantic::mem_chunk<const Matf4>::ofContainer(xf).view}));

        rq::runtime_queue::Unblock(
            component_task.threadId(), component_task.id());
    }
}

void RendererCleanup(
    Components::EntityContainer& entities,
    RendererState&               d,
    Components::time_point const&)
{
    Profiler::PushContext("Stopping workers");
    rq::runtime_queue::TerminateThread(d.component_queue);

#if defined(FEATURE_ENABLE_ASIO)
    entities.subsystem_cast<ASIO::Subsystem>().stop();
#endif
#if defined(FEATURE_ENABLE_DiscordLatte)
    entities.subsystem_cast<Discord::Subsystem>().stop();
#endif

    Profiler::PopContext();

    auto& state = entities.subsystem_cast<StateTag>().state;

    Profiler::PushContext("Saving time");
    cDebug("Saving time: {0}", state.time_base);

    state.time_base = Chrono::duration_cast<Chrono::milliseconds>(
                          (entities.subsystem_cast<TimeTag>().get_start() +
                           entities.subsystem_cast<TimeTag>().get_current())
                              .time_since_epoch())
                          .count();

    state.camera = entities.subsystem_cast<CameraTag>().get().camera_source;
    d.saving->save(Bytes::ofBytes(state));

    Profiler::PopContext();

    semantic::concepts::graphics::util::dealloc_all(
        d.g_data.array,
        d.g_data.program,
        d.g_data.sampler,
        d.g_data.tex,
        d.g_data.vao);
    d.g_data = {};
    d.component_task = {};
}
