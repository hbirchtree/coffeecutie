#include <coffee/CGraphics>
#include <coffee/CSDL2>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/graphics_apis/CGLeamRHI>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/image/cimage.h>

#include <coffee/core/platform_data.h>

#include <openvr.h>

//#define USE_NULL_RENDERER

vr::COpenVRContext vr_ctxt;

using namespace Coffee;
using namespace Display;

class CDRenderer : public CSDL2Renderer {
public:
#ifndef USE_NULL_RENDERER
    using GLM = GLEAMAPI;
#else
    using GLM = RHI::NullAPI;
#endif

    static void frame_count(uint32 f, c_cptr)
    {
        cDebug("Swaps/s: {0}",f);
    }

private:
    bool m_debugging = false;
    GLM::PRF::QRY_DBUF* buffer_debug_p;
    CGCamera camera;

    bool m_paused = false;

public:
    CDRenderer() : CSDL2Renderer(0) {}
    static const constexpr szptr num_textures = 5;

    void run() {
        cVerbose("Entering run() function");

        Profiler::PushContext("Renderer");

        const constexpr cstring textures[num_textures] = {
            "circle_red.png", "circle_blue.png", "circle_alpha.png",
            "floor-tile.png"};

        const scalar vertexdata[] = {
            -1.f, -1.f,  0.f,  0.f,  0.f,
            1.f, -1.f,  0.f, -1.f,  0.f,
            -1.f,  1.f,  0.f,  0.f, -1.f,

            -1.f,  1.f,  0.f,  0.f, -1.f,
            1.f,  1.f,  0.f, -1.f, -1.f,
            1.f, -1.f,  0.f, -1.f,  0.f,
        };

        cVerbose("Loading GLeam API");
        /*
         * Loading the GLeam API, chosen according to what is available at runtime
         */

        GLM::LoadAPI(true);

        {
            vr::EVRInitError err;
            if(vr::VR_Init(&err,vr::VRApplication_Scene)&&vr::VRCompositor()&&vr_ctxt.VRSystem())
            {
                cVerbose(5,"Initialized VR system");
            }else{
                cVerbose(5,"Failed to initialize VR system: {0}",
                         vr::VR_GetVRInitErrorAsEnglishDescription(err));
            }
        }

        GLM::BUF_A vertbuf(ResourceAccess::ReadOnly, sizeof(vertexdata));
        GLM::V_DESC vertdesc = {};

        cVerbose("Entering GL initialization stage");
        /* Uploading vertex data and creating descriptors */
        {
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
        }

        cVerbose("Generated vertex buffers");

        GLM::SHD v_shader = {};
        GLM::SHD f_shader = {};
        GLM::PIP eye_pip = {};

        /* Compiling shaders and assemble a graphics pipeline */
        {
            const constexpr cstring shader_files[] = {
                "vr/vshader.glsl", "vr/fshader.glsl", "vr/vshader_es.glsl",
                "vr/fshader_es.glsl"};

            CResources::Resource v_rsc(shader_files[PlatformData::IsGLES() * 2],
                    ResourceAccess::SpecifyStorage |
                    ResourceAccess::AssetFile);
            CResources::Resource f_rsc(shader_files[PlatformData::IsGLES() * 2 + 1],
                    ResourceAccess::SpecifyStorage |
                    ResourceAccess::AssetFile);
            if (!CResources::FileMap(v_rsc) || !CResources::FileMap(f_rsc)) {
                cWarning("Failed to map resources: {0}, {1}", v_rsc.resource(),
                         f_rsc.resource());
                return;
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
                    return;
                }
                cVerbose("GPU pipeline assembled");
            } else {
                cWarning("Shader compilation failed");
                return;
            }
            v_shader.dealloc();
            f_shader.dealloc();
            CResources::FileUnmap(v_rsc);
            CResources::FileUnmap(f_rsc);

            GLM::PRF::QRY_PIPDMP dumper(eye_pip);
            dumper.dump("hello.prg");
        }
        cVerbose("Compiled shaders");

