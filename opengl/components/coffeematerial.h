#ifndef COFFEEMATERIAL
#define COFFEEMATERIAL

#include "general/common.h"
#include "general/data/numbercontainer.h"
#include "opengl/components/coffeetexture.h"
#include "general/data/coffeegameasset.h"
class CoffeeMaterial : public QObject, public CoffeeGameAsset{

    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe)
    Q_PROPERTY(bool culling READ culling WRITE setCulling)
//    Q_PROPERTY(glm::vec3 transparency READ transparency WRITE setTransparency)
//    Q_PROPERTY(glm::vec3 diffuseColor READ diffuseColor WRITE setDiffuseColor)
//    Q_PROPERTY(glm::vec3 specularColor READ specularColor WRITE setSpecularColor)
//    Q_PROPERTY(glm::vec3 ambientColor READ ambientColor WRITE setAmbientColor)
//    Q_PROPERTY(glm::vec3 emissiveColor READ emissiveColor WRITE setEmissiveColor)
    Q_PROPERTY(float shininessStrength READ shininessStrength WRITE setShininessStrength)
    Q_PROPERTY(float refraction READ refraction WRITE setRefraction)
    Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess)
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)
//    Q_PROPERTY(GLenum blendMode READ blendMode WRITE setBlendMode)
//    Q_PROPERTY(GLenum shadingMode READ shadingMode WRITE setShadingMode)
//    Q_PROPERTY(glm::vec3 colorMultiplier READ colorMultiplier WRITE setColorMultiplier)

    Q_OBJECT

public:
    CoffeeMaterial(QObject* parent);
    CoffeeMaterial(QObject *parent, const aiMaterial* materialSource, const QString &filepath);

    bool wireframe() const;
    bool culling() const;
    float shininess() const;
    float reflectivity() const;
    float shininessStrength() const;
    float refraction() const;
    float opacity() const;
    QPointer<NumberContainer<glm::vec3>> getTransparency();
    QPointer<NumberContainer<float>> getShininess();
    glm::vec3 ambientColor() const;
    glm::vec3 specularColor() const;
    glm::vec3 colorMultiplier() const;
    glm::vec3 diffuseColor() const;
    glm::vec3 transparency() const;
    glm::vec3 emissiveColor() const;
    GLenum blendMode() const;
    GLenum shadingMode() const;

public slots:
    void setTexture(CoffeeTexture::CoffeeTextureType id,QPointer<CoffeeTexture> texture);
    QPointer<CoffeeTexture> getTexture(CoffeeTexture::CoffeeTextureType id);
    QList<CoffeeTexture::CoffeeTextureType> getTextureKeys();

    void setWireframe(bool wireframe);
    void setCulling(bool culling);
    void setShininess(float arg);
    void setRefraction(float refraction);
    void setOpacity(float opacity);
    void setReflectivity(float reflectivity);
    void setShininessStrength(float shininessStrength);
    void setDiffuseColor(const glm::vec3 &diffuseColor);
    void setTransparency(const glm::vec3& arg);
    void setAmbientColor(const glm::vec3& ambientColor);
    void setSpecularColor(const glm::vec3& arg);
    void setEmissiveColor(const glm::vec3& emissiveColor);
    void setColorMultiplier(const glm::vec3& arg);
    void setBlendMode(GLenum blendMode);
    void setShadingMode(GLenum shadingMode);

    void unloadData();

private:
    QHash<CoffeeTexture::CoffeeTextureType,QPointer<CoffeeTexture> > textures;

    bool m_wireframe = false;
    bool m_culling = true;
    QPointer<NumberContainer<float>> m_shininess;
    QPointer<NumberContainer<float>> m_shininessStrength;
    QPointer<NumberContainer<float>> m_refraction;
    QPointer<NumberContainer<glm::vec3>> m_transparency;
    QPointer<NumberContainer<glm::vec3>> m_diffuseColor;
    QPointer<NumberContainer<glm::vec3>> m_specularColor;
    QPointer<NumberContainer<glm::vec3>> m_ambientColor;
    QPointer<NumberContainer<glm::vec3>> m_emissiveColor;
    QPointer<NumberContainer<glm::vec3>> m_colorMultiplier;
    GLenum m_blendMode;
    GLenum m_shadingMode = GL_FLAT;
    QPointer<NumberContainer<float>> m_opacity;
    QPointer<NumberContainer<float>> m_reflectivity;
};

#endif // COFFEEMATERIAL
