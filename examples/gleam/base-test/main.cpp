#include <coffee/CCore>
#include <coffee/graphics_apis/CGLeam>
#include <coffee/COpenVR>
#include <coffee/CGraphics>

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

            CResources::FileUnmap(rsc);
        }

        cstring vshader = {
            "#version 430 core\n"
            ""
            ""
            "out gl_PerVertex{"
            "   vec4 gl_Position;"
            "};"
            "out VS_OUT{"
            "   vec3 tc;"
            "   flat int instance;"
            "} vs_out;"
            ""
            "uniform mat4 transform[2];"
            ""
            "void main(void)"
            "{"
            "   const vec3[4] vertices = vec3[4](vec3(-1.0,-1.0,1.0),"
            "                                    vec3( 1.0,-1.0,1.0),"
            "                                    vec3(-1.0, 1.0,1.0),"
            "                                    vec3( 1.0, 1.0,1.0));"
            "   vs_out.instance = gl_InstanceID;"
            "   gl_Position = transform[gl_InstanceID]*vec4(vertices[gl_VertexID],1.0);"
            "}"
        };
        cstring fshader = {
            "#version 430 core\n"
            "in VS_OUT{"
            "   vec3 tc;"
            "   flat int instance;"
            "} fs_in;"
            ""
            "layout(location = 0) out vec4 color;"
            "uniform float mx;"
            ""
            "layout(binding=0)uniform sampler2DArray texdata;"
            ""
            "void main(void)"
            "{"
            "   /*color = texture(texdata,vec3(0.3,0.3,fs_in.instance));*/"
            "   color = vec4(fs_in.instance);"
            "}"
        };

        GL::CGhnd vao;
        GL::VAOAlloc(1,&vao);

        GL::VAOBind(vao);

        GL::CGhnd vprogram = GL::ProgramCreate(GL::ShaderStage::Vertex,1,&vshader);
        cDebug("Compilation log: {0}",GL::ProgramGetLog(vprogram));
//        if(!GL::ProgramValidate(vprogram))
//            return;

        GL::CGhnd fprogram = GL::ProgramCreate(GL::ShaderStage::Fragment,1,&fshader);
        cDebug("Compilation log: {0}",GL::ProgramGetLog(fprogram));
//        if(!GL::ProgramValidate(fprogram))
//            return;

        GL::CGhnd pipeline;
        GL::PipelineAlloc(1,&pipeline);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::Vertex,vprogram);
        GL::PipelineUseStages(pipeline,GL::ShaderStage::Fragment,fprogram);

        GL::PipelineBind(pipeline);

        if(!GL::PipelineValidate(pipeline))
            return;

        GL::CGhnd texture_array;
        GL::TexAlloc(1,&texture_array);
        GL::TexActive(0);
        GL::TexBind(GL::Texture::T2DArray,texture_array);
        GL::TexStorage3D(GL::Texture::T2DArray,3,PixelFormat::RGBA32F,1024,1024,3);
        for(uint32 i=0;i<3;i++)
        {
            GL::BufBind(GL::BufType::PixelUData,pbobuf[i]);
            GL::TexSubImage3D(GL::Texture::T2DArray,0,
                              0,0,i,1024,1024,1,
                              PixelComponents::RGBA,BitFormat::Byte_2,nullptr);
        }

        CGraphicsData::CGCamera cam;
        cam.aspect = 1.6;
        cam.fieldOfView = 70.f;
        cam.position = CVec3(0,0,-3);

        CGraphicsData::CTransform obj;
        obj.position = CVec3(-1,0,0);
        obj.scale = CVec3(1);

        int32 cam_unif =
                GL::ProgramGetResourceLoc(vprogram,
                                          GL_UNIFORM,
                                          "transform");
        int32 tex_unif =
                GL::ProgramGetResourceLoc(fprogram,
                                          GL_UNIFORM,
                                          "texdata");

        int32 tex_bind = 0;

        CMat4 cam_mat = CGraphicsData::GenPerspective(cam)
                *CGraphicsData::GenTransform(cam);

        CMat4 objects[2] = {};
        objects[0] = cam_mat*CGraphicsData::GenTransform(obj);
        obj.position = CVec3(1,0,0);
        objects[1] = cam_mat*CGraphicsData::GenTransform(obj);

        GL::Uniformfv(vprogram,cam_unif,2,false,objects);
        GL::Uniformiv(fprogram,tex_unif,1,&tex_bind);

        cDebug("Setup time: {0}",timer->elapsed());

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
    props.gl.version.major = 4;
    props.gl.version.minor = 3;

    renderer->init(props);
    cDebug("Init renderer: {0}",timer->elapsed());

    if(!GL::SeparableShaderSupported())
        return 1;
    if(!GL::ViewportArraySupported())
        return 1;
    if(!GL::BufferStorageSupported())
        return 1;

    renderer->run();
    timer->start();
    renderer->cleanup();
    delete renderer;
    cDebug("Cleanup renderer: {0}",timer->elapsed());

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
