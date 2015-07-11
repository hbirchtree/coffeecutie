#ifndef COFFEEUNIFORMSETTER_H
#define COFFEEUNIFORMSETTER_H

#include "general/common.h"

class ShaderVariant;
class CoffeeTexture;
class CoffeeShader;

class ShaderMapping {
public:
    QString uniform;
    QPointer<ShaderVariant> data;
    bool constant = false;
};
class TextureMapping {
public:
    bool operator==(const TextureMapping& val);
    QPointer<CoffeeTexture> texture;
    QString samplerName;

    bool loaded = false;
};

class CoffeeUniformSetter
{
public:
    void applyUniforms();

    void bindTextures();
    void unbindTextures();

    void setUniform(QString uniformName,ShaderVariant* data);
    void setTexture(QString samplerName, CoffeeTexture *texture);

    void clearUniforms();
    void clearTextures();

protected:
    CoffeeUniformSetter();
    QVector<ShaderMapping*> uniforms;
    QVector<TextureMapping*> textures;
    QPointer<CoffeeShader> pshader;

};

#endif // COFFEEUNIFORMSETTER_H
