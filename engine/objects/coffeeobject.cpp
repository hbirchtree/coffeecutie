#include "coffeeobject.h"

#include "opengl/components/shadercontainer.h"

CoffeeObject::CoffeeObject(QObject *parent) : QObject(parent)
{
    m_position = new Vector3Value(this,glm::vec3(0,0,0));
    m_rotation = new QuatValue(this,glm::quat(2,0,0,0));
    m_scale = new Vector3Value(this,glm::vec3(1,1,1));
}

CoffeeObject::~CoffeeObject()
{
}

Vector3Value *CoffeeObject::position(){
    return m_position;
}

QuatValue *CoffeeObject::rotation(){
    return m_rotation;
}

Vector3Value *CoffeeObject::scale(){
    return m_scale;
}

PhysicsObject *CoffeeObject::physics(){
    return m_physics;
}

void CoffeeObject::setPhysicsObject(PhysicsObject *obj)
{
    this->m_physics = obj;
}

bool CoffeeObject::baked() const
{
    return m_baked;
}

void CoffeeObject::setBaked(bool baked)
{
    m_baked = baked;
}

QObject *CoffeeObject::positionValue()
{
    return position();
}

QObject *CoffeeObject::rotationValue()
{
    return rotation();
}

QObject *CoffeeObject::scaleValue()
{
    return scale();
}

QObject *CoffeeObject::physicsRef()
{
    return physics();
}

QObject *CoffeeObject::shaderRef()
{
    return m_shader.data();
}

GLenum CoffeeObject::_gl_depthFunc()
{
    return GL_LESS;
}

GLenum CoffeeObject::_gl_culling()
{
    return GL_BACK;
}

QString CoffeeObject::toString()
{
    return QString("CoffeeObject("
                   "instance=0x%1,"
                   "physics=0x%2,"
                   "shader=0x%3,"
                   "baked=%4)")
            .arg(QString::number((uintptr_t)this,16))
            .arg(QString::number((uintptr_t)physicsRef(),16))
            .arg(QString::number((uintptr_t)shaderRef(),16))
            .arg(baked());
}

void CoffeeObject::setPhysicsObject(QObject *obj)
{
    PhysicsObject* t = qobject_cast<PhysicsObject*>(obj);
    if(t)
        this->CoffeeObject::setPhysicsObject(t);
}

void CoffeeObject::setShaderRef(QObject *sh)
{
    CoffeeShader* shader = qobject_cast<CoffeeShader*>(sh);
    if(shader)
        setShader(shader);
}

bool CoffeeObject::hasPhysics() const
{
    return m_physics;
}

CoffeeMesh *CoffeeObject::mesh()
{
    return nullptr;
}

void CoffeeObject::setUniform(QString uniformName, QObject *data)
{
    VectorData* d = qobject_cast<VectorData*>(data);
    if(d)
        CoffeeUniformSetter::setUniform(uniformName,d);
}

void CoffeeObject::setTexture(QString samplerName, CoffeeTexture *texture)
{
    CoffeeUniformSetter::setTexture(samplerName,texture);
}
