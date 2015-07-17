#ifndef COFFEEUNIFORMSETTER_H
#define COFFEEUNIFORMSETTER_H

#include "general/common.h"

class VectorData;
class CoffeeTexture;
class CoffeeShader;

class ScalarValue;
class Vector2Value;
class Vector3Value;
class Matrix4Value;

class ShaderMapping {
public:
    QString uniform;
    VectorData* data;
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

    void setUniform(const QString &uniformName, VectorData* data);

    void setTexture(const QString &samplerName, CoffeeTexture *texture);

    void clearUniforms();
    void clearTextures();

protected:
    CoffeeUniformSetter();
    QVector<ShaderMapping*> uniforms;
    QVector<TextureMapping*> textures;
    QPointer<CoffeeShader> pshader;
};

#endif // COFFEEUNIFORMSETTER_H
