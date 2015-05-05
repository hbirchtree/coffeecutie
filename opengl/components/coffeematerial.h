#ifndef COFFEEMATERIAL
#define COFFEEMATERIAL

#include "general/common.h"
#include "general/numbercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "opengl/helpers/coffeegameasset.h"
class CoffeeMaterial : public QObject, public CoffeeGameAsset{
public:
    CoffeeMaterial(QObject* parent) : QObject(parent){}
    CoffeeMaterial(const CoffeeMaterial &mtl) : QObject(){
        m_transparency = new NumberContainer<float>(this->parent(),mtl.transparency());
        m_shininess = new NumberContainer<float>(this->parent(),mtl.shininess());
        m_specularColor = glm::vec3(mtl.specularColor());
        m_colorMultiplier = glm::vec3(mtl.colorMultiplier());
    }

    QPointer<NumberContainer<float>> transparency() const
    {
        return m_transparency;
    }

    QPointer<NumberContainer<float>> shininess() const
    {
        return m_shininess;
    }

    glm::vec3 specularColor() const
    {
        return m_specularColor;
    }

    glm::vec3 colorMultiplier() const
    {
        return m_colorMultiplier;
    }

    glm::vec3 getDiffuseColor() const{
        return m_diffuseColor;
    }
    void setDiffuseColor(const glm::vec3 &diffuseColor){
        m_diffuseColor = diffuseColor;
    }

public slots:
    void setTexture(int id,QSharedPointer<CoffeeTexture> texture){
        textures.insert(id,texture);
    }
    QSharedPointer<CoffeeTexture> getTexture(int id){
        return textures.value(id);
    }
    QList<int> getTextureKeys(){
        return textures.keys();
    }

    void setTransparency(NumberContainer<float> arg)
    {
        m_transparency = &arg;
    }

    void setShininess(NumberContainer<float> arg)
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
        for(QSharedPointer<CoffeeTexture> text : textures)
            text->unloadTexture();
    }

private:
    QHash<int,QSharedPointer<CoffeeTexture> > textures;

    QPointer<NumberContainer<float>> m_transparency = new NumberContainer<float>(this,1.f);
    QPointer<NumberContainer<float>> m_shininess = new NumberContainer<float>(this,50.f);
    glm::vec3 m_diffuseColor;
    glm::vec3 m_specularColor;
    glm::vec3 m_colorMultiplier;
};

#endif // COFFEEMATERIAL
