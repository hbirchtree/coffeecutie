#include <coffee/CGraphics>
#include <coffee/core/base/files/url.h>
#include <coffee/core/CFiles>
#include <coffee/CSDL2>
#include <coffee/core/CProfiling>
#include <coffee/graphics/apis/CGLeamRHI>
#include <coffee/image/cimage.h>
#include <coffee/core/base/types/counter.h>
#include <coffee/core/CDebug>

#include <coffee/asio/net_resource.h>

#include <coffee/core/platform_data.h>
#include <coffee/core/coffee_saving.h>

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
    bigscalar time_base = 0.;
    bool debug_enabled = false;
    uint8 padding[7];
};

static const constexpr szptr num_textures = 5;

struct RendererState
{
    // State that can be loaded from disk
    RuntimeState r_state;

    RuntimeQueue* rt_queue;

    uint32 frameCount;
    
    struct RGraphicsData{
        // GLEAM data
        GLM::API_CONTEXT loader = nullptr;
        
        GLM::PRF::QRY_DBUF* buffer_debug = nullptr;
        GLM::PRF::QRY_DBUF* buffer_debug_p = nullptr;
        
        GLM::FB_T* render_target = nullptr;
        
        GLM::BUF_A* vertbuf;
        GLM::V_DESC vertdesc = {};
        
        GLM::SHD v_shader = {};
        GLM::SHD f_shader = {};
        GLM::PIP eye_pip = {};
        
        GLM::S_2DA* eyetex;
        GLM::SM_2DA eyesamp = {};

        GLM::UNIFVAL transforms = {};
        GLM::UNIFVAL timeval = {};
        GLM::UNIFVAL texture_size = {};
        GLM::UNIFSMP textures_array = {};

        Bytes transform_data = {};
        Bytes time_data = {};
        Bytes texsize_data = {};
        
        // Graphics data
        CGCamera camera;
        Matf4 object_matrices[6] = {};
        scalar time_value = 0.f;
        
        Transform base_transform;
        Transform floor_transform;
        
        // Timing information
        bigscalar tprevious = 0.0;
        bigscalar tbase = 0.0;
        bigscalar tdelta = 0.0;
        
        Vecf4 clear_col = {.267f, .267f, .267f, 1.f};
        
        /* Now generating a drawcall, which only specifies small state that can be
         * shared */
        GLM::D_CALL call{false,true};
        GLM::Q_OCC* o_query = nullptr;
        
        /* Instance data is more akin to individual drawcalls, specifying vertex
         * buffer information */
        GLM::D_DATA instdata = {6,0,4};
        
        // View information
        GLM::VIEWSTATE viewportstate;
        GLM::RASTSTATE rasterstate_poly = {};
        GLM::RASTSTATE rasterstate_line = {};
        GLM::BLNDSTATE blendstate = {};
        GLM::PIXLSTATE pixlstate = {};
        GLM::DEPTSTATE deptstate = {};
        GLM::TSLRSTATE teslstate = {};
        GLM::STENSTATE stenstate = {};
        
        GLM::USTATE unifstate = {};
        GLM::USTATE unifstate_f = {};
        
        // Texture information
        bool did_apply_state = false;
        
    } g_data;
    
    struct RGPUQuery {
        GpuInfo::GpuQueryInterface fun;
        bool haveGpuQuery = false;
    } gq_data;
};

