#include "renderingmethods.h"


void RenderingMethods::rendering_simple(QPointer<RenderableObject> obj)
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

    //Set uniforms

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,obj->getTextureHandle(CoffeeTexture::Texture_Diffusion));

    glBindVertexArray(obj->getVaoHandle());
    glDrawArrays(GL_TRIANGLES,0,obj->getVertexDataSize());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,0);
    glUseProgram(0);

    if(!obj->isDepthTest())
        glEnable(GL_DEPTH_TEST);
}

void RenderingMethods::baking_simple(QPointer<RenderableObject> obj)
{
    obj->getTexture(CoffeeTexture::Texture_Diffusion)->loadTexture();
    VAOHelper::genVAO(obj,
                      obj->getShader()->getAttributeLocation("vert"),
                      obj->getShader()->getAttributeLocation("vertTexCoord"),
                      obj->getShader()->getAttributeLocation("vertNormal"),
                      obj->getShader()->getAttributeLocation("vertTangent"));
}
