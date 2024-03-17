#include <magic_enum.hpp>

#include <coffee/components/components.h>
#include <coffee/components/proxy.h>

#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/coffee_saving.h>
#include <coffee/core/platform_data.h>
#include <coffee/core/task_queue/task.h>
#include <coffee/core/url.h>
#include <peripherals/semantic/chunk.h>

#include <coffee/image/image_coder_system.h>
#include <peripherals/typing/vectors/camera.h>
#include <peripherals/typing/vectors/matrix_functions.h>
#include <peripherals/typing/vectors/transform.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <coffee/graphics/apis/CGLeam>

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
#include <discord/discord_system.h>
#endif

#include <coffee/comp_app/bundle.h>
#include <coffee/comp_app/fps_counter.h>

#include <fmt_extensions/info.h>
#include <fmt_extensions/vector_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Display;

using typing::vector_types::Matf4;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using typing::vector_types::Veci3;

struct RuntimeState
{
    typing::vectors::scene::camera<f32> camera;

    i64  time_base     = 0.;
    bool debug_enabled = false;
    u8   padding[7];
};

struct TransformPair
{
    using value_type = TransformPair;
    using type       = compo::alloc::VectorContainer<TransformPair>;

    typing::vectors::scene::transform<f32> first;
    Matf4                                  second;
    Vecf3                                  mask;
};

static constexpr u32 FloorTag    = 0x1;
static constexpr u32 BaseItemTag = 0x2;

struct MatrixContainer;
using matrix_tag = compo::value_tag<MatrixContainer, std::pair<Matf4*, Matf4*>>;

struct MatrixContainer : compo::alloc::VectorBaseContainer<matrix_tag>
{
    std::vector<Matf4> m_matrices;

    virtual void register_entity(u64 id)
    {
        compo::alloc::VectorBaseContainer<matrix_tag>::register_entity(id);

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

class CameraContainer : public compo::SubsystemBase
{
  public:
    using type = CameraContainer;

    CameraContainer()
    {
        reset();
    }

    void reset()
    {
        camera_source.aspect      = 1.6f;
        camera_source.fieldOfView = 90.f;
        camera_source.zVals.far_  = 100.f;
        camera_source.position    = {0, 0, -10};
    }

    virtual void start_frame(ContainerProxy&, time_point const&)
    {
        projection = GenPerspective<f32>(camera_source);
        transform  = GenTransform<f32>(camera_source);
    }

    typing::vectors::scene::camera<f32> camera_source;

    Matf4 projection;
    Matf4 transform;

    const f32 eye_distance = 0.1f;
};

class TimeSystem : public Components::SubsystemBase
{
  public:
    using type         = TimeSystem;
    using system_clock = compo::clock;

    TimeSystem(system_clock::time_point const& start)
        : start_time(start)
        , prev_time(start)
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

struct RuntimeStateSystem : Components::SubsystemBase
{
    using type = RuntimeStateSystem;

    RuntimeState state;
};

class TransformVisitor
    : public Components::EntityVisitor<
          Components::TypeList<TransformPair, matrix_tag>,
          Components::TypeList<CameraContainer>>
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

        auto& camera        = c.subsystem<CameraContainer>();
        auto& camera_source = camera.camera_source;

        camera_source.position.x -= camera.eye_distance;

        auto& object_matrix = c.get<TransformPair>().second;

        auto output_matrix = camera.projection *
                             GenTransform<f32>(camera_source) * object_matrix;

        auto& mats = c.get<matrix_tag>();

        *mats.first = output_matrix;

        camera_source.position.x += camera.eye_distance * 2;

        *mats.second = camera.projection * GenTransform<f32>(camera_source) *
                       object_matrix;

        camera_source.position.x -= camera.eye_distance;

        return true;
    }
};

class FloorVisitor
    : public Components::EntityVisitor<
          Components::TypeList<TransformPair>,
          Components::TypeList<TimeSystem>>
{
  public:
    FloorVisitor()
        : VisitorType(FloorTag)
    {
    }

    virtual bool visit(
        Proxy& c,
        const Components::Entity&,
        const Components::time_point&) override
    {
        auto& xf = c.get<TransformPair>();

        auto& time = c.subsystem<TimeSystem>().get_time();

        xf.first.position.x = std::sin(time / 4000.f) * xf.mask.x;
        xf.first.position.y = std::cos(time / 4000.f) * xf.mask.y;

        xf.first.rotation.y = std::sin(time / 4000.f);
        xf.first.rotation   = glm::normalize(xf.first.rotation);

        xf.second = GenTransform<f32>(xf.first);

        return true;
    }
};

class BaseItemVisitor
    : public Components::EntityVisitor<
          Components::TypeList<TransformPair>,
          Components::TypeList<TimeSystem>>
{
  public:
    BaseItemVisitor()
        : VisitorType(BaseItemTag)
    {
    }

    virtual bool visit(
        Proxy& c,
        const Components::Entity&,
        const Components::time_point&) override
    {
        auto& xf   = c.get<TransformPair>();
        auto  time = c.subsystem<TimeSystem>().get_time();

        xf.first.position.x = std::sin(time / 4000.f);

        xf.second = GenTransform<f32>(xf.first);

        return true;
    }
};

struct RendererState
{
    RendererState()
    {
    }

