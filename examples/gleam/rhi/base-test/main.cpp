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

using GL = CGL::CGL43;
using GLM = GLEAMAPI;

class CDRenderer : public CSDL2Renderer
{
public:
    CDRenderer()
        : CSDL2Renderer(0)
    {
    }

    static const constexpr szptr num_textures = 4;

    void run()
    {
        Profiler::PushContext("Renderer");

        const constexpr cstring textures[num_textures] = {
            "eye-demo/eye-normal.tga", "eye-demo/eye-weird.tga",
            "eye-demo/eye-alpha.tga", "eye-demo/eye-veins.tga"
        };

        const scalar vertexdata[] = {
            -1.f, -1.f,  1.f,    0.f,  0.f,
             1.f, -1.f,  1.f,   -1.f,  0.f,
            -1.f,  1.f,  1.f,    0.f, -1.f,

            -1.f,  1.f,  1.f,    0.f, -1.f,
             1.f,  1.f,  1.f,   -1.f, -1.f,
             1.f, -1.f,  1.f,   -1.f,  0.f,
        };

        GLM::LoadAPI();

        GLM::BUF_A vertbuf(ResourceAccess::ReadOnly,sizeof(vertexdata));
        GLM::V_DESC vertdesc = {};

        /* Uploading vertex data and creating descriptors */
        {
            vertdesc.alloc();
            vertbuf.alloc();

            vertbuf.commit(sizeof(vertexdata),vertexdata);

            GLM::V_ATTR pos = {};
            pos.m_idx = 0;
            pos.m_bassoc = 0;
            pos.m_size = 3;
            pos.m_type = TypeEnum::Scalar;
            pos.m_stride = sizeof(Vecf3);
            pos.m_off = 0;

            GLM::V_ATTR tc;
            tc.m_idx = 1;
            tc.m_bassoc = 1;
            tc.m_size = 2;
            tc.m_type = TypeEnum::Scalar;
            tc.m_stride = sizeof(Vecf2);
            tc.m_off = sizeof(Vecf3);

            vertdesc.addAttribute(pos);
            vertdesc.addAttribute(tc);
        }

        GLM::SHD v_shader;
        GLM::SHD f_shader;
        GLM::PIP eye_pip;

        /* Compiling shaders */
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

        eye_pip.bind();

        /* Uploading textures */
        GLM::S_2DA eyetex(PixelFormat::RGBA8UI,1,GLM::TextureDMABuffered);
        eyetex.allocate({1024,1024,4},PixCmp::RGBA);
        for(szptr i=0;i<4;i++)
        {
            CResources::Resource rsc(textures[i]);
            CResources::FileMap(rsc);

            CStbImageLib::CStbImage img;
            CStbImageLib::LoadData(&img,&rsc);

            eyetex.upload(BitFormat::UByte,PixCmp::RGBA,{img.size.w,img.size.h,1},
                          img.data,{0,0,(int32)i});
            CResources::FileUnmap(rsc);
        }
        GLM::SM_2DA eyesamp;
        eyesamp.alloc();
        eyesamp.attach(&eyetex);

        Bytes transform_data = {};
        Bytes mult_data = {};
        Bytes time_data = {};

        Matf4 object_matrices[2] = {};
        Vecf2 texture_multipliers[2] = {};
        scalar time_value = 0.f;

        transform_data.size = sizeof(object_matrices);
        transform_data.data = (byte_t*)object_matrices;
        mult_data.size = sizeof(texture_multipliers);
        mult_data.data = (byte_t*)texture_multipliers;
        time_data.size = sizeof(time_value);
        time_data.data = (byte_t*)&time_value;

        GLM::UNIFVAL transforms = {};
        GLM::UNIFVAL multipliers = {};
        GLM::UNIFVAL timeval = {};
        GLM::UNIFSMP textures_array = eyesamp.handle();

        Vector<GLM::UNIFDESC> unifs;

        GLM::VIEWSTATE viewportstate(1);
        GLM::BLNDSTATE blendstate;
        GLM::USTATE unifstate;

        blendstate.m_doBlend = true;
        viewportstate.m_view.push_back({
                                           0,0,

                                           this->windowSize().w,
                                           this->windowSize().h
                                       });

        GLM::GetShaderUniformState(eye_pip,&unifs);

        for(GLM::UNIFDESC const& u : unifs)
        {
            cDebug("Uniform value: {0}",u.m_name);
            if(u.m_name == "transform[0]")
                unifstate.setUniform(u,&transforms);
            else if(u.m_name == "tex_mul[0]")
                unifstate.setUniform(u,&multipliers);
            else if(u.m_name == "texdata")
                unifstate.setSampler(u,textures_array);
            else if(u.m_name == "mx")
                unifstate.setUniform(u,&timeval);
            else
            {
                cDebug("Unhandled uniform value: {0}",u.m_name);
            }
        }

        transforms.data = &transform_data;
        multipliers.data = &mult_data;
        timeval.data = &time_data;

        GLM::SetBlendState(blendstate);

        GLM::DrawCall call;
        call.m_idxd = false;
        call.m_inst = true;

        GLM::DrawInstanceData instdata = {};
        instdata.m_insts = 2;
        instdata.m_verts = (sizeof(vertexdata)/sizeof(scalar))/5;

        Vecf4 clear_col = {.1,.1,.1,1.};

        CGCamera camera;
        camera.aspect = 1.6;
        camera.fieldOfView = 70.f;

        camera.position.x() = 1.;
        camera.position.z() = 3.;

        CTransform base_transform;
        base_transform.scale = Vecf3(1);

        vertdesc.bind();
        vertdesc.bindBuffer(0,vertbuf);
        vertdesc.bindBuffer(1,vertbuf);

        eyesamp.bind(0);

        while(!closeFlag())
        {
            GL::ClearBufferfv(true,0,clear_col);

            this->pollEvents();

            /* Define frame data */
            time_value = this->contextTime();

            texture_multipliers[0] = Vecf2(1,1);
            texture_multipliers[1] = Vecf2(-1,1);

            camera.position.x() = -1.;
            object_matrices[0] = GenPerspective(camera) * GenTransform(camera) * GenTransform(base_transform);
            camera.position.x() = 1.;
            object_matrices[1] = GenPerspective(camera) * GenTransform(camera) * GenTransform(base_transform);

            GLM::SetShaderUniformState(eye_pip,CGL::ShaderStage::Vertex,unifstate);

            GLM::Draw(call,instdata);

            this->swapBuffers();
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
            switch(kev->key)
            {
            case CK_F11:
                this->setWindowState(CDProperties::WindowedFullScreen);
                break;
            case CK_F10:
                this->setWindowState(CDProperties::Windowed);
                break;
            }
        }
    }
};

