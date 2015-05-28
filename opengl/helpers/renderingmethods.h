#ifndef RENDERINGMETHODS_H
#define RENDERINGMETHODS_H

#include "general/common.h"
#include "opengl/helpers/vaohelper.h"
#include "opengl/components/coffeeworldopts.h"
class RenderingMethods
{

public:
//    static void rendering_simple(RenderableObject *obj, QPointer<CoffeeWorldOpts> world);
//    static void baking_simple(RenderableObject *obj);

    static glm::mat4 translateObjectMatrix(const glm::vec3& translation, const glm::quat& orientation, const glm::vec3& scale);
//    static void rendering_advanced(RenderableObject *obj, QPointer<CoffeeWorldOpts> world);
//    static void baking_advanced(RenderableObject *obj);
};

#endif // RENDERINGMETHODS_H
