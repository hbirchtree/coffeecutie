#include "coffeeoutputsurface.h"

#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeeframebufferobject.h"

CoffeeOutputSurface::CoffeeOutputSurface(QObject *parent,CoffeeFrameBufferObject* display) :
    QObject(parent),
    CoffeeObject(this)
{
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
    setBaked(true);
}

void CoffeeOutputSurface::render()
{
    framebuffer->unbindFramebuffer();

    if(!isBaked())
        load();

    framebuffer->bindFramebufferRead();
    glBlitFramebuffer(0,0,
                      framebuffer->getRenderSize().width(),framebuffer->getRenderSize().height(),

                      0,0,
                      framebuffer->getWindowSize().width(),framebuffer->getWindowSize().height(),

                      GL_COLOR_BUFFER_BIT,GL_LINEAR);

}

void CoffeeOutputSurface::unload()
{
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

void CoffeeOutputSurface::bind()
{
    framebuffer->bindFramebuffer();
}

void CoffeeOutputSurface::setBaked(bool val)
{
    this->baked = val;
}
