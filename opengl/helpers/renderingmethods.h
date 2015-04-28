#ifndef RENDERINGMETHODS_H
#define RENDERINGMETHODS_H

#include "general/common.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/helpers/vaohelper.h"
#include "opengl/components/coffeeworldopts.h"
class RenderingMethods
{

public:
    static void rendering_simple(RenderableObject *obj, QPointer<CoffeeWorldOpts> world);
    static void baking_simple(RenderableObject *obj);

    static glm::mat4 translateObjectMatrix(RenderableObject* obj);
};

#endif // RENDERINGMETHODS_H
