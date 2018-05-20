#include <coffee/CGraphics>
#include <coffee/CSDL2>
#include <coffee/core/CDebug>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/base/files/url.h>
#include <coffee/core/base/types/counter.h>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/image/cimage.h>

#include <coffee/interfaces/cgraphics_util.h>

#if defined(FEATURE_USE_ASIO)
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

#ifndef USE_NULL_RENDERER
using GLM = GLEAMAPI;
#else
using GLM = RHI::NullAPI;
#endif

struct RuntimeState
{
    bigscalar time_base     = 0.;
    bool      debug_enabled = false;
    uint8     padding[7];
};

static const constexpr szptr num_textures = 5;

struct RendererState
{
    // State that can be loaded from disk
    RuntimeState r_state;

    RuntimeQueue* rt_queue;

    uint32 frameCount;

    struct RGraphicsData
    {
        // GLEAM data
        GLM::API_CONTEXT loader = nullptr;

        GLM::FB_T* render_target = nullptr;

        GLM::BUF_A* vertbuf;
        GLM::V_DESC vertdesc = {};

        GLM::SHD v_shader = {};
        GLM::SHD f_shader = {};
        GLM::PIP eye_pip  = {};

        GLM::S_2DA* eyetex;
        GLM::SM_2DA eyesamp = {};

        GLM::UNIFVAL transforms     = {};
        GLM::UNIFVAL timeval        = {};
        GLM::UNIFVAL texture_size   = {};
        GLM::UNIFSMP textures_array = {};

        Bytes transform_data = {};
        Bytes time_data      = {};
        Bytes texsize_data   = {};

        // Graphics data
        CGCamera camera;
        Matf4    object_matrices[6] = {};
        scalar   time_value         = 0.f;

        Transform base_transform;
        Transform floor_transform;

        // Timing information
        bigscalar tprevious = 0.0;
        bigscalar tbase     = 0.0;
        bigscalar tdelta    = 0.0;

        Vecf4 clear_col = {.267f, .267f, .267f, 1.f};

        /* Instance data is more akin to individual drawcalls, specifying vertex
         * buffer information */
        GLM::D_DATA instdata = {6, 0, 4};

        // View information
        GLM::BLNDSTATE blendstate       = {};
        GLM::DEPTSTATE deptstate        = {};

        GLM::USTATE unifstate   = {};
        GLM::USTATE unifstate_f = {};

        GLM::PSTATE     pipstate = {};
        GLM::RenderPass rpass_s  = {};
        GLM::OPT_DRAW   rpass    = {};
    } g_data;
};

void KeyEventHandler(void* r, const CIEvent& e, c_cptr data)
{
    RendererState* rdata = C_FCAST<RendererState*>(r);

    if(e.type == CIEvent::Keyboard)
    {
        auto kev = C_CAST<CIKeyEvent const*>(data);

        /* Single presses */
        if(kev->mod & CIKeyEvent::PressedModifier &&
           !(kev->mod & CIKeyEvent::RepeatedModifier))
        {
            if(kev->key == CK_F10)
                rdata->r_state.debug_enabled = !rdata->r_state.debug_enabled;
        }
    }
}

void SetupRendering(CDRenderer& renderer, RendererState* d)
{
    auto& g = d->g_data;

    g = {};

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
    g.loader = GLM::GetLoadAPI();
    if(!g.loader(PlatformData::IsDebug()))
    {
        cDebug("Failed to load GLEAM API");
        return;
    }

    g.vertbuf = new GLM::BUF_A(ResourceAccess::ReadOnly, sizeof(vertexdata));
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

    auto& v_shader = g.v_shader;
    auto& f_shader = g.f_shader;
    auto& eye_pip  = g.eye_pip;

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

        CResources::Resource v_rsc(
            shader_files[isGles * 2 + isGles20 * 2],
            ResourceAccess::SpecifyStorage | ResourceAccess::AssetFile);
        CResources::Resource f_rsc(
            shader_files[isGles * 2 + isGles20 * 2 + 1],
            ResourceAccess::SpecifyStorage | ResourceAccess::AssetFile);

        if(!RHI::LoadPipeline<GLM>(eye_pip, v_rsc, f_rsc))
        {
            cFatal("Failed to load shaders");
        }

    } while(false);
    cVerbose("Compiled shaders");

    /* Uploading textures */
    g.eyetex = new GLM::S_2DA(PixelFormat::RGBA8, 1, GLM::TextureDMABuffered);
    auto& eyetex = *g.eyetex;

    eyetex.allocate({1024, 1024, 4}, PixCmp::RGBA);
    cVerbose("Texture allocation");

    Profiler::PushContext("Texture loading");
    for(i32 i = 0; i < eyetex.m_size.depth; i++)
    {
        CResources::Resource rsc(
            textures[i],
            ResourceAccess::SpecifyStorage | ResourceAccess::AssetFile);

        if(!RHI::LoadTexture<GLM>(eyetex, std::move(rsc), i))
            return;
    }

