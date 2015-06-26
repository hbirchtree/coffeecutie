#include "bulletphysics.h"

#include "general/qstringfunctions.h"
#include "engine/physics/physicsobject.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "LinearMath/btVector3.h"
#include "engine/objects/coffeeobjectfactory.h"
#include "engine/physics/physicsdescriptor.h"
#include <QTimer>

BulletPhysics::BulletPhysics(QObject *parent, const glm::vec3 &gravity) : QObject(parent)
{
    this->gravity = gravity;
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

    for(btCollisionShape* p : m_collideshapes)
        delete p;
    m_collideshapes.clear();

    delete m_dynamicsWorld;
    delete m_solver;
    delete m_broadphase;
    delete m_dispatch;
    delete m_collideconfig;
    qDebug("Deleted physics system: type=%s,instance=%p",systemName().toStdString().c_str(),this);
}

QString BulletPhysics::systemName()
{
    return "Bullet";
}

QString BulletPhysics::getGravity() const
{
    return QStringFunctions::toString(convert_bt(m_dynamicsWorld->getGravity()));
}

int BulletPhysics::getObjectsCount() const
{
    return m_dynamicsWorld->getCollisionObjectArray().size();
}

btQuaternion BulletPhysics::convert_glm(const glm::quat &v)
{
    return btQuaternion(v.w,v.x,v.y,v.z);
}

glm::quat BulletPhysics::convert_bt(const btQuaternion &v)
{
    return glm::quat(v.w(),v.x(),v.y(),v.z());
}

btVector3 BulletPhysics::convert_glm(const glm::vec3 &v)
{
    return btVector3(v.x,v.y,v.z);
}

glm::vec3 BulletPhysics::convert_bt(const btVector3 &v)
{
    //btScalar may be a typedef of float or double. It would be trivial swapping between them with this function.
    return glm::vec3((float)v.getX(),(float)v.getY(),(float)v.getZ());
}

void BulletPhysics::addObject(PhysicsObject *object)
{
    if(!object->getDescr())
        return;
    PhysicsDescriptor* desc = object->getDescr();
    btCollisionShape* shape = nullptr;
    switch(desc->shape()){
    case PhysicsDescriptor::Shape_Box:
        shape = new btBoxShape(convert_glm(
                                   CoffeeObjectFactory::varListToVec3(desc->scale())));
        break;
    case PhysicsDescriptor::Shape_Sphere:
        shape = new btSphereShape(desc->scale().at(0).toFloat());
        break;
    case PhysicsDescriptor::Shape_Cylinder:
        shape = new btCylinderShape(convert_glm(
                                        CoffeeObjectFactory::varListToVec3(desc->scale())));
        break;
    case PhysicsDescriptor::Shape_Capsule:
        shape = new btCapsuleShape(desc->scale().at(0).toFloat(),
                                   desc->scale().at(1).toFloat());
        break;
    case PhysicsDescriptor::Shape_Cone:
        shape = new btConeShape(desc->scale().at(0).toFloat(),
                                desc->scale().at(1).toFloat());
        break;
    case PhysicsDescriptor::Shape_StaticPlane:
        shape = new btStaticPlaneShape(convert_glm(
                                           CoffeeObjectFactory::varListToVec3(desc->normal())),
                                       desc->scale().at(0).toFloat());
        break;
    default:
        return;
    }
    m_collideshapes.push_back(shape);

    glm::quat rot = CoffeeObjectFactory::varListToQuat(desc->orientation());
    btDefaultMotionState* mstate = new btDefaultMotionState(btTransform(
                                                                btQuaternion(rot.x,rot.y,rot.z,rot.w),
                                    convert_glm(CoffeeObjectFactory::varListToVec3(desc->position()))));
    btRigidBody::btRigidBodyConstructionInfo ci(desc->mass(),
                                                mstate,
                                                shape,
                                                convert_glm(CoffeeObjectFactory::varListToVec3(desc->inertia())));
    btRigidBody* rb = new btRigidBody(ci);

    rb->setAngularVelocity(
                convert_glm(CoffeeObjectFactory::varListToVec3(desc->inertia())));
    rb->setFriction(desc->friction());
    rb->setRestitution(desc->restitution());

    rb->setUserPointer(object);
    object->setPhysicspointer(rb);

    connect(object,SIGNAL(deleteObject(void*)),SLOT(removeObject(void*)));
    connect(object,SIGNAL(propertyModified(CoffeePhysicsEvent*)),SLOT(updateObject(CoffeePhysicsEvent*)));

    m_dynamicsWorld->addRigidBody(rb);
    qDebug("Object added to %s physics: %s",systemName().toStdString().c_str(),
           object->objectName().toStdString().c_str());
}

void BulletPhysics::removeObject(void *pointer)
{
    if(pointer){
        m_dynamicsWorld->removeRigidBody((btRigidBody*)pointer);
        qDebug("Removed unparented physics object, instance=%p",
               this);
    }else
        qDebug("Failed to remove unparented physics object, instance=%p",
               this);
}

void BulletPhysics::removeObject(PhysicsObject *object)
{
    if(object&&object->getPhysicspointer()){
        m_dynamicsWorld->removeRigidBody((btRigidBody*)object->getPhysicspointer());
        qDebug("Removed physics object: %s, instance=%p",
               object->objectName().toStdString().c_str(),
               this);
    }else
        qDebug("Failed to remove physics object: %s, instance=%p",
               (object ? object->objectName().toStdString().c_str() : "unidentified"),
               this);
}

void BulletPhysics::tickSimulation(float d)
{
    //We know that, as long as the object is created, there is an instance of btDynamicsWorld
    //If not, we are pretty screwed to begin with.
    m_dynamicsWorld->stepSimulation(d);
}

