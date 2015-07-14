#include "coffeeuniformblock.h"

#include "opengl/components/coffeebuffer.h"

CoffeeUniformBlock::CoffeeUniformBlock(QObject *parent, uint64_t bufferSize) : QObject(parent)
{
    m_buffer = new CoffeeBuffer(this,GL_DYNAMIC_STORAGE_BIT,GL_UNIFORM_BUFFER);
    m_buffer->setSize(bufferSize);
}

uint16_t CoffeeUniformBlock::systemFloatSize()
{
    return sizeof(float);
}

uint16_t CoffeeUniformBlock::systemVec3Size()
{
    return sizeof(glm::vec3);
}

uint16_t CoffeeUniformBlock::systemVec4Size()
{
    return sizeof(glm::vec4);
}

uint16_t CoffeeUniformBlock::systemVec2Size()
{
    return sizeof(glm::vec2);
}

uint16_t CoffeeUniformBlock::systemMat4Size()
{
    return sizeof(glm::mat4);
}

uint16_t CoffeeUniformBlock::systemMat3Size()
{
    return sizeof(glm::mat3);
}

uint16_t CoffeeUniformBlock::systemUintSize()
{
    return sizeof(uint);
}

QString CoffeeUniformBlock::name() const
{
    return m_name;
}

void CoffeeUniformBlock::addUniform(CoffeeUniformValue *val)
{
    if(!m_uniforms.contains(val))
        m_uniforms.append(val);
    else
        qWarning("Uniform already exists!");
}

void CoffeeUniformBlock::setName(const QString &name)
{
    m_name = name;
}