    // State that can be loaded from disk
    std::shared_ptr<Store::SaveApi> saving;
    rq::runtime_queue*              online_queue;

    gleam::api gfx;

    struct RGraphicsData
    {
        RGraphicsData()
        {
        }

        void reset()
        {
        }

        std::shared_ptr<gleam::api::vertex_type> vao;
        std::shared_ptr<gleam::buffer_t>         array;
        std::shared_ptr<gleam::program_t>        program;
        std::shared_ptr<gleam::sampler_t>        sampler;

        std::shared_ptr<gleam::compat::texture_2da_t> tex;

        Vecf4 clear_col = {.267f, .267f, .267f, 1.f};
    } g_data;
};

void SetupRendering(
    Components::EntityContainer& e,
    RendererState&               d,
    Components::time_point const&)
{
    [[maybe_unused]] auto mq = rq::runtime_queue::CreateNewQueue("Main");

#if defined(FEATURE_ENABLE_ASIO)
    if(auto worker = rq::runtime_queue::CreateNewThreadQueue("Online Worker");
       worker.has_value())
    {
#if defined(FEATURE_ENABLE_DiscordLatte)
        e.register_subsystem_inplace<discord::Subsystem>(
            worker.value(), discord::DiscordOptions{"468164529617109002", 256});
#endif
        d.online_queue = worker.value();
    } else
        Throw(std::runtime_error("Failed to create queue for Discord"));

    e.register_subsystem_inplace<ASIO::Subsystem>();
    auto asio_context = e.subsystem_cast<ASIO::Subsystem>().context();

    ASIO::NetStats::register_service<ASIO::NetStats>(
        comp_app::createContainer(), std::ref(*asio_context));
#endif
    d.g_data.reset();

    cVerbose(8, "Entering run() function");

    ProfContext a("Renderer setup");

    const constexpr std::array<std::string_view, 4> textures = {{
        "circle_red.png",
        "circle_blue.png",
        "circle_alpha.png",
        "floor-tile.png",
    }};

    constexpr std::array<f32, 30> vertexdata = {{
        -1.f, -1.f, 0.f, 0.f,

        1.f,  -1.f, 1.f, 0.f,

        -1.f, 1.f,  0.f, 1.f,

        -1.f, 1.f,  0.f, 1.f,

        1.f,  -1.f, 1.f, 0.f,

        1.f,  1.f,  1.f, 1.f,
    }};

    cVerbose(8, "Loading GLeam API");
    /*
     * Loading the GLeam API, chosen according to what is available at runtime
     */
    cDebug(
        "Queried version: {0}\n"
        "Extensions: {1}",
        d.gfx.query_native_version(),
        d.gfx.query_native_extensions());
    auto err =
        d.gfx.load(/*{
 .api_version    = 0x200,
 .api_type = gleam::api_type_t::es,
//        .api_extensions = gleam::api::extensions_set{
//            "GL_EXT_discard_framebuffer",
//            "GL_OES_rgb8_rgba8",
//            "GL_OES_vertex_array_object",
//        },
 .api_workarounds = gleam::workarounds{
     .draw = {
         .emulated_instance_id = true,
     },
}}*/);
    if(err.has_value())
    {
        cWarning(
            "Cancelling setup, got error: ",
            magic_enum::enum_name(err.value()));
        e.service<comp_app::Windowing>()->close();
        return;
    }
    cDebug(
        "\nVersion: {0} {1}\nShader language version: {2}\nDevice: {3}",
        d.gfx.api_version(),
        d.gfx.device_driver(),
        d.gfx.shaderlang_version(),
        d.gfx.device());

    cDebug("Extensions:\n{0}", d.gfx.query_native_extensions());

    d.gfx.collect_info(*e.service<comp_app::AppInfo>());

    {
        auto size = e.service<comp_app::Windowing>()->size();
        d.gfx.default_rendertarget()->resize({0, 0, size.w, size.h}, 0);
    }

    /* Uploading vertex data and creating descriptors */
    {
        ProfContext _("Vertex data upload");

        auto array_vao = d.g_data.vao = d.gfx.alloc_vertex_array();
        auto array_buf                = d.g_data.array =
            d.gfx.alloc_buffer(gleam::buffers::vertex, RSCA::ReadOnly);
        array_vao->alloc();
        array_vao->add({
            .index = 0,
            .value =
                {
                    .stride = sizeof(Vecf2) + sizeof(Vecf2),
                    .count  = 2,
                },
        });
        array_vao->add({
            .index = 1,
            .value =
                {
                    .offset = sizeof(Vecf2),
                    .stride = sizeof(Vecf2) + sizeof(Vecf2),
                    .count  = 2,
                },
        });

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

        cDebug(
            "Selected shaders: {0} + {1}", v_rsc.resource(), f_rsc.resource());

        auto program = d.g_data.program = d.gfx.alloc_program();
        program->add(
            gleam::program_t::stage_t::Vertex,
            d.gfx.alloc_shader(C_OCAST<semantic::Bytes>(v_rsc)));
        program->add(
            gleam::program_t::stage_t::Fragment,
            d.gfx.alloc_shader(C_OCAST<semantic::Bytes>(f_rsc)));
        if(auto res = program->compile(); res.has_error())
        {
            auto [error_msg] = res.error();
            cFatal(
                "Failed to compile shaders: {0}\nSource:\n{1}\n------\n{2}",
                error_msg,
                v_rsc.data().data() ? v_rsc.data().data() : "",
                f_rsc.data().data() ? f_rsc.data().data() : "");
        } else
        {
            auto [info_msg, nothing] = res.value();
            cDebug("Shader compile info: {0}", info_msg);
        }
    }

    {
        /* Uploading textures */
        cVerbose(8, "Texture allocation");

        ProfContext _("Texture loading");
        auto        fmt = typing::pixels::PixDesc(
            typing::pixels::PixFmt::RGBA8,
            typing::pixels::BitFmt::UByte,
            typing::PixCmp::RGBA);

        auto texture = d.g_data.tex =
            std::make_shared<gleam::compat::texture_2da_t>(&d.gfx, fmt, 1);
        texture->set_usage_hint(
            gleam::compat::texture_usage_hint_t::per_instance);
        //        if(d.gfx.api_version() == std::make_tuple(2u, 0u))
        //            d.g_data.tex = d.gfx.alloc_texture(gleam::textures::d2,
        //            fmt, 1);
        //        else
        //            d.g_data.tex
        //                = d.gfx.alloc_texture(gleam::textures::d2_array, fmt,
        //                1);
        texture->alloc(size_3d<u32>{1024, 1024, 4}, true);
        for(i32 i = 0; i < static_cast<i32>(textures.size()); i++)
        {
            Resource      tex_src(textures.at(i), RSCA::AssetFile);
            stb::image_rw tex_img;
            stb::LoadData(&tex_img, C_OCAST<semantic::BytesConst>(tex_src));

            texture->upload(
                C_OCAST<semantic::Bytes>(tex_img).view,
                Veci3{0, 0, i},
                size_3d<i32>{1024, 1024, 1});
        }
        {
            auto sampler = d.g_data.sampler = texture->sampler();
            sampler->alloc();
            sampler->set_filtering(
                typing::Filtering::Linear, typing::Filtering::Linear);
        }
    }

#if defined(FEATURE_ENABLE_ASIO)
    /* We download a spicy meme and paste it into the texture */
    if(net::Supported())
    {
        using namespace net::url_literals;
        auto img_download = e.subsystem_cast<ASIO::Subsystem>().create_download(
            "http://i.imgur.com/nQdOmCJ.png"_http);
        auto img_decode = IMG::create_decoder(
            img_download->output.get_future(), typing::PixCmp::RGBA);
        auto img_upload = rq::dependent_task<stb::image_rw, void>::CreateSink(
            img_decode->output.get_future(),
            [texture = d.g_data.tex](stb::image_rw* img) {
                auto isize = img->size.convert<i32>();
                texture->upload(
                    img->data_owner.view,
                    Veci3{512, 512, 1},
                    size_3d<i32>{isize.w, isize.h, 1});
            });

        rq::runtime_queue::Queue(std::move(img_upload)).assume_value();
        rq::runtime_queue::Queue(d.online_queue, std::move(img_decode))
            .assume_value();
        rq::runtime_queue::Queue(d.online_queue, std::move(img_download))
            .assume_value();
    }

#if defined(FEATURE_ENABLE_DiscordLatte)
    {
        ProfContext _("Downloading Discord profile picture");

        auto& discord = e.subsystem_cast<discord::Subsystem>();

        auto img_data = e.subsystem_cast<ASIO::Subsystem>().create_download(
            e.subsystem_cast<discord::Subsystem>().on_started<Url>(
                [](discord::Subsystem& system) {
                    return system.playerInfo().avatarUrl;
                }));
        auto decoded_img = IMG::create_decoder(img_data->output.get_future());
        auto upload_img =
            rq::dependent_task<stb::image_rw, void>::CreateProcessor(
                decoded_img->output.get_future(), [&d](stb::image_rw* img) {
                    ProfContext _("Uploading image to GPU");
                    auto        imsize = img->size.convert<i32>();
                    d.g_data.tex->upload(
                        img->data_owner.view,
                        Veci3{0, 0, 0},
                        size_3d<i32>{imsize.w, imsize.h, 1});
                });

        rq::runtime_queue::Queue(discord.queue(), std::move(img_data))
            .assume_value();
        rq::runtime_queue::Queue(discord.queue(), std::move(decoded_img))
            .assume_value();
        rq::runtime_queue::Queue(std::move(upload_img)).assume_value();

        ProfContext __("Starting Discord RPC");
        discord.start();
    }
#endif
#endif

    {
        using Components::VisitorFlags;

        e.register_component_inplace<TransformPair>();
        e.register_component_inplace<matrix_tag>();

        e.register_system(std::make_unique<TransformVisitor>());
        e.register_system(std::make_unique<FloorVisitor>());
        e.register_system(std::make_unique<BaseItemVisitor>());

        e.register_subsystem_inplace<RuntimeStateSystem>();
        e.register_subsystem_inplace<comp_app::FrameTag>();
        e.register_subsystem_inplace<CameraContainer>();
    }

    {
        RuntimeState state = {};
        d.saving           = Store::CreateDefaultSave();

        d.saving->restore(semantic::Bytes::ofBytes(state));
        e.register_subsystem_inplace<TimeSystem>(
            TimeSystem::system_clock::now());

        e.subsystem_cast<RuntimeStateSystem>().state = state;
    }

    {
        Components::EntityRecipe floor_object;
        Components::EntityRecipe base_object;

        floor_object.components = {
            typeid(TransformPair).hash_code(),
            typeid(matrix_tag).hash_code(),
        };
        base_object = floor_object;

        floor_object.tags = FloorTag;
        base_object.tags  = BaseItemTag;

        e.create_entity(base_object);
        e.create_entity(floor_object);
    }

    for(auto& entity : e.select(FloorTag))
    {
        auto& xf = *e.get<TransformPair>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = Vecf3{2.5f};

        xf.mask.x = 2;
        xf.mask.y = 2;
    }

    for(auto& entity : e.select(BaseItemTag))
    {
        auto& xf = *e.get<TransformPair>(entity.id);

        xf.first.position = {0, 0, 5};
        xf.first.scale    = Vecf3{1.5f};

        xf.mask.x = 1;
        xf.mask.y = 0.2f;
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

    cDebug("Resolution: {0}", e.service<comp_app::Windowing>()->size());
}

void RendererLoop(
    Components::EntityContainer& e,
    RendererState&               d,
    Components::time_point const&,
    Components::duration const&)
{
    using namespace std::string_view_literals;
    using typing::graphics::ShaderStage;

    DProfContext _("Render loop");

    auto& g = d.g_data;

    {
        //        d.gfx.default_rendertarget()->discard();
        d.gfx.default_rendertarget()->clear(Vecf4{.5f, 0.f, .5f, 1.f}, 1.f, 0);

        auto const& xf  = e.container_cast<matrix_tag>().m_matrices;
        auto        err = d.gfx.submit(
            {
                       .program  = g.program,
                       .vertices = g.vao,
                       .call =
                    {
                               .instanced = true,
                    },
                       .data =
                    {
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
                    },
            },
            gleam::make_uniform_list(
                ShaderStage::Vertex,
                gleam::uniform_pair{
                           {"transform"sv},
                    semantic::mem_chunk<const Matf4>::ofContainer(xf).view}),
            gleam::compat::make_texture_list(
                gleam::compat::texture_definition_t{
                    ShaderStage::Fragment,
                    gleam::uniform_key{{"texdata"sv}},
                    g.tex,
                    g.sampler}));
        if(err.has_value())
        {
            auto [code, msg] = *err;
            cDebug("Draw error: {0}: {1}", magic_enum::enum_name(code), msg);
        }
    }
}

void RendererCleanup(
    Components::EntityContainer& entities,
    RendererState&               d,
    Components::time_point const&)
{
    Profiler::PushContext("Stopping workers");

#if defined(FEATURE_ENABLE_ASIO)
    entities.subsystem_cast<ASIO::Subsystem>().stop();
#endif
#if defined(FEATURE_ENABLE_DiscordLatte)
    entities.subsystem_cast<discord::Subsystem>().stop();
#endif

    Profiler::PopContext();

    auto& state = entities.subsystem_cast<RuntimeStateSystem>().state;

    Profiler::PushContext("Saving time");
    cDebug("Saving time: {0}", state.time_base);

    state.time_base = std::chrono::duration_cast<std::chrono::milliseconds>(
                          (entities.subsystem_cast<TimeSystem>().get_start() +
                           entities.subsystem_cast<TimeSystem>().get_current())
                              .time_since_epoch())
                          .count();

    state.camera = entities.subsystem_cast<CameraContainer>().camera_source;
    d.saving->save(semantic::Bytes::ofBytes(state));

    Profiler::PopContext();

    semantic::concepts::graphics::util::dealloc_all(
        d.g_data.array,
        d.g_data.program,
        d.g_data.sampler,
        d.g_data.tex,
        d.g_data.vao);
    d.g_data = {};
}
