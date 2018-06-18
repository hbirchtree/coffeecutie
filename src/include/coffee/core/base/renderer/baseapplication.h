#pragma once

#include <coffee/core/CDisplay>

namespace Coffee{
namespace Display{

class BaseApplication
{
protected:
    virtual const CDProperties &initialProperties() = 0;
};

}
}