void BulletPhysics::run()
{
    qDebug("Creating physics system: type=%s,gravity=%s,instance=%p,thread=%p",
           systemName().toStdString().c_str(),
           QStringFunctions::toString(gravity).toStdString().c_str(),
           this,this->thread());
    m_collideconfig = new btDefaultCollisionConfiguration();
    m_dispatch = new btCollisionDispatcher(m_collideconfig);
    m_broadphase = new btDbvtBroadphase();

    m_solver = new btSequentialImpulseConstraintSolver(); //Might want to use look at multi-threaded solver?

    m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatch,m_broadphase,m_solver,m_collideconfig);
    m_dynamicsWorld->setGravity(convert_glm(gravity));

    m_dynamicsWorld->setInternalTickCallback(internalTickCallback);
    m_dynamicsWorld->setWorldUserInfo(this);

    QTimer t;

    t.setInterval(10);

    connect(&t,&QTimer::timeout,[=](){
        tickSimulation(10);
    });

    t.start();

    evloop = new QEventLoop();
    evloop->exec();
}

void BulletPhysics::updateObject(CoffeePhysicsEvent *event)
{
    for(PhysicsObject* ev : *event->targetsList())
        if(ev->getPhysicspointer()){
            btRigidBody* obj = (btRigidBody*)ev->getPhysicspointer();
            switch(event->type()){
            case CoffeePhysicsEvent::ActionSetTransform:{
                btVector3 v1 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(0))->getRawVec3());
                btQuaternion v2 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(1))->getRawQuat());
                btTransform pt = obj->getWorldTransform();
                pt.setOrigin(v1);
                pt.setRotation(v2);
                obj->setWorldTransform(pt);
                break;
            }
            case CoffeePhysicsEvent::ActionSetPosition:{
                btVector3 v1 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(0))->getRawVec3());
                btTransform pt = obj->getWorldTransform();
                pt.setOrigin(v1);
                obj->setWorldTransform(pt);
                break;
            }
            case CoffeePhysicsEvent::ActionSetAngularVelocity:
                break;
            case CoffeePhysicsEvent::ActionSetOrientation:{
                btQuaternion v1 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(0))->getRawQuat());
                btTransform rt = obj->getWorldTransform();
                rt.setRotation(v1);
                obj->setWorldTransform(rt);
                break;
            }
            case CoffeePhysicsEvent::ActionApplyForce:{
                btVector3 v1 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(0))->getRawVec3());
                obj->applyCentralForce(v1);
                obj->activate(true);
                break;
            }
            case CoffeePhysicsEvent::ActionApplyRelativeForce:
                break;
            case CoffeePhysicsEvent::ActionApplyImpulse:{
                btVector3 v1 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(0))->getRawVec3());
                obj->applyCentralImpulse(v1);
                obj->activate(true);
                break;
            }
            case CoffeePhysicsEvent::ActionApplyRelativeImpulse:{
                btVector3 v1 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(0))->getRawVec3());
                btVector3 v2 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(1))->getRawVec3());
                obj->applyImpulse(v1,v2);
                obj->activate(true);
                break;
            }
            case CoffeePhysicsEvent::ActionApplyTorque:{
                btVector3 v1 = convert_glm(qvariant_cast<VectorVariant*>(event->getData().at(0))->getRawVec3());
                obj->applyTorque(v1);
                obj->activate(true);
                break;
            }
            default:
                break;
            }
        }
}

void BulletPhysics::internalTickCallback(btDynamicsWorld *wrld, btScalar timestep)
{
    Q_UNUSED(timestep);
    //Update values
    btCollisionObjectArray objects = wrld->getCollisionObjectArray();
    for(int i=0;i<objects.size();i++){
        btRigidBody* rb = btRigidBody::upcast(objects.at(i));
        if(!rb)
            continue;
        PhysicsObject* obj = (PhysicsObject*)rb->getUserPointer();
        btTransform t;
        rb->getMotionState()->getWorldTransform(t);
        obj->updatePosition(convert_bt(t.getOrigin()));
        obj->updateVelocity(convert_bt(rb->getLinearVelocity()));
        obj->updateAcceleration(convert_bt(rb->getTotalForce()));
        btQuaternion r = t.getRotation();
        obj->updateRotation(convert_bt(r));
        glm::quat av = glm::quat(2.f,convert_bt(rb->getAngularVelocity()));
        obj->updateAngularVelocity(av);
    }
    //Report collisions
    if(wrld->getWorldUserInfo()){
        BulletPhysics* ct = (BulletPhysics*)wrld->getWorldUserInfo();
        for(int i=0;i<wrld->getDispatcher()->getNumManifolds();i++){
            btPersistentManifold* man = wrld->getDispatcher()->getManifoldByIndexInternal(i);
            btRigidBody* b1 = (btRigidBody*)man->getBody0();
            btRigidBody* b2 = (btRigidBody*)man->getBody1();
            for(int j=0;j<man->getNumContacts();j++){
                btManifoldPoint mp = man->getContactPoint(i);
                if(mp.getDistance()<0.f)
                    if(b1->getUserPointer()&&b2->getUserPointer())
                        ct->objectCollision((PhysicsObject*)b1->getUserPointer(),
                                            (PhysicsObject*)b2->getUserPointer());
            }
        }
    }
}

void BulletPhysics::setGravity(float x, float y, float z)
{
    gravity.x = x;
    gravity.y = y;
    gravity.z = z;
    qDebug() << QStringFunctions::toString(gravity);
    m_dynamicsWorld->setGravity(convert_glm(gravity));
}
