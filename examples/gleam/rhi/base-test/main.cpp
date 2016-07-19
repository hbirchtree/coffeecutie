#include <coffee/core/CApplication>
#include <coffee/graphics_apis/CGLeamRHI>
#include <coffee/CGraphics>
#include <coffee/CSDL2>

#include <coffee/core/input/eventhandlers.h>
#include <coffee/graphics_apis/SMesh>

#include <coffee/dummyplug/hmd-dummy.h>
#include <coffee/COculusRift>


using namespace Coffee;
using namespace Display;

#ifdef COFFEE_OCULUSVR_ENABLED
using VR = OculusRift::OculusVR;
#else
using VR = HMD::DummyPlugHMD;
#endif

VR::Device* dev;

#ifdef COFFEE_GLEAM_DESKTOP
using GL = CGL::CGL43;
#else
using GL = CGL::CGLES30;
#endif

using GLM = GLEAMAPI;

class CDRenderer : public CSDL2Renderer
{
public:
    CDRenderer()
        : CSDL2Renderer(0)
    {
    }

    static const constexpr szptr num_textures = 5;

    void run()
    {
        Profiler::PushContext("Renderer");

        const constexpr cstring textures[num_textures] = {
            "eye-normal.tga", "eye-weird.tga",
            "eye-alpha.tga",  "eye-veins.tga",
            "floor-tile.png"
        };

        const scalar vertexdata[] = {
            -1.f, -1.f,  0.f,    0.f,  0.f,
             1.f, -1.f,  0.f,   -1.f,  0.f,
            -1.f,  1.f,  0.f,    0.f, -1.f,

            -1.f,  1.f,  0.f,    0.f, -1.f,
             1.f,  1.f,  0.f,   -1.f, -1.f,
             1.f, -1.f,  0.f,   -1.f,  0.f,
        };

        /*
         * Loading the GLeam API, chosen according to what is available at runtime
         */
        GLM::LoadAPI(true);

        GLM::BUF_A vertbuf(ResourceAccess::ReadOnly,sizeof(vertexdata));
        GLM::V_DESC vertdesc = {};

        /* Uploading vertex data and creating descriptors */
        {
            vertdesc.alloc();
            vertbuf.alloc();

            vertbuf.commit(sizeof(vertexdata),vertexdata);

            /*
             * Specifying a vertex format which is applied.
             * This is driver- and API-agnostic
             */
            GLM::V_ATTR pos = {};
            pos.m_idx = 0;
            pos.m_bassoc = 0;
            pos.m_size = 3;
            pos.m_type = TypeEnum::Scalar;
            pos.m_stride = sizeof(Vecf3)+sizeof(Vecf2);
            pos.m_off = 0;

            GLM::V_ATTR tc = {};
            tc.m_idx = 1;
            tc.m_bassoc = 1;
            tc.m_size = 2;
            tc.m_type = TypeEnum::Scalar;
            tc.m_stride = sizeof(Vecf3)+sizeof(Vecf2);
            tc.m_off = sizeof(Vecf3);

            /* Finally we add these attributes to the descriptor */
            vertdesc.addAttribute(pos);
            vertdesc.addAttribute(tc);
        }

        GLM::SHD v_shader = {};
        GLM::SHD f_shader = {};
        GLM::PIP eye_pip = {};

        /* Compiling shaders and assemble a graphics pipeline */
        {
            CResources::Resource v_rsc("vr/vshader.glsl");
            CResources::Resource f_rsc("vr/fshader.glsl");
            CResources::FileMap(v_rsc);
            CResources::FileMap(f_rsc);
            if(v_shader.compile(CGL::ShaderStage::Vertex,(cstring)v_rsc.data)&&
                    f_shader.compile(CGL::ShaderStage::Fragment,(cstring)f_rsc.data))
            {
                eye_pip.attach(v_shader,CGL::ShaderStage::Vertex);
                eye_pip.attach(f_shader,CGL::ShaderStage::Fragment);
                if(!eye_pip.assemble())
                    return;
            }else
                return;
            CResources::FileUnmap(v_rsc);
            CResources::FileUnmap(f_rsc);
        }

        /*
         * Binding the pipeline for usage
         * This has different meaning across GL3.3 and GL4.3+
         */
        eye_pip.bind();

        /* Uploading textures */
        GLM::S_2DA eyetex(PixelFormat::RGBA8,1,GLM::TextureDMABuffered);

        eyetex.allocate({1024,1024,5},PixCmp::RGBA);

        for(szptr i=0;i<eyetex.m_size.depth;i++)
        {
            CResources::Resource rsc(textures[i]);
            CResources::FileMap(rsc);

            CStbImageLib::CStbImage img;
            CStbImageLib::LoadData(&img,&rsc);

            eyetex.upload(BitFormat::UByte,PixCmp::RGBA,{img.size.w,img.size.h,1},
                          img.data,{0,0,(int32)i});

            CResources::FileUnmap(rsc);
        }

        /* Attaching the texture data to a sampler object */
        GLM::SM_2DA eyesamp;
        eyesamp.alloc();
        eyesamp.attach(&eyetex);
        eyesamp.setFiltering(Filtering::Linear,Filtering::Linear);

        /* Creating the uniform data store */
        Bytes transform_data = {};
        Bytes mult_data = {};
        Bytes time_data = {};

        uint8 object_id[3] = {};
        Matf4 object_matrices[6] = {};
        scalar time_value = 0.f;

        /*
         * These specify byte buffers which refer to other data
         * This makes it simple to redirect or reallocate the uniform data
         *
         * These can be rotated to achieve per-frame disposable buffers,
         *  allowing multiple frames to be processed concurrently without halt
         */
        transform_data.size = sizeof(object_matrices);
        transform_data.data = (byte_t*)object_matrices;
        time_data.size = sizeof(time_value);
        time_data.data = (byte_t*)&time_value;

        GLM::UNIFVAL transforms = {};
        GLM::UNIFVAL multipliers = {};
        GLM::UNIFVAL timeval = {};
        GLM::UNIFSMP textures_array = eyesamp.handle();

        Vector<GLM::UNIFDESC> unifs;

        /* We create some pipeline state, such as blending and viewport state */
        GLM::VIEWSTATE viewportstate(1);
        GLM::RASTSTATE rasterstate_poly = {};
        GLM::RASTSTATE rasterstate_line = {};
        GLM::BLNDSTATE blendstate = {};
        GLM::USTATE unifstate = {};
        GLM::PIXLSTATE pixlstate = {};
        GLM::DEPTSTATE deptstate = {};
        GLM::TSLRSTATE teslstate = {};

        blendstate.m_doBlend = true;
        viewportstate.m_view.push_back(
        {
                        0,
                        0,
                        this->windowSize().w,
                        this->windowSize().h
                    });
        rasterstate_line.m_wireframe = true;

        /* We query the current pipeline for possible uniform/texture/buffer values */
        GLM::GetShaderUniformState(eye_pip,&unifs);

        /* We assign CPU-side values to GPU-side values */
        for(GLM::UNIFDESC const& u : unifs)
        {
            if(u.m_name == "transform[0]")
                unifstate.setUniform(u,&transforms);
            else if(u.m_name == "texdata")
                unifstate.setSampler(u,textures_array);
            else if(u.m_name == "mx")
                unifstate.setUniform(u,&timeval);
            else
                cDebug("Unhandled uniform value: {0}",u.m_name);
        }

        transforms.data = &transform_data;
        multipliers.data = &mult_data;
        timeval.data = &time_data;

        /* Applying state information */
        GLM::SetBlendState(blendstate);
        GLM::SetPixelProcessState(pixlstate);
        GLM::SetDepthState(deptstate);
        GLM::SetTessellatorState(teslstate);

        /* Now generating a drawcall, which only specifies small state that can be shared */
        GLM::DrawCall call;
        call.m_idxd = false;
        call.m_inst = true;

        /* Instance data is more akin to individual drawcalls, specifying vertex buffer information */
        GLM::DrawInstanceData instdata = {};
        instdata.m_insts = 4;
        instdata.m_verts = (sizeof(vertexdata)/sizeof(scalar));

        /* Specifying the uniform data, such as camera matrices and transforms */
        Vecf4 clear_col = {.267f,.267f,.267f,1.f};

        CGCamera camera;
        camera.aspect = 1.6f;
        camera.fieldOfView = 70.f;

        camera.position = Vecf3(0,0,-9);

        CTransform base_transform;
        base_transform.position = Vecf3(0,0,5);
        base_transform.scale = Vecf3(1);

        CTransform floor_transform;
        floor_transform.position = Vecf3(0,-2,5);
        floor_transform.scale = Vecf3(2);
//        floor_transform.rotation.x() = -0.9;
        floor_transform.rotation = normalize_quat(floor_transform.rotation);

        /* Vertex descriptors are based upon the ideas from GL4.3 */
        vertdesc.bind();
        vertdesc.bindBuffer(0,vertbuf);
        vertdesc.bindBuffer(1,vertbuf);

        eyesamp.bind(0);

        bigscalar tprevious = this->contextTime();
        bigscalar tdelta = 0.1;

        scalar v0 = 0;

        /* Clipping between the two virtual viewports */
#ifdef COFFEE_GLEAM_DESKTOP
        GL::Enable(GL::Feature::ClipDist,0);
#endif

        /* These improve rendering quality */
//        GL::Enable(GL::Feature::PolygonSmooth);
//        GL::Enable(GL::Feature::LineSmooth);

        while(!closeFlag())
        {

            /*
             * This will probably be incorporated into the GLM:: namespace somehow
             * We want to only clear the parts of the buffer that are necessary
             * eg. don't clear stencil and depth if they are unused
             *
             */
            GL::ClearBufferfv(true,0,clear_col);

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
            this->pollEvents();

            base_transform.position.x() = CMath::sin(tprevious) * 2;
            base_transform.position.y() = CMath::cos(tprevious) * 2;

//            floor_transform.position.z() = CMath::sin(tprevious);

            /* Define frame data */
            time_value = CMath::sin(tprevious)+(CMath::pi/4.);

            floor_transform.rotation.x() = CMath::sin(tprevious);
            floor_transform.rotation = normalize_quat(floor_transform.rotation);

            camera.position.x() = -2;

            object_matrices[0] = GenPerspective(camera)
                    * GenTransform(camera)
                    * GenTransform(base_transform);
            object_matrices[2] = GenPerspective(camera)
                    * GenTransform(camera)
                    * GenTransform(floor_transform);
            object_matrices[4] = GenPerspective(camera)
                    * GenTransform(camera)
                    * GenTransform(floor_transform);

            camera.position.x() = 2;

            object_matrices[1] = GenPerspective(camera)
                    * GenTransform(camera)
                    * GenTransform(base_transform);
            object_matrices[3] = GenPerspective(camera)
                    * GenTransform(camera)
                    * GenTransform(floor_transform);
            object_matrices[5] = GenPerspective(camera)
                    * GenTransform(camera)
                    * GenTransform(floor_transform);

            /*
             * In APIs such as GL4.3+, this will apply vertex and fragment states separately.
             * With GL3.3 it sets all state with the vertex stage and drops the rest.
             */
            GLM::SetShaderUniformState(eye_pip,CGL::ShaderStage::Vertex,unifstate);
            GLM::SetShaderUniformState(eye_pip,CGL::ShaderStage::Fragment,unifstate);

            GLM::SetRasterizerState(rasterstate_poly);

            /*
             * For VR, we could add drawcall parameters to specify this
             * The user would still specify their own projection matrices per-eye,
             *  or we could fabricate these.
             * We would primarily support stereo instancing,
             *  because this has a lot of benefits to efficiency.
             */
            GLM::Draw(call,instdata);

            this->swapBuffers();

            tdelta = this->contextTime() - tprevious;
            tprevious = this->contextTime();
        }

        if(dev)
            VR::Shutdown();

        Profiler::PopContext();
    }
    void eventHandleD(const Display::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);