int32 coffee_main(int32 argc, cstring_w* argv)
{
    /* Set a prefix from which resources are fetched */
    CString prefix = Env::ApplicationDir()+"/sample_data/";
    CResources::FileResourcePrefix(prefix.c_str());

    /*Required for SDL2 applications*/
    SubsystemWrapper<SDL2::SDL2> sdl2;
    C_UNUSED(sdl2);

    /*Moving on to regular rendering*/
    Profiler::PushContext("Root");

    CSDL2Renderer *renderer = new CDRenderer();

    Profiler::Profile("Object creation");

    /* Set up the window visual */
    CDProperties props = GetDefaultVisual();
    props.gl.flags |= GLProperties::GLDebug;
    props.gl.flags |= GLProperties::GLVSync;

    /* The Oculus SDK configures some OpenGL state,
     *  so it needs to be done before any GL context is active */
    Profiler::PushContext("Oculus setup");
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

    cDebug("OpenGL core profile version: {0}",
           (_cbasic_version<uint8> const&)GL::Debug::ContextVersion());

    cDebug("Device info: {0}", GL::Debug::Renderer());
    Profiler::Profile("Get renderer info");

    /* Check if required extensions are present */
    if(!(  GL::SeparableShaderSupported()
           ||GL::VertexAttribBinding()
           ||GL::ViewportArraySupported()
           ||GL::BufferStorageSupported()))
    {
        cDebug("Unable to start: Required OpenGL extensions not found");
        return 1;
    }

    /* Check if optional extensions is present */
    if(!CGL::CGL45::CullDistanceSupported())
    {
        cDebug("Some optional extensions were not found. Your experience might suffer.");
    }

    Profiler::Profile("Get GL requirements");

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
