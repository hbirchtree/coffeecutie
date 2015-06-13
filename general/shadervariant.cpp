#include "shadervariant.h"

#include "general/data/mutabledatacontainer.h"

ShaderVariant::ShaderVariant(std::function<glm::vec2 ()> func)
{
    data = new MutableDataContainer<std::function<glm::vec2 ()>>(this,func);
    type = ShaderVec2;
}
ShaderVariant::ShaderVariant(std::function<glm::vec3 ()> func)
{
    data = new MutableDataContainer<std::function<glm::vec3 ()>>(this,func);
    type = ShaderVec3;
}
ShaderVariant::ShaderVariant(std::function<glm::vec4 ()> func)
{
    data = new MutableDataContainer<std::function<glm::vec4 ()>>(this,func);
    type = ShaderVec4;
}
ShaderVariant::ShaderVariant(std::function<glm::mat3 ()> func)
{
    data = new MutableDataContainer<std::function<glm::mat3 ()>>(this,func);
    type = ShaderMat3;
}
ShaderVariant::ShaderVariant(std::function<glm::mat4 ()> func)
{
    data = new MutableDataContainer<std::function<glm::mat4 ()>>(this,func);
    type = ShaderMat4;
}

ShaderVariant::~ShaderVariant()
{
    delete data;
}
ShaderVariant::ShaderVariant(std::function<float ()> func)
{
    data = new MutableDataContainer<std::function<float()>>(this,func);
    type = ShaderDub;
}

ShaderVariant::ShaderVariantType ShaderVariant::getType() const
{
    return type;
}

std::function<glm::vec2 ()> ShaderVariant::getVec2() const
{
    //Welcome to an object-oriented hell.
    if(type==ShaderVec2)
        return ((MutableDataContainer<std::function<glm::vec2 ()>>*)data)->getValue();
    return nullptr;
}
std::function<glm::vec3 ()> ShaderVariant::getVec3() const
{
    if(type==ShaderVec3)
        return ((MutableDataContainer<std::function<glm::vec3 ()>>*)data)->getValue();
    return nullptr;
}
std::function<glm::vec4 ()> ShaderVariant::getVec4() const
{
    if(type==ShaderVec4)
        return ((MutableDataContainer<std::function<glm::vec4 ()>>*)data)->getValue();
    return nullptr;
}
std::function<glm::mat3 ()> ShaderVariant::getMat3() const
{
    if(type==ShaderMat3)
        return ((MutableDataContainer<std::function<glm::mat3 ()>>*)data)->getValue();
    return nullptr;
}
std::function<glm::mat4 ()> ShaderVariant::getMat4() const
{
    if(type==ShaderMat4)
        return ((MutableDataContainer<std::function<glm::mat4 ()>>*)data)->getValue();
    return nullptr;
}
std::function<float ()> ShaderVariant::getDouble() const
{
    if(type==ShaderDub)
        return ((MutableDataContainer<std::function<double()>>*)data)->getValue();
    return nullptr;
}
