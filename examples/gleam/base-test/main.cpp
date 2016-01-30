#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>
#include <coffee/CGraphics>

using namespace Coffee;
using namespace CDisplay;

void framecount_fun(uint32 t, c_cptr)
{
    cDebug("Frames: {0}",t);
}

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
        CElapsedTimer* ftimer = AllocTimer();
        timer->start();

        FrameCounter fcounter(framecount_fun);
        fcounter.interval = 1000;

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

        //        GLEXT::ViewportSet(0,&leftEye);
        //        GLEXT::ViewportSet(1,&rightEye);

        GL::Enable(GL::Feature::Blend);
//        GL::Enable(GL::Feature::DepthTest);

        const cstring textures[3] = {"eye-normal.tga","eye-weird.tga","eye-alpha.tga"};

        GL::CGhnd pbobuf[3] = {};
        GL::BufAlloc(3,pbobuf);

        for(uint32 i=0;i<3;i++)
        {
            CResources::CResource rsc(textures[i]);
            CResources::FileMap(rsc);

            CStbImageLib::CStbImage img;
            CStbImageLib::LoadData(&img,&rsc);

            GL::BufBind(GL::BufType::PixelUData,pbobuf[i]);
            GL::BufStorage(GL::BufType::PixelUData,
                              img.size.area()*img.bpp,
                              img.data,ResourceAccess::ReadOnly);

            CResources::CResource out("test.png");
            CStbImageLib::SavePNG(&out,&img);

            CResources::FileCommit(out);
            CResources::FileFree(out);
            CStbImageLib::ImageFree(&img);
            CResources::FileUnmap(rsc);
        }

        const scalar vertexdata[] = {
            -1.f, -1.f,  1.f,    0.f,  0.f,
             1.f, -1.f,  1.f,   -1.f,  0.f,
            -1.f,  1.f,  1.f,    0.f, -1.f,

            -1.f,  1.f,  1.f,    0.f, -1.f,
             1.f,  1.f,  1.f,   -1.f, -1.f,
             1.f, -1.f,  1.f,   -1.f,  0.f,
        };

        GL::CGhnd vertbuf;
        GL::BufAlloc(1,&vertbuf);
        GL::BufBind(GL::BufType::ArrayData,vertbuf);
        GL::BufStorage(GL::BufType::ArrayData,
                       sizeof(vertexdata),vertexdata,
                       ResourceAccess::ReadOnly);

        cstring vshader = {
            "#version 430 core\n"
            ""
            "layout(location=0)in vec3 pos;"
            "layout(location=1)in vec2 tex;"
            ""
            "out gl_PerVertex{"
            "   vec4 gl_Position;"
            "};"
            "out VS_OUT{"
            "   vec2 tc;"
            "   flat int instance;"
            "} vs_out;"
            ""
            "uniform mat4 transform[2];"
            "uniform vec2 tex_mul[2];"
            ""
            "void main(void)"
            "{"
            "   vs_out.instance = gl_InstanceID;"
            "   vs_out.tc = tex*tex_mul[gl_InstanceID];"
            "   gl_Position = transform[gl_InstanceID]*vec4(pos,1.0);"
            "}"
        };
        cstring fshader = {
            "#version 430 core\n"
            "in VS_OUT{"
            "   vec2 tc;"
            "   flat int instance;"
            "} fs_in;"
            ""
            "layout(location = 0) out vec4 color;"
            "uniform float mx;"
            ""
            "uniform sampler2DArray texdata;"
            ""
            "void main(void)"
            "{"
            "   vec4 c1 = texture(texdata,vec3(fs_in.tc,0));"
            "   vec4 c2 = texture(texdata,vec3(fs_in.tc,1));"
            "   float a1 = texture(texdata,vec3(fs_in.tc,2)).a;"
            "   if(mx>a1)"
            "       color = c1;"
            "   else"
            "       color = c2;"
            "}"
        };

        GL::CGhnd vao;
        GL::VAOAlloc(1,&vao);

        GL::VAOBind(vao);

        GL::VAOEnableAttrib(0);
        GL::VAOEnableAttrib(1);

        GL::VAOAttribFormat(0,3,TypeEnum::Scalar,false,0);
        GL::VAOAttribFormat(1,2,TypeEnum::Scalar,false,0);

        GL::VAOBindVertexBuffer(0,vertbuf,0,sizeof(CVec3)+sizeof(CVec2));
        GL::VAOBindVertexBuffer(1,vertbuf,sizeof(CVec3),sizeof(CVec3)+sizeof(CVec2));

        GL::VAOAttribBinding(0,0);
        GL::VAOAttribBinding(1,1);

        GL::CGhnd vprogram = GL::ProgramCreate(GL::ShaderStage::Vertex,1,&vshader);
        cDebug("Compilation log: {0}",GL::ProgramGetLog(vprogram));

        GL::CGhnd fprogram = GL::ProgramCreate(GL::ShaderStage::Fragment,1,&fshader);
        cDebug("Compilation log: {0}",GL::ProgramGetLog(fprogram));

        GL::CGhnd pipeline;
        GL::PipelineAlloc(1,&pipeline);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::Vertex,vprogram);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::Fragment,fprogram);

        GL::PipelineBind(pipeline);

        GL::ShaderReleaseCompiler();

        if(!GL::PipelineValidate(pipeline))
            return;

        GL::CGhnd texture_array;
        GL::TexAlloc(1,&texture_array);
        GL::TexActive(0);
        GL::TexBind(GL::Texture::T2DArray,texture_array);
        GL::TexStorage3D(GL::Texture::T2DArray,1,PixelFormat::RGBA32F,1024,1024,3);
        for(uint32 i=0;i<3;i++)
        {
            GL::BufBind(GL::BufType::PixelUData,pbobuf[i]);
            GL::TexSubImage3D(GL::Texture::T2DArray,0,
                              0,0,i,1024,1024,1,
                              PixelComponents::RGBA,BitFormat::UByte,nullptr);
        }

        CGraphicsData::CGCamera cam;
        cam.aspect = 1.6;
        cam.fieldOfView = 70.f;
        cam.position = CVec3(0,0,-3);

        CGraphicsData::CTransform obj;
        obj.position = CVec3(-1,0,0);
        obj.scale = CVec3(1);

        int32 cam_unif = GL::ProgramGetResourceLoc(vprogram,GL_UNIFORM,"transform");
        int32 texm_unif = GL::ProgramGetResourceLoc(vprogram,GL_UNIFORM,"tex_mul");
        int32 tex_unif = GL::ProgramGetResourceLoc(fprogram,GL_UNIFORM,"texdata");
        int32 tim_unif = GL::ProgramGetResourceLoc(fprogram,GL_UNIFORM,"mx");

        int32 tex_bind = 0;
        CMat4 cam_mat = CGraphicsData::GenPerspective(cam)
                *CGraphicsData::GenTransform(cam);

        CMat4 objects[2] = {};
        objects[0] = cam_mat*CGraphicsData::GenTransform(obj);
        obj.position = CVec3(1,0,0);
        objects[1] = cam_mat*CGraphicsData::GenTransform(obj);

        CVec2 tex_mul[2] = {};
        tex_mul[0] = CVec2(1,1);
        tex_mul[1] = CVec2(-1,1);

        GL::Uniformfv(vprogram,cam_unif,2,false,objects);
        GL::Uniformfv(vprogram,texm_unif,2,tex_mul);
        GL::Uniformiv(fprogram,tex_unif,1,&tex_bind);

        cDebug("Setup time: {0}",timer->elapsed());

        scalar depth_zero = 0.f;
        bool cycle_color = false;

        ftimer->start();
        while(!closeFlag())
        {
            scalar time = this->contextTime();

            if(cycle_color)
            {
                clearcol.r() = CMath::sin(time+0.5);
                clearcol.g() = CMath::sin(time+5.0);
                clearcol.b() = CMath::sin(time+50.0);
            }

            time = (CMath::sin(time)+CMath::sin(time*0.5)+1.0)/2.0;
            GL::Uniformfv(fprogram,tim_unif,1,&time);

            clearcol = normalize(clearcol);

            GL::ClearBufferfv(true,0,clearcol);
            GL::ClearBufferfv(false,0,&depth_zero);

            GL::DrawArraysInstanced(GL_TRIANGLES,0,6,2);

            fcounter.update(ftimer->elapsed());

            this->pollEvents();
            this->swapBuffers();
        }

        //Write code here
    }
    void eventHandleD(const CDisplay::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);
        if(e.type==CDisplay::CDEvent::Resize)
        {
            auto rev = (const CDisplay::CDResizeEvent*)data;
            CRectF view;
            view.x = 0;
            view.y = 0;
            view.w = rev->w;
            view.h = rev->h;

            GL::ViewportSet(0,&view);
        }
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

