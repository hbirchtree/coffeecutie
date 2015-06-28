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

    QRect buffer;
    buffer.setSize(framebuffer->getRenderSize());

    QRect screen;
    screen.setSize(framebuffer->getWindowSize());

    framebuffer->bindFramebufferRead();

    glReadBuffer(GL_COLOR_ATTACHMENT1);
//    glBlitFramebuffer(0,0,
//                      buffer.center().x(),buffer.center().y(),
//                      0,0,
//                      screen.center().x(),screen.center().y(),
//                      GL_COLOR_BUFFER_BIT,GL_LINEAR);
    glBlitFramebuffer(0,0,
                      buffer.width(),buffer.height(),
                      0,0,
                      screen.width(),screen.height(),
                      GL_COLOR_BUFFER_BIT,GL_LINEAR);

//    glReadBuffer(GL_COLOR_ATTACHMENT1);
//    glBlitFramebuffer(buffer.center().x(),0,
//                      buffer.width(),buffer.center().y(),
//                      screen.center().x(),0,
//                      screen.width(),screen.center().y(),
//                      GL_COLOR_BUFFER_BIT,GL_LINEAR);

//    glReadBuffer(GL_COLOR_ATTACHMENT2);
//    glBlitFramebuffer(0,buffer.center().y(),
//                      buffer.center().x(),buffer.height(),
//                      0,screen.center().y(),
//                      screen.center().x(),screen.height(),
//                      GL_COLOR_BUFFER_BIT,GL_LINEAR);

//    glReadBuffer(GL_COLOR_ATTACHMENT4);
//    glBlitFramebuffer(buffer.center().x(),buffer.center().y(),
//                      buffer.width(),buffer.height(),
//                      screen.center().x(),screen.center().y(),
//                      screen.width(),screen.height(),
//                      GL_COLOR_BUFFER_BIT,GL_LINEAR);
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
