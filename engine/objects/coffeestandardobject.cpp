#include "coffeestandardobject.h"

#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/components/shadercontainer.h"
#include "engine/models/coffeeinstancecontainer.h"
#include "general/shadervariant.h"
#include "opengl/helpers/renderingmethods.h"

CoffeeStandardObject::CoffeeStandardObject(QObject *parent) :
    QObject(parent),
    CoffeeObject(this)
{
    modelMatrix = new ShaderVariant([=](){
        return RenderingMethods::translateObjectMatrix(position()->getValue(),
                                                       rotation()->getValue(),
                                                       scale()->getValue());
    });
}

CoffeeStandardObject::~CoffeeStandardObject()
{
    for(ShaderMapping *map : uniforms)
        map->data->deleteLater();
}

void CoffeeStandardObject::render()
{
    if(!baked)
        load();

    if(!pmesh)
        qFatal("Cannot render object: No mesh");

    pmesh->getInstances()->setRenderPrepare(true);
    if(pmesh->useInstancing()/*&&pmesh->hasNewMatrices()*/){ //Necessary to avoid crash
        pmesh->loadModelMatrices();
    }

    glUseProgram(pshader->getProgramId());
    for(ShaderMapping* m : uniforms){
        pshader->setUniform(m->uniform,m->data);
    }
    for(TextureMapping* m : textures){
        if(!m->loaded){
            m->texture->loadTexture();
            m->loaded = true;
        }
        int index = textures.indexOf(m);
        glActiveTexture(GL_TEXTURE0+index);
        glBindTexture(GL_TEXTURE_2D,
                      m->texture->getHandle());
        pshader->setUniform(m->samplerName,
                            index);
    }

    glBindVertexArray(pmesh->getVertexArrayHandle());
    if(!this->mesh()->useInstancing())
        glDrawElements(GL_TRIANGLES,
                       pmesh->getIndicesCount(),
                       GL_UNSIGNED_INT,
                       0);
    else{
        glDrawElementsInstanced(GL_TRIANGLES,
                                          pmesh->getIndicesCount(),
                                          GL_UNSIGNED_INT,
                                          0,
                                          pmesh->getInstances()->instanceCount());
    }
    pmesh->getInstances()->setRenderPrepare(false);

    for(TextureMapping *m : textures){
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
    for(TextureMapping* m : textures)
        m->texture->unloadTexture();
    pshader->unload();
    baked = false;
}

void CoffeeStandardObject::load()
{
    pshader->buildProgram();
    if(pmesh)
        pmesh->loadMesh();
    for(TextureMapping* m : textures){
        m->loaded = true;
        m->texture->loadTexture();
    }
    baked = true;
}

CoffeeMesh *CoffeeStandardObject::mesh()
{
    return pmesh;
}

void CoffeeStandardObject::setMesh(CoffeeMesh *mesh)
{
    if(pmesh)
        pmesh->removeConsumer();
    this->pmesh = mesh;
    pmesh->addConsumer();
}

ShaderContainer *CoffeeStandardObject::shader()
{
    return pshader;
}

void CoffeeStandardObject::setShader(ShaderContainer *shader)
{
    if(pshader)
        pshader->removeConsumer();
    this->pshader = shader;
    pshader->addConsumer();
}

CoffeeMaterial *CoffeeStandardObject::material()
{
    return pmaterial;
}

void CoffeeStandardObject::setMaterial(CoffeeMaterial *mtl)
{
    if(pmaterial)
        pmaterial->removeConsumer();
    this->pmaterial = mtl;
    pmaterial->addConsumer();
}

QObject *CoffeeStandardObject::materialRef()
{
    return pmaterial;
}

QObject *CoffeeStandardObject::meshRef()
{
    return pmesh;
}

QObject *CoffeeStandardObject::shaderRef()
{
    return pshader;
}

QObject *CoffeeStandardObject::positionValue()
{
    return position();
}

QObject *CoffeeStandardObject::rotationValue()
{
    return rotation();
}

QObject *CoffeeStandardObject::scaleValue()
{
    return scale();
}

QObject *CoffeeStandardObject::physicsRef()
{
    return physics();
}

QObject *CoffeeStandardObject::getModelMatrix() const
{
    return modelMatrix;
}

bool CoffeeStandardObject::hasPhysics()
{
    return physics();
}

void CoffeeStandardObject::setUniform(QString uniformName, ShaderVariant* data, bool constant)
{
    ShaderMapping *map = new ShaderMapping;
    map->constant = constant;
    map->uniform = uniformName;
    map->data = data;
    uniforms.append(map);
}

void CoffeeStandardObject::setTexture(QString samplerName, CoffeeTexture* texture)
{
    TextureMapping *map = new TextureMapping;
    map->samplerName = samplerName;
    map->texture = texture;
    textures.append(map);
}

void CoffeeStandardObject::setPosition(float x, float y, float z)
{
    position()->setValue(glm::vec3(x,y,z));
}

void CoffeeStandardObject::setRotation(float x, float y, float z)
{
    rotation()->setValue(glm::quat(glm::vec3(x,y,z)));
}

void CoffeeStandardObject::setPhysicsObject(QObject *obj)
{
    PhysicsObject* t = qobject_cast<PhysicsObject*>(obj);
    if(t)
        this->CoffeeObject::setPhysicsObject(t);
}

void CoffeeStandardObject::setShaderRef(QObject *sh)
{
    ShaderContainer* shader = qobject_cast<ShaderContainer*>(sh);
    if(shader)
        setShader(shader);
}

