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

    if(!obj->isStreamDraw()){
        //Animate skeleton here
    }

    QPointer<ShaderContainer> shader = obj->getShader();
    shader->setUniform("camera",world->getCamera()->getProjection());
    shader->setUniform("cameraPosition",world->getCamera()->getPosition()->getValue());
    shader->setUniform("model",translateObjectMatrix(obj));

    for(QPointer<CoffeeOmniLight> light : world->getLights()){
        shader->setUniform("light.position",light->getPosition()->getValue());
        shader->setUniformRgb("light.intensities",light->getColor()->getValue());
        shader->setUniform("light.attenuation",light->getAttenuation()->getValue());
        shader->setUniform("light.ambientCoefficient",light->getAmbientCoefficient()->getValue());
    }

    shader->setUniform("materialTex",0);
    shader->setUniform("materialBump", 1);
    shader->setUniform("materialSpecular", 2);
    shader->setUniform("materialHighlight", 3);
    shader->setUniform("materialTransparency", 4);
    shader->setUniform("materialShininess", obj->getShininess());
    shader->setUniformRgb("materialSpecularColor", obj->getSpecularColor());
    shader->setUniform("materialTransparencyValue", obj->getTransparency());
    shader->setUniform("colorMul", obj->getColorMultiplier());

    shader->setUniform("fogParams.fDensity", world->getFogDensity());
    shader->setUniformRgba("fogParams.fColor", world->getFogColor());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,obj->getTextureHandle(CoffeeTexture::Texture_Diffusion));

    glBindVertexArray(obj->getVaoHandle());
    glDrawArrays(GL_TRIANGLES,0,obj->getVerticesCount());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);
    glUseProgram(0);

    if(!obj->isDepthTest())
        glEnable(GL_DEPTH_TEST);
}

void RenderingMethods::baking_simple(RenderableObject* obj)
{

    obj->getTexture(CoffeeTexture::Texture_Diffusion)->loadTexture();
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

glm::mat4 RenderingMethods::translateObjectMatrix(RenderableObject *obj)
{
    glm::mat4 modelMatrix = glm::translate(glm::mat4(),obj->getPosition());
    modelMatrix = glm::scale(modelMatrix,obj->getScale());
    modelMatrix = glm::rotate(modelMatrix,obj->getRotation().x,glm::vec3(1,0,0));
    modelMatrix = glm::rotate(modelMatrix,obj->getRotation().y,glm::vec3(0,1,0));
    modelMatrix = glm::rotate(modelMatrix,obj->getRotation().z,glm::vec3(0,0,1));
    return modelMatrix;
}
