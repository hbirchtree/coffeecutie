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
        cam.position = CVec3(0.5,0.5,-10);

        CVec4 clearCol;
        clearCol.a() = 1.0;

        CRect blit_source;
        blit_source.w = 1024;
        blit_source.h = 1024;
        CRect blit_target;
        blit_target.w = 720;
        blit_target.h = 720;

        /* Creating a VAO just in case */
        GL::CGhnd vao;
        GL::VAOAlloc(1,&vao);

        GL::VAOBind(vao);

        CResources::CResource shader_file("cshader.glsl");
        CResources::FilePull(shader_file,true);

        /* Shader specification */


        static uint32 indices_data_l1[4096] = {
            0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20
        };

        static uint32 indices_data_l2[4096] = {
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        };

        static uint32* indices_pointers[] = {
            indices_data_l1,
            indices_data_l2,
        };

        static scalar array_data_l0[] = {
            1.f,
        };

        GL::CGhnd cprogram = GL::ProgramCreate(GL::ShaderStage::Compute,
                                               1,(cstring*)&shader_file.data);

        cDebug("Program log: {0}",GL::ProgramGetLog(cprogram));

        if(!GL::ProgramValidate(cprogram))
            return;

        GL::ProgramUse(cprogram);

        uint32 num_levels = 2;
        GL::CGhnd indices_levels[num_levels];
        GL::BufAlloc(num_levels,indices_levels);
        for(uint32 i=0;i<num_levels;i++)
        {
            GL::BufBind(GL::BufType::ShaderData,indices_levels[i]);
            GL::BufData(GL::BufType::ShaderData,4096*4,indices_pointers[i],ResourceAccess::ReadOnly);
            GL::BufBindRange(GL::BufType::ShaderData,i,indices_levels[i],0,4096*4);
        }

        GL::CGhnd array_data;
        GL::BufAlloc(1,&array_data);
        GL::BufBind(GL::BufType::ShaderData,array_data);
        GL::BufData(GL::BufType::ShaderData,sizeof(array_data_l0),array_data_l0,ResourceAccess::ReadOnly);
        GL::BufBindRange(GL::BufType::ShaderData,num_levels,array_data,0,sizeof(array_data_l0));

        GL::CGhnd pipeline;
        GL::PipelineAlloc(1,&pipeline);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::All,cprogram);

        if(!GL::PipelineValidate(pipeline))
        {
            cDebug("Pipeline log: {0}",GL::PipelineGetLog(pipeline));
            return;
        }

        GL::PipelineBind(pipeline);

        /* Create our image target */
        GL::CGhnd tex;
        GL::CGhnd treedata;
        uint32 imageUnit = 0;
        GL::CGhnd fb;

        GL::TexAlloc(1,&tex);
        GL::TexAlloc(1,&treedata);
        GL::TexBind(GL::Texture::T2D,tex);
        GL::TexStorage2D(GL::Texture::T2D,1,PixelFormat::RGBA32F,1024,1024);
        GL::TexBind(GL::Texture::T3D,treedata);

        int32* data = (int32*)Alloc(CMath::pow<int32>(512,3)*4);

//        for(szptr i=0;i<512;i++)
//            for(szptr j=0;j<512;j++)
//                for(szptr k=0;k<512;k++)
//                    data[i+j*512+512*512*k] = (i-256)*(i-256)+(j-256)*(j-256)+(k-256)*(k-256) < 200*200;

//        GL::TexImage3D(GL::Texture::T3D,1,PixelFormat::R32I,512,512,512,
//                       0,PixelComponents::R,BitFormat::Int_8,data);

        CFree(data);

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

        GL::FBBind(GL::FramebufferT::Draw,0);
        GL::FBBind(GL::FramebufferT::Read,fb);

        /* Set up uniforms */
        GL::ImageBindTexture(imageUnit,tex,0,false,0,ResourceAccess::WriteOnly,
                             PixelFormat::RGBA32F);

        int32 unifTreeLoc = GL::ProgramUnifGetLoc(cprogram,"treeSource");

        int32 unifcamPos = GL::ProgramUnifGetLoc(cprogram,"cPos");
        GL::Uniformfv(cprogram,unifcamPos,1,&cam.position);

        /**/

        cDebug("Setup time: {0}",timer->elapsed());


        while(!closeFlag())
        {
            clearCol.r() = CMath::sin(this->contextTime()+0.5);
            clearCol.g() = CMath::sin(this->contextTime()+5.0);
            clearCol.b() = CMath::sin(this->contextTime()+50.0);

            clearCol = normalize(clearCol);

            GL::ClearBufferfv(true,0,clearCol);

            GL::ProgramUse(cprogram);
            GL::ComputeDispatch(1024/16,1024/16,1);

            GL::FBBlit(blit_source,blit_target,GL_COLOR_BUFFER_BIT,GL_LINEAR);

            this->pollEvents();
            this->swapBuffers();
        }

        CGL::CGLUtil::DumpTexture(GL::Texture::T2D,tex,0,"raytrace.png");

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
    CResources::FileResourcePrefix("sample_data/");

    CElapsedTimerD* timer = AllocTimerD();
    timer->start();
    CSDL2Renderer *renderer = new CDRenderer();
    cDebug("Allocated renderer: {0}",timer->elapsed());
    CDProperties props = GetDefaultVisual();
    props.gl.flags = props.gl.flags|GLProperties::GLDebug;
    props.gl.version.major = 4;
    props.gl.version.minor = 3;

    renderer->init(props);
    cDebug("Init renderer: {0}",timer->elapsed());

    renderer->run();
    timer->start();
    renderer->cleanup();
    delete renderer;
    cDebug("Cleanup renderer: {0}",timer->elapsed());

//    CRGBA* img = (CRGBA*)Alloc(256*256*4);

//    for(uint32 x=0;x<256;x++)
//        for(uint32 y=0;y<256;y++)
//        {
//            img[y*256+x].r = img[y*256+x].g = img[y*256+x].b =
//                    NoiseGen::Perlin(CVec3(x,y,1),_cbasic_vec3<int32>(256));
//            img[y*256+x].a = 255;
//        }

//    CStbImageLib::CStbImageConst img_c;
//    img_c.size = CSize(256,256);
//    img_c.data = (ubyte_t*)img;
//    img_c.bpp = 4;

//    CResources::CResource img_f("dump.png");

//    CStbImageLib::SavePNG(&img_f,&img_c);
//    CResources::FileCommit(img_f);

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