int32 coffee_main(int32, cstring_w*)
{
    if(!OpenVRDev::InitializeBinding())
    {
        cDebug("Move on, there is nothing to see here.");
    }else{
        cDebug("By the gods... So it was true! You must be the hero of Kvatch!");
        cDebug("[HERE BE DRAGONS]");
        uint32 devs;
        if(!OpenVRDev::PollDevices(&devs))
        {
            cDebug("I used to be a reality tripper like you,"
                   " until I took a cyberkick to the knee.");
        }
        OpenVRDev::OVRDevice* dev = OpenVRDev::GetDevice(0);
        cDebug("What you got: {0}",(const HWDeviceInfo&)*dev);
    }

    CResources::FileResourcePrefix("sample_data/");

    CElapsedTimerD* timer = AllocTimerD();
    timer->start();
    CSDL2Renderer *renderer = new CDRenderer();
    cDebug("Allocated renderer: {0}",timer->elapsed());
    CDProperties props = GetDefaultVisual();
    props.gl.flags = props.gl.flags|GLProperties::GLDebug;
    props.gl.version.major = 3;
    props.gl.version.minor = 3;

    renderer->init(props);
    cDebug("Init renderer: {0}",timer->elapsed());

    if(!(GL::SeparableShaderSupported()
         ||GL::VertexAttribBinding()
         ||GL::ViewportArraySupported()
         ||GL::BufferStorageSupported()))
        return 1;

    renderer->run();
    timer->start();
    renderer->cleanup();
    delete renderer;
    cDebug("Cleanup renderer: {0}",timer->elapsed());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