        EventHandlers::WindowManagerCloseWindow(this,e,data);
        EventHandlers::ResizeWindow<GL>(e,data);
    }

    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);

        EventHandlers::EscapeCloseWindow(this,e,data);

        if(e.type==CIEvent::Keyboard)
        {
            auto kev = (CIKeyEvent const*)data;

            if(kev->mod & CIKeyEvent::RepeatedModifier || kev->mod & CIKeyEvent::PressedModifier)
                return;

            switch(kev->key)
            {
            case CK_F11:
                if(this->windowState() & CDProperties::Windowed)
                    this->setWindowState(CDProperties::WindowedFullScreen);
                else
                    this->setWindowState(CDProperties::Windowed);
                break;
            default:
                break;
            }
        }
    }
};

int32 coffee_main(int32 argc, cstring_w* argv)
{
    /* Set a prefix from which resources are fetched */
    CResources::FileResourcePrefix("sample_data/eye-demo/");

    /* Required for SDL2 applications, initializes SDL state */
    SubsystemWrapper<SDL2::SDL2> sdl2;
    C_UNUSED(sdl2);

    /*Moving on to regular rendering*/
    Profiler::PushContext("Root");

    CSDL2Renderer *renderer = new CDRenderer();

    Profiler::Profile("Object creation");

    /* Set up the window visual */
    CDProperties props = GetDefaultVisual();
    props.flags ^= CDProperties::Resizable;
    props.gl.flags |= GLProperties::GLDebug;
    props.gl.flags |= GLProperties::GLVSync;
#ifndef COFFEE_GLEAM_DESKTOP
    props.gl.version.minor = 2;
#endif

    /* The VR SDK configures some OpenGL state,
     *  so it needs to be done before any GL context is active
     * This is a general trait to VR APIs that hook into GL
     */
    Profiler::PushContext("VR setup");
    dev = QuickLoadHMD<VR,VR::Device>();
    Profiler::PopContext();

    CString err;

    /* Initialize the window and check for errors */
    if(!renderer->init(props,&err))
    {
        cDebug("Initialization error: {0}",err);
        return 1;
    }
    Profiler::Profile("Initialize renderer");

    /* Run the program */
    renderer->run();

    Profiler::Profile("Runtime");

    /* Clean all resources */
    renderer->cleanup();
    delete renderer;

    Profiler::Profile("Cleanup");

    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
