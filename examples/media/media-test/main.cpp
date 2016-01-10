#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/graphics_apis/COpenGL>
#include <coffee/CAudio>
#include <coffee/CFFMedia>

#include <coffee/graphics_apis/opengl/include/glfunctions.h>

using namespace Coffee;
using namespace Coffee::CGraphicsData;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CFFMedia;
using namespace COpenAL;
using namespace CSoundAbstraction;

class CDRenderer : public CDisplay::CGLBindingRenderer
{
public:
    CDRenderer():
        CGLBindingRenderer(0)
    {
        m_msg_filter = coffee_graphics_debug_filter_ignore_notifications;
    }

    void run()
    {
        //Create an FFMPEG player
        CResource video_file("test-.webm");
        FilePull(video_file);

        CFFPlayer player(video_file);

        //Set output descriptor
        player.descriptor().video.size.width = 1280;
        player.descriptor().video.size.height = 720;

        player.createDecoder();

        //OpenGL stuff

        const CVec3 vertexdata[] = {
            CVec3(-1.f, 1.f, 0.f),
            CVec3( 1.f, 1.f, 0.f),
            CVec3(-1.f,-1.f, 0.f),
            CVec3( 1.f,-1.f, 0.f),
        };

        const CVec2 texdata[] = {
            CVec2(0.f, 0.f), //1
            CVec2(1.f, 0.f), //2
            CVec2(0.f, 1.f), //3
            CVec2(1.f, 1.f), //4
        };

        constexpr uint32 indexdata[] = {
            0, 1, 2, 1, 2, 3,
        };
        constexpr byte_t vshader_src[] = {
            "#version 330\n"
            "layout(location = 0) in vec3 position;"
            "layout(location = 1) in vec2 texcoord;"
            "uniform mat4 transform;"
            "out gl_PerVertex {"
            "   vec4 gl_Position;"
            "};"
            "out VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
            "   vdata.vtex = texcoord;"
            "   gl_Position = transform * vec4(position,1.0);"
            "}"
        };

        constexpr byte_t fshader_src[] = {
            "#version 330\n"
            "layout(location = 0) out vec4 Out_color;"
            "uniform sampler2D diffsamp;"
            "in VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
            "   vec4 smp = texture(diffsamp,vdata.vtex);"
            "   Out_color = smp;"
            "}"
        };

        constexpr byte_t fshader_blank_src[] = {
            "#version 330\n"
            "layout(location = 0) out vec4 Out_color;"
            "in VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
            "   Out_color = vec4(1.0);"
            "}"
        };

        //Construct a shader pipeline
        CSimplePipeline pipeline;
        pipeline.create(vshader_src,fshader_src);

        CShaderStageProgram blankshader;
        coffee_graphics_shader_compile(blankshader,fshader_blank_src,CProgramStage::Fragment);

        //Define buffers
        CBuffer vertexbuffer;
        CBuffer texcrdbuffer;
        CBuffer indexbuffer;
        {
            vertexbuffer.type = CBufferType::Array;
            indexbuffer.type = CBufferType::Index;

            coffee_graphics_alloc(vertexbuffer);
            coffee_graphics_alloc(texcrdbuffer);
            coffee_graphics_alloc(indexbuffer);

            coffee_graphics_activate(vertexbuffer);
            coffee_graphics_activate(texcrdbuffer);
            coffee_graphics_activate(indexbuffer);

            coffee_graphics_buffer_store(vertexbuffer,vertexdata,sizeof(vertexdata),
                                         CBufferUsage::StaticDraw);

            coffee_graphics_buffer_store(texcrdbuffer,texdata,sizeof(texdata),
                                         CBufferUsage::StaticDraw);

            coffee_graphics_buffer_store(indexbuffer,indexdata,sizeof(indexdata),
                                         CBufferUsage::StaticDraw);
        }
        //Define vertex attributes
        CVertexDescription vdescriptor;
        vdescriptor.addAttribute<scalar,3,CDataType::Scalar>(0,vertexdata);
        vdescriptor.addAttribute<scalar,2,CDataType::Scalar>(1,texdata);

        vdescriptor.getBinding(0)->binding = 0;
        vdescriptor.getBinding(0)->buffer = &vertexbuffer;
        vdescriptor.getBinding(1)->binding = 1;
        vdescriptor.getBinding(1)->buffer = &texcrdbuffer;
        //Define vertex array object
        CVertexArrayObject vao;
        coffee_graphics_alloc(vao);
        coffee_graphics_bind(vao);
        coffee_graphics_vao_attribute_index_buffer(vao,indexbuffer);
        vdescriptor.applyAttributes(vao);
        vdescriptor.bindAttributes(vao);
        //Define uniform data
        CUniform matrixuni;
        CUniform texuni;

        texuni.object_name = "diffsamp";
        matrixuni.object_name = "transform";

        coffee_graphics_uniform_get(pipeline.frag,texuni);
        coffee_graphics_uniform_get(pipeline.vert,matrixuni);

        //Create a video target

        CByteData initTexture;
        initTexture.size = coffee_ffmedia_video_framesize(CSize(player.descriptor().video.size.width,
                                                                player.descriptor().video.size.height));
        initTexture.data = (byte_t*)Alloc(initTexture.size);

        player.videoTarget().v.location = initTexture.data;
        player.videoTarget().v.max_size = initTexture.size;

        player.createDecoder();

        CASSERT(player.startDecoder());

        //Define output texture
        CBufferedTexture<2> texture;
        texture.createTexture(player.descriptor().video.size,CTexIntFormat::RGBA8,
                              CTexType::Tex2D,1,initTexture,CTexFormat::RGBA);

        CFree(initTexture.data);

        texture.sampler().unit = 0;

        coffee_graphics_tex_load_safe(texture.sampler(),texture.texture());

        //

        //Bind a pipeline
        coffee_graphics_bind(pipeline.data_ref());

        //Set uniforms
        CGCamera camera;
        camera.aspect = 1.6f;
        camera.fieldOfView = 60.f;
        camera.position = CVec3(0,0,-3);

        CMat4 cam_matrix = GenPerspective(camera)
                * GenTransform(camera);

        camera.orthoview.w = 1280;
        camera.orthoview.h = 720;
        camera.orthoview.x = 0;
        camera.orthoview.y = 0;

        CTransform quad_trans;
        quad_trans.position = CVec3(0,0,0);
        quad_trans.scale = CVec3(1.6f,1.f,1.f);

        CMat4 quad_matrix = GenTransform(quad_trans);

        CNode root;
        root.transform = &cam_matrix;
        CNode quad;
        quad.parent = &root;
        quad.transform = &quad_matrix;

        CMat4 final_transform = coffee_node_get_transform(&quad);

        coffee_graphics_uniform_set_texhandle_safe(pipeline.frag,texuni,
                                                   texture.sampler().unit);
        glProgramUniformMatrix4fv(pipeline.vert.handle,matrixuni.index,
                                  1,GL_FALSE,(scalar*)&final_transform);
        //

        //Create a drawcall
        CGLDrawCall drawcall = {};
        drawcall.count = sizeof(indexdata)/sizeof(indexdata[0]);
        drawcall.instanceCount = 1;

        //Create a dummy buffer for audio
        CByteData audiobuf;
        audiobuf.size = coffee_ffmedia_audio_samplesize(player.player())*48000*4;
        audiobuf.data = (byte_t*)Alloc(audiobuf.size);

        double timeout = this->contextTime();
        int counter = 0;

        CElapsedTimerMicro* timer = coffee_fun_alloc_timer_micro();

        CALSoundManager snd_man;
        CSoundDevice<CALSource,CALBuffer>* snd_dev =
                snd_man.createDevice(snd_man.defaultSoundDevice());

        CALSoundFormat snd_fmt;
        snd_fmt.setBitDepth(16);
        snd_fmt.setChannels(2);
        snd_fmt.setSamplerate(48000);

        CSoundStream<CALSource,CALBuffer>& snd_streamer = snd_dev->genStream(snd_fmt);

        this->showWindow();
        while(!this->closeFlag())
        {
            timer->start();
            coffee_graphics_clear(CClearFlag::Color);


            //FFMPEG
            if(player.isDisplayTime())
            {
                player.videoTarget().v.location = texture.buffers().current().data;

                player.awaitFrames();

                if(player.checkTimer())
                    texture.uploadData(CTexFormat::RGBA,0);

                while(player.videoTarget().a.packet_queue.size())
                {
                    CFFAudioPacket& pckt = player.videoTarget().a.packet_queue.front();

                    CALSoundFormat fmt;
                    fmt.setBitDepth(pckt.bitdepth);
                    fmt.setChannels(pckt.channels);
                    fmt.setSamplerate(pckt.frequency);

                    snd_streamer.feedData(pckt.data,fmt,pckt.samples);

                    CFree(pckt.data);

                    player.videoTarget().a.packet_queue.pop();
                }
            }
            //

            coffee_graphics_draw_indexed(CPrimitiveMode::Triangles,drawcall);
            if(player.checkTimerLast())
            {
                texture.advance();
            }

            this->swapBuffers();
            this->pollEvents();

            counter++;
            if((this->contextTime()-timeout)>=1.0)
            {
                timeout = this->contextTime();
                cDebug("framerate={0}/s, frametime={1}us",counter,timer->elapsed());
                counter=0;
            }

            if(player.finished())
                this->closeWindow();
        }

        //Free all the FFMPEG data
        FileFree(video_file);

        for(const std::pair<CString,CString>& ft : CDisplay::coffee_glbinding_get_graphics_feature_level())
        {
            cBasicPrint("{0} : {1}",ft.first.c_str(),ft.second.c_str());
        }
    }

    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);
        if(e.type == CIEvent::Keyboard)
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)data;
            if(kev->key == CK_Escape)
                this->closeWindow();
        }
    }
};

int32 coffee_main(int32, byte_t**)
{
    FileResourcePrefix("sample_data/");

    CDisplay::CDRendererBase* renderer = new CDRenderer;
    std::atomic_bool sync;
    sync.store(false);
    CDisplay::CDProperties props = CDisplay::coffee_get_default_visual();
    props.gl.flags = props.gl.flags|
            CDisplay::GLProperties::GLDebug|
//            CDisplay::CGLContextProperties::GLFeatureLevelProfile|
//            CDisplay::CGLContextProperties::GLVSync|
            CDisplay::GLProperties::GLAutoResize;
    props.flags = CDisplay::CDProperties::Resizable;

    std::future<void> status = CDisplay::coffee_display_start_async(&sync,renderer,props);

    status.get();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
