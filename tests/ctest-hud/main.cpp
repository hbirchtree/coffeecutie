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
        };

        CBuffer vertices;
        CBuffer indices;
        vertices.bufferType = GL_ARRAY_BUFFER;
        indices.bufferType = GL_ELEMENT_ARRAY_BUFFER;
        coffee_graphics_alloc(&vertices);
        coffee_graphics_alloc(&indices);

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
        coffee_graphics_activate(&vao);

        CVertexBufferBinding vrt_bind;
        vrt_bind.buffer = &vertices;
        vrt_bind.stride = sizeof(CVec3);

        CVertexBufferBinding ind_bind;
        ind_bind.buffer = &indices;
        ind_bind.binding = 1;
        vrt_bind.stride = sizeof(uint32);

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

        coffee_graphics_bind(&vao);
        coffee_graphics_bind(&indices);

        CTransform root;
        root.position = CMath::vec3(1,2,3);
        root.scale = CMath::vec3(1,1,1);
        coffee_graphics_gen_matrix(&root);

        CGCamera camera;
        camera.aspect = 1.6f;

        coffee_graphics_gen_matrix_perspective(&camera);

        CNode worldNode;
        worldNode.transform = (CMat4*)&camera.matrix;

        CNode rootNode;
        rootNode.parent = &worldNode;
        rootNode.transform = (CMat4*)&root.matrix;

        CMat4 wt = coffee_node_get_transform(&worldNode);
        CMat4 rt = coffee_node_get_transform(&rootNode);

        this->showWindow();
        while(!closeFlag())
        {
//            glClearColor(CMath::cfrand(0,1),CMath::cfrand(0,1),CMath::cfrand(0,1),1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            counter.update(clock.elapsed());
            glDrawElements(GL_TRIANGLES,1,GL_UNSIGNED_INT,0);

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
