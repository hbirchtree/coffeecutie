#include "coffeerendergraph.h"

/* Goal:
 *  - Set up shaders, objects, buffers, uniforms and textures for rendering
 *  - Put shaders in uniform buffers such that we may update uniforms only when they change
 *  - Only set uniforms which exist by checking the shader. If it fails once, never try again.
 *  - Avoid storing too much information about state
 */

#include "engine/objects/coffeeobject.h"
#include "opengl/context/coffeerenderer.h"

CoffeeRenderGraph::CoffeeRenderGraph(QObject *parent) : QObject(parent)
{
}

void CoffeeRenderGraph::setRenderer(CoffeeRenderer *renderer)
{
    if(m_renderer_connected)
        return;
    m_renderConnect = connect(this,
                              SIGNAL(submitRenderCall(std::function<void()>*)),
                              renderer,
                              SLOT(queueFunction(std::function<void()>*)));
    m_renderer_connected = true;
}

void CoffeeRenderGraph::clearRenderer()
{
    if(!m_renderer_connected)
        return;
    disconnect(m_renderConnect);
    m_renderer_connected = false;
}

void CoffeeRenderGraph::queueRender()
{
    std::function<void()> *func = new std::function<void()>([=](){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        qDebug() << "Rendering";
        for(CoffeeRenderGroup* _grp : m_renderGroups)
            for(CoffeeObject* o : _grp->m_objects)
                o->render();
    });
    submitRenderCall(func);
}

void CoffeeRenderGraph::includeObject(CoffeeObject *obj)
{
    if(m_renderGroups.contains(obj->_shader_obj())){
        m_renderGroups.value(obj->_shader_obj())->m_objects.append(obj);
    }else{
        CoffeeRenderGroup* _grp = new CoffeeRenderGroup;
        _grp->m_objects.append(obj);
        _grp->m_shader = obj->_shader_obj();
        m_renderGroups.insert(obj->_shader_obj(),_grp);
    }

    qDebug() << m_renderGroups;
}
