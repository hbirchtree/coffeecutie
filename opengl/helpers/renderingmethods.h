#ifndef RENDERINGMETHODS_H
#define RENDERINGMETHODS_H

#include "general/common.h"
class RenderingMethods
{

public:
    static glm::mat4 translateObjectMatrix(const glm::vec3& translation, const glm::quat& orientation, const glm::vec3& scale);

    static bool checkGLError(const QString &status = QString());
};

#endif // RENDERINGMETHODS_H
