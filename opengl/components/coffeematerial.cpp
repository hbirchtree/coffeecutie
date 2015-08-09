#include "coffeematerial.h"

#include "engine/data/coffeeresource.h"
#include <QMetaEnum>
#include <QFileInfo>

CoffeeMaterial::CoffeeMaterial(QObject *parent) : QObject(parent){
    m_shininess = new ScalarValue(this,1.f);
    m_shininessStrength = new ScalarValue(this,1.f);
    m_refraction = new ScalarValue(this,1.f);
    m_reflectivity = new ScalarValue(this,0.f);
    m_opacity = new ScalarValue(this,1.f);

    m_transparency = new Vector3Value(this,glm::vec3(1));
    m_emissiveColor = new Vector3Value(this,glm::vec3(0));
    m_diffuseColor = new Vector3Value(this,glm::vec3(1));
    m_specularColor = new Vector3Value(this,glm::vec3(1));
    m_ambientColor = new Vector3Value(this,glm::vec3(1));
    m_colorMultiplier = new Vector3Value(this,glm::vec3(1));
}

CoffeeMaterial::CoffeeMaterial(QObject *parent, const aiMaterial *materialSource, const QString& filepath) :
    CoffeeMaterial(parent)
{
    aiString name;
    aiColor3D color,specular,ambient,transparency;
    int wireframe = 0,twosided = 1,shadingmodel = 0,blendfunc = 0;
    float opacity = 1,shininess = 1,shininessStrength = 50,refract = 1,reflectivity = 0;
    materialSource->Get(AI_MATKEY_NAME,name);
    materialSource->Get(AI_MATKEY_COLOR_DIFFUSE,color);
    materialSource->Get(AI_MATKEY_COLOR_SPECULAR,specular);
    materialSource->Get(AI_MATKEY_COLOR_AMBIENT,ambient);
    materialSource->Get(AI_MATKEY_COLOR_TRANSPARENT,transparency);

    materialSource->Get(AI_MATKEY_ENABLE_WIREFRAME,wireframe);
    materialSource->Get(AI_MATKEY_TWOSIDED,twosided);
    materialSource->Get(AI_MATKEY_SHADING_MODEL,shadingmodel);
    materialSource->Get(AI_MATKEY_BLEND_FUNC,blendfunc);

    materialSource->Get(AI_MATKEY_OPACITY,opacity);
    materialSource->Get(AI_MATKEY_SHININESS,shininess);
    materialSource->Get(AI_MATKEY_SHININESS_STRENGTH,shininessStrength);
    materialSource->Get(AI_MATKEY_REFRACTI,refract);
    materialSource->Get(AI_MATKEY_REFLECTIVITY,reflectivity);

    QVector<aiTextureType> types;
    types << aiTextureType_AMBIENT << aiTextureType_DIFFUSE << aiTextureType_DISPLACEMENT
              << aiTextureType_EMISSIVE << aiTextureType_HEIGHT << aiTextureType_LIGHTMAP
                  << aiTextureType_NORMALS << aiTextureType_OPACITY << aiTextureType_REFLECTION
                      << aiTextureType_SHININESS << aiTextureType_SPECULAR;
    for(aiTextureType type : types)
        for(uint i=0;i<materialSource->GetTextureCount(type);i++){
            CoffeeTexture::CoffeeTextureType ctype;
            switch(type){
            case aiTextureType_AMBIENT:
                ctype = CoffeeTexture::CoffeeTexture_Ambient;
                break;
            case aiTextureType_DIFFUSE:
                ctype = CoffeeTexture::CoffeeTexture_Diffusion;
                break;
            case aiTextureType_DISPLACEMENT:
                ctype = CoffeeTexture::CoffeeTexture_Displacement;
                break;
            case aiTextureType_EMISSIVE:
                ctype = CoffeeTexture::CoffeeTexture_Emissive;
                break;
            case aiTextureType_NORMALS:
                ctype = CoffeeTexture::CoffeeTexture_Bumpmap;
                break;
            case aiTextureType_HEIGHT:
                ctype = CoffeeTexture::CoffeeTexture_Heightmap;
                break;
            case aiTextureType_LIGHTMAP:
                ctype = CoffeeTexture::CoffeeTexture_Lightmap;
                break;
            case aiTextureType_OPACITY:
                ctype = CoffeeTexture::CoffeeTexture_Transparency;
                break;
            case aiTextureType_REFLECTION:
                ctype = CoffeeTexture::CoffeeTexture_Reflection;
                break;
            case aiTextureType_SHININESS:
                ctype = CoffeeTexture::CoffeeTexture_Highlight;
                break;
            case aiTextureType_SPECULAR:
                ctype = CoffeeTexture::CoffeeTexture_Specular;
                break;
            default:
                ctype = CoffeeTexture::CoffeeTexture_Undefined;
            }
            aiString textureFile;
            materialSource->GetTexture(type,i,&textureFile);
            //Problem: We need to know if the path is absolute or relative.
            //Solution: Use QFileInfo to test
            QFileInfo fileProbe(QString(textureFile.C_Str()));
            if(fileProbe.isRelative())
                fileProbe.setFile(filepath+QString(textureFile.C_Str()));
            if(!fileProbe.exists()){
                qWarning("Failed to locate texture: %s",fileProbe.fileName().toStdString().c_str());
                continue;
            }
            textures.insert(ctype,new CoffeeTexture(this,
                                                    new CoffeeResource(this,fileProbe.filePath())));
            textures.value(ctype)->setObjectName(
                        CoffeeTexture::staticMetaObject.
                        enumerator(CoffeeTexture::staticMetaObject.
                                   indexOfEnumerator("CoffeeTextureType")).
                        valueToKey(ctype));
        }


    this->setObjectName(name.C_Str());

    m_diffuseColor->setValue(glm::vec3(color.r,color.g,color.b));
    m_specularColor->setValue(glm::vec3(specular.r,specular.g,specular.b));
    m_ambientColor->setValue(glm::vec3(ambient.r,ambient.g,ambient.b));
    m_transparency->setValue(glm::vec3(transparency.r,transparency.g,transparency.b));

    setWireframe(wireframe);
    setCulling(twosided);

    m_opacity->setValue(opacity);
    m_shininess->setValue(shininess);
    m_shininessStrength->setValue(shininessStrength);
    m_refraction->setValue(refract);
    m_reflectivity->setValue(reflectivity);
}

