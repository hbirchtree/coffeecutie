#ifndef COFFEEMATERIAL
#define COFFEEMATERIAL

#include "general/common.h"
#include "general/floatcontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/helpers/coffeegameasset.h"
class CoffeeMaterial : public QObject, public CoffeeGameAsset{
public:
    CoffeeMaterial(QObject* parent) : QObject(parent){}
    CoffeeMaterial(const CoffeeMaterial &mtl) : QObject(){
        m_transparency = new FloatContainer(mtl.transparency());
        m_shininess = new FloatContainer(mtl.shininess());
        m_specularColor = glm::vec3(mtl.specularColor());
        m_colorMultiplier = glm::vec3(mtl.colorMultiplier());
    }

    QPointer<FloatContainer> transparencyObject()
    {
        return m_transparency;
    }

    QPointer<FloatContainer> shininessObject()
    {
        return m_shininess;
    }

    FloatContainer transparency() const
    {
        return m_transparency.data();
    }

    FloatContainer shininess() const
    {
        return m_shininess.data();
    }

    glm::vec3 specularColor() const
    {
        return m_specularColor;
    }

    glm::vec3 colorMultiplier() const
    {
        return m_colorMultiplier;
    }

public slots:
    void setTexture(int id,QPointer<CoffeeTexture> texture){
        textures.insert(id,texture);
    }
    QPointer<CoffeeTexture> getTexture(int id){
        return textures.value(id);
    }

    void setTransparency(FloatContainer arg)
    {
        m_transparency = &arg;
    }

    void setShininess(FloatContainer arg)
    {
        m_shininess = &arg;
    }

    void setSpecularColor(glm::vec3 arg)
    {
        if (m_specularColor == arg)
            return;

        m_specularColor = arg;
    }

    void setColorMultiplier(glm::vec3 arg)
    {
        if (m_colorMultiplier == arg)
            return;

        m_colorMultiplier = arg;
    }

    void unloadData(){
        for(QPointer<CoffeeTexture> text : textures)
            text->unloadTexture();
    }

private:
    QHash<int,QPointer<CoffeeTexture> > textures;

    QPointer<FloatContainer> m_transparency = new FloatContainer(this);
    QPointer<FloatContainer> m_shininess = new FloatContainer(this);
    glm::vec3 m_specularColor;
    glm::vec3 m_colorMultiplier;
};

#endif // COFFEEMATERIAL

