#pragma once

#include <coffee/core/stl_types.h>

struct aiScene;

namespace Assimp {
class Importer;
}

namespace Coffee {
/*!
 * \brief ASSIMP bindings with utility functions
 */
namespace ASSIMP {

struct AssimpData;

struct AssimpData_deleter
{
    void operator()(AssimpData* data);
};

struct Importer_deleter
{
    void operator()(::Assimp::Importer* importer);
};

struct AssimpData
{
    UqPtr<::Assimp::Importer, Importer_deleter> importer;
    const ::aiScene*                            scene;
};

} // namespace ASSIMP
} // namespace Coffee