        /*
         * Binding the pipeline for usage
         * This has different meaning across GL3.3 and GL4.3+
         */
        eye_pip.bind();
        cVerbose("Pipeline bind");

        /* Uploading textures */
        GLM::S_2DA eyetex(PixelFormat::SRGB8A8, 1, GLM::TextureDMABuffered);

        eyetex.allocate({1024, 1024, 4}, PixCmp::RGBA);
        cVerbose("Texture allocation");

        Profiler::Profile("Pre-texture loading");
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
        Profiler::Profile("Texture loading");
        cVerbose("Uploading textures");

        /* Attaching the texture data to a sampler object */
        GLM::SM_2DA eyesamp;
        eyesamp.alloc();
        eyesamp.attach(&eyetex);
        eyesamp.setFiltering(Filtering::Linear, Filtering::Linear);
        cVerbose("Setting sampler properties");

        /* Creating the uniform data store */
        Matf4 object_matrices[6] = {};
        scalar time_value = 0.f;

        /*
         * These specify byte buffers which refer to other data
         * This makes it simple to redirect or reallocate the uniform data
         *
         * These can be rotated to achieve per-frame disposable buffers,
         *  allowing multiple frames to be processed concurrently without halt
         */
        Bytes transform_data = {(byte_t *)object_matrices, sizeof(object_matrices)};
        Bytes time_data = {(byte_t *)&time_value, sizeof(time_value)};

        GLM::UNIFVAL transforms = {};
        GLM::UNIFVAL timeval = {};
        GLM::UNIFSMP textures_array = eyesamp.handle();

        transforms.data = &transform_data;
        timeval.data = &time_data;

        /* We create some pipeline state, such as blending and viewport state */
        GLM::VIEWSTATE viewportstate(1);
        GLM::RASTSTATE rasterstate_poly = {};
        GLM::RASTSTATE rasterstate_line = {};
        GLM::BLNDSTATE blendstate = {};
        GLM::USTATE unifstate = {};
        GLM::PIXLSTATE pixlstate = {};
        GLM::DEPTSTATE deptstate = {};
        GLM::TSLRSTATE teslstate = {};

        GLM::STENSTATE stenstate = {}, stenstate_write = {};

        stenstate.m_test = true;
        blendstate.m_doBlend = true;
        viewportstate.m_view.clear();
        viewportstate.m_view.push_back({0, 0, 1280, 720});
        viewportstate.m_scissor.clear();
        viewportstate.m_scissor.push_back({0,0,1280,720});
        viewportstate.m_mview = true;

        GLM::VIEWSTATE viewstate_vr = viewportstate;
        viewstate_vr.m_view.clear();

        rasterstate_line.m_wireframe = true;
        deptstate.m_test = true;
        deptstate.m_func = (uint32)ValueComparison::Less;

        /* Applying state information */
        GLM::SetViewportState(viewportstate, 0);
        GLM::SetStencilState(stenstate,0);
        GLM::SetBlendState(blendstate);
        GLM::SetPixelProcessState(pixlstate);
        GLM::SetDepthState(deptstate);
        GLM::SetTessellatorState(teslstate);
        cVerbose("Set renderer state");

        /* We query the current pipeline for possible uniform/texture/buffer values
         */
        Vector<GLM::UNIFDESC> unifs;
        GLM::GetShaderUniformState(eye_pip, &unifs);

        /* We assign CPU-side values to GPU-side values */
        for (GLM::UNIFDESC const &u : unifs) {
            if (u.m_name == "transform[0]")
                unifstate.setUniform(u, &transforms);
            else if (u.m_name == "texdata")
                unifstate.setSampler(u, &textures_array);
            else if (u.m_name == "mx")
                unifstate.setUniform(u, &timeval);
            else
                cVerbose(4,"Unhandled uniform value: {0}", u.m_name);
        }

