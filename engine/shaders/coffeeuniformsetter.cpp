#include "coffeeuniformsetter.h"

#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeeuniformblock.h"
#include "engine/scripting/qscriptvectorvalue.h"

void CoffeeUniformSetter::applyUniforms()
{
    for(ShaderMapping* m : uniforms){
        if(!m->value){
            m->value = m_shader->getUniformValue(m->uniform);
        }
        if(!m->value)
            continue; //This means it is not a valid uniform.
        m->value->setUniform(m->data->getVectorData(),m->data->getVectorDataSize());
    }
//        m_shader->setUniform(m->uniform,m->data);
}

void CoffeeUniformSetter::unbindTextures()
{
    for(TextureMapping *m : textures){
        glActiveTexture(GL_TEXTURE0+textures.indexOf(m));
        glBindTexture(m->texture->getGlTextureType(),0);
    }
}

void CoffeeUniformSetter::setUniform(const QString &uniformName, VectorData *data)
{
    ShaderMapping *map = new ShaderMapping;
    map->uniform = uniformName;
    map->data = data;
    uniforms.append(map);
}

void CoffeeUniformSetter::bindTextures()
{
    for(TextureMapping* m : textures){
        if(!m->loaded){
            m->texture->load();
            m->loaded = true;
        }
        int index = textures.indexOf(m);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(m->texture->getGlTextureType(),
                      m->texture->getHandle());
        m_shader->setUniform(m->samplerName,
                            index);
    }
}

void CoffeeUniformSetter::setTexture(const QString &samplerName, CoffeeTexture *texture)
{
    TextureMapping *map = new TextureMapping;
    map->samplerName = samplerName;
    map->texture = texture;
    textures.append(map);
}

void CoffeeUniformSetter::clearUniforms()
{
    for(ShaderMapping* d : uniforms)
        delete d;
    uniforms.clear();
}

void CoffeeUniformSetter::clearTextures()
{
    for(TextureMapping* t : textures)
        delete t;
    textures.clear();
}

const QVector<ShaderMapping *> CoffeeUniformSetter::getUniforms() const
{
    return uniforms;
}

const QVector<TextureMapping *> CoffeeUniformSetter::getTextures() const
{
    return textures;
}

CoffeeShader *CoffeeUniformSetter::_shader_obj()
{
    return m_shader;
}

CoffeeUniformSetter::CoffeeUniformSetter()
{
}

bool TextureMapping::operator==(const TextureMapping &val){
    return val.texture==this->texture.data()&&
            val.samplerName==this->samplerName;
}

void CoffeeUniformSetter::setShader(CoffeeShader *shader)
{
    if(m_shader)
        m_shader->removeConsumer();
    this->m_shader = shader;
    m_shader->addConsumer();
}
