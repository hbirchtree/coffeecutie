#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/CFFMedia>

#include <coffee/graphics_apis/opengl/include/glfunctions.h>

using namespace Coffee;
using namespace Coffee::CGraphicsData;
using namespace Coffee::CGraphicsWrappers;
using namespace Coffee::CFFMedia;

class CDRenderer : public CDisplay::CGLBindingRenderer
{
public:
    CDRenderer():
        CGLBindingRenderer(0)
    {
        m_msg_filter = coffee_graphics_debug_filter_all;
    }

    void run()
    {
        //FFMPEG stuff here
        coffee_ffmedia_init(nullptr,false);

        //Open video file
        CResource testfile("test.mp4");
        coffee_file_pull(testfile);
        //Create a video player for the data
        CFFVideoPlayer* video = coffee_ffmedia_create_player(testfile);

        if(!video)
            return;

        //Define video format
        CFFVideoDescriptor v_fmt;
        v_fmt.video.size.w = 1280;
        v_fmt.video.size.h = 720;
        //Create a decoding context
        CFFDecodeContext* dCtxt = coffee_ffmedia_create_decodecontext(video,v_fmt);
        //

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
        //Construct a shader pipeline
        CSimplePipeline pipeline;
        pipeline.create(vshader_src,fshader_src);

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
        initTexture.size = coffee_ffmedia_video_framesize(CSize(v_fmt.video.size.w,
                                                                v_fmt.video.size.h));
        initTexture.data = (byte_t*)c_alloc(initTexture.size);

        CRGBA* pixels = (CRGBA*)initTexture.data;

        C_UNUSED(pixels);

        CFFVideoTarget trg = {};
        trg.v.location = initTexture.data;

        bool status = coffee_ffmedia_decode_frame(video,dCtxt,&trg);

        C_UNUSED(status);

        //Define output texture
        CBufferedTexture<2> texture;
        texture.createTexture(v_fmt.video.size,CTexIntFormat::RGBA8,
                              CTexType::Tex2D,1,initTexture,CTexFormat::RGBA);

        c_free(initTexture.data);

        coffee_graphics_tex_load_safe(texture.sampler(),texture.texture());

        //

        //Bind a pipeline
        coffee_graphics_bind(pipeline.data_ref());

        //Set uniforms
        CGCamera camera;
        camera.aspect = 1.6f;
        camera.fieldOfView = 60.f;
        camera.position = CVec3(0,0,-3);

        CMat4 cam_matrix = coffee_graphics_gen_perspective(camera)
                * coffee_graphics_gen_transform(camera);

        CTransform quad_trans;
        quad_trans.position = CVec3(0,0,0);
        quad_trans.scale = CVec3(1.6,1.0,1.0);

        CMat4 quad_matrix = coffee_graphics_gen_transform(quad_trans);

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

        double timeout = this->contextTime();
        int counter;

        this->showWindow();
        while(!this->closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color);


            //FFMPEG
            trg.v.location = texture.buffers().current().data;
            coffee_ffmedia_decode_frame(video,dCtxt,&trg);

            texture.uploadData(CTexFormat::RGBA,0);
            //

            coffee_graphics_draw_indexed(CPrimitiveMode::Triangles,&drawcall);
            texture.advance();

            counter++;
            if((this->contextTime()-timeout)>=1.0)
            {
                timeout = this->contextTime();
                cDebug("Framerate: {0}",counter);
                counter=0;
            }

            this->swapBuffers();
            this->pollEvents();
        }

        //Free all the FFMPEG data
        coffee_ffmedia_free_decodecontext(dCtxt);
        coffee_ffmedia_free_player(video);
        coffee_file_free(testfile);

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
    coffee_file_set_resource_prefix("sample_data/");

    CDisplay::CDRendererBase* renderer = new CDRenderer;
    std::atomic_bool sync;
    sync.store(false);
    CDisplay::CDWindowProperties props = CDisplay::coffee_get_default_visual();
    props.contextProperties.flags = props.contextProperties.flags|
            CDisplay::CGLContextProperties::GLDebug|
            CDisplay::CGLContextProperties::GLFeatureLevelProfile|
            CDisplay::CGLContextProperties::GLAutoResize/*|
            CDisplay::CGLContextProperties::GLVSync*/;
    props.flags = CDisplay::CDWindowProperties::Resizable;

    std::future<void> status = CDisplay::coffee_display_start_async(&sync,renderer,props);

    status.get();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
