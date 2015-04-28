#ifndef RENDERINGMETHODS_H
#define RENDERINGMETHODS_H

#include "general/common.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/helpers/vaohelper.h"
class RenderingMethods
{

public:
    static void rendering_simple(QPointer<RenderableObject> obj);
    static void baking_simple(QPointer<RenderableObject> obj);
};

#endif // RENDERINGMETHODS_H
