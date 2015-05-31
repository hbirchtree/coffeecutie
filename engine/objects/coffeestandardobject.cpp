#include "coffeestandardobject.h"

CoffeeStandardObject::CoffeeStandardObject(QObject *parent) : CoffeeObject(parent)
{
}

void CoffeeStandardObject::render()
{
    if(!baked)
        load();
    glUseProgram(pshader->getProgramId());
    for(ShaderMapping m : uniforms)
        if(!m.constant)
            pshader->setUniform(m.uniform,m.data);
    for(TextureMapping m : textures){
        //Might want to secure this in the future
        glActiveTexture(static_cast<GLenum>(GL_TEXTURE0+textures.indexOf(m)));
        glBindTexture(GL_TEXTURE_2D,
                      m.texture->getHandle());
//        pshader->setUniform(m.samplerName,
//                            static_cast<GLint>(m.unit)+textures.indexOf(m)-static_cast<GLint>(GL_TEXTURE0));
    }

    glBindVertexArray(pmesh->getVertexArrayHandle());
    glDrawElements(GL_TRIANGLES,
                   pmesh->getIndicesCount(),
                   GL_UNSIGNED_INT,
                   (GLvoid*)0);

    for(TextureMapping m : textures){
        glActiveTexture(GL_TEXTURE0+textures.indexOf(m));
        glBindTexture(GL_TEXTURE_2D,0);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void CoffeeStandardObject::unload()
{
    if(pmesh)
        pmesh->unloadMesh();
    for(TextureMapping m : textures)
        m.texture->unloadTexture();
}

void CoffeeStandardObject::load()
{
    if(pmesh)
        pmesh->loadMesh();
    for(ShaderMapping m : uniforms){
        pshader->getUniformLocation(m.uniform);
        if(m.constant)
            pshader->setUniform(m.uniform,m.data);
    }
    for(TextureMapping m : textures){
        pshader->getUniformLocation(m.samplerName);
        pshader->setUniform(m.samplerName,
                            static_cast<GLint>(m.unit)+textures.indexOf(m)-static_cast<GLint>(GL_TEXTURE0));
        m.texture->loadTexture();
    }
    baked = true;
}

QString CoffeeStandardObject::getStringPosition() const{
    return QStringFunctions::toString(vposition->getValue());
}

QString CoffeeStandardObject::getStringRotation() const{
    return QStringFunctions::toString(qrotation->getValue());
}

QString CoffeeStandardObject::getStringScale() const{
    return QStringFunctions::toString(vscale->getValue());
}

CoffeeMesh *CoffeeStandardObject::mesh()
{
    return pmesh;
}

void CoffeeStandardObject::setMesh(CoffeeMesh *mesh)
{
    this->pmesh = mesh;
}

ShaderContainer *CoffeeStandardObject::shader()
{
    return pshader;
}

void CoffeeStandardObject::setShader(ShaderContainer *shader)
{
    this->pshader = shader;
}

CoffeeMaterial *CoffeeStandardObject::material()
{
    return pmaterial;
}

void CoffeeStandardObject::setMaterial(CoffeeMaterial *mtl)
{
    this->pmaterial = mtl;
}

void CoffeeStandardObject::setUniform(QString uniformName, ShaderVariant* data, bool constant)
{
    ShaderMapping map;
    map.constant = constant;
    map.uniform = uniformName;
    map.data = data;
    uniforms.append(map);
}

void CoffeeStandardObject::setTexture(QString samplerName, CoffeeTexture* texture, GLenum unit)
{
    TextureMapping map;
    map.samplerName = samplerName;
    map.texture = texture;
    map.unit = unit;
    textures.append(map);
}

