#include "coffeerendergraph.h"

/* Goal:
 *  - Set up shaders, objects, buffers, uniforms and textures for rendering
 *  - Put shaders in uniform buffers such that we may update uniforms only when they change
 *  - Only set uniforms which exist by checking the shader. If it fails once, never try again.
 *  - Avoid storing too much information about state
 */

#include "engine/objects/coffeeobject.h"
#include "opengl/context/coffeerenderer.h"
#include "opengl/components/coffeeframebufferobject.h"

CoffeeRenderGraph::CoffeeRenderGraph(QObject *parent) : QObject(parent)
{
}

bool CoffeeRenderGraph::continuousRender() const
{
    return m_continuousRender;
}

void CoffeeRenderGraph::setRenderTarget(CoffeeFramebufferBaseClass *fb)
{
    m_renderTarget = fb;
}

void CoffeeRenderGraph::setRenderer(CoffeeRenderer *renderer)
{
    if(m_renderer_connected)
        return;
    m_renderConnections.append(connect(this,
                              SIGNAL(submitRenderCall(std::function<void()>)),
                              renderer,
                              SLOT(queueFunction(std::function<void()>))));

    //Set up the rendering surface, mainly resizing its framebuffer to the whole viewport
    std::function<void()> func = [=](){
        m_renderSurface->getFramebuffer()->createFramebuffer(renderer->getCurrentFramebufferSize(),1);
    };
    submitRenderCall(func);
    //It also needs to react to resizing
    m_renderConnections.
            append(connect(renderer,
                           &CoffeeRenderer::winFrameBufferResize,
                           [=](QResizeEvent e){
        std::function<void()> func = [=](){
            m_renderSurface->resize(e.size());
        };
        submitRenderCall(func);
    }));

    m_renderer_connected = true;
}

void CoffeeRenderGraph::clearRenderer()
{
    if(!m_renderer_connected)
        return;
    for(QMetaObject::Connection m_renderConnect : m_renderConnections)
        disconnect(m_renderConnect);
    m_renderConnections.clear();
    m_renderer_connected = false;
}

void CoffeeRenderGraph::queueRender()
{
    std::function<void()> func = [=](){
        m_renderTarget->bindFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        for(CoffeeRenderGroup* _grp : m_renderGroups.values())
            for(CoffeeObject* o : _grp->m_objects)
                o->render();

        m_renderTarget->unbindFramebuffer();
        m_renderSurface->render();
    };
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
}
void CoffeeRenderGraph::setRenderSurface(CoffeeOutputSurface *renderSurface)
{
    m_renderSurface = renderSurface;
}

void CoffeeRenderGraph::setContinuousRender(bool continuousRender)
{
    m_continuousRender = continuousRender;
}