        cVerbose("Acquire and set shader uniforms");

        /* Now generating a drawcall, which only specifies small state that can be
         * shared */
        GLM::DrawCall call{false,true};

        /* Instance data is more akin to individual drawcalls, specifying vertex
         * buffer information */
        GLM::DrawInstanceData instdata = {6,0,4};

        /* Specifying the uniform data, such as camera matrices and transforms */
        Vecf4 clear_col = {.267f, .267f, .267f, 1.f};

        camera.aspect = 1.6f;
        camera.fieldOfView = 70.f;
        camera.zVals.far = 100.;

        camera.position = Vecf3(0, 0, -9);

        CTransform base_transform;
        base_transform.position = Vecf3(0, 0, 5);
        base_transform.scale = Vecf3(1);

        CTransform floor_transform;
        floor_transform.position = Vecf3(0, -2, 5);
        floor_transform.scale = Vecf3(2);

        /* Vertex descriptors are based upon the ideas from GL4.3 */
        vertdesc.bind();
        vertdesc.bindBuffer(0, vertbuf);

        bigscalar tprevious = this->contextTime();
        bigscalar tdelta = 0.1;

        GLM::PreDrawCleanup();

        GLM::DefaultFramebuffer.clear(0, clear_col, 1.f);

        GLM::PRF::QRY_DBUF buffer_debug(GLM::DefaultFramebuffer,
                                        DBuffers::Color | DBuffers::Depth);

        buffer_debug_p = &buffer_debug;

        GLM::FB_T* render_target = &GLM::DefaultFramebuffer;

        Coffee::Counter frame_counter(frame_count);

        bool do_debugging = false;
        bool did_apply_state = false;

        vr::Texture_t eye_texture;
        vr::VRTextureBounds_t eye_bounds[2] = {};

        GLM::FB_T vr_target;
        GLM::S_2D *vr_ctarget = nullptr, *vr_dtarget = nullptr;

        if(vr_ctxt.VRSystem())
        {
            /* Set primary color for VR stuffs */
            vr::HmdColor_t sceneCol;
            sceneCol.r = sceneCol.g = 0.5;
            sceneCol.b = sceneCol.a = 1.0;
            vr_ctxt.VRChaperone()->SetSceneColor(sceneCol);
        }

        {
            _cbasic_size_2d<uint32> _s = {540, 720};

            if (vr_ctxt.VRSystem())
            {
                vr_ctxt.VRSystem()->GetRecommendedRenderTargetSize(&_s.w, &_s.h);
            }

            CSize s = {(int32)_s.w, (int32)_s.h};

            vr_target.alloc();
            vr_ctarget = new GLM::S_2D(PixelFormat::SRGB8A8,1);
            vr_dtarget = new GLM::S_2D(PixelFormat::Depth24Stencil8,1);

            eye_bounds[0].uMax = eye_bounds[1].uMin = 0.5;
            eye_bounds[1].uMax = 1.0;
            eye_bounds[0].vMax = eye_bounds[1].vMax = 1.0;

            s.w *= 2;
            vr_ctarget->allocate(s,PixCmp::RGBA);
            vr_dtarget->allocate(s,PixCmp::Depth);

            vr_target.attachDepthStencilSurface(*vr_dtarget,0);
            vr_target.attachSurface(*vr_ctarget,0);

            vr_target.resize(0,{0,0,s.w,s.h});

            eye_texture.eType = vr::API_OpenGL;
            eye_texture.eColorSpace = vr::ColorSpace_Gamma;
            eye_texture.handle = FitIntegerInPtr(vr_ctarget->handle());


            CRect vr_renderSize = { 0,0, (int32)_s.w*2,(int32)_s.h };

            viewstate_vr.m_view.push_back(vr_renderSize);

            camera.aspect = scalar(vr_renderSize.w/2.f) / scalar(vr_renderSize.h);
            setWindowSize(vr_renderSize.size()/2);
        }


