#ifndef BULLETPHYSICS_H
#define BULLETPHYSICS_H

#include "general/common.h"
#include "engine/physics/genericphysicsinterface.h"

#include "LinearMath/btAlignedObjectArray.h"

class PhysicsObject;
class btDynamicsWorld;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class btDefaultCollisionConfiguration;
class btQuaternion;
class btVector3;
class btCollisionShape;
class btRigidBody;

class ScalarValue;
class Vector3Value;
class QuatValue;

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

    static btVector3 convert_coffee(Vector3Value* v);
    static btQuaternion convert_coffee(QuatValue* q);
    static btScalar convert_coffee(ScalarValue* q);

    Q_INVOKABLE QString toString();
signals:
    void objectCollision(PhysicsObject* o1, PhysicsObject* o2);
    void physicsInitialized();

public slots:
    void handlePhysicsEvent(CoffeePhysicsEvent* event);
    void tickSimulation(float d);
    void run();

private:
    btRigidBody* createObject(CoffeePhysicsEvent* ev);
    void updateObject(CoffeePhysicsEvent *event, btRigidBody *body);
    void removeObject(PhysicsObject* obj);

    QEventLoop *evloop;

    glm::vec3 gravity;

    btDynamicsWorld* m_dynamicsWorld;
    btCollisionDispatcher* m_dispatch;
    btBroadphaseInterface* m_broadphase;
    btConstraintSolver* m_solver;
    btDefaultCollisionConfiguration* m_collideconfig;

    QVector<btCollisionShape*> m_collideshapes;
};

#endif // BULLETPHYSICS_H
