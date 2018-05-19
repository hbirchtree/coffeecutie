#pragma once

#include <Importer.hpp>

namespace Coffee{
/*!
 * \brief ASSIMP bindings with utility functions
 */
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
