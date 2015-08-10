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

btRigidBody *BulletPhysics::createObject(CoffeePhysicsEvent *ev)
{
    //ev should not be null
    //shape should not be NullShape
    //intent should be creation
    if(!(ev||
         ev->getInt(CoffeePhysicsEvent::ShapeProperty)||
         ev->intent()==CoffeePhysicsEvent::CreateIntent))
        return nullptr;

    CoffeePhysicsEvent::PhysicShape v_shape =
            static_cast<CoffeePhysicsEvent::PhysicShape>
            (ev->getInt(CoffeePhysicsEvent::ShapeProperty));
    btVector3 scale =
            convert_glm(ev->getVector3(CoffeePhysicsEvent::ScaleProperty));

    btCollisionShape* shape = nullptr;

    switch(v_shape){
    case CoffeePhysicsEvent::SphereShape:
        shape = new btSphereShape(scale.x());
        break;
    case CoffeePhysicsEvent::BoxShape:
        shape = new btBoxShape(scale);
        break;
    case CoffeePhysicsEvent::CylinderShape:
        shape = new btCylinderShape(scale);
        break;
    case CoffeePhysicsEvent::CapsuleShape:
        shape = new btCapsuleShape(scale.x(),scale.y());
        break;
    case CoffeePhysicsEvent::ConeShape:
        shape = new btConeShape(scale.x(),scale.y());
        break;
    case CoffeePhysicsEvent::StaticPlaneShape:{
        shape = new btStaticPlaneShape(
                    convert_glm(ev->getVector3(CoffeePhysicsEvent::OrientationProperty)),
                    (btScalar)ev->getScalar(CoffeePhysicsEvent::PlaneConstantProperty));
        break;
    }
    default:
        return nullptr; //TODO : Implement the other shapes
    }
    m_collideshapes.push_back(shape);

    btTransform transform,comTransform;
    transform.setOrigin(
                convert_glm(ev->getVector3(CoffeePhysicsEvent::PositionProperty)));
    transform.setRotation(
                convert_glm(ev->getQuaternion(CoffeePhysicsEvent::OrientationProperty)));

    comTransform.setOrigin(
                convert_glm(ev->getVector3(CoffeePhysicsEvent::CMPositionProperty)));

    btDefaultMotionState* mstate = new btDefaultMotionState(transform,comTransform);

    btVector3 localInertia =
            convert_glm(ev->getVector3(CoffeePhysicsEvent::LocalInertiaProperty));

    btRigidBody::btRigidBodyConstructionInfo ci(ev->getScalar(CoffeePhysicsEvent::MassProperty),
                                                mstate,
                                                shape,
                                                localInertia);

    return new btRigidBody(ci);
}

void BulletPhysics::updateObject(CoffeePhysicsEvent *event,btRigidBody* body)
{
    for(uint32_t property_raw : event->getPropertyList()){
        CoffeePhysicsEvent::PropertyEnum prop =
                static_cast<CoffeePhysicsEvent::PropertyEnum>(property_raw);
        switch(prop){
        case CoffeePhysicsEvent::ForceApplication:
            body->applyCentralForce(convert_glm(event->getVector3(prop)));
            break;
        case CoffeePhysicsEvent::RelativeForceApplication:
            body->applyForce(convert_glm(event->getVector3(prop)),
                             convert_glm(event->getVector3(CoffeePhysicsEvent::RelativePositionProperty)));
            break;
        case CoffeePhysicsEvent::ImpulseApplication:
            body->applyCentralImpulse(convert_glm(event->getVector3(prop)));
            break;
        case CoffeePhysicsEvent::RelativeImpulseApplication:
            body->applyImpulse(convert_glm(event->getVector3(prop)),
                             convert_glm(event->getVector3(CoffeePhysicsEvent::RelativePositionProperty)));
            break;
        case CoffeePhysicsEvent::TorqueApplication:
            break;
        case CoffeePhysicsEvent::TorqueImpulseApplication:
            break;
        case CoffeePhysicsEvent::ScaleProperty:
            break;
        case CoffeePhysicsEvent::PositionProperty:
            break;
        case CoffeePhysicsEvent::VelocityProperty:
            break;
        case CoffeePhysicsEvent::AccelerationProperty:
            break;
        case CoffeePhysicsEvent::CMPositionProperty:
            break;
        case CoffeePhysicsEvent::OrientationProperty:
            break;
        case CoffeePhysicsEvent::AngularVelocityProperty:
            break;
        case CoffeePhysicsEvent::MassProperty:
            break;
        case CoffeePhysicsEvent::LocalInertiaProperty:
            break;
        case CoffeePhysicsEvent::ActivationStateProperty:
            break;
        case CoffeePhysicsEvent::AngularFactorProperty:
            break;
        case CoffeePhysicsEvent::FrictionProperty:
            break;
        case CoffeePhysicsEvent::RestitutionProperty:
            break;
        case CoffeePhysicsEvent::RollingFrictionProperty:
            break;
        case CoffeePhysicsEvent::LinearFactorProperty:
            break;
        case CoffeePhysicsEvent::PlaneConstantProperty:
            break;
        default:
            break;
        }
    }
}

