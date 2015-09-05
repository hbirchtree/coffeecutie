#ifndef COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
#define COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H

#include "coffee.h"
#include "coffee/cdebug.h"
#include "coffee/cfiles.h"

namespace Coffee {
namespace CResourceTypes {
namespace CAssimp {

using namespace Coffee::CResources;

struct CAssimpData
{

};

class CAssimpImporters
{
private:
    CAssimpImporters();
public:
    static CAssimpData* importResource(CResource* );
};

} // namespace CAssimp
} // namespace CResourceTypes
} // namespace Coffee

#endif // COFFEE_CRESOURCETYPES_CASSIMP_CASSIMPIMPORTERS_H
