#ifndef COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
#define COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H

#include "coffee.h"
#include "coffee/cfiles.h"
#include "coffee_impl/assimp/cassimptypes.h"

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

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
