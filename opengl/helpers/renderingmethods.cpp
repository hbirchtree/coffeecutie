#include "renderingmethods.h"

void RenderingMethods::rendering_simple(RenderableObject* obj, QPointer<CoffeeWorldOpts> world)
{
    if(!obj->isDrawn())
        return;
    if(!obj->isBaked()){
        baking_simple(obj);
    }

    if(!obj->isDepthTest())
        glDisable(GL_DEPTH_TEST);

    glUseProgram(obj->getShader()->getProgramId());
    QPointer<ShaderContainer> shader = obj->getShader();

    if(!obj->isStreamDraw()){
        //Animate skeleton here
    }

    shader->setUniform("camera",world->getCamera()->getProjection());
    shader->setUniform("cameraPosition",world->getCamera()->getPosition()->getValue());
    shader->setUniform("model",translateObjectMatrix(obj));

    for(QPointer<CoffeeOmniLight> light : world->getLights()){
        shader->setUniform("light.position",light->getPosition()->getValue());
        shader->setUniform("light.intensities",light->getColor()->getValue());
        shader->setUniform("light.attenuation",light->getAttenuation()->getValue());
        shader->setUniform("light.ambientCoefficient",light->getAmbientCoefficient()->getValue());
    }

    shader->setUniform("materialTex",0);
    shader->setUniform("materialShininess", obj->getMaterial()->shininess()->getValue());
    shader->setUniform("materialSpecularColor", obj->getMaterial()->specularColor());
    shader->setUniform("materialTransparencyValue", obj->getMaterial()->transparency()->getValue());
//    shader->setUniform("colorMul", obj->getColorMultiplier());

    shader->setUniform("fogParams.fDensity", world->getFogDensity());
    shader->setUniformRgba("fogParams.fColor", world->getFogColor());

    glBindVertexArray(obj->getVaoHandle());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,obj->getMaterial()->getTexture(CoffeeTexture::Texture_Diffusion)->getHandle());

    glDrawArrays(GL_TRIANGLES,0,obj->getVerticesCount());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);
    glUseProgram(0);

    if(!obj->isDepthTest())
        glEnable(GL_DEPTH_TEST);
}

void RenderingMethods::baking_simple(RenderableObject* obj)
{
//    qDebug() << obj->getVerticesCount();
    QPointer<CoffeeTexture> texture = obj->getMaterial()->getTexture(CoffeeTexture::Texture_Diffusion);
    texture->loadTexture();
    VAOHelper::genVAO(obj,
                      obj->getShader()->getAttributeLocation("vert"),
                      obj->getShader()->getAttributeLocation("vertTexCoord"),
                      obj->getShader()->getAttributeLocation("vertNormal"),
                      obj->getShader()->getAttributeLocation("vertTangent"));
    obj->getShader()->getUniformLocations(QList<QString> {
                                              "materialTex",
                                              "camera",
                                              "cameraPosition",
                                              "model",
                                              "fogParams.fDensity",
                                              "fogParams.fColor",
                                              "materialShininess",
                                              "materialTransparencyValue",
                                              "materialSpecularColor",
//                                              "colorMul",
                                              "light.position",
                                              "light.attenuation",
                                              "light.ambientCoefficient",
                                              "light.intensities"
                                          });
    obj->setBaked(true);
}

void RenderingMethods::rendering_advanced(RenderableObject* obj, QPointer<CoffeeWorldOpts> world)
{
    if(!obj->isDrawn())
        return;
    if(!obj->isBaked()){
        baking_advanced(obj);
    }

    if(!obj->isDepthTest())
        glDisable(GL_DEPTH_TEST);

    glUseProgram(obj->getShader()->getProgramId());
    QPointer<ShaderContainer> shader = obj->getShader();

    if(!obj->isStreamDraw()){
        //Animate skeleton here
    }

    shader->setUniform("camera",world->getCamera()->getProjection());
    shader->setUniform("cameraPosition",world->getCamera()->getPosition()->getValue());
    shader->setUniform("model",translateObjectMatrix(obj));

    for(QPointer<CoffeeOmniLight> light : world->getLights()){
        shader->setUniform("light.position",light->getPosition()->getValue());
        shader->setUniform("light.intensities",light->getColor()->getValue());
        shader->setUniform("light.attenuation",light->getAttenuation()->getValue());
        shader->setUniform("light.ambientCoefficient",light->getAmbientCoefficient()->getValue());
    }

    shader->setUniform("materialTex",0);
    shader->setUniform("materialBump", 1);
    shader->setUniform("materialSpecular", 2);
    shader->setUniform("materialHighlight", 3);
    shader->setUniform("materialTransparency", 4);
    shader->setUniform("materialShininess", obj->getMaterial()->shininess()->getValue());
    shader->setUniform("materialSpecularColor", obj->getMaterial()->specularColor());
    shader->setUniform("materialTransparencyValue", obj->getMaterial()->transparency()->getValue());
    shader->setUniform("colorMul", obj->getMaterial()->colorMultiplier());

    shader->setUniform("fogParams.fDensity", world->getFogDensity());
    shader->setUniformRgba("fogParams.fColor", world->getFogColor());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,obj->getMaterial()->getTexture(CoffeeTexture::Texture_Diffusion)->getHandle());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,obj->getMaterial()->getTexture(CoffeeTexture::Texture_Bumpmap)->getHandle());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,obj->getMaterial()->getTexture(CoffeeTexture::Texture_Specular)->getHandle());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,obj->getMaterial()->getTexture(CoffeeTexture::Texture_Highlight)->getHandle());
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,obj->getMaterial()->getTexture(CoffeeTexture::Texture_Transparency)->getHandle());

    glBindVertexArray(obj->getVaoHandle());
    glDrawArrays(GL_TRIANGLES,0,obj->getVerticesCount());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,0);
    glUseProgram(0);

    if(!obj->isDepthTest())
        glEnable(GL_DEPTH_TEST);
}

void RenderingMethods::baking_advanced(RenderableObject* obj)
{
    for(int key : obj->getMaterial()->getTextureKeys())
        obj->getMaterial()->getTexture(key)->loadTexture();

    VAOHelper::genVAO(obj,
                      obj->getShader()->getAttributeLocation("vert"),
                      obj->getShader()->getAttributeLocation("vertTexCoord"),
                      obj->getShader()->getAttributeLocation("vertNormal"),
                      obj->getShader()->getAttributeLocation("vertTangent"));
    obj->getShader()->getUniformLocations(QList<QString> {
                                              "materialBump",
                                              "materialSpecular",
                                              "materialHighlight",
                                              "materialTransparency",
                                              "materialTex",
                                              "camera",
                                              "cameraPosition",
                                              "model",
                                              "fogParams.fDensity",
                                              "fogParams.fColor",
                                              "materialShininess",
                                              "materialTransparencyValue",
                                              "materialSpecularColor",
                                              "colorMul",
                                              "light.position",
                                              "light.attenuation",
                                              "light.ambientCoefficient",
                                              "light.intensities"
                                          });
    obj->setBaked(true);
}

glm::mat4 RenderingMethods::translateObjectMatrix(RenderableObject const *obj)
{
    glm::mat4 modelMatrix;
    modelMatrix = glm::translate(glm::mat4(),obj->getPosition());
    modelMatrix *= glm::mat4_cast(glm::normalize(obj->getRotation()));
    modelMatrix = glm::scale(modelMatrix,obj->getScale());
    return modelMatrix;
}
