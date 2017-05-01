#pragma once

#include <assimp/Importer.hpp>

namespace Coffee{
namespace ASSIMP{

struct AssimpData
{
    AssimpData():
        scene(nullptr)
    {
    }

    ~AssimpData()
    {
        if(scene)
            importer.FreeScene();
    }

    ::Assimp::Importer importer;
    const ::aiScene* scene;
};

}
}
