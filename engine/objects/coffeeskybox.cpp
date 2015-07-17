#include "coffeeskybox.h"

#include "engine/models/coffeemesh.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/helpers/renderingmethods.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeecamera.h"
#include "engine/data/coffeeresource.h"

CoffeeSkybox::CoffeeSkybox(QObject *parent,CoffeeCamera* camera) :
    QObject(parent),
    CoffeeObject(this)
{
    setCamera(camera);
}

void CoffeeSkybox::setCamera(CoffeeCamera *camera)
{
    this->m_camera = camera;
}

void CoffeeSkybox::render()
{
    if(!baked)
        load();
    glUseProgram(m_shader->getProgramId());

    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(m_texture->getGlTextureType(),m_texture->getHandle());

    m_shader->setUniform("cubemapTexture",0);
    m_shader->setUniform("wvp",m_camera->getMatrix()*
                       RenderingMethods::translateObjectMatrix(
                           m_camera->getPosition()->getValue(),
                           rotation()->getValue(),
                           scale()->getValue()));

    glBindVertexArray(m_mesh->getVertexArrayHandle());
    glDrawElements(GL_TRIANGLES,m_mesh->getIndicesCount(),GL_UNSIGNED_INT,0);

    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    glBindTexture(m_texture->getGlTextureType(),0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void CoffeeSkybox::unload()
{
    glDeleteVertexArrays(1,&vao);
    glDeleteBuffers(2,buffs);
    m_texture->unloadTexture();
    glDisable(GL_TEXTURE_CUBE_MAP);
}

void CoffeeSkybox::load()
{
    m_texture->loadTexture();
    m_mesh->loadMesh();
    m_shader->buildProgram();

    baked = true;
}

void CoffeeSkybox::setMesh(QObject *mesh)
{
    CoffeeMesh* m = qobject_cast<CoffeeMesh*>(mesh);
    if(m)
        setSkymesh(m);
}

void CoffeeSkybox::setShader(QObject *shader)
{
    CoffeeShader* m = qobject_cast<CoffeeShader*>(shader);
    if(m)
        setShader(m);
}

void CoffeeSkybox::setCamera(QObject *camera)
{
    CoffeeCamera* m = qobject_cast<CoffeeCamera*>(camera);
    if(m)
        setCamera(m);
}

void CoffeeSkybox::setTexture(QObject *texture)
{
    CoffeeTexture* m = qobject_cast<CoffeeTexture*>(texture);
    if(m)
        setTexture(m);
}
QPointer<CoffeeShader> CoffeeSkybox::getShader() const
{
    return m_shader;
}

void CoffeeSkybox::setShader(CoffeeShader* value)
{
    if(value){
        if(m_shader)
            m_shader->removeConsumer();
        m_shader = value;
        m_shader->addConsumer();
    }else
        qDebug("Skybox shader does not exist!");
}

QObject *CoffeeSkybox::camera() const
{
    return m_camera;
}

QObject *CoffeeSkybox::shader() const
{
    return m_shader;
}

QObject *CoffeeSkybox::mesh() const
{
    return m_mesh;
}

QObject *CoffeeSkybox::texture() const
{
    return m_texture;
}

QPointer<CoffeeMesh> CoffeeSkybox::getSkymesh() const
{
    return m_mesh;
}

void CoffeeSkybox::setSkymesh(CoffeeMesh *value)
{
    if(value&&value->hasPositions()){
        if(m_mesh)
            m_mesh->removeConsumer();
        m_mesh = value;
        m_mesh->addConsumer();
    }else
        qDebug("Could not assign mesh without positions!");
}

QPointer<CoffeeTexture> CoffeeSkybox::getTexture() const
{
    return m_texture;
}

void CoffeeSkybox::setTexture(CoffeeTexture* value)
{
    if(value&&value->isCubemap()){
        if(this->m_texture)
            this->m_texture->removeConsumer();
        this->m_texture = value;
        this->m_texture->addConsumer();
    }else
        qDebug("Could not assign 2D texture as cubemap!");
}
