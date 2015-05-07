#include "bulletphysics.h"

BulletPhysics::BulletPhysics(QObject *parent, const glm::vec3 &gravity) : QThread(parent)
{
    qDebug("Creating physics system: type=%s,gravity=%s,instance=%p","Bullet",QStringFunctions::toString(gravity).toStdString().c_str(),this);
    m_collideconfig = new btDefaultCollisionConfiguration();
    m_dispatch = new btCollisionDispatcher(m_collideconfig);
    m_broadphase = new btDbvtBroadphase();

    m_solver = new btSequentialImpulseConstraintSolver(); //Might want to use look at multi-threaded solver?

    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatch,m_broadphase,m_solver,m_collideconfig);
    m_dynamicsWorld->setGravity(convert_glm(gravity));
}

BulletPhysics::~BulletPhysics()
{
    int i;

    for(i=0;i<m_dynamicsWorld->getNumConstraints();i++){
        m_dynamicsWorld->removeConstraint(m_dynamicsWorld->getConstraint(i));
    }

    for(i=0;i<m_dynamicsWorld->getNumCollisionObjects();i++){
        btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if(body && body->getMotionState())
            delete body->getMotionState();
        m_dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }

    for(i=0;i<m_collideshapes.size();i++)
        delete m_collideshapes[i];
    m_collideshapes.clear();

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_broadphase;
    delete m_dispatch;
    delete m_collideconfig;
    qDebug("Deleted physics system: type=%s,instance=%p","Bullet",this);
}

btVector3 BulletPhysics::convert_glm(const glm::vec3 &v)
{
    return btVector3(v.x,v.y,v.z);
}

glm::vec3 BulletPhysics::convert_bt(const btVector3 &v)
{
    //btScalar is apparently a typedef of float?
    //If this is incorrect, all the code will be simple to change anyway.
    return glm::vec3((float)v.getX(),(float)v.getY(),(float)v.getZ());
}

void BulletPhysics::addObject(PhysicsObject *object)
{

}

void BulletPhysics::tickSimulation(float d)
{
    //We know that, as long as the object is created, there is an instance of btDynamicsWorld
    m_dynamicsWorld->stepSimulation(d);
}
