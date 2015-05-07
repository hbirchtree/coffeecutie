#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H

#include "general/common.h"
#include "general/qstringfunctions.h"
#include "engine/physics/physicsobject.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
class BulletPhysics : public QThread
{
    Q_OBJECT
public:
    BulletPhysics(QObject *parent, const glm::vec3 &gravity);
    ~BulletPhysics();

    static btVector3 convert_glm(const glm::vec3 &v);
    static glm::vec3 convert_bt(const btVector3 &v);
signals:
    void objectCollision();

public slots:
    void addObject(PhysicsObject* object);
    void tickSimulation(float d);

private:
    btDynamicsWorld* m_dynamicsWorld;
    btCollisionDispatcher* m_dispatch;
    btBroadphaseInterface* m_broadphase;
    btConstraintSolver* m_solver;
    btDefaultCollisionConfiguration* m_collideconfig;

    btAlignedObjectArray<btCollisionObject*> m_collideobjects;
    btAlignedObjectArray<btCollisionShape*> m_collideshapes;
};

#endif // BULLETPHYSICS_H
