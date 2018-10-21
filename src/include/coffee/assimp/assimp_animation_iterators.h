#pragma once

#include "assimp_data.h"
#include "assimp_iterators.h"
#include "cassimpimporters.h"

#include <assimp/anim.h>
#include <assimp/scene.h>

namespace Coffee {
namespace ASSIMP {

struct AnimationParser
{
    using vert_idx = u64;
    using bone_idx = u32;

    enum class ChannelType : u8
    {
        Node = 0x0,
        Bone = 0x1,
    };

    template<typename T>
    struct RefArray
    {
        u64 offset;
        u64 num;

        /* TODO: Define operators */
    };

    template<typename T>
    struct HeapArray
    {
        u64 offset;
        u64 num;

        /* TODO: Define operators */
    };

    struct Weight : non_copy
    {
        scalar   weight;
        bone_idx boneId;
    };

    struct VertexWeights : non_copy
    {
        vert_idx         vertexId;
        RefArray<Weight> weights;
    };

    struct Bone : non_copy
    {
        u64   name;
        Matf4 offset;
    };

    /*!
     * \brief Describes the following structure:
     *
     * |-----------------------------------|
     * | Vertex --> Weight --> Bone matrix |
     * |-----------------------------------|
     *
     * The bone array tells us which matrices need to be loaded
     * The vertex array tells us about the weights and references to bones
     * The weight structure could be mapped directly to shaders
     *
     */
    struct Armature : non_copy
    {
        u64                     name;
        RefArray<Bone>          bones;
        RefArray<VertexWeights> vertices;
    };

    struct AnimChannel : non_copy
    {
        u64 nodeName;
        union
        {
            struct
            {
                bone_idx id;       /*!< ID for the bone */
                u32      armature; /*!< Index of the armature */
            } bone;

            u64 nodeId; /*!< Refers to a node in the scene hierarchy */
        } ref;
        ChannelType type;
    };

    struct Animation : non_copy
    {
        u64                   name;
        RefArray<AnimChannel> channels;
    };

    struct AnimationHeader : non_copy
    {
        HeapArray<Weight>        weights;
        HeapArray<VertexWeights> vertices;
        HeapArray<Bone>          bones;
        HeapArray<Armature>      armatures;

        HeapArray<Animation> animations;
    };

    struct AnimationHeap
    {
        AnimationHeader header;

        Vector<Weight>        weights;
        Vector<VertexWeights> vertices;
        Vector<Bone>          bones;
        Vector<Armature>      armatures;

        Vector<AnimChannel> channels;
        Vector<Animation>   animations;
    };

    struct StringData
    {
        szptr           storeSize;
        Vector<cstring> references;
    };

    NO_DISCARD static u64 addString(StringData& store, ::aiString const& string)
    {
        szptr stringIndex = store.storeSize;

        store.references.push_back(string.C_Str());
        store.storeSize += str::len(string.C_Str()) + 1;

        return stringIndex;
    }

