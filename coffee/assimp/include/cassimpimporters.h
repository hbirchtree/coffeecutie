#ifndef COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
#define COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H

#include <coffee.h>
#include <base/cfiles.h>
#include "cassimptypes.h"

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

using namespace Coffee::CResources;

class CAssimpImporters
{
private:
    CAssimpImporters();
public:
    static CAssimpData* importResource(CResource* source,
                                       cstring hint = nullptr);
};

extern void coffee_assimp_free(CAssimpData* data);

extern cstring assimp_reflexive_string_get(
        const void *basePtr,
        const assimp_reflexive& ref);

extern bool coffee_assimp_dump_mesh(
        CAssimp::CAssimpMesh* mesh, CResources::CResource* resource);

extern byte_t* coffee_assimp_get_reflexive_ptr(void* baseptr, const assimp_reflexive* ref);

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