bool CoffeeMaterial::wireframe() const
{
    return m_wireframe;
}

bool CoffeeMaterial::culling() const
{
    return m_culling;
}

GLenum CoffeeMaterial::blendMode() const
{
    return m_blendMode;
}

GLenum CoffeeMaterial::shadingMode() const
{
    return m_shadingMode;
}

QObject *CoffeeMaterial::shininessVariant() const
{
    return m_shininess;
}

QObject *CoffeeMaterial::shininessStrengthVariant() const
{
    return m_shininessStrength;
}

QObject *CoffeeMaterial::reflectivityVariant() const
{
    return m_reflectivity;
}

QObject *CoffeeMaterial::refractionVariant() const
{
    return m_reflectivity;
}

QObject *CoffeeMaterial::opacityVariant() const
{
    return m_opacity;
}

QObject *CoffeeMaterial::transparencyVariant() const
{
    return m_transparency;
}

QObject *CoffeeMaterial::diffuseColorVariant() const
{
    return m_diffuseColor;
}

QObject *CoffeeMaterial::specularColorVariant() const
{
    return m_specularColor;
}

QObject *CoffeeMaterial::ambientColorVariant() const
{
    return m_ambientColor;
}

QObject *CoffeeMaterial::emissiveColorVariant() const
{
    return m_emissiveColor;
}

QObject *CoffeeMaterial::colorMultiplierVariant() const
{
    return m_colorMultiplier;
}

void CoffeeMaterial::setTexture(CoffeeTexture::CoffeeTextureType id, QPointer<CoffeeTexture> texture){
    textures.insert(id,texture);
}

CoffeeTexture* CoffeeMaterial::getTexture(CoffeeTexture::CoffeeTextureType id){
    return textures.value(id);
}

CoffeeTexture* CoffeeMaterial::getTexture(int id){
    CoffeeTexture::CoffeeTextureType t = static_cast<CoffeeTexture::CoffeeTextureType>(id);
    return getTexture(t);
}

QList<CoffeeTexture::CoffeeTextureType> CoffeeMaterial::getTextureKeys() const{
    return textures.keys();
}

QVariantMap CoffeeMaterial::textureObjects() const
{
    QVariantMap m;
    for(CoffeeTexture::CoffeeTextureType t : textures.keys()){
        QString ts = CoffeeTexture::staticMetaObject.
                enumerator(CoffeeTexture::staticMetaObject.
                           indexOfEnumerator("CoffeeTextureType")).
                valueToKey(t);
        m.insert(ts,QVariant::fromValue(textures.value(t).data()));
    }
    return m;
}

QVariantList CoffeeMaterial::textureTypes() const
{
    QVariantList l;
    for(CoffeeTexture::CoffeeTextureType t : getTextureKeys())
        l.append(t);
    return l;
}

void CoffeeMaterial::unload(){
    for(QPointer<CoffeeTexture> text : textures)
        text->unload();
}

void CoffeeMaterial::setWireframe(bool wireframe)
{
    m_wireframe = wireframe;
}

void CoffeeMaterial::setCulling(bool culling)
{
    m_culling = culling;
}

void CoffeeMaterial::setBlendMode(GLenum blendMode)
{
    m_blendMode = blendMode;
}

void CoffeeMaterial::setShadingMode(GLenum shadingMode)
{
    m_shadingMode = shadingMode;
}

void CoffeeMaterial::load()
{
    //Stub
}
