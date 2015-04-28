#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "general/physics/physicsobject.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/rendering/coffeemesh.h"

class CoffeeObject : public PhysicsObject, public RenderableObject
{
public:

    //RenderableObject interface
    glm::vec3 getPosition(){
        return v_position->getValue();
    }
    glm::vec3 getRotation(){
        return v_rotation->getValue();
    }
    glm::vec3 getScale(){
        return v_scale->getValue();
    }

    void setTexture(int id,QPointer<CoffeeTexture> texture){
        material->setTexture(id,texture);
    }
    QPointer<CoffeeTexture> getTexture(int id){
        return material->getTexture(id);
    }
    GLint getTextureHandle(int id){
        return material->getTexture(id)->getHandle();
    }

    GLint getVaoHandle(){
        return model->vaoHandle();
    }

    GLint getVboHandle(){
        return model->vboHandle();
    }
    void setVaoHandle(GLint handle){
        model->setVaoHandle(handle);
    }
    void setVboHandle(GLint handle){
        model->setVboHandle(handle);
    }

    glm::vec3 getSpecularColor(){
        return material->specularColor();
    }
    glm::vec3 getColorMultiplier(){
        return material->colorMultiplier();
    }

    float getShininess(){
        return material->shininess().getValue();
    }
    float getTransparency(){
        return material->transparency().getValue();
    }

    QPointer<ShaderContainer> getShader(){
        return shader;
    }
    void setShader(QPointer<ShaderContainer> shader){
        this->shader = shader;
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

    CoffeeObject(QObject* parent) : PhysicsObject(parent){}
    ~CoffeeObject(){

    }

    QPointer<Vector3Container> getRotationObject(){
        return v_rotation;
    }
    QPointer<Vector3Container> getPositionOffsetObject(){
        return v_rotation;
    }
    void setMaterial(QPointer<CoffeeMaterial> material){
        this->material = material;
    }
    QPointer<CoffeeMaterial> getMaterial(){
        return material;
    }

    QPointer<CoffeeMesh> getModel(){
        return model;
    }
    void setModel(QPointer<CoffeeMesh> model){
        this->model = model;
    }

private:
    QPointer<CoffeeMesh> model;
    QPointer<CoffeeMaterial> material;
    QPointer<ShaderContainer> shader;

    QString vertShader;
    QString fragShader;

    QPointer<Vector3Container> v_model_offset;
    QPointer<Vector3Container> v_rotation;
    QPointer<Vector3Container> v_scale;

    // RenderableObject interface
public slots:
    void unloadAssets()
    {
        model->unloadMesh();
        material->unloadData();
    }
};

#endif // COFFEEOBJECT_H
