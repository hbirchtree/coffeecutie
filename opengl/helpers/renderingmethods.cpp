#include "renderingmethods.h"

glm::mat4 RenderingMethods::translateObjectMatrix(const glm::vec3& translation,
                                                  const glm::quat& orientation,
                                                  const glm::vec3& scale)
{
    glm::mat4 modelMatrix;
    modelMatrix *= glm::translate(glm::mat4(),translation);
    modelMatrix *= glm::mat4(glm::normalize(orientation));
    modelMatrix *= glm::scale(glm::mat4(),scale);
    return modelMatrix;
}
