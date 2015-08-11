#include "coffeeoutputsurface.h"

#include "opengl/components/shadercontainer.h"
#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeeframebufferobject.h"
#include "opengl/components/coffeecamera.h"
#include "engine/objects/coffeestandardobject.h"

CoffeeOutputSurface::CoffeeOutputSurface(QObject *parent,CoffeeFramebufferObject* display) :
    CoffeeObject(parent)
{
//    m_shader = new m_shaderContainer(this);
    setObjectName("screenSurface");
    setFramebuffer(display);
}

void CoffeeOutputSurface::setFramebuffer(CoffeeFramebufferObject *display)
{
    this->framebuffer = display;
}

void CoffeeOutputSurface::setFramebuffer(QObject *display)
{
    CoffeeFramebufferObject* fb = qobject_cast<CoffeeFramebufferObject*>(display);
    if(fb)
        setFramebuffer(display);
}

void CoffeeOutputSurface::load()
{
    if(m_shader){
        if(m_shader->isAllocated())
            m_shader->unload();
        m_shader->buildProgram();
    }
    if(surface){
        if(surface->isAllocated())
            surface->unload();
        surface->load();
    }
    setBaked(true);
}

void CoffeeOutputSurface::render()
{
    if(framebuffer->getRenderSize().isEmpty())
        return;

    if(!baked())
        load();

    QRect buffer;
    buffer.setSize(framebuffer->getRenderSize());

    if(!surface||!m_shader)
        return;

    glUseProgram(m_shader->getProgramId());

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    for(CoffeeOutputChannel* s : textures){
        glActiveTexture(GL_TEXTURE0+s->textureUnit);
        glBindTexture(GL_TEXTURE_2D,framebuffer->getTextureHandle()->at(s->textureIndex));
        m_shader->setUniform(s->uniformName,s->textureUnit);
    }

    applyUniforms();

    surface->renderMesh();

    for(CoffeeOutputChannel* s : textures){
        glActiveTexture(GL_TEXTURE0+s->textureUnit);
        glBindTexture(GL_TEXTURE_2D,0);
    }

    glUseProgram(0);

//    framebuffer->bindFramebufferRead();
//    glReadBuffer(GL_COLOR_ATTACHMENT0);
//    glBlitFramebuffer(0,0,
//                      buffer.width(),buffer.height(),
//                      0,0,
//                      screen.width(),screen.height(),
//                      GL_COLOR_BUFFER_BIT,GL_LINEAR);
}

void CoffeeOutputSurface::unload()
{
    if(m_shader)
        m_shader->unload();
    if(surface)
        surface->unload();
}

void CoffeeOutputSurface::resize(const QSize &size)
{
    framebuffer->setNumTextures(textures.size());
    framebuffer->resize(size);
}

void CoffeeOutputSurface::setShader(QObject *m_shader)
{
    CoffeeShader* msh = qobject_cast<CoffeeShader*>(m_shader);
    if(msh){
        this->m_shader = msh;
        setBaked(false);
    }
}

QObject *CoffeeOutputSurface::framebufferQObject()
{
    return framebuffer;
}

CoffeeFramebufferObject *CoffeeOutputSurface::getFramebuffer()
{
    return framebuffer;
}

QObject *CoffeeOutputSurface::getShader()
{
    return m_shader;
}

void CoffeeOutputSurface::addUiTexture(CoffeeTexture *tex)
{
    textureTest = tex;
}

void CoffeeOutputSurface::setUniform(const QString &uniformName, QObject *data)
{
    VectorData* d = qobject_cast<VectorData*>(data);
    if(d)
        CoffeeUniformSetter::setUniform(uniformName,d);
}

void CoffeeOutputSurface::setTexture(const QString &samplerName, QObject *texture)
{
    CoffeeTexture* d = qobject_cast<CoffeeTexture*>(texture);
    if(d)
        CoffeeUniformSetter::setTexture(samplerName,d);
}

void CoffeeOutputSurface::setFramebufferMapping(const QString &uniformName, int textureIndex, int textureUnit)
{
    CoffeeOutputChannel* toDelete = nullptr;
    for(CoffeeOutputChannel* c : textures)
        if(c->textureUnit == textureUnit)
            toDelete = c;
    if(toDelete){
        textures.removeOne(toDelete);
        delete toDelete;
    }
    textures.append(new CoffeeOutputChannel(uniformName,textureIndex,textureUnit));
}

void CoffeeOutputSurface::setMesh(CoffeeMesh *mesh)
{
    this->setBaked(false);
    this->surface = mesh;
}

void CoffeeOutputSurface::bind()
{
    framebuffer->bindFramebuffer();
}

CoffeeOutputChannel::CoffeeOutputChannel(const QString &uniform, int textureIndex, int textureUnit)
{
    this->uniformName = uniform;
    this->textureIndex = textureIndex;
    this->textureUnit = textureUnit;
}
