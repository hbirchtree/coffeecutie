#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "general/physics/physicsobject.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/components/coffeematerial.h"
#include "opengl/rendering/coffeemesh.h"

class CoffeeObject : public PhysicsObject, public RenderableObject
{
public:
    glm::vec3 getPosition(){
        return v_position->getValue();
    }
    glm::vec3 getRotation(){
        return v_rotation->getValue();
    }
    glm::vec3 getScale(){
        return v_scale->getValue();
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

    QPointer<ShaderContainer> getShader(){
        return shader;
    }
    void setShader(QPointer<ShaderContainer> shader){
        this->shader = shader;
    }

    int getVertexDataSize(){
        return model->getVerticesDataSize();
    }
    FloatBuffer* getVertexData(){
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
    void setBaked(bool val){
        model->setBaked(val);
    }

    CoffeeObject(QObject* parent) : PhysicsObject(parent){
        v_model_offset = new Vector3Container(this);
        v_rotation = new Vector3Container(this);
        v_scale = new Vector3Container(this);
    }
    ~CoffeeObject(){

    }

    QPointer<Vector3Container> getRotationObject(){
        return v_rotation;
    }
    QPointer<Vector3Container> getScaleObject(){
        return v_scale;
    }

    QPointer<Vector3Container> getPositionOffsetObject(){
        return v_rotation;
    }
    void setMaterial(QPointer<CoffeeMaterial> material){
        this->material = material;
        material->addUser();
        material->setParent(this);
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
    int getVerticesCount(){
        return model->getVertices().size()*CoffeeVertex::VERTEX_COUNT;
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

public slots:
    void unloadAssets()
    {
        model->unloadMesh();
        material->unloadData();
    }

};

#endif // COFFEEOBJECT_H
