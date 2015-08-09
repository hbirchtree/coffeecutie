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

    Q_PROPERTY(QObject* shininess READ shininessVariant)
    Q_PROPERTY(QObject* shininessStrength READ shininessStrengthVariant)
    Q_PROPERTY(QObject* opacity READ opacityVariant)
    Q_PROPERTY(QObject* reflectivity READ reflectivityVariant)
    Q_PROPERTY(QObject* refraction READ refractionVariant)

    Q_PROPERTY(QVariantList textureTypes READ textureTypes)
    Q_PROPERTY(QVariantMap textures READ textureObjects)

    Q_OBJECT

public:
    CoffeeMaterial(QObject* parent);
    CoffeeMaterial(QObject *parent, const aiMaterial* materialSource, const QString &filepath);

    bool wireframe() const;
    bool culling() const;
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
    QVariantMap textureObjects() const;

    QVariantList textureTypes() const;

public slots:
    void setTexture(CoffeeTexture::CoffeeTextureType id,QPointer<CoffeeTexture> texture);

    void setWireframe(bool wireframe);
    void setCulling(bool culling);
    void setBlendMode(GLenum blendMode);
    void setShadingMode(GLenum shadingMode);

    void load();
    void unload();

private:
    QHash<CoffeeTexture::CoffeeTextureType,QPointer<CoffeeTexture> > textures;

    bool m_wireframe = false;
    bool m_culling = true;
    GLenum m_blendMode;
    GLenum m_shadingMode = GL_FLAT;
    QPointer<ScalarValue> m_shininess;
    QPointer<ScalarValue> m_shininessStrength;
    QPointer<ScalarValue> m_refraction;
    QPointer<Vector3Value> m_transparency;
    QPointer<Vector3Value> m_diffuseColor;
    QPointer<Vector3Value> m_specularColor;
    QPointer<Vector3Value> m_ambientColor;
    QPointer<Vector3Value> m_emissiveColor;
    QPointer<Vector3Value> m_colorMultiplier;
    QPointer<ScalarValue> m_opacity;
    QPointer<ScalarValue> m_reflectivity;
};

#endif // COFFEEMATERIAL
