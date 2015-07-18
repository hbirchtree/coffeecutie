#include "coffeestandardobject.h"

#include "engine/models/coffeemesh.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/components/shadercontainer.h"
#include "engine/models/coffeeinstancecontainer.h"
#include "opengl/helpers/renderingmethods.h"

CoffeeStandardObject::CoffeeStandardObject(QObject *parent) :
    CoffeeObject(parent)
{
    modelMatrix = new Matrix4Value(this,[=](const glm::mat4& v){
        Q_UNUSED(v)
        return RenderingMethods::translateObjectMatrix(position()->getValue(),
                                                       rotation()->getValue(),
                                                       scale()->getValue());
    });
}

CoffeeStandardObject::~CoffeeStandardObject()
{
    clearUniforms();
    clearTextures();
}

void CoffeeStandardObject::render()
{
    if(!baked)
        load();

    if(!pmesh)
        qFatal("Cannot render object: No mesh");

    if(pmesh->useInstancing()/*&&pmesh->hasNewMatrices()*/){ //Necessary to avoid crash
        pmesh->loadModelMatrices();
    }

    glUseProgram(pshader->getProgramId());

    applyUniforms();
    bindTextures();

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

    unbindTextures();

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

CoffeeShader *CoffeeStandardObject::shader()
{
    return pshader;
}

void CoffeeStandardObject::setShader(CoffeeShader *shader)
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
    CoffeeShader* shader = qobject_cast<CoffeeShader*>(sh);
    if(shader)
        setShader(shader);
}

void CoffeeStandardObject::setUniform(QString uniformName, QObject *data)
{
    VectorData* d = qobject_cast<VectorData*>(data);
    if(d)
        CoffeeUniformSetter::setUniform(uniformName,d);
}

void CoffeeStandardObject::setTexture(QString samplerName, CoffeeTexture *texture)
{
    CoffeeUniformSetter::setTexture(samplerName,texture);
}

