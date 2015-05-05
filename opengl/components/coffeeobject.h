#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "engine/physics/physicsobject.h"
#include "opengl/rendering/renderableobject.h"
#include "opengl/components/coffeematerial.h"
#include "engine/models/coffeemesh.h"
#include "general/data/numbercontainer.h"
#include "opengl/components/shadercontainer.h"
#include "engine/rendering/coffeerenderingmethod.h"

class CoffeeObject : public PhysicsObject, public RenderableObject
{
public:
    glm::vec3 getPosition() const{
        return v_position->getValue();
    }
    glm::quat getRotation() const{
        return v_rotation->getValue();
    }
    glm::vec3 getScale() const{
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
        this->shader->setObjectName(this->objectName()+".shader");
    }

    int getVertexDataSize(){
        return model->getVerticesDataSize();
    }
    NumberBuffer<GLfloat>* getVertexData(){
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
        v_model_offset = new NumberContainer<glm::vec3>(this,glm::vec3(0,0,0));
        v_rotation = new NumberContainer<glm::quat>(this,glm::quat(0,0,0,0));
        v_scale = new NumberContainer<glm::vec3>(this,glm::vec3(1,1,1));
    }
    ~CoffeeObject(){

    }

    QPointer<NumberContainer<glm::quat>> getRotationObject(){
        return v_rotation;
    }
    QPointer<NumberContainer<glm::vec3>> getScaleObject(){
        return v_scale;
    }

    QPointer<NumberContainer<glm::vec3>> getPositionOffsetObject(){
        return v_model_offset;
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
    void render(){
        if(renderer)
            renderer->render();
        else
            qDebug() << "failed to render!";
    }

    QPointer<CoffeeRenderingMethod> getRenderer()
    {
    return renderer;
    }

    void setRenderer(QPointer<CoffeeRenderingMethod> value)
    {
        renderer = value;
    }


    QString getVertShader() const
    {
        return vertShader;
    }

    void setVertShader(const QString &value)
    {
        vertShader = value;
    }
    QString getFragShader() const
    {
        return fragShader;
    }

    void setFragShader(const QString &value)
    {
        fragShader = value;
    }

private:
    QPointer<CoffeeRenderingMethod> renderer;
    QPointer<CoffeeMesh> model;
    QPointer<CoffeeMaterial> material;
    QPointer<ShaderContainer> shader;

    QString vertShader;
    QString fragShader;

    QPointer<NumberContainer<glm::vec3>> v_model_offset;
    QPointer<NumberContainer<glm::quat> > v_rotation;
    QPointer<NumberContainer<glm::vec3>> v_scale;

public slots:
    void unloadAssets()
    {
        model->unloadMesh();
        material->unloadData();
        model->setBaked(false);
    }

};

#endif // COFFEEOBJECT_H
