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
            "   imageStore(target,pix,vec4(1.0,1.0,0.0,1.0));"
            "}"
        };

        GL::CGhnd cprogram = GL::ProgramCreate(GL::ShaderStage::Compute,1,&cshader);
        if(!GL::ProgramValidate(cprogram))
            cDebug("Compilation log: {0}",GL::ProgramGetLog(cprogram));

        GL::CGhnd pipeline;
        GL::PipelineAlloc(1,&pipeline);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::Compute,cprogram);

        GL::PipelineBind(pipeline);

        /* Create our image target */
        GL::CGhnd tex;
        GL::CGhnd imageUnit = 1;

        GL::TexAlloc(1,&tex);
        GL::TexBind(GL::Texture::T2D,tex);
        GL::TexStorage2D(GL::Texture::T2D,1,PixelFormat::RGBA32F,1024,1024);

        GL::ImageBindTexture(imageUnit,tex,0,false,0,
                             ResourceAccess::WriteOnly,
                             PixelFormat::RGBA32F);

        /* Set up uniforms */

        /* Missing! */

        cDebug("Setup time: {0}",timer->elapsed());

        while(!closeFlag())
        {
            clearCol.r() = CMath::sin(this->contextTime()+0.5);
            clearCol.g() = CMath::sin(this->contextTime()+5.0);
            clearCol.b() = CMath::sin(this->contextTime()+50.0);

            clearCol = normalize(clearCol);

            GL::ClearBufferfv(true,0,clearCol);

            GL::ComputeDispatch(16,16,1);

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

    CBitmap* icon_bitm = nullptr;

    {
        CResources::CResource icon_file(
                    "/home/havard/Bilder/Always invite the Hero of Time.png",
                    true);
        CResources::FilePull(icon_file);
        CStbImageLib::CStbImage icon_img;
        CStbImageLib::LoadData(&icon_img,&icon_file);
        CResources::FileFree(icon_file);

        icon_bitm = new CBitmap(icon_img.size.w,icon_img.size.h);
        szptr data_size = icon_img.size.area()*4;
        CMemCpy(icon_bitm->data(),icon_img.data,data_size);
        CStbImageLib::ImageFree(&icon_img);
    }

    props.icon = icon_bitm;

    renderer->init(props);
    cDebug("Init renderer: {0}",timer->elapsed());
    if(icon_bitm)
        delete icon_bitm;

    renderer->run();
    timer->start();
    renderer->cleanup();
    delete renderer;
    cDebug("Cleanup renderer: {0}",timer->elapsed());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