void KeyEventHandler(void* r, const CIEvent& e, c_cptr data)
{
    RendererState* rdata = C_FCAST<RendererState*>(r);

    if(e.type == CIEvent::Keyboard)
    {
        auto kev = C_CAST<CIKeyEvent const*>(data);
        
        /* Single presses */
        if(kev->mod & CIKeyEvent::PressedModifier
           && !(kev->mod&CIKeyEvent::RepeatedModifier))
        {
//            if(kev->key == CK_F10)
//                m_debugging = !m_debugging;
//            else if(kev->key == CK_F9)
//                setWindowPosition({0,0});
        }
    }
    
    if(e.type == CIEvent::TouchTap)
    {
        cDebug("Success!");
        rdata->g_data.camera.position = {0.f, 0.f, -15.f};
    }

    if(e.type == CIEvent::TouchPan)
    {
        auto pev = C_FCAST<CIMTouchMotionEvent*>(data);
        rdata->g_data.camera.position.x() = pev->origin.x / 1920.f;
        rdata->g_data.camera.position.y() = pev->origin.y / 1200.f;
    }
    
    if(e.type == CIEvent::TouchRotate)
    {
        auto rev = C_FCAST<CITouchRotateEvent*>(data);
        
        Quatf rotation = Quatf(1.f, 0.f, rev->radians * 0.05f, 0.f);
        
        rdata->g_data.camera.rotation =
                    normalize_quat(rotation * rdata->g_data.camera.rotation);
    }
    
    if(e.type == CIEvent::TouchPinch)
    {
        CITouchPinchEvent* pev = C_FCAST<CITouchPinchEvent*>(data);
        
        rdata->g_data.camera.position.z() += (-pev->factor + 1.f) / 100.f;
    }
}

//void DisplayEventHandler(void* r, const CIEvent& e, c_cptr data)
//{
//    RendererState* rdata = C_FCAST<RendererState*>(r);
//    if(e.type == CDEvent::Resize)
//    {
//        auto rev = C_CAST<Display::CDResizeEvent const*>(data);
//        buffer_debug_p->resize(*rev);
//        
//        camera.aspect = scalar(rev->w)/scalar(rev->h);
//    }
//}

