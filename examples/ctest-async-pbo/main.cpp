#include <coffee/CCore>
#include <coffee/core/CGraphics>
#include <coffee/CImage>
#include <coffee/core/plat/application_start.h>

#include <coffee/core/graphics/cvertexspecification.h>

#include <coffee/core/base/cmath_glm.h>

using namespace Coffee;
using namespace CDisplay;
using namespace CGraphicsData;

void framefun(uint32 t, const void*)
{
    cDebug("Framerate: %i",t);
}

class CDHudRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
public:
    CDHudRenderer()
        : CGLBindingRenderer(0)
    {
        m_msg_filter = glbinding_default_filter;
    }

    void run()
    {
        CFunctional::CFrameCounter counter(framefun);
        counter.interval = 1000000;
        CElapsedTimerMicro* clock = coffee_fun_alloc_timer_micro();
        clock->start();

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

        const uint32 indexdata[] = {
            0, 1, 2, 1, 2, 3,
        };

        constexpr byte_t vshader_src[] = {
            "#version 330\n"
            "layout(location = 0) in vec3 position;"
            "layout(location = 1) in vec2 texcoord;"
            "layout(location = 2) in mat4 transform;"
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

        //Creating a shader pipeline
        CSimplePipeline basePipeline;
        basePipeline.create(vshader_src,fshader_src);
        coffee_graphics_bind(basePipeline.data());

        CBuffer vertices;
        CBuffer texcoords;
        CBuffer indices;
        CNBuffer<3> transforms;

        {
            texcoords.type = CBufferType::Array;
            vertices.type = CBufferType::Array;
            indices.type = CBufferType::Index;
            coffee_graphics_alloc(&vertices);
            coffee_graphics_alloc(&texcoords);
            coffee_graphics_alloc(&indices);
            coffee_graphics_alloc(transforms.size,CBufferType::Array,transforms.data);

            coffee_graphics_activate(&texcoords);
            coffee_graphics_activate(&vertices);
            coffee_graphics_activate(&indices);

            coffee_graphics_buffer_store(&vertices,
                                         vertexdata,
                                         sizeof(vertexdata),
                                         CBufferUsage::StaticDraw);
            coffee_graphics_buffer_store(&texcoords,
                                         texdata,
                                         sizeof(texdata),
                                         CBufferUsage::StaticDraw);
            coffee_graphics_buffer_store(&indices,
                                         indexdata,
                                         sizeof(indexdata),
                                         CBufferUsage::StaticDraw);
        }

        CMat4 rtf,wtf,rt;

        //Vertex specification
        CVertexArrayObject vao;
        CVertexDescription vao_desc;

        coffee_graphics_alloc(&vao);
        coffee_graphics_bind(&vao);

        vao_desc.addAttribute<scalar,3,CDataType::Scalar>(0,vertexdata);
        vao_desc.addAttribute<scalar,2,CDataType::Scalar>(1,texdata);

        vao_desc.getBinding(0)->binding = 0;
        vao_desc.getBinding(0)->buffer = &vertices;
        vao_desc.getBinding(1)->binding = 1;
        vao_desc.getBinding(1)->buffer = &texcoords;

        vao_desc.addAttributeMatrix(2,&rt);

        for(size_t i=0;i<4;i++)
        {
            vao_desc.getBinding(2+i)->binding = 2+i;
            vao_desc.getBinding(2+i)->divisor = 1;
            vao_desc.getBinding(2+i)->buffer = &transforms.current();
        }
        coffee_graphics_vao_attribute_index_buffer(&vao,&indices);

        vao_desc.applyAttributes(&vao);
        vao_desc.bindAttributes(&vao);

        //Creating transforms
        CTransform root;
        root.position = CVec3(0,0,0);
        root.scale = CVec3(0.5);

        CGCamera camera;
        camera.aspect = 1.6f;
        camera.position = CVec3(0,0,-3);

        rtf = coffee_graphics_gen_transform(root);
        wtf = coffee_graphics_gen_perspective(camera)
                * coffee_graphics_gen_transform(camera);

        CNode worldNode;
        worldNode.transform = &wtf;

        CNode rootNode;
        rootNode.parent = &worldNode;
        rootNode.transform = &rtf;

        rt = coffee_node_get_transform(&rootNode);

        for(uint32 i=0;i<transforms.size;i++)
        {
            coffee_graphics_activate(&transforms.current());
            coffee_graphics_buffer_store_immutable(
                        &transforms.current(),&rt,sizeof(CMat4),
                        CBufferStorage::Coherent|
                        CBufferStorage::Persistent|
                        CBufferStorage::WriteBit);
            coffee_graphics_buffer_map(
                        &transforms.current(),
                        CBuffer_PersistentBufferFlags);
            transforms.advance();
        }

        //Creating texture

        CUniform texuni;
        CTextureSampler glsamp;
        CTexture gltext;
        CTextureSize texsize = {};
        CTextureRegion texreg = {};
        CNBuffer<2> pbos;
        CRGBA* texstorage_1;
        CRGBA* texstorage_2;

        {
            texuni.object_name = "diffsamp";

            coffee_graphics_uniform_get(&basePipeline.frag,&texuni);
        }

        {
            texsize.w = texsize.h = texreg.w = texreg.h = 1024;

            texstorage_1 = (CRGBA*)c_calloc(sizeof(CRGBA),texsize.w*texsize.h);
            texstorage_2 = (CRGBA*)c_calloc(sizeof(CRGBA),texsize.w*texsize.h);

            for(int32 i=0;i<texsize.w;i++)
                for(int32 j=0;j<texsize.h;j++)
                {
                    texstorage_1[j*texsize.h + i].a = 255;
                    texstorage_1[j*texsize.h + i].r = 255;

                    texstorage_2[j*texsize.h + i].a = 255;
                    texstorage_2[j*texsize.h + i].r = 200;
                }

            coffee_graphics_alloc(pbos.size,CBufferType::PixelUnpack,pbos.data);

            gltext.textureType = CTexType::Tex2D;
            gltext.format = CTexIntFormat::RGBA8;
            gltext.levels = 1;
            gltext.size = texsize;
            coffee_graphics_alloc(&gltext);
            coffee_graphics_activate(&gltext);

            coffee_graphics_tex_define(&gltext);

            for(size_t i=0;i<pbos.size;i++)
            {
                coffee_graphics_activate(&pbos.current());
                coffee_graphics_buffer_store_immutable(
                            &pbos.current(),
                            texstorage_1,texsize.w*texsize.h*sizeof(CRGBA),
                            CBufferStorage::WriteBit|
                            CBufferStorage::Persistent|
                            CBufferStorage::Coherent);
                coffee_graphics_buffer_map(&pbos.current(),
                                           CBuffer_PersistentBufferFlags);

                pbos.advance();
            }

            coffee_graphics_tex_pbo_upload(
                        &gltext,&pbos.current(),
                        CTexFormat::RGBA,texreg,CDataType::UByte,0);

            coffee_graphics_alloc(&glsamp);

            coffee_graphics_tex_get_handle(gltext,glsamp);
            coffee_graphics_tex_make_resident(glsamp);
        }

        //Enable some states
        coffee_graphics_blend(true);
        coffee_graphics_depth(true);

        //Create a drawcall
        CGLDrawCall drawcall;
        drawcall.count = sizeof(indexdata)/sizeof(uint32);
        drawcall.instanceCount = 1;

        //Set uniform, a texture handle
        coffee_graphics_uniform_set_texhandle(&basePipeline.frag,&texuni,glsamp.bhandle);

        bool flop = false;

        this->showWindow();
        while(!closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color|CClearFlag::Depth);

            camera.position.x() = CMath::fmod(this->contextTime(),3.0)-1.5;

            wtf = coffee_graphics_gen_perspective(camera)
                    * coffee_graphics_gen_transform(camera);
            rt = coffee_node_get_transform(&rootNode);

            c_memcpy(transforms.current().data,&rt,sizeof(rt));

            for(int i=0;i<4;i++)
            {
                CVertexBufferBinding* bind = vao_desc.getBinding(2+i);
                coffee_graphics_vao_attribute_bind_buffer(
                            &vao,
                            *bind,
                            &transforms.current());
            }

            pbos.awaitCurrent();

            coffee_graphics_tex_pbo_upload(
                        &gltext,&pbos.current(),
                        CTexFormat::RGBA,texreg,CDataType::UByte,0);


            {
                CRGBA* tex = (CRGBA*)pbos.next().data;

                CRGBA* d;
                if(flop)
                    d = texstorage_1;
                else
                    d = texstorage_2;
                flop = !flop;

                c_memcpy(tex,d,sizeof(CRGBA)*texsize.w*texsize.h);
            }


            counter.update(clock->elapsed());
            coffee_graphics_draw_indexed(CPrimitiveMode::Triangles,&drawcall);
            pbos.lockCurrent();

            pbos.advance();

            this->swapBuffers();
            this->pollEvents();
        }

        coffee_graphics_free(&vao);
        coffee_graphics_free(&gltext);
        coffee_graphics_free(&vertices);
        coffee_graphics_free(&texcoords);
        coffee_graphics_free(transforms.size,transforms.data);
        coffee_graphics_free(&indices);
    }
    void eventWindowsHandle(const CDisplay::CDEvent *e)
    {
        if(e->type==CDEvent::State)
        {
            const CDStateEvent* sev = (const CDStateEvent*)&e[1];
            if(sev->type==CDStateEvent::Closed)
                this->closeWindow();
        }
    }
    void eventInputHandle(const CIEvent *e)
    {
        CSDL2Renderer::eventInputHandle(e);
        if(e->type==CIEvent::Keyboard)
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)&e[1];
            if(kev->key == CK_Escape)
                this->closeWindow();
        }
    }
private:
    CQuat t;
};

int32 coffee_main(int32, byte_t**)
{
    CResources::coffee_file_set_resource_prefix("sample_data/");

    CDRendererBase *renderer = new CDHudRenderer();
    CDWindowProperties props = coffee_get_default_visual();
//    props.contextProperties.flags |= CGLContextProperties::GLDebug;
    props.contextProperties.flags |= CGLContextProperties::GLVSync;
    renderer->init(props);
    renderer->run();
    renderer->cleanup();
    delete renderer;

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
