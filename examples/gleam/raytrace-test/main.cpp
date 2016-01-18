#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>
#include <coffee/CGraphics>
#include <coffee/CImage>

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
        glContext()->acquireContext();

        CElapsedTimerD* timer = AllocTimerD();
        timer->start();

        /* Data definition and declaration */
        CGraphicsData::CGCamera cam;
        cam.aspect = 1.6;
        cam.fieldOfView = 70.f;
        cam.position = CVec3(0,0,-3);

        CVec4 clearCol;
        clearCol.a() = 1.0;

        /* Creating a VAO just in case */
        GL::CGhnd vao;
        GL::VAOAlloc(1,&vao);

        GL::VAOBind(vao);

        /* Shader specification */
        cstring cshader = {
            "#version 430 core\n"
            ""
            "layout(local_size_x=8,local_size_y=8) in;"
            ""
            "layout(binding = 0,rgba32f) uniform image2D target;"
            ""
            "uniform vec3 cPos;"
            "uniform vec3 rayBL; //0,0"
            "uniform vec3 rayTL; //0,1"
            "uniform vec3 rayBR; //1,0"
            "uniform vec3 rayTR; //1,1"
            ""
            "void main(void){"
            "   ivec2 pix = ivec2(gl_GlobalInvocationID.xy);"
            "   ivec2 size = imageSize(target);"
            "   if(pix.x>=size.x || pix.y>=size.y)"
            "       return;"
            "   imageStore(target,pix,vec4(1.0,1.0,0.0,1.0));"
            "}"
        };

        GL::CGhnd cprogram = GL::ProgramCreate(GL::ShaderStage::Compute,
                                               1,&cshader);

        cDebug("Program log: {0}",GL::ProgramGetLog(cprogram));

        if(!GL::ProgramValidate(cprogram))
            return;

        GL::CGhnd pipeline;
        GL::PipelineAlloc(1,&pipeline);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::All,cprogram);

        if(!GL::PipelineValidate(pipeline))
        {
            cDebug("Invalid pipeline!");
            return;
        }

        GL::PipelineBind(pipeline);

        /* Create our image target */
        GL::CGhnd tex;
        int32 imageUnit = 1;
        GL::CGhnd fb;

        GL::TexAlloc(1,&tex);
        GL::TexBind(GL::Texture::T2D,tex);
        GL::TexStorage2D(GL::Texture::T2D,1,PixelFormat::RGBA32F,1024,1024);

        GL::ImageBindTexture(imageUnit,tex,0,false,0,
                             ResourceAccess::WriteOnly,
                             PixelFormat::RGBA32F);

        /* Create dummy framebuffer to blit from */
        GL::FBAlloc(1,&fb);
        GL::FBBind(GL::FramebufferT::All,fb);
        GL::FBAttachTexture(GL::FramebufferT::All,GL_COLOR_ATTACHMENT0,tex,0);

        if(!GL::FBValidate(GL::FramebufferT::All))
        {
            cDebug("Invalid FB config!");
            return;
        }

        GL::FBBind(GL::FramebufferT::All,0);
        GL::FBBind(GL::FramebufferT::Read,fb);

        /* Set up uniforms */
        GL::CGhnd imguni = GL::ProgramUnifGetLoc(cprogram,"target");
        GL::Uniformiv(cprogram,imguni,1,&imageUnit);

        CRect blit_source;
        blit_source.w = 1024;
        blit_source.h = 720;
        CRect blit_target;
        blit_target.w = 1024;
        blit_target.h = 720;

        cDebug("Setup time: {0}",timer->elapsed());

        GL::ComputeDispatch(64,64,1);
        GL::MemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        while(!closeFlag())
        {
            clearCol.r() = CMath::sin(this->contextTime()+0.5);
            clearCol.g() = CMath::sin(this->contextTime()+5.0);
            clearCol.b() = CMath::sin(this->contextTime()+50.0);

            clearCol = normalize(clearCol);

            GL::ClearBufferfv(true,0,clearCol);

            GL::FBBlit(blit_source,blit_target,GL_COLOR_BUFFER_BIT,GL_LINEAR);

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
    CElapsedTimerD* timer = AllocTimerD();
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
