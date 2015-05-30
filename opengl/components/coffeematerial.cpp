#include "coffeematerial.h"


CoffeeMaterial::CoffeeMaterial(QObject *parent) : QObject(parent){
    m_shininess = new NumberContainer<float>(this,1.f);
    m_shininessStrength = new NumberContainer<float>(this,1.f);
    m_refraction = new NumberContainer<float>(this,1.f);
    m_reflectivity = new NumberContainer<float>(this,0.f);
    m_opacity = new NumberContainer<float>(this,1.f);

    m_transparency = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    m_diffuseColor = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    m_specularColor = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    m_ambientColor = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    m_colorMultiplier = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
}

CoffeeMaterial::CoffeeMaterial(CoffeeMaterial &mtl) : CoffeeMaterial(0){
    m_transparency = new NumberContainer<glm::vec3>(this,mtl.transparency());
    m_shininess = new NumberContainer<float>(this,mtl.shininess());
    *m_specularColor = glm::vec3(mtl.specularColor());
    *m_colorMultiplier = glm::vec3(mtl.colorMultiplier());
}

CoffeeMaterial::CoffeeMaterial(QObject *parent, const aiMaterial *materialSource, const QString& filepath) : CoffeeMaterial(parent){
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
        for(int i=0;i<materialSource->GetTextureCount(type);i++){
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
            case aiTextureType_HEIGHT:
                ctype = CoffeeTexture::CoffeeTexture_Heightmap;
                break;
            case aiTextureType_LIGHTMAP:
                ctype = CoffeeTexture::CoffeeTexture_Lightmap;
                break;
            case aiTextureType_NORMALS:
                ctype = CoffeeTexture::CoffeeTexture_Bumpmap;
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
            }
            aiString textureFile;
            materialSource->GetTexture(type,i,&textureFile);
            textures.insert(ctype,new CoffeeTexture(this,
                                                    filepath+QString(textureFile.C_Str())));
        }


    this->setObjectName(name.C_Str());

    setDiffuseColor(glm::vec3(color.r,color.g,color.b));
    setSpecularColor(glm::vec3(specular.r,specular.g,specular.b));
    setAmbientColor(glm::vec3(ambient.r,ambient.g,ambient.b));
    setTransparency(glm::vec3(transparency.r,transparency.g,transparency.b));

    setWireframe(wireframe);
    setCulling(twosided);

    setOpacity(opacity);
    setShininess(shininess);
    setShininessStrength(shininessStrength);
    setRefraction(refract);
    setReflectivity(reflectivity);
}

glm::vec3 CoffeeMaterial::transparency() const
{
    return m_transparency->getValue();
}

QPointer<NumberContainer<glm::vec3> > CoffeeMaterial::getTransparency()
{
    return m_transparency;
}

float CoffeeMaterial::shininess() const
{
    return m_shininess->getValue();
}

QPointer<NumberContainer<float>> CoffeeMaterial::getShininess()
{
    return m_shininess;
}

glm::vec3 CoffeeMaterial::specularColor() const
{
    return m_specularColor->getValue();
}

glm::vec3 CoffeeMaterial::colorMultiplier() const
{
    return m_colorMultiplier->getValue();
}

glm::vec3 CoffeeMaterial::diffuseColor() const{
    return m_diffuseColor->getValue();
}

void CoffeeMaterial::setDiffuseColor(const glm::vec3 &diffuseColor){
    *m_diffuseColor = diffuseColor;
}

bool CoffeeMaterial::wireframe() const
{
    return m_wireframe;
}

bool CoffeeMaterial::culling() const
{
    return m_culling;
}

glm::vec3 CoffeeMaterial::ambientColor() const
{
    return m_ambientColor->getValue();
}

float CoffeeMaterial::shininessStrength() const
{
    return m_shininessStrength->getValue();
}

float CoffeeMaterial::refraction() const
{
    return m_refraction->getValue();
}

GLenum CoffeeMaterial::blendMode() const
{
    return m_blendMode;
}

GLenum CoffeeMaterial::shadingMode() const
{
    return m_shadingMode;
}

float CoffeeMaterial::reflectivity() const
{
    return m_reflectivity->getValue();
}

float CoffeeMaterial::opacity() const
{
    return m_opacity->getValue();
}

void CoffeeMaterial::setTexture(CoffeeTexture::CoffeeTextureType id, QPointer<CoffeeTexture> texture){
    textures.insert(id,texture);
}

QPointer<CoffeeTexture> CoffeeMaterial::getTexture(CoffeeTexture::CoffeeTextureType id){
    return textures.value(id);
}

QList<CoffeeTexture::CoffeeTextureType> CoffeeMaterial::getTextureKeys(){
    return textures.keys();
}

void CoffeeMaterial::setTransparency(const glm::vec3 &arg)
{
    *m_transparency = arg;
}

void CoffeeMaterial::setShininess(float arg)
{
    *m_shininess = arg;
}

void CoffeeMaterial::setSpecularColor(const glm::vec3& arg)
{
    if (*m_specularColor == arg)
        return;

    *m_specularColor = arg;
}

void CoffeeMaterial::setColorMultiplier(const glm::vec3 &arg)
{
    if (*m_colorMultiplier == arg)
        return;

    *m_colorMultiplier = arg;
}

void CoffeeMaterial::unloadData(){
    for(QPointer<CoffeeTexture> text : textures)
        text->unloadTexture();
}

void CoffeeMaterial::setReflectivity(float reflectivity)
{
    *m_reflectivity = reflectivity;
}

void CoffeeMaterial::setOpacity(float opacity)
{
    *m_opacity = opacity;
}

void CoffeeMaterial::setWireframe(bool wireframe)
{
    m_wireframe = wireframe;
}

void CoffeeMaterial::setCulling(bool culling)
{
    m_culling = culling;
}

void CoffeeMaterial::setAmbientColor(const glm::vec3& ambientColor)
{
    *m_ambientColor = ambientColor;
}

void CoffeeMaterial::setShininessStrength(float shininessStrength)
{
    *m_shininessStrength = shininessStrength;
}

void CoffeeMaterial::setRefraction(float refraction)
{
    *m_refraction = refraction;
}

void CoffeeMaterial::setBlendMode(GLenum blendMode)
{
    m_blendMode = blendMode;
}

void CoffeeMaterial::setShadingMode(GLenum shadingMode)
{
    m_shadingMode = shadingMode;
}
