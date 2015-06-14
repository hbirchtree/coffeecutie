#ifndef SHADERVARIANT_H
#define SHADERVARIANT_H

#include "general/common.h"
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
    ShaderVariant(std::function<float()> func);

    ~ShaderVariant();


    ShaderVariantType getType() const;

    std::function<glm::vec2()> *getVec2() const;
    std::function<glm::vec3 ()> *getVec3() const;
    std::function<glm::vec4()> *getVec4() const;

    std::function<glm::mat3()> *getMat3() const;
    std::function<glm::mat4()> *getMat4() const;

    //std::function for float return values are interpreted as int return values.
    //That is why we go for doubles in this case.
    std::function<float ()> *getDouble() const;

private:
    QObject* data;
    ShaderVariantType type;
};

#endif // SHADERVARIANT_H
