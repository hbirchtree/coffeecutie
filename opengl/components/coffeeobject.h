#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "general/physics/physicsobject.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/rendering/coffeemesh.h"

class CoffeeObject : public PhysicsObject,RenderableObject
{
public:
    //RenderableObject interface
    glm::vec3 getPosition(){
        return v_position.getValue();
    }
    glm::vec3 getRotation(){
        return v_rotation.getValue();
    }
    glm::vec3 getScale(){
        return v_scale.getValue();
    }

    QString getBumpTexture(){
        return material->bumpTexture();
    }
    QString getDiffuseTexture(){
        return material->diffuseTexture();
    }
    QString getSpecularTexture(){
        return material->specularTexture();
    }
    QString getHighlightTexture(){
        return material->highlightTexture();
    }
    QString getTransparencyTexture(){
        return material->transparencyTexture();
    }

    GLint getBumpTextureHandle(){
        return material->bumpHandle();
    }
    GLint getDiffuseTextureHandle(){
        return material->diffuseHandle();
    }
    GLint getSpecularTextureHandle(){
        return material->specularHandle();
    }
    GLint getHighlightTextureHandle(){
        return material->highlightHandle();
    }
    GLint getTransparencyTextureHandle(){
        return material->transparencyHandle();
    }
    void setBumpTextureHandle(GLint handle){
        material->setBumpHandle(handle);
    }
    void setDiffuseTextureHandle(GLint handle){
        material->setDiffuseHandle(handle);
    }
    void setSpecularTextureHandle(GLint handle){
        material->setSpecularHandle(handle);
    }
    void setHighlightTextureHandle(GLint handle){
        material->setHighlightHandle(handle);
    }
    void setTransparencyTextureHandle(GLint handle){
        material->setTransparencyHandle(handle);
    }

    GLint getVaoHandle(){

    }

    GLint getVboHandle() = 0;
    void setVaoHandle(GLint handle) = 0;
    void setVboHandle(GLint handle) = 0;

    glm::vec3 getSpecularColor(){
        return material->specularColor();
    }
    glm::vec3 getColorMultiplier(){
        return material->colorMultiplier();
    }

    float getShininess(){
        return material->shininess()->getValue();
    }
    float getTransparency(){
        return material->transparency()->getValue();
    }

    QPointer<ShaderContainer> getShader(){
        return &shader;
    }
    void setShader(QPointer<ShaderContainer> shader){
        this->shader = shader;
    }

    void render(){

    }
    int getVertexDataSize(){
        return model->getDataSize();
    }
    GLfloat* getVertexData(){
        return model->getData();
    }

    bool isStreamDraw(){
        return model->streamDraw();
    }
    bool isDepthTest(){
        return model->depthTest();
    }
    bool isDrawn(){
        return model->drawn();
    }
    bool isBaked(){
        return model->baked();
    }


    CoffeeObject(){}
    ~CoffeeObject(){}

    QPointer<Vector3Container> getRotationObject(){
        return &v_rotation;
    }
    QPointer<Vector3Container> getPositionOffsetObject(){
        return &v_rotation;
    }
    void setMaterial(QPointer<CoffeeMaterial> material){
        this->material = material;
    }

private:
    QPointer<CoffeeMesh> model = new CoffeeMesh();
    QPointer<CoffeeMaterial> material = new CoffeeMaterial();
    QPointer<ShaderContainer> shader;

    QString vertShader;
    QString fragShader;

    Vector3Container v_model_offset;
    Vector3Container v_rotation;
    Vector3Container v_scale;
};

#endif // COFFEEOBJECT_H
