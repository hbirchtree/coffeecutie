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
    if(!baked())
        load();

    glUseProgram(m_shader->getProgramId());

    applyUniforms();
    bindTextures();

    m_mesh->renderMesh();

    unbindTextures();

    glUseProgram(0);
}

void CoffeeStandardObject::unload()
{
    if(m_mesh)
        m_mesh->unload();
    for(TextureMapping* m : textures)
        m->texture->unload();
    m_shader->unload();
    setBaked(false);
}

void CoffeeStandardObject::load()
{
    m_shader->buildProgram();
    if(m_mesh)
        m_mesh->load();
    for(TextureMapping* m : textures){
        m->loaded = true;
        m->texture->load();
    }
    setBaked(true);
}

CoffeeMesh *CoffeeStandardObject::mesh()
{
    return m_mesh;
}

void CoffeeStandardObject::setMesh(CoffeeMesh *mesh)
{
    if(m_mesh)
        m_mesh->removeConsumer();
    this->m_mesh = mesh;
    m_mesh->addConsumer();
}

CoffeeMaterial *CoffeeStandardObject::material()
{
    return m_material;
}

void CoffeeStandardObject::setMaterial(CoffeeMaterial *mtl)
{
    if(m_material)
        m_material->removeConsumer();
    this->m_material = mtl;
    m_material->addConsumer();
}

QObject *CoffeeStandardObject::materialRef()
{
    return m_material;
}

QObject *CoffeeStandardObject::meshRef()
{
    return m_mesh;
}

QObject *CoffeeStandardObject::getModelMatrix() const
{
    return modelMatrix;
}

void CoffeeStandardObject::setMeshRef(QObject *me)
{
    CoffeeMesh* mesh = qobject_cast<CoffeeMesh*>(me);
    if(mesh)
        setMesh(mesh);
}

void CoffeeStandardObject::setMaterialRef(QObject *ma)
{
    CoffeeMaterial* material = qobject_cast<CoffeeMaterial*>(ma);
    if(material)
        setMaterial(material);
}