#if defined(FEATURE_USE_ASIO)
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

    /*
     * These specify byte buffers which refer to other data
     * This makes it simple to redirect or reallocate the uniform data
     *
     * These can be rotated to achieve per-frame disposable buffers,
     *  allowing multiple frames to be processed concurrently without halt
     */
    auto& transform_data = g.transform_data;
    auto& time_data      = g.time_data;

    transform_data = Bytes::Create(g.object_matrices);
    time_data      = Bytes::Create(g.time_value);

    g.textures_array = eyesamp.handle();

    g.transforms.data   = &transform_data;
    g.timeval.data      = &time_data;
    g.texture_size.data = &g.texsize_data;

    /* We create some pipeline state, such as blending and viewport state */
    auto& blendstate       = g.blendstate;
    auto& deptstate        = g.deptstate;

    blendstate.m_doBlend = true;
    deptstate.m_test             = true;
    deptstate.m_func             = C_CAST<uint32>(ValueComparison::Less);

    /* Applying state information */
    GLM::SetBlendState(g.blendstate);
    GLM::SetDepthState(deptstate);
    cVerbose("Set renderer state");

    /* We query the current pipeline for possible uniform/texture/buffer values
     */
    Vector<GLM::UNIFDESC> unifs;
    GLM::GetShaderUniformState(eye_pip, &unifs);
    auto& unifstate_v = g.unifstate;
    auto& unifstate_f = g.unifstate_f;
    /* We assign CPU-side values to GPU-side values */
    for(GLM::UNIFDESC const& u : unifs)
    {
        if(u.m_name == "transform")
        {
            unifstate_v.setUniform(u, &g.transforms);
        } else if(u.m_name == "texdata")
            unifstate_f.setSampler(u, &g.textures_array);
        else if(u.m_name == "mx")
            unifstate_f.setUniform(u, &g.timeval);
        else if(u.m_name == "texdata_gridSize")
            unifstate_f.setUniform(u, &g.texture_size);
        else
            cVerbose(4, "Unhandled uniform value: {0}", u.m_name);
    }

    cVerbose("Acquire and set shader uniforms");

    /* Specifying the uniform data, such as camera matrices
     *  and transforms */

    auto& camera = g.camera;

    camera = CGCamera();

    camera.aspect      = 1.6f;
    camera.fieldOfView = 85.f;
    camera.zVals.far_  = 100.;

    camera.position = Vecf3(0, 0, -15);

    auto& base_transform  = g.base_transform;
    auto& floor_transform = g.floor_transform;

    base_transform.position = Vecf3(0, 0, 5);
    base_transform.scale    = Vecf3(1);

    floor_transform.position = Vecf3(0, -2, 5);
    floor_transform.scale    = Vecf3(2);

    /* Vertex descriptors are based upon the ideas from GL4.3 */
    vertdesc.bind();
    vertdesc.bindBuffer(0, vertbuf);

    GLM::PreDrawCleanup();

    GLM::DefaultFramebuffer().clear(0, g.clear_col, 0.5);

    g.tprevious = d->r_state.time_base;
    g.tbase     = d->r_state.time_base;

    g.rpass_s.pipeline    = &g.eye_pip;
    g.rpass_s.blend       = &g.blendstate;
    g.rpass_s.depth       = &g.deptstate;
    g.rpass_s.framebuffer = &GLM::DefaultFramebuffer();

    g.pipstate[ShaderStage::Vertex]   = &g.unifstate;
    g.pipstate[ShaderStage::Fragment] = &g.unifstate_f;

    g.rpass_s.draws.push_back({&g.vertdesc, &g.pipstate, g.call, g.instdata});

    GLM::OptimizeRenderPass(g.rpass_s, g.rpass);
}

void LogicLoop(CDRenderer& renderer, RendererState* d)
{
    auto& g = d->g_data;

    renderer.pollEvents();

    auto& base_transform  = g.base_transform;
    auto& floor_transform = g.floor_transform;
    auto& camera          = g.camera;

    auto& tprevious = g.tprevious;
    auto& tdelta    = g.tdelta;
    auto& tbase     = g.tbase;

    auto& time_value = g.time_value;

    auto& object_matrices = g.object_matrices;

    /* Define frame data */
    base_transform.position.x() = C_CAST<scalar>(CMath::sin(tprevious) * 2.);
    base_transform.position.y() = C_CAST<scalar>(CMath::cos(tprevious) * 2.);

    time_value = C_CAST<scalar>(CMath::sin(tprevious) + (CMath::pi / 4.));

    floor_transform.rotation.y() = C_CAST<scalar>(CMath::sin(tprevious));
    floor_transform.rotation     = normalize_quat(floor_transform.rotation);

    camera.position.x() -= 0.1;
    object_matrices[0] = GenPerspective<scalar>(camera) *
                         GenTransform<scalar>(camera) *
                         GenTransform<scalar>(base_transform);
    object_matrices[2] = GenPerspective<scalar>(camera) *
                         GenTransform<scalar>(camera) *
                         GenTransform<scalar>(floor_transform);

    camera.position.x() += 0.2;
    object_matrices[1] = GenPerspective<scalar>(camera) *
                         GenTransform<scalar>(camera) *
                         GenTransform<scalar>(base_transform);
    object_matrices[3] = GenPerspective<scalar>(camera) *
                         GenTransform<scalar>(camera) *
                         GenTransform<scalar>(floor_transform);

    camera.position.x() -= 0.1;

    tdelta    = tbase + renderer.contextTime() - tprevious;
    tprevious = tbase + renderer.contextTime();

    d->r_state.time_base = tprevious;
}

void RendererLoop(CDRenderer& renderer, RendererState* d)
{
    auto& g = d->g_data;

    GLM::DefaultFramebuffer().clear(0, g.clear_col, 1.);

    scalar texSize = i32(CMath::sqrt(g.eyetex->texSize().depth));
    g.texsize_data = Bytes::Create(texSize);

    GLM::MultiDraw(g.eye_pip, g.rpass);

    renderer.swapBuffers();

    d->frameCount++;
}

void RendererCleanup(CDRenderer& renderer, RendererState* d)
{
    auto& g = d->g_data;

    g.vertbuf->dealloc();
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

