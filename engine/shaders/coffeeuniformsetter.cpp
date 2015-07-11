#include "coffeeuniformsetter.h"

#include "opengl/components/shadercontainer.h"
#include "general/shadervariant.h"
#include "opengl/components/coffeetexture.h"

void CoffeeUniformSetter::applyUniforms()
{
    for(ShaderMapping* m : uniforms){
        pshader->setUniform(m->uniform,m->data);
    }
}

void CoffeeUniformSetter::unbindTextures()
{
    for(TextureMapping *m : textures){
        glActiveTexture(GL_TEXTURE0+textures.indexOf(m));
        glBindTexture(m->texture->getGlTextureType(),0);
    }
}

void CoffeeUniformSetter::bindTextures()
{
    for(TextureMapping* m : textures){
        if(!m->loaded){
            m->texture->loadTexture();
            m->loaded = true;
        }
        int index = textures.indexOf(m);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(m->texture->getGlTextureType(),
                      m->texture->getHandle());
        pshader->setUniform(m->samplerName,
                            index);
    }
}

void CoffeeUniformSetter::setUniform(QString uniformName, ShaderVariant *data)
{
    ShaderMapping *map = new ShaderMapping;
    map->uniform = uniformName;
    map->data = data;
    uniforms.append(map);
}

void CoffeeUniformSetter::setTexture(QString samplerName, CoffeeTexture *texture)
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

CoffeeUniformSetter::CoffeeUniformSetter()
{
}

bool TextureMapping::operator==(const TextureMapping &val){
    return val.texture==this->texture.data()&&
            val.samplerName==this->samplerName;
}
