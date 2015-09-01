#include "coffeeskybox.h"

#include "engine/models/coffeemesh.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/helpers/renderingmethods.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/components/coffeecamera.h"
#include "engine/data/coffeeresource.h"

CoffeeSkybox::CoffeeSkybox(QObject *parent,CoffeeCamera* camera) :
    CoffeeObject(parent)
{
    setCamera(camera);

    m_matrix = new Matrix4Value(this,[=](const glm::mat4& m){
        Q_UNUSED(m)
        return this->m_camera.data()->getMatrix()*
                RenderingMethods::translateObjectMatrix(
                    m_camera->getPosition()->getValue(),
                    rotation()->getValue(),
                    scale()->getValue());
    });
}

void CoffeeSkybox::setCamera(CoffeeCamera *camera)
{
    this->m_camera = camera;
}

void CoffeeSkybox::render()
{
    if(!baked())
        load();
    glUseProgram(m_shader->getProgramId());

    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_FRONT);

    applyUniforms();
    bindTextures();

    m_mesh->renderMesh();

    glCullFace(GL_BACK);
    glDepthFunc(GL_LESS);

    unbindTextures();

    glUseProgram(0);
}

void CoffeeSkybox::unload()
{
    m_mesh->unload();
    m_shader->unload();
    m_texture->unload();

    setBaked(false);
}

void CoffeeSkybox::load()
{
    m_texture->load();
    m_mesh->load();
    m_shader->buildProgram();

    setBaked(true);
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

GLenum CoffeeSkybox::_gl_depthFunc()
{
    return GL_LEQUAL;
}

GLenum CoffeeSkybox::_gl_culling()
{
    return GL_FRONT;
}
CoffeeShader* CoffeeSkybox::getShader()
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

QObject *CoffeeSkybox::meshQObject() const
{
    return m_mesh;
}

QObject *CoffeeSkybox::texture() const
{
    return m_texture;
}

QObject *CoffeeSkybox::skyboxMatrix()
{
    return m_matrix;
}

CoffeeMesh* CoffeeSkybox::mesh()
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

CoffeeTexture* CoffeeSkybox::getTexture()
{
    return m_texture;
}

void CoffeeSkybox::setTexture(CoffeeTexture* value)
{
    if(value&&
            (value->type()==CoffeeTexture::Cubemap||
             value->type()==CoffeeTexture::CubemapDice)){
        if(this->m_texture)
            this->m_texture->removeConsumer();
        this->m_texture = value;
        this->m_texture->addConsumer();
    }else
        qDebug("Could not assign 2D texture as cubemap!");
}
