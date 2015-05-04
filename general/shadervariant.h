#ifndef SHADERVARIANT_H
#define SHADERVARIANT_H

#include "general/common.h"
#include "general/mutabledatacontainer.h"
class ShaderVariant : public QObject
{
public:
    enum ShaderVariantType {
        ShaderVec2,ShaderVec3,ShaderVec4,
        ShaderMat2,ShaderMat3,ShaderMat4,

        ShaderDub
    };
    ShaderVariant(std::function<glm::vec2()> func);
    ShaderVariant(std::function<glm::vec3()> func);
    ShaderVariant(std::function<glm::vec4()> func);
    ShaderVariant(std::function<glm::mat3()> func);
    ShaderVariant(std::function<glm::mat4()> func);

    ShaderVariant(std::function<double()> func);

    ShaderVariantType getType();

    std::function<glm::vec2()> getVec2();
    std::function<glm::vec3()> getVec3();
    std::function<glm::vec4()> getVec4();

    std::function<glm::mat3()> getMat3();
    std::function<glm::mat4()> getMat4();

    //std::function for float return values are interpreted as int return values.
    //That is why we go for doubles in this case.
    std::function<double()> getDouble();

private:
    QObject* data;
    ShaderVariantType type;
};

#endif // SHADERVARIANT_H