    static bool Extract(
        AssimpPtr&                        scene,
        MeshLoader::SerialNodeList const& nodes,
        AnimationHeap&                    heap)
    {
        using NodeData = MeshLoader::SerialNodeList::NodeData;

        if(scene->scene->mNumAnimations == 0)
            return false;

        StringData stringStore = {};
        stringStore.storeSize  = 0;

        stringStore.references.reserve(scene->scene->mNumAnimations);

        Vector<Bone>&          outBones     = heap.bones;
        Vector<Weight>&        outWeights   = heap.weights;
        Vector<VertexWeights>& outVertices  = heap.vertices;
        Vector<Armature>&      outArmatures = heap.armatures;

        Vector<AnimChannel>& outChannels   = heap.channels;
        Vector<Animation>&   outAnimations = heap.animations;

        auto animations = scene->scene->mAnimations;
        auto meshes     = scene->scene->mMeshes;

        fprintf(stderr, "Root: %s\n", scene->scene->mRootNode->mName.C_Str());

        /* 1. extract bones, slap it in a block */
        for(auto meshIdx : Range<>(scene->scene->mNumMeshes))
        {
            auto& mesh = *meshes[meshIdx];

            if(mesh.mNumBones == 0)
                continue;

            outArmatures.emplace_back();

            auto& outArmature           = outArmatures.back();
            outArmature.name            = addString(stringStore, mesh.mName);
            outArmature.bones.offset    = outBones.size();
            outArmature.vertices.offset = outVertices.size();

            /* Sort weights by vertex */
            Map<vert_idx, Vector<Weight>> vertexWeights;

            fprintf(stderr, "Mesh bones: %s\n", mesh.mName.C_Str());
            for(auto boneIdx : Range<>(mesh.mNumBones))
            {
                outBones.emplace_back();

                auto& bone    = *mesh.mBones[boneIdx];
                auto& outBone = outBones.back();

                outBone.name = addString(stringStore, bone.mName);

                fprintf(stderr, "- Bone: %s\n", bone.mName.C_Str());

                for(auto weightIdx : Range<>(bone.mNumWeights))
                {
                    auto& weight     = bone.mWeights[weightIdx];
                    auto& vertexPool = vertexWeights[weight.mVertexId];

                    vertexPool.push_back({});
                    vertexPool.back().weight = weight.mWeight;
                    vertexPool.back().boneId = C_FCAST<bone_idx>(boneIdx);
                }
            }

            outArmature.bones.num = outBones.size() - outArmature.bones.offset;

            /* Insert weights into final weight heap,
             *  storing reference to the weights */
            for(auto& v : vertexWeights)
            {
                outVertices.emplace_back();
                auto& outVertex = outVertices.back();

                outVertex.vertexId       = v.first;
                outVertex.weights.offset = outWeights.size();

                outWeights.reserve(outWeights.size() + v.second.size());

                std::move(
                    v.second.begin(),
                    v.second.end(),
                    std::back_inserter(outWeights));

                outVertex.weights.num =
                    outWeights.size() - outVertex.weights.offset;
            }

            outArmature.vertices.num =
                outVertices.size() - outArmature.vertices.offset;
        }

        /* 2. extract all animations, store them in a block */
        for(auto i : Range<>(scene->scene->mNumAnimations))
        {
            outAnimations.emplace_back();

            auto& animation    = *animations[i];
            auto& outAnimation = outAnimations.back();
            outAnimation.name  = addString(stringStore, animation.mName);

            outAnimation.channels.offset = outChannels.size();

            fprintf(stderr, "Animation: %s\n", animation.mName.C_Str());

            /* Node animations */
            for(auto channelIdx : Range<>(animation.mNumChannels))
            {
                outChannels.emplace_back();

                auto& channel       = *animation.mChannels[channelIdx];
                auto& outChannel    = outChannels.back();
                outChannel.nodeName = addString(stringStore, channel.mNodeName);

                auto nodePredicate = [&](NodeData const& node) {
                    return nodes.stringStore.at(node.objectName) ==
                           channel.mNodeName.C_Str();
                };

                auto nodeIt = std::find_if(
                    nodes.nodes.begin(), nodes.nodes.end(), nodePredicate);

                if(nodeIt == nodes.nodes.end())
                    continue;

                auto& nodeRef =
                    *scene->scene->mRootNode->FindNode(channel.mNodeName);

                outChannel.type =
                    nodeRef.mNumMeshes ? ChannelType::Node : ChannelType::Bone;

                if(outChannel.type == ChannelType::Bone)

                    fprintf(
                        stderr,
                        "- Channel for %s -> %i\n",
                        channel.mNodeName.C_Str(),
                        nodeIt->mesh);
            }

            outAnimation.channels.num =
                outChannels.size() - outAnimation.channels.offset;

            if(animation.mNumMeshChannels)
                fprintf(
                    stderr,
                    "%u mesh channels found!\n",
                    animation.mNumMeshChannels);

            if(animation.mNumMorphMeshChannels)
                fprintf(
                    stderr,
                    "%u morph channels found!\n",
                    animation.mNumMorphMeshChannels);
        }

        return true;
    }
};

} // namespace ASSIMP
} // namespace Coffee
