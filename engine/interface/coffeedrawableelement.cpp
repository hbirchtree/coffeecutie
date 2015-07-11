#include "coffeedrawableelement.h"

#include "engine/scripting/qscriptvectorvalue.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"

CoffeeDrawableElement::CoffeeDrawableElement(QObject *parent) : QObject(parent)
{
    m_size = new Vector2Value(this,glm::vec2(0.5,0.5));
    m_position = new Vector2Value(this,glm::vec2(0,0));
}

QObject *CoffeeDrawableElement::size()
{
    return m_size;
}

QObject *CoffeeDrawableElement::positionQObject()
{
    return m_position;
}

QObject *CoffeeDrawableElement::shader()
{
    return m_shader;
}

QObject *CoffeeDrawableElement::texture()
{
    return m_texture;
}

CoffeeTexture *CoffeeDrawableElement::getTexture()
{
    return m_texture;
}

CoffeeShader *CoffeeDrawableElement::getShader()
{
    return m_shader;
}

QString CoffeeDrawableElement::text() const
{
    return m_text;
}

void CoffeeDrawableElement::setSize(QObject *size)
{
    Vector2Value *s = qobject_cast<Vector2Value*>(size);
    if(s)
        m_size = s;
}

void CoffeeDrawableElement::setPosition(QObject *position)
{
    Vector2Value *p = qobject_cast<Vector2Value*>(position);
    if(p)
        m_position = p;
}

void CoffeeDrawableElement::setShader(QObject *shader)
{
    CoffeeShader *s = qobject_cast<CoffeeShader*>(shader);
    if(s)
        m_shader = s;
}

void CoffeeDrawableElement::setTexture(QObject *texture)
{
    CoffeeTexture *t = qobject_cast<CoffeeTexture*>(texture);
    if(t)
        m_texture = t;
}

void CoffeeDrawableElement::setText(const QString &text)
{
    m_text = text;
}

void CoffeeDrawableElement::render()
{
    if(!m_shader||!m_texture)
        return;
}

void CoffeeDrawableElement::unload()
{
    if(m_shader)
        m_shader->unload();
    if(m_texture)
        m_texture->unloadTexture();
}

void CoffeeDrawableElement::load()
{
    if(m_shader&&!m_shader->isAllocated())
        m_shader->buildProgram();
    if(m_texture&&!m_texture->isAllocated())
        m_texture->loadTexture();
}
