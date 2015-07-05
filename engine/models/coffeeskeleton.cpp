#include "coffeeskeleton.h"

CoffeeSkeleton::CoffeeSkeleton(QObject *parent, aiBone **bones, uint numBones, aiNode *rootNode) : QObject(parent)
{
    for(uint b=0;b<numBones;b++){
        aiBone* bone = bones[b];
        CoffeeBone* cbone = new CoffeeBone(this,CoffeeBone::SkeletonBone);
        aiNode* node = rootNode->FindNode(bone->mName.C_Str());
        if(node){
            cbone->transform = assimpToMat4(node->mTransformation);
        }
        cbone->setObjectName(bone->mName.C_Str());
        cbone->offset = assimpToMat4(bone->mOffsetMatrix);
        for(uint w=0;w<bone->mNumWeights;w++){
            aiVertexWeight weight = bone->mWeights[w];
            cbone->weights.insert(weight.mVertexId,weight.mWeight);
        }
        this->bones.append(cbone);
    }
    if(!validateWeights())
        throw CoffeeSkeletonException(CoffeeSkeletonException::InvalidWeights);
    qDebug("Created skeleton: %i bones",this->bones.size());
}

glm::mat4 CoffeeSkeleton::assimpToMat4(aiMatrix4x4 &m){
    glm::mat4 g;
    g[0][0] = m.a1;
    g[0][1] = m.a2;
    g[0][2] = m.a3;
    g[0][3] = m.a4;

    g[1][0] = m.b1;
    g[1][1] = m.b2;
    g[1][2] = m.b3;
    g[1][3] = m.b4;

    g[2][0] = m.c1;
    g[2][1] = m.c2;
    g[2][2] = m.c3;
    g[2][3] = m.c4;

    g[3][0] = m.d1;
    g[3][1] = m.d2;
    g[3][2] = m.d3;
    g[3][3] = m.d4;
    return g;
}

glm::vec3 CoffeeSkeleton::assimpToVec3(aiVector3D &v)
{
    return glm::vec3(v.x,v.y,v.z);
}

glm::quat CoffeeSkeleton::assimpToQuat(aiQuaternion &v)
{
    return glm::quat(v.w,v.x,v.y,v.z);
}

bool CoffeeSkeleton::validateWeights()
{
    QMap<uint,float> weights;
    for(CoffeeBone* b : bones)
        for(uint id : b->weights.keys())
            weights.insert(id,weights.value(id,0.f)+b->weights.value(id,0.f));
    for(float f : weights.values())
        if(!f==1.f){
            return false;
        }
    return true;
}

CoffeeBone::CoffeeBone(QObject *parent, DataType t) : QObject(parent)
{
    m_type = t;
}

CoffeeBone::DataType CoffeeBone::type() const
{
    return m_type;
}

CoffeeSkeletonException::CoffeeSkeletonException(CoffeeSkeletonException::SkeletonError t)
{
    m_type = t;
}

CoffeeSkeletonException::SkeletonError CoffeeSkeletonException::type()
{
    return m_type;
}

CoffeeAnimation::CoffeeAnimation(QObject *parent, aiAnimation *anim, aiNode *rootNode) : QObject(parent)
{
    m_tickRate = anim->mTicksPerSecond;
    m_duration = anim->mDuration;
    for(uint i=0;i<anim->mNumChannels;i++){
        aiNodeAnim* node = anim->mChannels[i];
        CoffeeBoneAnimation* bone = new CoffeeBoneAnimation();
        bone->setObjectName(node->mNodeName.C_Str());
        switch(node->mPostState){
        case aiAnimBehaviour_LINEAR:
            bone->type = CoffeeBoneAnimation::Linear;
            break;
        case aiAnimBehaviour_REPEAT:
            bone->type = CoffeeBoneAnimation::Repeat;
            break;
        default:
            bone->type = CoffeeBoneAnimation::Constant;
        }

        for(uint j=0;j<node->mNumPositionKeys;j++){
            CoffeeAnimationStep<glm::vec3>* step = new CoffeeAnimationStep<glm::vec3>();
            step->timestamp = node->mPositionKeys[j].mTime;
            step->value = CoffeeSkeleton::assimpToVec3(node->mPositionKeys[j].mValue);
            bone->positionSteps.append(step);
        }
        for(uint j=0;j<node->mNumScalingKeys;j++){
            CoffeeAnimationStep<glm::vec3>* step = new CoffeeAnimationStep<glm::vec3>();
            step->timestamp = node->mScalingKeys[j].mTime;
            step->value = CoffeeSkeleton::assimpToVec3(node->mScalingKeys[j].mValue);
            bone->scaleSteps.append(step);
        }
        for(uint j=0;j<node->mNumRotationKeys;j++){
            CoffeeAnimationStep<glm::quat>* step = new CoffeeAnimationStep<glm::quat>();
            step->timestamp = node->mRotationKeys[j].mTime;
            step->value = CoffeeSkeleton::assimpToQuat(node->mRotationKeys[j].mValue);
            bone->rotationSteps.append(step);
        }
        boneAnimations.append(bone);
    }
}

double CoffeeAnimation::tickRate() const
{
    return m_tickRate;
}

double CoffeeAnimation::duration() const
{
    return m_duration;
}

uint CoffeeAnimation::numVertices() const
{
    return m_numVertices;
}
