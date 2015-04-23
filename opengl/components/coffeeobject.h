#ifndef COFFEEOBJECT_H
#define COFFEEOBJECT_H

#include "general/physics/physicsobject.h"
#include "opengl/rendering/renderableobject.h"

class CoffeeObject : public PhysicsObject,RenderableObject
{
public:
    CoffeeObject(){

    }
    ~CoffeeObject(){

    }

    QPointer<Vector3Container> getRotationObject(){
        return &v_rotation;
    }
    QPointer<Vector3Container> getPositionOffsetObject(){
        return &v_rotation;
    }

    glm::vec3 getPosition() const{
        return v_position.getValue();
    }
    QPointer<ShaderContainer> getShader(){
        return &shader;
    }

private:
    QPointer<ShaderContainer> shader;

    QString vertShader;
    QString fragShader;

    Vector3Container v_model_offset;
    Vector3Container v_rotation;
};

#endif // COFFEEOBJECT_H
