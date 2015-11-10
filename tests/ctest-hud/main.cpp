#include <coffee/Core>
#include <coffee/core/Graphics>
#include <coffee/core/plat/application_start.h>

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
            CVec3(-1.f, -1.f, 0.f), //2
            CVec3( 1.f, -1.f, 0.f), //1
            CVec3(-1.f,  1.f, 0.f), //4
            CVec3( 1.f, -1.f, 0.f), //1
            CVec3( 1.f,  1.f, 0.f), //3
            CVec3(-1.f,  1.f, 0.f), //4
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
            3, 4, 5
        };

        const byte vshader_src[] = {
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

        const byte fshader_src[] = {
            "#version 330\n"
            "layout(location = 0) out vec4 Out_color;"
            "uniform sampler2D diffsamp;"
            "in VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
//            "   vec4 smp = texture(diffsamp,vdata.vtex);"
            "   vec4 smp = vec4(1.0);"
            "   Out_color = smp;"
            "}"
        };

        CPipeline basePipeline;
        CShaderStageProgram vertshader;
        CShaderStageProgram fragshader;
        coffee_graphics_alloc(&basePipeline);
        coffee_graphics_shader_compile(&vertshader,vshader_src,GL_VERTEX_SHADER,
                                       GL_VERTEX_SHADER_BIT);
        coffee_graphics_shader_compile(&fragshader,fshader_src,GL_FRAGMENT_SHADER,
                                       GL_FRAGMENT_SHADER_BIT);
        coffee_graphics_shader_attach(&basePipeline,&vertshader,GL_VERTEX_SHADER_BIT);
        coffee_graphics_shader_attach(&basePipeline,&fragshader,GL_FRAGMENT_SHADER_BIT);

        coffee_graphics_bind(&basePipeline);

        CBuffer vertices;
        CBuffer texcoords;
        CBuffer indices;
        CBuffer transforms[3];
        texcoords.type = GL_ARRAY_BUFFER;
        vertices.type = GL_ARRAY_BUFFER;
        indices.type = GL_ELEMENT_ARRAY_BUFFER;
        coffee_graphics_alloc(&vertices);
        coffee_graphics_alloc(&texcoords);
        coffee_graphics_alloc(&indices);
        coffee_graphics_alloc(3,GL_ARRAY_BUFFER,(CBuffer*)transforms);

        coffee_graphics_activate(&texcoords);
        coffee_graphics_activate(&vertices);
        coffee_graphics_activate(&indices);

        coffee_graphics_buffer_store(&vertices,
                                     vertexdata,
                                     sizeof(vertexdata),
                                     GL_STATIC_DRAW);
        coffee_graphics_buffer_store(&texcoords,
                                     texdata,
                                     sizeof(texdata),
                                     GL_STATIC_DRAW);
        coffee_graphics_buffer_store(&indices,
                                     indexdata,
                                     sizeof(indexdata),
                                     GL_STATIC_DRAW);

        CVertexArrayObject vao;
        coffee_graphics_alloc(&vao);
        coffee_graphics_bind(&vao);

        CVertexBufferBinding vrt_bind;
        vrt_bind.buffer = &vertices;
        vrt_bind.stride = sizeof(CVec3);

        CVertexBufferBinding tex_bind;
        tex_bind.buffer = &texcoords;
        tex_bind.stride = sizeof(CVec2);

        CVertexFormat vrt_fmt;
        vrt_fmt.normalized = GL_FALSE;
        vrt_fmt.offset = 0;
        vrt_fmt.size = 3;
        vrt_fmt.type = GL_FLOAT;

        CVertexFormat tex_fmt;
        tex_fmt.normalized = GL_FALSE;
        tex_fmt.offset = 0;
        tex_fmt.size = 2;
        tex_fmt.type = GL_FLOAT;

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

        CVertexFormat mat_fmt;
        mat_fmt.offset = 0;
        mat_fmt.size = 4;
        mat_fmt.type = GL_FLOAT;

        CVertexBufferBinding mat_bnd[4];

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

        coffee_graphics_bind(&indices);

        CTransform root;
        root.position = CMath::vec3(0,-0.5,0);

        CGCamera camera;
        camera.aspect = 1.6f;
        camera.position = CMath::vec3(0,0,-3);

        coffee_graphics_gen_matrix(&root);
        coffee_graphics_gen_matrix_perspective(&camera);

        CNode worldNode;
        worldNode.transform = (CMat4*)&camera.matrix;

        CNode rootNode;
        rootNode.parent = &worldNode;
        rootNode.transform = (CMat4*)&root.matrix;

        CMat4 wt = coffee_node_get_transform(&worldNode);
        CMat4 rt = coffee_node_get_transform(&rootNode);

        for(int i=0;i<3;i++)
        {
            coffee_graphics_activate(&transforms[i]);
            coffee_graphics_buffer_store_immutable(
                        &transforms[i],&rt,sizeof(CMat4),
                        GL_MAP_PERSISTENT_BIT|GL_MAP_COHERENT_BIT|GL_MAP_WRITE_BIT);
            coffee_graphics_buffer_map(
                        &transforms[i],
                        GL_MAP_PERSISTENT_BIT|GL_MAP_COHERENT_BIT|GL_MAP_WRITE_BIT);
        }

        CResources::CResource texture("ubw/models/textures/particle_fx.png");
        CResources::coffee_file_pull(&texture);

        CStbImageLib::CStbImage ptext;
        CStbImageLib::coffee_stb_image_load(&ptext,&texture);

        CTexture gltext;
        gltext.textureType = GL_TEXTURE_2D;
        gltext.format = GL_RGBA;
        coffee_graphics_alloc(&gltext);
        coffee_graphics_tex_activate(&gltext);
        CTextureTools::CTextureData gtexdata;
        gtexdata.data = ptext.data;
        gtexdata.datatype = GL_UNSIGNED_BYTE;
        CTextureTools::coffee_create_texturesize(&gtexdata,ptext.size.w,ptext.size.h);
        gtexdata.format = GL_RGBA8;

        CTextureTools::coffee_graphics_tex_define(&gltext,&gtexdata);
        CTextureTools::coffee_graphics_tex_store(&gltext,&gtexdata,0);

        coffee_graphics_tex_mipmap(&gltext);

        coffee_graphics_tex_get_handle(&gltext);
        coffee_graphics_tex_make_resident(&gltext);

        glProgramUniformHandleui64ARB(fragshader.handle,glGetUniformLocation(fragshader.handle,"diffsamp"),gltext.bhandle);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        int transform_index = 0;

        this->showWindow();
        while(!closeFlag())
        {
            glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

            for(int i=0;i<4;i++)
                coffee_graphics_vao_attribute_bind_buffer(
                            &vao,
                            mat_bnd[i],
                            &transforms[transform_index]);

            counter.update(clock->elapsed());
            glDrawElementsInstanced(GL_TRIANGLES,
                                    sizeof(indexdata)/sizeof(uint32),
                                    GL_UNSIGNED_INT,0,1);

            this->pollEvents();
            this->swapBuffers();
        }

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
        }
    }
};

typedef CVectors::_cbasic_tvector<scalar,3> VEC3;

int32 coffee_main(int32 argc, byte** argv)
{
    VEC3 t(0);
    t[0] = 1,t[1] = 2,t[2] = 3;
    VEC3 t2(0);
    t2[0] = 1,t2[1] = 2,t2[2] = 3;
    t += t2;
    t /= 2;
    t *= 3;

    t = CVectors::normalize(t);

    cDebug("Length of vec3: %f",CVectors::distance(t,t2));

    CDRendererBase *renderer = new CDHudRenderer();
    CDWindowProperties props = coffee_get_default_visual();
    props.contextProperties.flags |= CGLContextProperties::GLDebug;
    renderer->init(props);
    renderer->run();
    renderer->cleanup();
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
