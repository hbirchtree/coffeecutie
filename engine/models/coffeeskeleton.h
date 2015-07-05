#ifndef COFFEESKELETON_H
#define COFFEESKELETON_H

#include "general/common.h"

#include "assimp/matrix4x4.h"
#include "assimp/vector3.h"

class CoffeeSkeletonException
{
public:
    enum SkeletonError {
        Incomplete,
        InvalidAnimation,
        InvalidWeights
    };
    CoffeeSkeletonException(SkeletonError t);
    SkeletonError type();
private:
    SkeletonError m_type = Incomplete;
};

class CoffeeBone : public QObject
{
public:
    enum DataType{
        SkeletonBone //Contains weight
    };
    CoffeeBone(QObject* parent, DataType t);
    QMap<uint,float> weights;
    glm::mat4 transform;
    glm::mat4 offset;
    DataType type() const;

private:
    DataType m_type;
};

class CoffeeSkeleton : public QObject
{
    Q_OBJECT
public:
    CoffeeSkeleton(QObject* parent,aiBone** bones, uint numBones,aiNode* rootNode);

    static glm::mat4 assimpToMat4(aiMatrix4x4 &m);
    static glm::vec3 assimpToVec3(aiVector3D &v);
    static glm::quat assimpToQuat(aiQuaternion &v);
private:
    bool validateWeights();
    QVector<CoffeeBone*> bones;
};

template<class T> class CoffeeAnimationStep {
public:
    double timestamp;
    T value;
};

class CoffeeBoneAnimation : public QObject{
public:
    enum AnimationType {
        Linear,
        Constant,
        Repeat,
    };
    AnimationType type = Linear;
    QVector<CoffeeAnimationStep<glm::vec3>*> positionSteps;
    QVector<CoffeeAnimationStep<glm::vec3>*> scaleSteps;
    QVector<CoffeeAnimationStep<glm::quat>*> rotationSteps;
};

class CoffeeAnimation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double duration READ duration)
    Q_PROPERTY(double tickRate READ tickRate)
    Q_PROPERTY(uint numVertices READ numVertices)

    double m_tickRate;
    double m_duration;

public:
    CoffeeAnimation(QObject* parent,aiAnimation* anim,aiNode* rootNode);
    double tickRate() const;
    double duration() const;

    uint numVertices() const;

private:
    QVector<CoffeeBoneAnimation*> boneAnimations;
    uint m_numVertices;
};

#endif // COFFEESKELETON_H
