#ifndef COFFEEDEPTHBUFFEROBJECT_H
#define COFFEEDEPTHBUFFEROBJECT_H

#include "coffeeframebufferobject.h"
class CoffeeDepthBufferObject : public CoffeeFrameBufferObject
{
public:
    CoffeeDepthBufferObject(QObject *parent);

protected:
    void resizeFramebuffer();
};

#endif // COFFEEDEPTHBUFFEROBJECT_H