        GLM::FB_T* default_fb = &GLM::DefaultFramebuffer;
        //	if(vr_ctxt.VRSystem())
        default_fb = &vr_target;

        Matf4 camera_mat;
        Matf4 camera_tmp;
        Matf4 camera_pre[2] = {};
        Matf4 camera_pose;
        Matf4 aspect_scaling = scale(Matf4(), Vecf3(0.5,1,1));

        if (vr_ctxt.VRSystem())
        {
            auto mat = vr_ctxt.VRSystem()->GetProjectionMatrix(vr::Eye_Left, 1, 10, vr::API_OpenGL);
            MemCpy(&camera_mat, &mat, sizeof(Matf4));
            auto mat_tf = vr_ctxt.VRSystem()->GetEyeToHeadTransform(vr::Eye_Left);
            camera_tmp = Matf4();
            MemCpy(&camera_tmp, &mat_tf, sizeof(mat_tf));
            camera_pre[0] = camera_mat * inverse(camera_tmp);

            mat = vr_ctxt.VRSystem()->GetProjectionMatrix(vr::Eye_Right, 1, 10, vr::API_OpenGL);
            MemCpy(&camera_mat, &mat, sizeof(Matf4));
            mat_tf = vr_ctxt.VRSystem()->GetEyeToHeadTransform(vr::Eye_Right);
            camera_tmp = Matf4();
            MemCpy(&camera_tmp, &mat_tf, sizeof(mat_tf));
            camera_pre[1] = camera_mat * inverse(camera_tmp);

            camera_pre[0] = camera_pre[0] * aspect_scaling;
            camera_pre[1] = camera_pre[1] * aspect_scaling;
        }

        vr::TrackedDevicePose_t trackedDevicePoses[vr::k_unMaxTrackedDeviceCount];

        cDebug("Monitor resolution: {0}",this->monitor().screenArea.size());