void SetupRendering(CDRenderer& renderer, RendererState* d)
{
    auto& g = d->g_data;

    g = {};

    {
        szptr restore_size = sizeof(d->r_state);
        Store::RestoreMemory(&d->r_state, &restore_size, 0);
    }
    
    d->gq_data.haveGpuQuery = GpuInfo::LoadDefaultGpuQuery(&d->gq_data.fun);
    
    cVerbose("Entering run() function");
    
    ProfContext a("Renderer setup");
    
    const constexpr cstring textures[num_textures] = {
        "circle_red.png", "circle_blue.png", "circle_alpha.png",
        "floor-tile.png"};
    
    const scalar vertexdata[] = {
        -1.f, -1.f,  0.f,  0.f,  0.f,
        1.f, -1.f,  0.f,  1.f,  0.f,
        -1.f,  1.f,  0.f,  0.f,  1.f,
        
        -1.f,  1.f,  0.f,  0.f,  1.f,
        1.f,  1.f,  0.f,  1.f,  1.f,
        1.f, -1.f,  0.f,  1.f,  0.f,
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
    
    g.vertbuf = new GLM::BUF_A(ResourceAccess::ReadOnly,
                                       sizeof(vertexdata));
    auto& vertbuf = *g.vertbuf;
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
        pos.m_size = 3;
        pos.m_type = TypeEnum::Scalar;
        pos.m_stride = sizeof(Vecf3) + sizeof(Vecf2);
        
        GLM::V_ATTR tc = {};
        tc.m_idx = 1;
        tc.m_size = 2;
        tc.m_off = sizeof(Vecf3);
        tc.m_type = TypeEnum::Scalar;
        tc.m_stride = sizeof(Vecf3) + sizeof(Vecf2);
        
        /* Finally we add these attributes to the descriptor */
        vertdesc.addAttribute(pos);
        vertdesc.addAttribute(tc);

        Profiler::PopContext();
    }
    
    cVerbose("Generated vertex buffers");
    
    auto& v_shader = g.v_shader;
    auto& f_shader = g.f_shader;
    auto& eye_pip = g.eye_pip;
    
    /* Compiling shaders and assemble a graphics pipeline */
    
    do{
        ProfContext b("Shader compilation");
        const constexpr cstring shader_files[] = {
            "vr/vshader.glsl", "vr/fshader.glsl",
            "vr/vshader_es.glsl", "vr/fshader_es.glsl",
            "vr/vshader_es2.glsl", "vr/fshader_es2.glsl"};
        
        bool isGles = (GLM::LevelIsOfClass(GLM::Level(),GLM::APIClass::GLES));
        bool isGles20 = GLM::Level() == RHI::GLEAM::GLES_2_0;
        
        CResources::Resource v_rsc(shader_files[isGles * 2 + isGles20 * 2],
                                   ResourceAccess::SpecifyStorage |
                                   ResourceAccess::AssetFile);
        CResources::Resource f_rsc(shader_files[isGles * 2 + isGles20 * 2 + 1],
                                   ResourceAccess::SpecifyStorage |
                                   ResourceAccess::AssetFile);
        if (!CResources::FileMap(v_rsc) || !CResources::FileMap(f_rsc)) {
            cWarning("Failed to map resources: {0}, {1}", v_rsc.resource(),
                     f_rsc.resource());
            break;
        }
        
        Bytes v_shader_code = FileGetDescriptor(v_rsc);
        Bytes f_shader_code = FileGetDescriptor(f_rsc);
        
        if (v_shader.compile(ShaderStage::Vertex, v_shader_code) &&
            f_shader.compile(ShaderStage::Fragment, f_shader_code)) {
            cVerbose("Shaders compiled");
            eye_pip.attach(v_shader, ShaderStage::Vertex);
            eye_pip.attach(f_shader, ShaderStage::Fragment);
            cVerbose("Shaders attached");
            if (!eye_pip.assemble()) {
                cWarning("Invalid pipeline");
                break;
            }
            cVerbose("GPU pipeline assembled");
        } else {
            cWarning("Shader compilation failed");
            break;
        }
        v_shader.dealloc();
        f_shader.dealloc();
        CResources::FileUnmap(v_rsc);
        CResources::FileUnmap(f_rsc);
        
        GLM::PRF::QRY_PIPDMP dumper(eye_pip);
        dumper.dump("hello.prg");
    } while(false);
    cVerbose("Compiled shaders");
    
    /*
     * Binding the pipeline for usage
     * This has different meaning across GL3.3 and GL4.3+
     */
    eye_pip.bind();
    cVerbose("Pipeline bind");
    
    /* Uploading textures */
    g.eyetex = new GLM::S_2DA(PixelFormat::RGBA8, 1,
                              GLM::TextureDMABuffered
                              | GLM::TextureArrayPerInstance);
    auto& eyetex = *g.eyetex;
    
    eyetex.allocate({1024, 1024, 4}, PixCmp::RGBA);
    cVerbose("Texture allocation");
    
    Profiler::PushContext("Texture loading");
    for (int32 i = 0; i < eyetex.m_size.depth; i++) {
        CResources::Resource rsc(textures[i], ResourceAccess::SpecifyStorage |
                                 ResourceAccess::AssetFile);
        CResources::FileMap(rsc);
        
        CStbImageLib::CStbImage img;
        CStbImageLib::LoadData(&img, &rsc);
        
        eyetex.upload(BitFormat::UByte, PixCmp::RGBA, {img.size.w, img.size.h, 1},
                      img.data, {0, 0, i});
        cVerbose("Texture upload #{0}", i);
        
        CStbImageLib::ImageFree(&img);
        CResources::FileUnmap(rsc);
    }

    {
        ASIO::AsioContext ctx = ASIO::ASIO_Client::InitService();

        Net::Resource rsc(ctx, "http://i.imgur.com/nQdOmCJ.png"_http);

        if(rsc.fetch())
        {
            auto imdata = rsc.data();

            CStbImageLib::CStbImage img;
            CStbImageLib::LoadData(&img, BytesConst(imdata.data,
                                                    imdata.size, 0));

            eyetex.upload(BitFormat::UByte, PixCmp::RGBA,
                          {img.size.w, img.size.h, 1},
                          img.data, {0, 0, 0});
        }else{
            cWarning("Failed to retrieve spicy meme");
        }
    }

    Profiler::PopContext();
    cVerbose("Uploading textures");
    
    /* Attaching the texture data to a sampler object */
    auto& eyesamp = g.eyesamp;
    eyesamp = {};
    eyesamp.alloc();
    eyesamp.attach(&eyetex);
    eyesamp.setFiltering(Filtering::Linear, Filtering::Linear);
    cVerbose("Setting sampler properties");
    
    /* Creating the uniform data store */
    
    /*
     * These specify byte buffers which refer to other data
     * This makes it simple to redirect or reallocate the uniform data
     *
     * These can be rotated to achieve per-frame disposable buffers,
     *  allowing multiple frames to be processed concurrently without halt
     */
    auto& transform_data = g.transform_data;
    auto& time_data = g.time_data;

    transform_data = Bytes::Create(g.object_matrices);
    time_data = Bytes::Create(g.time_value);

    g.textures_array = eyesamp.handle();
    
    g.transforms.data = &transform_data;
    g.timeval.data = &time_data;
    g.texture_size.data = &g.texsize_data;
    
    /* We create some pipeline state, such as blending and viewport state */
    auto& viewportstate = g.viewportstate;
    auto& blendstate = g.blendstate;
    auto& deptstate = g.deptstate;
    auto& rasterstate_line = g.rasterstate_line;
    
    CSize win_size = renderer.windowSize();
    
    blendstate.m_doBlend = true;
    viewportstate.m_view.clear();
    viewportstate.m_view.push_back({0, 0, win_size.w, win_size.h});
    viewportstate.m_scissor.clear();
    viewportstate.m_scissor.push_back({0,0,win_size.w,win_size.h});
    viewportstate.m_mview = true;
    rasterstate_line.m_wireframe = true;
    deptstate.m_test = true;
    deptstate.m_func = C_CAST<uint32>(ValueComparison::Less);
    
    /* Applying state information */
    GLM::SetViewportState(viewportstate, 0);
    GLM::SetStencilState(g.stenstate,0);
    GLM::SetBlendState(g.blendstate);
    GLM::SetPixelProcessState(g.pixlstate);
    GLM::SetDepthState(deptstate);
    GLM::SetTessellatorState(g.teslstate);
    cVerbose("Set renderer state");
    
    /* We query the current pipeline for possible uniform/texture/buffer values
     */
    Vector<GLM::UNIFDESC> unifs;
    GLM::GetShaderUniformState(eye_pip, &unifs);
    auto& unifstate_v = g.unifstate;
    auto& unifstate_f = g.unifstate_f;
    /* We assign CPU-side values to GPU-side values */
    for (GLM::UNIFDESC const &u : unifs) {
        if (u.m_name == "transform")
        {
            unifstate_v.setUniform(u, &g.transforms);
            cVerbose(4, "Array size: {0}", u.m_arrSize);
        }
        else if (u.m_name == "texdata")
            unifstate_f.setSampler(u, &g.textures_array);
        else if (u.m_name == "mx")
            unifstate_f.setUniform(u, &g.timeval);
        else if (u.m_name == "texdata_gridSize")
            unifstate_f.setUniform(u, &g.texture_size);
        else
            cVerbose(4,"Unhandled uniform value: {0}", u.m_name);
    }
    
    cVerbose("Acquire and set shader uniforms");
    
    /* Specifying the uniform data, such as camera matrices
     *  and transforms */
    
    
    auto& camera = g.camera;
    
    camera = CGCamera();
    
    camera.aspect = 1.6f;
    camera.fieldOfView = 85.f;
    camera.zVals.far_ = 100.;
    
    camera.position = Vecf3(0, 0, -15);
    
    auto& base_transform = g.base_transform;
    auto& floor_transform = g.floor_transform;
    
    base_transform.position = Vecf3(0, 0, 5);
    base_transform.scale = Vecf3(1);
    
    floor_transform.position = Vecf3(0, -2, 5);
    floor_transform.scale = Vecf3(2);
    
    /* Vertex descriptors are based upon the ideas from GL4.3 */
    vertdesc.bind();
    vertdesc.bindBuffer(0, vertbuf);
    
    GLM::PreDrawCleanup();
    
    GLM::DefaultFramebuffer().clear(0, g.clear_col, 0.5);
    
    g.buffer_debug = new GLM::PRF::QRY_DBUF(GLM::DefaultFramebuffer(),
                                                    DBuffers::Color | DBuffers::Depth);
    
    g.buffer_debug_p = g.buffer_debug;
    
    g.render_target = &GLM::DefaultFramebuffer();
    
    g.tprevious = d->r_state.time_base;
    g.tbase = d->r_state.time_base;

    g.o_query = new GLM::Q_OCC(CGL::QueryT::AnySamplesCon);
}

