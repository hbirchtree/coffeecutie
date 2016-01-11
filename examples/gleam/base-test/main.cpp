#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>

using namespace Coffee;
using namespace CDisplay;

using GL = CGL::CGL43;

class CDRenderer : public Coffee::CDisplay::CGLeamRenderer
{
public:
    CDRenderer()
        : CGLeamRenderer(0)
    {
    }

    void run()
    {
        CSize winSize = this->windowSize();
        winSize.w = winSize.w/2;
        CRectF leftEye;
        CRectF rightEye;
        leftEye.x = 0;
        leftEye.y = 0;
        leftEye.w = winSize.w;
        leftEye.h = winSize.h;

        rightEye.x = winSize.w;
        rightEye.y = 0;
        rightEye.w = winSize.w;
        rightEye.h = winSize.h;

        CVec4 clearcol(0.0);
        clearcol.a() = 1.0;

        GL::CGhnd pbobuf;
        GL::BufAlloc(1,&pbobuf);

        GL::ViewportSet(0,&leftEye);
        GL::ViewportSet(1,&rightEye);

        cstring vshader = {
            "#version 430 core\n"
            ""
            ""
            "out gl_PerVertex{"
            "   vec4 gl_Position;"
            "};"
            "out VS_OUT{"
            "   vec3 tc;"
            "} vs_out;"
            ""
            "void main(void)"
            "{"
            "   const vec3[4] vertices = vec3[4](vec3(-1.0,-1.0,1.0),"
            "                              vec3( 1.0,-1.0,1.0),"
            "                              vec3(-1.0, 1.0,1.0),"
            "                              vec3( 1.0, 1.0,1.0));"
            "   "
            "   gl_Position = vec4(vertices[gl_VertexID],1.0);"
            "}"
        };
        cstring gshader = {
            "#version 430 core\n"
            ""
            "in int gl_InvocationID;"
            "out int gl_ViewportIndex;"
            ""
            "out VS_OUT{"
            "   vec4 gl_Position;"
            "   vec3 tc;"
            "} gs_in[];"
            "out GS_OUT{"
            "   vec3 tc;"
            "} gs_out;"
            ""
            "void main(void)"
            "{"
            "   for(int i=0;i<gl_in.length();i++)"
            "   {"
            "       gs_out.tc = gs_in[i].tc;"
            "       gl_Position = gs_in[i].gl_Position;"
            "       gl_ViewportIndex = gl_InvocationID;"
            "       EmitVertex();"
            "   }."
            "   EndPrimitive();"
            "}"
        };
        cstring fshader = {
            "#version 430 core\n"
            "in GS_OUT{"
            "   vec3 tc;"
            "} fs_in;"
            ""
            "layout(location = 0) out vec4 color;"
            ""
            "void main(void)"
            "{"
            "   color = vec4(1.0,0.0,0.0,1.0);"
            "}"
        };

        GL::CGhnd vao;
        GL::VAOAlloc(1,&vao);

        GL::VAOBind(vao);

        GL::CGhnd vprogram = GL::ProgramCreate(GL::ShaderStage::Vertex,1,&vshader);
        if(!GL::ProgramValidate(vprogram))
            cDebug("Compilation log: {0}",GL::ProgramGetLog(vprogram));

        GL::CGhnd gprogram = GL::ProgramCreate(GL::ShaderStage::Geometry,1,&gshader);
        if(!GL::ProgramValidate(gprogram))
            cDebug("Compilation log: {0}",GL::ProgramGetLog(gprogram));

        GL::CGhnd fprogram = GL::ProgramCreate(GL::ShaderStage::Fragment,1,&fshader);
        if(!GL::ProgramValidate(fprogram))
            cDebug("Compilation log: {0}",GL::ProgramGetLog(fprogram));

        GL::CGhnd pipeline;
        GL::PipelineAlloc(1,&pipeline);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::Vertex,vprogram);
//        GL::PipelineUseStages(pipeline,GL::ShaderStage::Geometry,gprogram);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::Fragment,fprogram);

        GL::PipelineBind(pipeline);

        GL::CGhnd cube;
        GL::TexAlloc(1,&cube);
        GL::TexBind(GL::Texture::Cubemap,cube);
        GL::TexStorage2D(GL::Texture::Cubemap,1,PixelFormat::RGBA8UI,512,512);

        for(int32 i=0;i<6;i++)
        {
//            GL::TexSubImage2D(GL::Texture::CubeX_N,0,0,0,512,512,
//                              PixelComponents::RGBA,BitFormat::Byte_2,0);
            GL::MemoryBarrier(GL_PIXEL_BUFFER_BARRIER_BIT);
        }

        while(!closeFlag())
        {
            clearcol.r() = CMath::sin(this->contextTime()+0.5);
            clearcol.g() = CMath::sin(this->contextTime()+5.0);
            clearcol.b() = CMath::sin(this->contextTime()+50.0);

            clearcol = normalize(clearcol);

            GL::ClearBufferfv(true,0,clearcol);

            GL::DrawArraysInstanced(GL_TRIANGLES,0,4,2);

            this->pollEvents();
            this->swapBuffers();
        }

        //Write code here
    }
    void eventHandleD(const CDisplay::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);
        switch(e.type)
        {
        case CIEvent::Keyboard:
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)data;
            switch(kev->key)
            {
            case CK_Escape:
                this->closeWindow();
                break;
            default:
                break;
            }
            break;
        }
        default:
            break;
        }
    }
};

int32 coffee_main(int32, byte_t**)
{
    if(!OpenVRDev::InitializeBinding())
    {
        cDebug("No VR 4 u!");
    }else{
        cDebug("By the gods... So it was true! The time of the VR-born has come!");
        uint32 devs;
        if(!OpenVRDev::PollDevices(&devs))
        {
            cDebug("No devices found? What?");
        }
        OpenVRDev::OVRDevice* dev = OpenVRDev::GetDevice(0);
        cDebug("What you got: {0}",(const HWDeviceInfo&)*dev);
    }

    CElapsedTimerD* timer = coffee_fun_alloc_timerd();
    timer->start();
    CSDL2Renderer *renderer = new CDRenderer();
    cDebug("Allocated renderer: {0}",timer->elapsed());
    CDProperties props = GetDefaultVisual();
    props.gl.flags = props.gl.flags|GLProperties::GLDebug;
    props.gl.version.major = 4;

    renderer->init(props);
    cDebug("Init renderer: {0}",timer->elapsed());
    renderer->run();
    timer->start();
    renderer->cleanup();
    delete renderer;
    cDebug("Cleanup renderer: {0}",timer->elapsed());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
