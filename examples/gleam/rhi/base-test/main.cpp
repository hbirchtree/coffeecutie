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

        GLM::SHD v_shader;
        GLM::SHD f_shader;
        GLM::PIP eye_pip;

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

        GLM::BLNDSTATE blendstate;
        GLM::USTATE unifstate;

        GLM::SetBlendState(blendstate);
        GLM::SetShaderUniformState(eye_pip,CGL::ShaderStage::Vertex,unifstate);
        GLM::SetShaderUniformState(eye_pip,CGL::ShaderStage::Fragment,unifstate);

        while(!closeFlag())
        {
            this->pollEvents();
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