        while (!closeFlag()) {

            do_debugging = m_debugging && buffer_debug.enabled();

            if(do_debugging)
            {
                did_apply_state = false;
                render_target = &buffer_debug.debugTarget();
            }
            else
                render_target = default_fb;

            render_target->clear(0,clear_col,1.);

            /*
           * Events are always late for the drawcall.
           * The best we can do is update the state as often as possible.
           * Or we could maximize the time of each iteration for VSYNC,
           *  polling once to begin with and also later.
           *
           * Example:
           * poll();
           * ...
           * upload();33
           * process();
           * ...
           * millisleep(15);
           * poll();
           * draw();
           *
           */

            this->pollEvents();

            if(!m_paused)
            {
                /* Define frame data */
                base_transform.position.x() = CMath::sin(tprevious) * 2;
                base_transform.position.y() = CMath::cos(tprevious) * 2;

                floor_transform.rotation.y() = CMath::sin(tprevious);
                floor_transform.rotation = normalize_quat(floor_transform.rotation);

//                camera.position.z() = 1. + CMath::fmod(tprevious,5.0);
            }

            if (vr_ctxt.VRCompositor())
            {
                vr_ctxt.VRCompositor()->WaitGetPoses(trackedDevicePoses, vr::k_unMaxTrackedDeviceCount,
                                                     nullptr, 0);
                if (trackedDevicePoses[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
                {
                    auto& mat = trackedDevicePoses[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking;
                    MemCpy(&camera_pose,&mat,sizeof(mat));
                    camera_pose = camera_pose;
                }
            }

            camera.position.x() = -1;
            if(vr_ctxt.VRCompositor())
            {
                camera_mat = camera_pre[0] * camera_pose * GenTransform(camera) * aspect_scaling;
            }else{
                camera_mat = GenPerspective(camera) * GenTransform(camera) * aspect_scaling;
            }
            object_matrices[0] = camera_mat * GenTransform(base_transform);
            object_matrices[2] = camera_mat * GenTransform(floor_transform);

            camera.position.x() = 1;
            if(vr_ctxt.VRCompositor())
            {
                camera_mat = camera_pre[1] * camera_pose * GenTransform(camera) * aspect_scaling;
            }else{
                camera_mat = GenPerspective(camera) * GenTransform(camera) * aspect_scaling;
            }
            object_matrices[1] = camera_mat * GenTransform(base_transform);
            object_matrices[3] = camera_mat * GenTransform(floor_transform);

            /*
           * In APIs such as GL4.3+, this will apply vertex and fragment states
           * separately.
           * With GL3.3 it sets all state with the vertex stage and drops the rest.
           */
            if(do_debugging || !did_apply_state)
            {
                did_apply_state = true;

                vertdesc.bind();
                vertdesc.bindBuffer(0, vertbuf);
                eye_pip.bind();

                GLM::SetRasterizerState(rasterstate_poly);
                GLM::SetDepthState(deptstate);
            }

            GLM::SetShaderUniformState(eye_pip, ShaderStage::Vertex, unifstate);
            GLM::SetShaderUniformState(eye_pip, ShaderStage::Fragment, unifstate);

            /*
           * For VR, we could add drawcall parameters to specify this
           * The user would still specify their own projection matrices per-eye,
           *  or we could fabricate these.
           * We would primarily support stereo instancing,
           *  because this has a lot of benefits to efficiency.
           */
            GLM::SetViewportState(viewstate_vr);
            GLM::Draw(call, instdata);
            GLM::SetViewportState(viewportstate);

            frame_counter.update(Time::CurrentMicroTimestamp()/1000);

            if(vr_ctxt.VRCompositor())
            {
                vr_ctxt.VRCompositor()->Submit(vr::Eye_Left,&eye_texture,
                                               &eye_bounds[0]);
                vr_ctxt.VRCompositor()->Submit(vr::Eye_Right,&eye_texture,
                                               &eye_bounds[1]);

                CGL::CGL33::Flush();

                vr_ctxt.VRCompositor()->WaitGetPoses(nullptr,0,nullptr,0);
            }

            vr_target.blit(
                        viewstate_vr.m_view[0].convert<int64>(),GLM::DefaultFramebuffer,
                    CRect64(this->windowSize().convert<int64>()),
                    DBuffers::Color,Filtering::Linear);

            if(do_debugging)
            {
                did_apply_state = false;
                default_fb->clear(0,clear_col,1.f);
                buffer_debug.end();
            }

            this->swapBuffers();

            tdelta = this->contextTime() - tprevious;
            tprevious = this->contextTime();
        }

        if(vr_ctxt.VRSystem())
        {
            vr::VR_Shutdown();
        }

        Profiler::PopContext();
    }
    void eventHandleD(const Display::CDEvent &e, c_cptr data) {
        CSDL2Renderer::eventHandleD(e, data);

        if(e.type == CDEvent::Resize)
        {
            auto rev = (Display::CDResizeEvent const*)data;
            buffer_debug_p->resize(*rev);

//            camera.aspect = scalar(rev->w)/scalar(rev->h);
        }
    }

    void eventHandleI(const CIEvent &e, c_cptr data) {
        CSDL2Renderer::eventHandleI(e, data);

        if(e.type == CIEvent::Keyboard)
        {
            auto kev = (CIKeyEvent const*)data;

            if(kev->key == CK_F10
                    && kev->mod & CIKeyEvent::PressedModifier
                    && !(kev->mod&CIKeyEvent::RepeatedModifier) )
                m_debugging = !m_debugging;

            if (kev->key == CK_p && kev->mod & CIKeyEvent::PressedModifier && !(kev->mod & CIKeyEvent::RepeatedModifier))
                m_paused = !m_paused;

        }
    }
};
