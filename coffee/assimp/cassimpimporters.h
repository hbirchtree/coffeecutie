#ifndef COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
#define COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H

#include <coffee/core/coffee.h>
#include <coffee/core/base/cfiles.h>
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

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
