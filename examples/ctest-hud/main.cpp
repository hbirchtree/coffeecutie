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

        rtf = coffee_graphics_gen_transform(&root);
        wtf = coffee_graphics_gen_perspective(&camera)
                * coffee_graphics_gen_transform(camera.position,CVec3(1),camera.rotation);

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
        CResources::CResource texture("ctest_hud/particle_sprite.png");
        CResources::coffee_file_pull(&texture);

        CUniform texuni;
        CTexture gltext;

        {
            texuni.object_name = "diffsamp";

            coffee_graphics_uniform_get(&basePipeline.frag,&texuni);
        }

        {
            gltext.textureType = CTexType::Tex2D;
            gltext.format = CTexIntFormat::RGBA8;
            coffee_graphics_alloc(&gltext);
            coffee_graphics_activate(&gltext);

            CImportedTexture gtexdata = coffee_graphics_tex_create_rtexdata(texture);

            gltext.size = gtexdata.data()->size;

            coffee_graphics_tex_define(&gltext);
            coffee_graphics_tex_store(&gltext,gtexdata.data(),0);

            CResources::coffee_file_free(&texture);

            coffee_graphics_tex_mipmap(&gltext);

            coffee_graphics_tex_get_handle(&gltext);
            coffee_graphics_tex_make_resident(&gltext);
        }

        //Enable some states
        coffee_graphics_blend(true);
        coffee_graphics_depth(true);

        //Create a drawcall
        CGLDrawCall drawcall;
        drawcall.count = sizeof(indexdata)/sizeof(uint32);
        drawcall.instanceCount = 1;

        CVec3 camera_pos(0,0,-3);

        //Set uniform, a texture handle
        coffee_graphics_uniform_set_texhandle(&basePipeline.frag,&texuni,gltext.bhandle);

        this->showWindow();
        while(!closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color|CClearFlag::Depth);

            camera_pos.x() = CMath::fmod(this->contextTime(),3.0)-1.5;

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

            counter.update(clock->elapsed());
            coffee_graphics_draw_indexed(CPrimitiveMode::Triangles,&drawcall);


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
        }else if(e->type==CIEvent::MouseMove)
        {
            const CIMouseMoveEvent* mev = (const CIMouseMoveEvent*)&e[1];
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
    props.contextProperties.flags |= CGLContextProperties::GLDebug;
    renderer->init(props);
    renderer->run();
    renderer->cleanup();
    delete renderer;

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
