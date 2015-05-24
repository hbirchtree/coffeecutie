#ifndef COFFEEPARTICLESYSTEM_H
#define COFFEEPARTICLESYSTEM_H

#include <QObject>
#include "engine/objects/coffeesimpleobject.h"
#include "opengl/components/coffeecamera.h"

class CoffeeParticleSystem : public CoffeeSimpleObject
{
public:
    CoffeeParticleSystem(QObject *parent, const CoffeeCamera *camera);

    glm::vec3 getPosition() const;
    QPointer<NumberContainer<glm::vec3>> getPositionObject();
    void setPosition(const glm::vec3& pos);

protected:
    const CoffeeCamera* camera;

    CoffeeTexture* sprite;

    QPointer<NumberContainer<glm::vec3>> pos;
};

#endif // COFFEEPARTICLESYSTEM_H