void LogicLoop(CDRenderer& renderer, RendererState* d)
{
    ProfContext a("Logic frame", Profiler::DataPoint::Hot);

    auto& g = d->g_data;

    /*
     * Events are always late for the drawcall.
     * The best we can do is update the state as often as possible.
     * Or we could maximize the time of each iteration for VSYNC,
     *  polling once to begin with and also later.
     *
     * Example:
     * poll();
     * ...
     * upload();
     * process();
     * ...
     * millisleep(15);
     * poll();
     * draw();
     *
     */

    renderer.pollEvents();

    auto& base_transform = g.base_transform;
    auto& floor_transform = g.floor_transform;
    auto& camera = g.camera;

    auto& tprevious = g.tprevious;
    auto& tdelta = g.tdelta;
    auto& tbase = g.tbase;

    auto& time_value = g.time_value;

    auto& object_matrices = g.object_matrices;

    /* Define frame data */
    base_transform.position.x() = C_CAST<scalar>(CMath::sin(tprevious) * 2.);
    base_transform.position.y() = C_CAST<scalar>(CMath::cos(tprevious) * 2.);

    //      camera.position.z() = -tprevious;

    time_value = C_CAST<scalar>(CMath::sin(tprevious) + (CMath::pi / 4.));

    floor_transform.rotation.y() = C_CAST<scalar>(CMath::sin(tprevious));
    floor_transform.rotation = normalize_quat(floor_transform.rotation);

    camera.position.x() -= 0.1;
    object_matrices[0] = GenPerspective(camera) * GenTransform(camera) *
    GenTransform(base_transform);
    object_matrices[2] = GenPerspective(camera) * GenTransform(camera) *
    GenTransform(floor_transform);

    camera.position.x() += 0.2;
    object_matrices[1] = GenPerspective(camera) * GenTransform(camera) *
    GenTransform(base_transform);
    object_matrices[3] = GenPerspective(camera) * GenTransform(camera) *
    GenTransform(floor_transform);

    camera.position.x() -= 0.1;

    tdelta = tbase + renderer.contextTime() - tprevious;
    tprevious = tbase + renderer.contextTime();

    d->r_state.time_base = tprevious;
}

