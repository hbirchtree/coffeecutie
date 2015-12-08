#include <coffee/CCore>
#include <coffee/core/CGraphics>
#include <coffee/CImage>
#include <coffee/core/plat/application_start.h>

#include <coffee/core/base/cmath_glm.h>

using namespace Coffee;
using namespace CDisplay;
using namespace CGraphicsData;

void framefun(uint32 t, const void*)
{
    cDebug("Framerate: %i",t);
}

bool _glmessagefilter(CGLReport* report)
{
//    return !(report->severity==GL_DEBUG_SEVERITY_NOTIFICATION);
    return true;
}

class CDHudRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
public:
    CDHudRenderer()
        : CGLBindingRenderer(0)
    {
        m_msg_filter = _glmessagefilter;
    }

    void run()
    {
        CFunctional::CFrameCounter counter(framefun);
        counter.interval = 1000000;
        CElapsedTimerMicro* clock = coffee_fun_alloc_timer_micro();
        clock->start();

        const CVec3 vertexdata[] = {
            CVec3( 1.f, -1.f, 0.f), //2
            CVec3( 1.f, -1.f, 0.f), //1
            CVec3( 1.f, -1.f, 0.f), //4
        };

        const CVec2 texdata[] = {
            CVec2(0.f, 0.f), //1
            CVec2(1.f, 0.f), //2
            CVec2(0.f, 1.f), //3
            CVec2(1.f, 0.f), //2
            CVec2(1.f, 1.f), //4
            CVec2(0.f, 1.f), //3
        };

        const uint32 indexdata[] = {
            0, 1, 2,
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

        CPipeline basePipeline;
        CShaderStageProgram vertshader;
        CShaderStageProgram fragshader;
        coffee_graphics_alloc(&basePipeline);
        coffee_graphics_shader_compile(&vertshader,
                                       vshader_src,CProgramStage::Vertex);
        coffee_graphics_shader_compile(&fragshader,
                                       fshader_src,CProgramStage::Fragment);
        coffee_graphics_shader_attach(&basePipeline,&vertshader,
                                      CProgramStage::Vertex);
        coffee_graphics_shader_attach(&basePipeline,&fragshader,
                                      CProgramStage::Fragment);

        coffee_graphics_bind(&basePipeline);

        CBuffer vertices;
        CBuffer texcoords;
        CBuffer indices;
        CBuffer transforms[3];

        {
            texcoords.type = CBufferType::Array;
            vertices.type = CBufferType::Array;
            indices.type = CBufferType::Index;
            coffee_graphics_alloc(&vertices);
            coffee_graphics_alloc(&texcoords);
            coffee_graphics_alloc(&indices);
            coffee_graphics_alloc(3,CBufferType::Array,(CBuffer*)transforms);

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

        CVertexArrayObject vao;
        CVertexBufferBinding tex_bind;
        CVertexBufferBinding vrt_bind;
        CVertexFormat mat_fmt;
        CVertexBufferBinding mat_bnd[4];

        {
            coffee_graphics_alloc(&vao);
            coffee_graphics_bind(&vao);

            vrt_bind.buffer = &vertices;
            vrt_bind.stride = sizeof(CVec3);

            tex_bind.buffer = &texcoords;
            tex_bind.stride = sizeof(CVec2);

            CVertexFormat vrt_fmt;
            vrt_fmt.offset = 0;
            vrt_fmt.size = 3;
            vrt_fmt.type = CDataType::Scalar;

            CVertexFormat tex_fmt;
            tex_fmt.offset = 0;
            tex_fmt.size = 2;
            tex_fmt.type = CDataType::Scalar;

            CVertexAttribute vrt_att;
            vrt_att.attribIdx = 0;
            vrt_att.bnd = &vrt_bind;
            vrt_att.fmt = &vrt_fmt;

            CVertexAttribute tex_att;
            tex_att.attribIdx = 1;
            tex_att.bnd = &tex_bind;
            tex_att.fmt = &tex_fmt;

            coffee_graphics_vao_attribute_format(&vao,vrt_att,vrt_fmt);
            coffee_graphics_vao_attribute_buffer(&vao,vrt_att,vrt_bind);
            coffee_graphics_vao_attribute_bind_buffer(&vao,vrt_bind);

            coffee_graphics_vao_attribute_format(&vao,tex_att,tex_fmt);
            coffee_graphics_vao_attribute_buffer(&vao,tex_att,tex_bind);
            coffee_graphics_vao_attribute_bind_buffer(&vao,tex_bind);

            mat_fmt.offset = 0;
            mat_fmt.size = 4;
            mat_fmt.type = CDataType::Scalar;


            for(int i=0;i<4;i++)
            {
                CVertexBufferBinding* bnd = &mat_bnd[i];
                bnd->binding = 2+i;
                bnd->buffer = &transforms[0];
                bnd->offset = sizeof(CVec4)*i;
                bnd->stride = sizeof(CMat4);
                bnd->divisor = 1;
                CVertexAttribute attr;
                attr.attribIdx = 2+i;
                attr.bnd = bnd;
                attr.fmt = &mat_fmt;

                coffee_graphics_vao_attribute_format(&vao,attr,mat_fmt);
                coffee_graphics_vao_attribute_buffer(&vao,attr,*bnd);
            }
            coffee_graphics_vao_attribute_index_buffer(&vao,&indices);
        }

        CTransform root;
        root.position = CVec3(0,0,0);

        CGCamera camera;
        camera.aspect = 1.6f;
        camera.position = CVec3(0,0,-3);

        CMat4 rtf = coffee_graphics_gen_transform(&root);
        CMat4 wtf = coffee_graphics_gen_perspective(&camera);

        glm::mat4 g_wtf = glm::perspective(60.f,1.6f,1.f,10.f);
        glm::mat4 g_rtf = glm::mat4();

        glm::mat4 g_rt = glm::scale(glm::mat4(),glm::vec3(1)) * glm::translate(glm::mat4(),glm::vec3(0,0,-3));

        CNode worldNode;
        worldNode.transform = &wtf;

        CNode rootNode;
        rootNode.parent = &worldNode;
        rootNode.transform = &rtf;

//        CMat4 wt = coffee_node_get_transform(&worldNode);
//        CMat4 rt = coffee_node_get_transform(&rootNode);

        CMat4 rt = wtf
                * coffee_graphics_gen_transform(CVec3(0,0,-3),
                                                CVec3(1),
                                                CQuat())
                * rtf;

        glm::mat4 g_rtv = g_wtf * g_rt * g_rtf;

        for(int i=0;i<3;i++)
        {
            coffee_graphics_activate(&transforms[i]);
            coffee_graphics_buffer_store_immutable(
                        &transforms[i],&rt,sizeof(CMat4),
                        CBufferStorage::Coherent|
                        CBufferStorage::Persistent|
                        CBufferStorage::WriteBit);
            coffee_graphics_buffer_map(
                        &transforms[i],
                        CBufferAccess::Coherent|
                        CBufferAccess::Persistent|
                        CBufferAccess::WriteBit);
        }

        CResources::CResource texture("ctest_hud/particle_sprite.png");
        CResources::coffee_file_pull(&texture);

        CUniform texuni;
        CTexture gltext;

        {
            CStbImageLib::CStbImage ptext;
            CStbImageLib::coffee_stb_image_load(&ptext,&texture);

            gltext.textureType = CTexType::Tex2D;
            gltext.format = CTexFormat::RGBA;
            coffee_graphics_alloc(&gltext);
            coffee_graphics_activate(&gltext);
            CTextureData gtexdata;
            gtexdata.data = ptext.data;
            gtexdata.datatype = CDataType::UByte;
            gtexdata.size.w = ptext.size.w; gtexdata.size.h = ptext.size.h;
            gtexdata.format = CTexIntFormat::RGBA8;

            coffee_graphics_tex_define(&gltext,&gtexdata);
            coffee_graphics_tex_store(&gltext,&gtexdata,0);

            CStbImageLib::coffee_stb_image_free(&ptext);
            CResources::coffee_file_free(&texture);

            coffee_graphics_tex_mipmap(&gltext);

            coffee_graphics_tex_get_handle(&gltext);
            coffee_graphics_tex_make_resident(&gltext);

            texuni.object_name = "diffsamp";

            coffee_graphics_uniform_get(&fragshader,&texuni);

            coffee_graphics_uniform_set_texhandle(&fragshader,&texuni,gltext.bhandle);
        }

        coffee_graphics_blend(true);
        coffee_graphics_depth(true);

        int transform_index = 0;

        CGLDrawCall drawcall;
        drawcall.count = sizeof(indexdata)/sizeof(uint32);
        drawcall.instanceCount = 1;

        CFramebuffer cfb;
        CTexture dtex;
        CTexture ctex;
        CFramebufferAttachment fbatt;

        {
            coffee_graphics_alloc(&cfb);
            cfb.size.w = 1280;
            cfb.size.h = 720;

            CTextureData filler;
            filler.size.w = 1280;
            filler.size.h = 720;
            filler.format = CTexIntFormat::Depth;
            filler.datatype = CDataType::UByte;

            dtex.format = CTexFormat::Depth;
            dtex.levels = 1;
            dtex.textureType = CTexType::Tex2D;
            coffee_graphics_alloc(&dtex);
            coffee_graphics_activate(&dtex);
            coffee_graphics_tex_2d_define(&dtex,&filler);

            ctex.format = CTexFormat::RGBA;
            ctex.levels = 1;
            ctex.textureType = CTexType::Tex2D;
            coffee_graphics_alloc(&ctex);
            coffee_graphics_activate(&ctex);
            filler.format = CTexIntFormat::RGBA8;
            coffee_graphics_tex_2d_define(&ctex,&filler);

            fbatt.texture = &dtex;
            fbatt.attachLevel = 0;
            fbatt.level = 0;
            fbatt.target = CFBAttachment::Depth;
            coffee_graphics_framebuffer_attach_texture(&cfb,&fbatt);

            fbatt.texture = &ctex;
            fbatt.attachLevel = 0;
            fbatt.level = 0;
            fbatt.target = CFBAttachment::Color;
            coffee_graphics_framebuffer_attach_texture(&cfb,&fbatt);

        }

        CVec3 camera_pos(0,0,-3);

//        coffee_graphics_bind(&cfb);

        this->showWindow();
        while(!closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color|CClearFlag::Depth);

            camera_pos.x() = CMath::fmod(this->contextTime(),3.0)-1.5;

            g_rtv = (g_wtf * (glm::mat4_cast(glm::quat())*glm::translate(glm::mat4(),glm::vec3(CMath::fmod(this->contextTime(),3.0)-1.5,0,-2)))) * g_rtf;

//            rt = wtf
//                    * coffee_graphics_gen_transform(camera_pos,
//                                                     CVec3(1),
//                                                     CQuat(1,0,0,0))
//                    * rtf;

            c_memcpy(transforms[transform_index].data,&g_rtv,sizeof(rt));

            for(int i=0;i<4;i++)
                coffee_graphics_vao_attribute_bind_buffer(
                            &vao,
                            mat_bnd[i],
                            &transforms[transform_index]);

            transform_index = (transform_index+1)%3;

            counter.update(clock->elapsed());
            coffee_graphics_draw_indexed(CPrimitiveMode::Triangles,&drawcall);

            this->swapBuffers();
            this->pollEvents();
        }
//        coffee_graphics_unbind(&cfb);

//        coffee_graphics_tex_dump(&dtex,"depth.png");

        coffee_graphics_free(&vao);
        coffee_graphics_free(&gltext);
        coffee_graphics_free(&vertices);
        coffee_graphics_free(&texcoords);
        for(int i=0;i<4;i++)
        {
            coffee_graphics_free(&transforms[i]);
        }
        coffee_graphics_free(&basePipeline);
        coffee_graphics_free(&vertshader);
        coffee_graphics_free(&fragshader);
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
