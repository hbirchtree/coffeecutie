#include "renderingmethods.h"

glm::mat4 RenderingMethods::translateObjectMatrix(const glm::vec3& translation,
                                                  const glm::quat& orientation,
                                                  const glm::vec3& scale)
{
    glm::mat4 modelMatrix;
    modelMatrix *= glm::translate(glm::mat4(),translation);
    modelMatrix *= glm::mat4_cast(glm::normalize(orientation));
    modelMatrix *= glm::scale(glm::mat4(),scale);
    return modelMatrix;
}

bool RenderingMethods::checkGLError(const QString &status)
{
    GLenum err = glGetError();
    if(err != GL_NO_ERROR){
        qDebug("OpenGL error code detected: %s (%s)",
               glbinding::Meta::getString(err).c_str(),
               status.toStdString().c_str());
        return false;
    }
    return true;
}