void RendererLoop(CDRenderer& renderer, RendererState* d)
{
    ProfContext a("Render frame", Profiler::DataPoint::Hot);

    auto& g = d->g_data;
    
    bool do_debugging = d->r_state.debug_enabled && PlatformData::IsDebug();
    if(do_debugging)
    {
        g.did_apply_state = false;
        g.render_target = &g.buffer_debug->debugTarget();
    }
    else
        g.render_target = &GLM::DefaultFramebuffer();
    
    g.render_target->clear(0,g.clear_col,1.);
    
    /*
     * Events are always late for the drawcall.
     * The best we can do is update the state as often as possible.
     * Or we could maximize the time of each iteration for VSYNC,
     *  polling once to begin with and also later.
     *
     * Example:
     * poll();
     * ...
     * upload();
     * process();
     * ...
     * millisleep(15);
     * poll();
     * draw();
     *
     */

    /*
     * In APIs such as GL4.3+, this will apply vertex and fragment states
     * separately.
     * With GL3.3 it sets all state with the vertex stage and drops the rest.
     */
    if(d->r_state.debug_enabled || !g.did_apply_state)
    {
        g.did_apply_state = true;
        
        g.vertdesc.bind();
        g.vertdesc.bindBuffer(0, *g.vertbuf);
        g.eye_pip.bind();
        
        GLM::SetRasterizerState(g.rasterstate_poly);
        GLM::SetDepthState(g.deptstate);
    }

    scalar texSize = i32(CMath::sqrt(g.eyetex->texSize().depth));
    g.texsize_data = Bytes::Create(texSize);
    
    GLM::PipelineState pstate = {
        {ShaderStage::Vertex, g.unifstate},
        {ShaderStage::Fragment, g.unifstate_f}
    };
    
    /*
     * For VR, we could add drawcall parameters to specify this
     * The user would still specify their own projection matrices per-eye,
     *  or we could fabricate these.
     * We would primarily support stereo instancing,
     *  because this has a lot of benefits to efficiency.
     */
    
    GLM::Draw(g.eye_pip, pstate, g.vertdesc, g.call, g.instdata);
    
//    GLM::DrawConditional(g.eye_pip, pstate, g.vertdesc,
//                         g.call, g.instdata, *g.o_query);
    
    if(d->r_state.debug_enabled)
    {
        g.did_apply_state = false;
        GLM::DefaultFramebuffer().clear(0,g.clear_col,1.);
        g.buffer_debug->end();
    }
    
    renderer.swapBuffers();
    
    d->frameCount++;
}

