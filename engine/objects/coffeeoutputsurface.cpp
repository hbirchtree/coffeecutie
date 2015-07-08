#include "coffeeoutputsurface.h"

#include "opengl/components/shadercontainer.h"
#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeeframebufferobject.h"
#include "opengl/components/coffeeworldopts.h"
#include "opengl/components/coffeecamera.h"
#include "general/shadervariant.h"
#include "engine/objects/coffeestandardobject.h"

CoffeeOutputSurface::CoffeeOutputSurface(QObject *parent,CoffeeFrameBufferObject* display) :
    QObject(parent),
    CoffeeObject(this)
{
    shader = new ShaderContainer(this);
    setObjectName("screenSurface");
    setFramebuffer(display);
}

void CoffeeOutputSurface::setFramebuffer(CoffeeFrameBufferObject *display)
{
    this->framebuffer = display;
}

void CoffeeOutputSurface::setFramebuffer(QObject *display)
{
    CoffeeFrameBufferObject* fb = qobject_cast<CoffeeFrameBufferObject*>(display);
    if(fb)
        setFramebuffer(display);
}

void CoffeeOutputSurface::load()
{
    if(!shader->isAllocated())
        shader->buildProgram(new CoffeeResource(0,"shaders/deferred/vsh.vs"),
                             new CoffeeResource(0,"shaders/deferred/fsh.fs"));
    if(surface)
        surface->loadMesh();
    setBaked(true);
}

void CoffeeOutputSurface::render()
{
    framebuffer->unbindFramebuffer();

    if(!isBaked())
        load();

    QRect buffer;
    buffer.setSize(framebuffer->getRenderSize());

    QRect screen;
    screen.setSize(framebuffer->getWindowSize());

    if(!surface)
        return;

    glUseProgram(shader->getProgramId());

    glClear(GL_DEPTH_BUFFER_BIT);
    for(CoffeeOutputChannel* s : textures){
        glActiveTexture(GL_TEXTURE0+s->textureUnit);
        glBindTexture(GL_TEXTURE_2D,framebuffer->getTextureHandle()->at(s->textureIndex));
        shader->setUniform(s->uniformName,s->textureUnit);
    }

    for(ShaderMapping* m : uniforms){
        shader->setUniform(m->uniform,m->data);
    }

    glBindVertexArray(surface->getVertexArrayHandle());

    glDrawElements(GL_TRIANGLES,surface->getIndicesCount(),GL_UNSIGNED_INT,0);

    for(CoffeeOutputChannel* s : textures){
        glActiveTexture(GL_TEXTURE0+s->textureUnit);
        glBindTexture(GL_TEXTURE_2D,0);
    }

    glBindVertexArray(0);
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
}

void CoffeeOutputSurface::setShader(QObject *shader)
{
    ShaderContainer* msh = qobject_cast<ShaderContainer*>(shader);
    if(msh)
        this->shader = msh;
}

bool CoffeeOutputSurface::isBaked() const
{
    return baked;
}

QObject *CoffeeOutputSurface::framebufferQObject()
{
    return framebuffer;
}

CoffeeFrameBufferObject *CoffeeOutputSurface::getFramebuffer()
{
    return framebuffer;
}

QObject *CoffeeOutputSurface::getShader()
{
    return shader;
}

void CoffeeOutputSurface::setUniform(const QString &uniformName, ShaderVariant *data)
{
    ShaderMapping *map = new ShaderMapping;
    map->uniform = uniformName;
    map->data = data;
    uniforms.append(map);
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
    this->baked = false;
    this->surface = mesh;
}

void CoffeeOutputSurface::bind()
{
    framebuffer->bindFramebuffer();
}

void CoffeeOutputSurface::setBaked(bool val)
{
    this->baked = val;
}
void CoffeeOutputSurface::setWorld(CoffeeWorldOpts *value)
{
    world = value;
}

CoffeeOutputChannel::CoffeeOutputChannel(const QString &uniform, int textureIndex, int textureUnit)
{
    this->uniformName = uniform;
    this->textureIndex = textureIndex;
    this->textureUnit = textureUnit;
}
