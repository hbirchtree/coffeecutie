#include "coffeeskybox.h"

#include "engine/models/coffeemesh.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/helpers/renderingmethods.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeecamera.h"

CoffeeSkybox::CoffeeSkybox(QObject *parent,CoffeeCamera* camera) :
    QObject(parent),
    CoffeeObject(this)
{
    setCamera(camera);
}

void CoffeeSkybox::addMap(GLenum side, QString source)
{
    cubemapping.insert(side,source);
}

void CoffeeSkybox::setCamera(CoffeeCamera *camera)
{
    this->camera = camera;
}

void CoffeeSkybox::render()
{
    if(!baked)
        load();
    glUseProgram(shader->getProgramId());

    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP,texture->getHandle());

    shader->setUniform("cubemapTexture",0);
    shader->setUniform("wvp",camera->getMatrix()*
                       RenderingMethods::translateObjectMatrix(
                           camera->getPosition()->getValue(),
                           rotation()->getValue(),
                           scale()->getValue()));

    glBindVertexArray(skymesh->getVertexArrayHandle());
    glDrawElements(GL_TRIANGLES,skymesh->getIndicesCount(),GL_UNSIGNED_INT,0);

    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    glBindTexture(GL_TEXTURE_CUBE_MAP,0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void CoffeeSkybox::unload()
{
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(2,buffs);
    texture->unloadTexture();
    glDisable(GL_TEXTURE_CUBE_MAP);
}

void CoffeeSkybox::load()
{
    if(!texture)
        texture = new CoffeeTexture(this,cubemapping);

    if(glIsEnabled(GL_TEXTURE_CUBE_MAP)==GL_FALSE)
        glEnable(GL_TEXTURE_CUBE_MAP);
    qDebug("Skybox initializing");

    texture->loadTexture();
    skymesh->loadMesh();
    shader->buildProgram();

    baked = true;

    qDebug("Skybox loaded");
}
QPointer<ShaderContainer> CoffeeSkybox::getShader() const
{
    return shader;
}

void CoffeeSkybox::setShader(QPointer<ShaderContainer> value)
{
    if(value){
        if(shader)
            shader->removeConsumer();
        shader = value;
        shader->addConsumer();
    }else
        qDebug("Skybox shader does not exist!");
}

QPointer<CoffeeMesh> CoffeeSkybox::getSkymesh() const
{
    return skymesh;
}

void CoffeeSkybox::setSkymesh(QPointer<CoffeeMesh> value)
{
    if(value&&value->hasPositions()){
        if(skymesh)
            skymesh->removeConsumer();
        skymesh = value;
        skymesh->addConsumer();
    }else
        qDebug("Could not assign mesh without positions!");
}

QPointer<CoffeeTexture> CoffeeSkybox::getTexture() const
{
    return texture;
}

void CoffeeSkybox::setTexture(QPointer<CoffeeTexture> value)
{
    if(value&&value->isCubemap()){
        if(this->texture)
            this->texture->removeConsumer();
        this->texture = value;
        this->texture->addConsumer();
    }else
        qDebug("Could not assign 2D texture as cubemap!");
}