void RendererCleanup(CDRenderer& renderer, RendererState* d)
{
//    Vector<byte_t> m_framebuffer;
//    GLM::DumpFramebuffer(GLM::DefaultFramebuffer(), PixelComponents::RGBA, TypeEnum::UByte,
//                         m_framebuffer);

//    PNG::Save(m_framebuffer, GLM::DefaultFramebuffer().size(), "test.png");

    auto& g = d->g_data;

    g.vertbuf->dealloc();
    g.vertdesc.dealloc();
    g.v_shader.dealloc();
    g.f_shader.dealloc();
    g.eye_pip.dealloc();
    g.eyesamp.dealloc();

    delete g.buffer_debug;
    g.buffer_debug = nullptr;

    delete g.o_query;
    g.o_query = nullptr;

    delete g.vertbuf;
    g.vertbuf = nullptr;

    delete g.eyetex;
    g.eyetex = nullptr;

    g.render_target = nullptr;

    g.loader = nullptr;
    GLM::UnloadAPI();

    cDebug("Saving time: {0}", d->r_state.time_base);
    Store::SaveMemory(&d->r_state, sizeof(d->r_state), 0);
}

void frame_count(RendererState* d)
{
    cDebug("Swaps/s: {0}", d->frameCount);

    d->frameCount = 0;

    if(d->gq_data.haveGpuQuery)
    {
        auto fun = d->gq_data.fun;
        cDebug("GPU Driver: {0}", fun.GetDriver());
        cDebug("GPU Devices: {0}", fun.GetNumGpus());
        for(GpuInfo::GpuView e : GpuInfo::GpuQueryView(fun))
        {
            cDebug("GPU Model: {0}", e.model());

            auto temp = e.temp();
            cDebug("Temperature: {0} // {1}",
                   temp.current, temp.max);

            auto mem = e.mem();
            cDebug("Memory use: tot={0}, used={1}, free={2}",
                   mem.total, mem.used, mem.free);
            cDebug("Memory used by this application: {0}",
                   e.memUsage(ProcessProperty::Pid()));

            auto clk = e.clock(GpuInfo::Clock::Graphics);
            cDebug("Clock limits: {0} / {1} / {2}",
                   clk.current, clk.min, clk.max);
            clk = e.clock(GpuInfo::Clock::Memory);
            cDebug("Memory limits: {0} / {1} / {2}",
                   clk.current, clk.min, clk.max);
            clk = e.clock(GpuInfo::Clock::VideoDecode);
            cDebug("Video limits: {0} / {1} / {2}",
                   clk.current, clk.min, clk.max);

            auto bus = e.bus();
            cDebug("Bus information: rx:{0} KB/s tx:{1} KB/s",
                   bus.rx, bus.tx);

            auto util = e.usage();
            cDebug("GPU usage: GPU={0}%,"
                   " MEM={1}%, DECODER={2}%,"
                   " ENCODER={3}%",
                   util.gpu, util.mem, util.decoder, util.encoder);

            cDebug("Power mode: {0}", C_CAST<uint32>(e.pMode()));
        }
    }

    cDebug("Memory: {0}", ProcessProperty::Mem(ProcessProperty::Pid()));
}

