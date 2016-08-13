#include <coffee/core/CApplication>
#include <coffee/core/CProfiling>

#include <coffee/CGraphics>
#include <coffee/sdl2/CSDL2GLRenderer>
#include <coffee/sdl2/CSDL2Dialog>
#include <coffee/sdl2/CSDL2System>

#include <coffee/graphics_apis/CGLeam>

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

void framecount_fun(uint32 t, c_cptr)
{
    cDebug("Frames: {0}",t);
}

using GL = CGL::CGL43;

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
        CElapsedTimer ftimer;

        Counter fcounter(framecount_fun);
        fcounter.interval = 1000;

        const constexpr cstring textures[num_textures] = {
            "circle_red.png", "circle_blue.png", "circle_alpha.png",
            "floor-tile.png"
        };

        GL::CGhnd pbobuf[num_textures] = {};
        GL::BufAlloc(num_textures,pbobuf);

        Profiler::Profile("Create PBO buffers");

        for(uint32 i=0;i<num_textures;i++)
        {
            CResources::Resource rsc(textures[i],
                                     ResourceAccess::SpecifyStorage
                                     |ResourceAccess::AssetFile);
            if(!CResources::FileMap(rsc))
            {
                /* ABORT! */
                ABORTEVERYTHINGGOGOGO();
            }

            CStbImageLib::CStbImage img;
            CStbImageLib::LoadData(&img,&rsc);

            GL::BufBind(GL::BufType::PixelUData,pbobuf[i]);
            GL::BufStorage(GL::BufType::PixelUData,
                           img.size.area()*img.bpp,
                           img.data,ResourceAccess::ReadOnly);

            CStbImageLib::ImageFree(&img);
            CResources::FileUnmap(rsc);
        }

        Profiler::Profile("Pre-load textures");

        const scalar vertexdata[] = {
            -1.f, -1.f,  1.f,    0.f,  0.f,
            1.f, -1.f,  1.f,   -1.f,  0.f,
            -1.f,  1.f,  1.f,    0.f, -1.f,

            -1.f,  1.f,  1.f,    0.f, -1.f,
            1.f,  1.f,  1.f,   -1.f, -1.f,
            1.f, -1.f,  1.f,   -1.f,  0.f,
        };

        GL::CGhnd vertbuf;
        {
            GL::BufAlloc(1,&vertbuf);
            GL::BufBind(GL::BufType::ArrayData,vertbuf);
            GL::BufStorage(GL::BufType::ArrayData,
                           sizeof(vertexdata),vertexdata,
                           ResourceAccess::ReadOnly);
        }

        Profiler::Profile("Create vertex buffers");

        GL::CGhnd vao;
        {
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
        }

        Profiler::Profile("Specify vertex format");

        GL::CGhnd vprogram = 0;
        GL::CGhnd fprogram = 0;

        {
            cstring shader_files[2] = {"vr/vshader.glsl","vr/fshader.glsl"};
            CString tmp;
            cstring cstr;
            for(int32 i=0;i<2;i++)
            {
                CResources::Resource shader_handle(shader_files[i],
                                                   ResourceAccess::SpecifyStorage
                                                   |ResourceAccess::AssetFile);
                if(!CResources::FileMap(shader_handle))
                    continue;

                tmp.clear();
                tmp.insert(0,(sbyte_t*)shader_handle.data,shader_handle.size);
                cstr = &tmp[0];
                if(i==0)
                {
                    vprogram = GL::ProgramCreate(ShaderStage::Vertex,1,&cstr);
                }else if(i==1)
                {
                    fprogram = GL::ProgramCreate(ShaderStage::Fragment,1,&cstr);
                }

                CResources::FileUnmap(shader_handle);
            }
        }

        cDebug("Compilation log: {0}",GL::ProgramGetLog(vprogram));
        cDebug("Compilation log: {0}",GL::ProgramGetLog(fprogram));

        GL::CGhnd pipeline;
        {
            GL::PipelineAlloc(1,&pipeline);
            GL::PipelineUseStages(pipeline,ShaderStage::Vertex,vprogram);
            GL::PipelineUseStages(pipeline,ShaderStage::Fragment,fprogram);

            GL::PipelineBind(pipeline);

            GL::ShaderReleaseCompiler();

            if(!GL::PipelineValidate(pipeline))
                return;
        }

        Profiler::Profile("Compile shaders");

        GL::CGhnd texture_array;
        {
            GL::TexAlloc(1,&texture_array);
            GL::TexActive(0);
            GL::TexBind(GL::Texture::T2DArray,texture_array);
            GL::TexStorage3D(GL::Texture::T2DArray,1,PixelFormat::RGBA32F,1024,1024,num_textures);
            for(uint32 i=0;i<num_textures;i++)
            {
                GL::BufBind(GL::BufType::PixelUData,pbobuf[i]);
                GL::TexSubImage3D(GL::Texture::T2DArray,0,
                                  0,0,i,1024,1024,1,
                                  PixelComponents::RGBA,BitFormat::UByte,nullptr);
            }
        }

        Profiler::Profile("Create textures");

        int32 tim_unif;
        int32 cam_unif;
        CGraphicsData::CTransform obj;
        CMat4 objects[2] = {};
        CMat4 cam_mat;
        CGraphicsData::CGCamera cam;

        {
            cam.aspect = 1.6;
            cam.fieldOfView = 70.f;
            cam.position = CVec3(0,0,-3);

            /* Acquire texture uniforms */
            cam_unif = GL::ProgramGetResourceLoc(vprogram,GL_UNIFORM,"transform");
            int32 texm_unif =
                    GL::ProgramGetResourceLoc(vprogram,GL_UNIFORM,"tex_mul");
            int32 tex_unif =
                    GL::ProgramGetResourceLoc(fprogram,GL_UNIFORM,"texdata");
            tim_unif = GL::ProgramGetResourceLoc(fprogram,GL_UNIFORM,"mx");

            int32 tex_bind = 0;

            /**/
            cam_mat = CGraphicsData::GenPerspective(cam)
                    *CGraphicsData::GenTransform(cam);

            obj.scale = CVec3(1);

            CVec2 tex_mul[2] = {};
            tex_mul[0] = CVec2(1,1);
            tex_mul[1] = CVec2(-1,1);

            GL::Uniformfv(vprogram,texm_unif,2,tex_mul);
            GL::Uniformiv(fprogram,tex_unif,1,&tex_bind);
        }

        Profiler::Profile("Create uniform data");

        Profiler::PopContext();

        CVec4 clearcol(0.0);
        clearcol.a() = 1.0;

        scalar depth_zero = 0.f;
        bool cycle_color = true;

        GL::Enable(GL::Feature::Blend);
        //        GL::Enable(GL::Feature::DepthTest);

        ftimer.start();

        /* Head pose value */
        CMat4 testmat;
        CMat4 testmat_rot;

        CGraphicsData::CTransform test_transform;

        CGraphicsData::CTransform eyeTransform;
        eyeTransform.scale = CVec3(0.5);

        GL::Enable(GL::Feature::ClipDist,0);

        CGL::DrwMd mode = {CGL::Prim::Triangle,CGL::PrimCre::Explicit};

        while(!closeFlag())
        {
            scalar time = this->contextTime();

            /* Cycle eye color that is displayed */
            if(cycle_color)
            {
                clearcol.r() = CMath::sin(time+0.5);
                clearcol.g() = CMath::sin(time+5.0);
                clearcol.b() = CMath::sin(time+50.0);
            }

            time = (CMath::sin(time)+CMath::sin(time*0.5)+1.0)/2.0;

            /* Update matrices */
            CGraphicsData::CTransform test_transform;
            test_transform.rotation.y() = time;
            test_transform.rotation = normalize_quat(test_transform.rotation);
            testmat_rot = CGraphicsData::GenTransform(test_transform);

            cam.position.x() = -2;
            cam_mat = CGraphicsData::GenPerspective(cam)*CGraphicsData::GenTransform(cam);
//            if(dev)
//                testmat = dev->view(VR::Eye::Left)*3;
            testmat *= testmat_rot;

            objects[0] =
                    cam_mat
                    *testmat
                    *CGraphicsData::GenTransform(obj);

            cam.position.x() = 2;
            cam_mat = CGraphicsData::GenPerspective(cam)*CGraphicsData::GenTransform(cam);
//            if(dev)
//                testmat = dev->view(VR::Eye::Right)*3;
            testmat *= testmat_rot;

            objects[1] =
                    cam_mat
                    *testmat
                    *CGraphicsData::GenTransform(obj);

            /* Upload matrices */
            GL::Uniformfv(vprogram,cam_unif,2,false,objects);
            GL::Uniformfv(fprogram,tim_unif,1,&time);

            /* Clear buffer */
            clearcol = normalize(clearcol);

            GL::ClearBufferfv(true,0,clearcol);
            GL::ClearBufferfv(&depth_zero);

            /* Draw objects */
            GL::VAOBind(vao);
            GL::DrawArraysInstanced(mode,0,6,2);

            /*
            GL::VAOBind(distortVao);
            GL::DrawElements(mode,100,TypeEnum::UInt,0);
            */

            /* Update framecounter, print frames */
            fcounter.update(ftimer.elapsed());

            this->pollEvents();
            this->swapBuffers();
        }

        Profiler::PushContext("Renderer cleanup");

        GL::ProgramFree(1,&vprogram);
        GL::ProgramFree(1,&fprogram);
        GL::BufFree(3,pbobuf);
        GL::BufFree(1,&vertbuf);
        GL::VAOFree(1,&vao);
        GL::TexFree(1,&texture_array);
        GL::PipelineFree(1,&pipeline);

        Profiler::Profile();

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
    CResources::FileResourcePrefix("sample_data/eye-demo/");

    /*Required for SDL2 applications*/
    SubsystemWrapper<SDL2::SDL2> sdl2;
    C_UNUSED(sdl2);

    /*Moving on to regular rendering*/
    Profiler::PushContext("Root");

    CSDL2Renderer *renderer = new CDRenderer();

    Profiler::Profile("Object creation");

    CDProperties props = GetDefaultVisual();
    props.gl.flags |= GLProperties::GLDebug;
