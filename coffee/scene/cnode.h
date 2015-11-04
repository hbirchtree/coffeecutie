#ifndef COFFEE_SCENE_CNODE_H
#define COFFEE_SCENE_CNODE_H

#include <coffee/core/coffee_types.h>
#include <coffee/core/graphics/cgraphicsdata.h>

namespace Coffee{
namespace CGraphicsData{

struct CNode
{
    CMat4* transform;
    byte   name[32];
};

}
}

#endif
