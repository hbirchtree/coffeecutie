#ifndef COFFEEMATERIAL
#define COFFEEMATERIAL

#include "general/common.h"
#include "engine/scripting/qscriptvectorvalue.h"
#include "opengl/components/coffeetexture.h"
#include "general/data/coffeegameasset.h"

class ShaderVariant;

class CoffeeMaterial : public QObject, public CoffeeGameAsset{

    Q_PROPERTY(bool wireframe READ wireframe WRITE setWireframe)
    Q_PROPERTY(bool culling READ culling WRITE setCulling)
    //These are made to operate with the shader
    Q_PROPERTY(QObject* transparency READ transparencyVariant)
    Q_PROPERTY(QObject* diffuseColor READ diffuseColorVariant)
    Q_PROPERTY(QObject* specularColor READ specularColorVariant)
    Q_PROPERTY(QObject* ambientColor READ ambientColorVariant)
    Q_PROPERTY(QObject* emissiveColor READ emissiveColorVariant)
    Q_PROPERTY(QObject* colorMultiplier READ colorMultiplierVariant)

    Q_PROPERTY(QObject* shininessVariant READ shininessVariant)
    Q_PROPERTY(QObject* shininessStrengthVariant READ shininessStrengthVariant)
    Q_PROPERTY(QObject* opacityVariant READ opacityVariant)
    Q_PROPERTY(QObject* reflectivityVariant READ reflectivityVariant)
    Q_PROPERTY(QObject* refractionVariant READ refractionVariant)

    Q_PROPERTY(float shininessStrength READ shininessStrength WRITE setShininessStrength)
    Q_PROPERTY(float refraction READ refraction WRITE setRefraction)
    Q_PROPERTY(float reflectivity READ reflectivity WRITE setReflectivity)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess)
    Q_PROPERTY(float opacity READ opacity WRITE setOpacity)

    Q_PROPERTY(QVariantList textureTypes READ textureTypes)

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
    QPointer<Vector3Value> getTransparency();
    QPointer<ScalarValue> getShininess();
    glm::vec3 ambientColor() const;
    glm::vec3 specularColor() const;
    glm::vec3 colorMultiplier() const;
    glm::vec3 diffuseColor() const;
    glm::vec3 transparency() const;
    glm::vec3 emissiveColor() const;
    GLenum blendMode() const;
    GLenum shadingMode() const;

    QObject* shininessVariant() const;
    QObject* shininessStrengthVariant() const;
    QObject* reflectivityVariant() const;
    QObject* refractionVariant() const;
    QObject* opacityVariant() const;

    QObject* transparencyVariant() const;
    QObject* diffuseColorVariant() const;
    QObject* specularColorVariant() const;
    QObject* ambientColorVariant() const;
    QObject* emissiveColorVariant() const;
    QObject* colorMultiplierVariant() const;
    CoffeeTexture* getTexture(CoffeeTexture::CoffeeTextureType id);
    Q_INVOKABLE CoffeeTexture* getTexture(int id);
    QList<CoffeeTexture::CoffeeTextureType> getTextureKeys() const;

    QVariantList textureTypes() const;

public slots:
    void setTexture(CoffeeTexture::CoffeeTextureType id,QPointer<CoffeeTexture> texture);

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
    QPointer<ScalarValue> m_shininess;
    QPointer<ScalarValue> m_shininessStrength;
    QPointer<ScalarValue> m_refraction;
    QPointer<Vector3Value> m_transparency;
    QPointer<Vector3Value> m_diffuseColor;
    QPointer<Vector3Value> m_specularColor;
    QPointer<Vector3Value> m_ambientColor;
    QPointer<Vector3Value> m_emissiveColor;
    QPointer<Vector3Value> m_colorMultiplier;
    GLenum m_blendMode;
    GLenum m_shadingMode = GL_FLAT;
    QPointer<ScalarValue> m_opacity;
    QPointer<ScalarValue> m_reflectivity;

    ShaderVariant* p_shininess;
    ShaderVariant* p_shininessStrength;
    ShaderVariant* p_refraction;
    ShaderVariant* p_transparency;
    ShaderVariant* p_diffuseColor;
    ShaderVariant* p_ambientColor;
    ShaderVariant* p_specularColor;
    ShaderVariant* p_emissiveColor;
    ShaderVariant* p_colorMultiplier;
    ShaderVariant* p_opacity;
    ShaderVariant* p_reflectivity;
};

#endif // COFFEEMATERIAL