//    props.gl.flags |= GLProperties::GLVSync;

    /* The Oculus SDK configures some OpenGL state,
     *  so it needs to be done before any GL context is active */
    Profiler::PushContext("Oculus setup");
    dev = nullptr;
    {
        int32 devcount;
        if(VR::InitializeBinding())
        {
            if(VR::PollDevices(&devcount)&&devcount>0)
                dev = VR::GetDefaultDevice();
            if(dev)
                cDebug("Here's Johnny!");
            VR::PollDevices();
        }
    }
    Profiler::PopContext();

    CString err;

    if(!renderer->init(props,&err))
    {
        SDL2Dialog::ErrorMessage("Initialization Error",err.c_str());
        cDebug("Initialization error: {0}",err);
        return 1;
    }
    Profiler::Profile("Initialize renderer");

    if(!(  GL::SeparableShaderSupported()
           ||GL::VertexAttribBinding()
           ||GL::ViewportArraySupported()
           ||GL::BufferStorageSupported()))
    {
        cDebug("Unable to start: Required OpenGL extensions not found");
        return 1;
    }

    if(!CGL::CGL45::CullDistanceSupported())
    {
        cDebug("Some optional extensions were not found. Your experience might suffer.");
    }

    Profiler::Profile("Get GL requirements");

    renderer->run();

    Profiler::Profile("Runtime");

    renderer->cleanup();
    delete renderer;

    Profiler::Profile("Cleanup");

    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
