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
    return !(report->severity==GL_DEBUG_SEVERITY_NOTIFICATION);
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
        CElapsedTimerMicro clock;
        clock.start();

        const scalar vertexdata[] = {
            -1.f,  1.f, 1.f,
            -1.f, -1.f, 1.f,
             1.f,  1.f, 1.f,
             1.f, -1.f, 1.f,
        };

        const uint32 indexdata[] = {
            0, 1, 2,
            2, 1, 3
        };

        const byte vshader_src[] = {
            "#version 330\n"
            "layout(location = 0) in vec3 position;"
            "layout(location = 1) in mat4 transform;"
            "out gl_PerVertex {"
            "   vec4 gl_Position;"
            "};"
            "void main(){"
            "   gl_Position = transform * vec4(position,1.0);"
            "}"
        };

        const byte fshader_src[] = {
            "#version 330\n"
            "layout(location = 0) out vec4 Out_color;"
            "void main(){"
            "   Out_color = vec4(1.0,0.0,0.0,1.0);"
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
        CBuffer indices;
        CBuffer transforms[3];
        vertices.bufferType = GL_ARRAY_BUFFER;
        indices.bufferType = GL_ELEMENT_ARRAY_BUFFER;
        coffee_graphics_alloc(&vertices);
        coffee_graphics_alloc(&indices);
        coffee_graphics_alloc(3,GL_ARRAY_BUFFER,(CBuffer*)transforms);

        coffee_graphics_activate(&vertices);
        coffee_graphics_activate(&indices);

        coffee_graphics_buffer_store(&vertices,
                                     vertexdata,
                                     sizeof(vertexdata),
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

        CVertexFormat vrt_fmt;
        vrt_fmt.normalized = GL_FALSE;
        vrt_fmt.offset = 0;
        vrt_fmt.size = 3;
        vrt_fmt.type = GL_FLOAT;

        CVertexAttribute vrt_att;
        vrt_att.attribIdx = 0;
        vrt_att.bnd = &vrt_bind;
        vrt_att.fmt = &vrt_fmt;

        coffee_graphics_vao_attribute_format(&vao,vrt_att,vrt_fmt);
        coffee_graphics_vao_attribute_buffer(&vao,vrt_att,vrt_bind);
        coffee_graphics_vao_attribute_bind_buffer(&vao,vrt_bind);

        CVertexFormat mat_fmt;
        mat_fmt.offset = 0;
        mat_fmt.size = 4;
        mat_fmt.type = GL_FLOAT;

        CVertexBufferBinding mat_bnd[4];

        for(int i=0;i<4;i++)
        {
            CVertexBufferBinding* bnd = &mat_bnd[i];
            bnd->binding = 1+i;
            bnd->buffer = &transforms[0];
            bnd->offset = sizeof(CVec4)*i;
            bnd->stride = sizeof(CMat4);
            bnd->divisor = 1;
            CVertexAttribute attr;
            attr.attribIdx = 1+i;
            attr.bnd = bnd;
            attr.fmt = &mat_fmt;
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
                        GL_NONE_BIT);
//            coffee_graphics_buffer_map(
//                        &transforms[i],
//                        GL_MAP_PERSISTENT_BIT|GL_MAP_COHERENT_BIT|GL_MAP_WRITE_BIT);
        }

//        glProgramUniformMatrix4fv(
//                    vertshader.handle,
//                    glGetUniformLocation(vertshader.handle,"transform"),
//                    1,GL_FALSE,(scalar*)&rt.m);

        int transform_index = 0;

        this->showWindow();
        while(!closeFlag())
        {
            glClear(GL_COLOR_BUFFER_BIT);

            for(int i=0;i<4;i++)
                coffee_graphics_vao_attribute_bind_buffer(
                            &vao,
                            mat_bnd[i],
                            &transforms[transform_index]);

            counter.update(clock.elapsed());
            glDrawElementsInstanced(GL_TRIANGLES,sizeof(indexdata)/sizeof(uint32),GL_UNSIGNED_INT,0,1);

            this->pollEvents();
            this->swapBuffers();
        }

        coffee_graphics_free(&vao);
        coffee_graphics_free(&vertices);
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

int32 coffee_main(int32 argc, byte** argv)
{
    CVec3 tvec(4,5,6);
    CMath::vec3 gvec(1,2,3);
    gvec = tvec;
    cDebug("Vector: %f",gvec.x);

    CDRendererBase *renderer = new CDHudRenderer();
    CDWindowProperties props = coffee_get_default_visual();
    props.contextProperties.flags |= CGLContextProperties::GLDebug;
    renderer->init(props);
    renderer->run();
    renderer->cleanup();
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
