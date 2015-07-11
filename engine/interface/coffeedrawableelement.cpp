#include "coffeedrawableelement.h"

#include "engine/scripting/qscriptvectorvalue.h"
#include "opengl/components/shadercontainer.h"
#include "opengl/components/coffeetexture.h"

CoffeeDrawableElement::CoffeeDrawableElement(QObject *parent) : QObject(parent)
{
}

QObject *CoffeeDrawableElement::size()
{
    return m_size;
}

QObject *CoffeeDrawableElement::position()
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

QString CoffeeDrawableElement::text() const
{
    return m_text;
}

void CoffeeDrawableElement::setSize(QObject *size)
{
    Vector3Value *s = qobject_cast<Vector3Value*>(size);
    if(s)
        m_size = s;
}

void CoffeeDrawableElement::setPosition(QObject *position)
{
    Vector3Value *p = qobject_cast<Vector3Value*>(position);
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
