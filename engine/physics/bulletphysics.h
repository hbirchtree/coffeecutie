#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H

#include <QEventLoop>
#include "general/common.h"
#include "engine/physics/genericphysicsinterface.h"
#include "general/qstringfunctions.h"
#include "engine/physics/physicsobject.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
class BulletPhysics : public QObject
{
    Q_PROPERTY(QString gravity READ getGravity)
    Q_PROPERTY(int objects READ getObjectsCount)

    Q_OBJECT
public:
    BulletPhysics(QObject *parent, const glm::vec3 &gravity);
    ~BulletPhysics();
    QString systemName();

    QString getGravity() const;
    int getObjectsCount() const;

    static btQuaternion convert_glm(const glm::quat &v);
    static glm::quat convert_bt(const btQuaternion &v);
    static btVector3 convert_glm(const glm::vec3 &v);
    static glm::vec3 convert_bt(const btVector3 &v);
    static void internalTickCallback(btDynamicsWorld* wrld,btScalar timestep);
signals:
    void objectCollision(QPointer<PhysicsObject> o1, QPointer<PhysicsObject> o2);
    void physicsInitialized();

    void stopping();

public slots:
    void updateObject(PhysicsObject* object,GenericPhysicsInterface::PhysicsProperty prop);
    void addObject(PhysicsObject* object);
    void removeObject(void* pointer);
    void removeObject(PhysicsObject* object);
    void tickSimulation(float d);

    void run();
    void stopThread();

private:
    glm::vec3 gravity;

    btDynamicsWorld* m_dynamicsWorld;
    btCollisionDispatcher* m_dispatch;
    btBroadphaseInterface* m_broadphase;
    btConstraintSolver* m_solver;
    btDefaultCollisionConfiguration* m_collideconfig;

    btAlignedObjectArray<btCollisionShape*> m_collideshapes;
};

#endif // BULLETPHYSICS_H
