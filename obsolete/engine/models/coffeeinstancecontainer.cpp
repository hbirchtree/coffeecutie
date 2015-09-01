#include "coffeeinstancecontainer.h"

#include "opengl/helpers/renderingmethods.h"
#include "engine/physics/physicsobject.h"

#include "engine/scripting/qscriptvectorvalue.h"

#include <QtConcurrent>

CoffeeInstanceContainer::CoffeeInstanceContainer(QObject *parent) : QObject(parent)
{
    //InstanceData is not a QObject, and should not be listed in the inspector.
    //Doing this would murder performance when there are 10 000 or more objects to add every 1 seconds.
    //The details of these objects are not interesting in most cases.
    instanceAnchor = new QObject();
}

CoffeeInstanceContainer::~CoffeeInstanceContainer()
{
    delete instanceAnchor;
//    for(CoffeeInstanceData* d : instances)
//        delete d;
}

uint CoffeeInstanceContainer::instanceCount() const
{
    return instances.size();
}

CoffeeInstanceData *CoffeeInstanceContainer::getInstance(int index)
{
    if(index>instances.size())
        qFatal("Invalid instance index");
    return new CoffeeInstanceData(0,instances.at(index)); //temporary object, should be deleted
}

QObject *CoffeeInstanceContainer::getInstanceQObject(int index)
{
    return getInstance(index);
}

QVector<glm::mat4> CoffeeInstanceContainer::getData() const
{
    QVector<glm::mat4> d;

    //TODO : Implement QtConcurrent for the case where we have tons of instances
    for(CoffeeInstanceChunk* i : instances)
        d.append(RenderingMethods::translateObjectMatrix(i->position,
                                                         i->rotation,
                                                         i->scale));
    return d;
}

uintptr_t CoffeeInstanceContainer::getDataSize() const
{
    return m_tmpSize;
}

//QObjectList CoffeeInstanceContainer::instanceObjects() const
//{
//    QObjectList l;
//    for(CoffeeInstanceData* i : instances)
//        l.append(i);
//    return l;
//}

bool CoffeeInstanceContainer::renderPrepare() const
{
    return m_renderPrepare;
}

void CoffeeInstanceContainer::createInstance()
{
    instances.append(createInstanceData());
}

void CoffeeInstanceContainer::addInstance(CoffeeInstanceChunk *i)
{
    instances.append(i);
}

void CoffeeInstanceContainer::clearInstances()
{
    for(CoffeeInstanceChunk* d : instances)
        free(d);
    instances.clear();
}

void CoffeeInstanceContainer::setRenderPrepare(bool renderPrepare)
{
    m_renderPrepare = renderPrepare;
}

CoffeeInstanceChunk *CoffeeInstanceContainer::createInstanceData()
{
    CoffeeInstanceChunk* chunk = (CoffeeInstanceChunk*)malloc(sizeof(CoffeeInstanceChunk));
    chunk->position = glm::vec3();
    chunk->scale = glm::vec3(1.f,1.f,1.f);
    chunk->rotation = glm::quat();

    return chunk;

//    CoffeeInstanceData* p = new CoffeeInstanceData(glm::vec3(0,0,0),glm::quat(1,0,0,0),glm::vec3(1,1,1),0);
//    p->moveToThread(instanceAnchor->thread());
//    p->setParent(instanceAnchor);
//    return p;
}


//CoffeeInstanceData::CoffeeInstanceData(glm::vec3 pos, glm::quat rot, glm::vec3 scale, QObject *parent) : QObject(parent){
//    this->pos = new Vector3Value(this,pos);
//    this->scale = new Vector3Value(this,scale);
//    this->rot = new QuatValue(this,rot);
//}

CoffeeInstanceData::CoffeeInstanceData(QObject *parent, CoffeeInstanceChunk *chunk) :
    QObject(parent)
{
    this->pos = new Vector3Value(this,&(chunk->position));
    this->scale = new Vector3Value(this,&(chunk->scale));
    this->rot = new QuatValue(this,&(chunk->rotation));
}

QObject *CoffeeInstanceData::rotationRef()
{
    return rot;
}

QObject *CoffeeInstanceData::positionRef()
{
    return pos;
}

QObject *CoffeeInstanceData::scaleRef()
{
    return scale;
}

void CoffeeInstanceData::bindObject(PhysicsObject *target)
{
    target->getPositionObject()->attachPointers(pos->_data_val(),nullptr,nullptr);
    target->getPhysicalRotation()->attachPointers(rot->_data_val(),nullptr,nullptr);
    target->getScaleObject()->attachPointers(scale->_data_val(),nullptr,nullptr);
}