void BulletPhysics::removeObject(PhysicsObject *obj)
{
    if(!obj||!obj->getPhysicspointer())
        return;
    m_dynamicsWorld->removeRigidBody((btRigidBody*)obj->getPhysicspointer());
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

        obj->getPositionObject()->setValue(convert_bt(t.getOrigin()));
        obj->getPositionObject()->setVelocity(convert_bt(rb->getLinearVelocity()));
        obj->getPositionObject()->setAcceleration(convert_bt(rb->getTotalForce()
                                                             +rb->getGravity()));
        btQuaternion r = t.getRotation();
        obj->getPhysicalRotation()->setValue(glm::normalize(convert_bt(r)));

        glm::quat av = glm::quat(0.f,convert_bt(rb->getAngularVelocity()));
        obj->getPhysicalRotation()->setVelocity(av);
    }
    //Report collisions
    if(wrld->getWorldUserInfo()){
        BulletPhysics* ct = (BulletPhysics*)wrld->getWorldUserInfo();
        for(uint32_t i=0;i<wrld->getDispatcher()->getNumManifolds();i++){
            btPersistentManifold* man = wrld->getDispatcher()->getManifoldByIndexInternal(i);
            btRigidBody* b1 = (btRigidBody*)man->getBody0();
            btRigidBody* b2 = (btRigidBody*)man->getBody1();
            for(uint32_t j=0;j<man->getNumContacts();j++){
                btManifoldPoint mp = man->getContactPoint(i);
                if(mp.getDistance()<0.f)
                    if(b1->getUserPointer()&&b2->getUserPointer())
                        ct->objectCollision((PhysicsObject*)b1->getUserPointer(),
                                            (PhysicsObject*)b2->getUserPointer());
            }
        }
    }
}

btVector3 BulletPhysics::convert_coffee(Vector3Value *v)
{
    if(!v) //If it is an invalid cast
        return btVector3();
    glm::vec3 val = v->getValue();
    return btVector3(val.x,val.y,val.z);
}

btQuaternion BulletPhysics::convert_coffee(QuatValue *q)
{
    if(!q) //If it is an invalid cast
        return btQuaternion();
    glm::quat val = q->getValue();
    return btQuaternion(val.x,val.y,val.z,val.w);
}

btScalar BulletPhysics::convert_coffee(ScalarValue *q)
{
    if(!q)
        return 0.f;
    return q->getValue();
}

QString BulletPhysics::toString()
{
    return QString("Bullet Physics(instance=0x%1,objects=%2)")
            .arg(QString::number((uintptr_t)this,16))
            .arg(getObjectsCount());
}

void BulletPhysics::handlePhysicsEvent(CoffeePhysicsEvent *event)
{
    switch(event->intent()){
    case CoffeePhysicsEvent::CreateIntent:{
        if(event->physicsTargets().size()!=1)
            qFatal("Unsupported operation: Assigning same btRigidBody to several PhysicsObject instances");
        PhysicsObject* cobj = event->physicsTargets().first();
        btRigidBody* body = createObject(event);
        if(!body)
            qFatal("Failed to create physics-object: Invalid parameters");
        body->setUserPointer(cobj);
        cobj->setPhysicspointer(body);

        connect(cobj,SIGNAL(deleteObject(void*)),SLOT(removePhysicsObject(void*)));
        connect(cobj,SIGNAL(propertyModified(CoffeePhysicsEvent*)),
                SLOT(handlePhysicsEvent(CoffeePhysicsEvent*)));

        qDebug() << "Physics:" << body;

        m_dynamicsWorld->addRigidBody(body);

    }break;
    case CoffeePhysicsEvent::DefineIntent:
        for(PhysicsObject* o : event->physicsTargets())
            updateObject(event,(btRigidBody*)o->getPhysicspointer());
        break;
    case CoffeePhysicsEvent::RemoveIntent:
        for(PhysicsObject* o : event->physicsTargets())
            removeObject(o);
        break;
    default:
        break;
    }
    if(event->dispose())
        event->deleteLater();
}

void BulletPhysics::removePhysicsObject(void *p)
{
    m_dynamicsWorld->removeRigidBody((btRigidBody*)p);
}
