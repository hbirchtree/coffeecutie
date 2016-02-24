#pragma once

#include <coffee/core/CDisplay>

namespace Coffee{
namespace CDisplay{

class BaseApplication
{
protected:
    virtual const CDProperties &initialProperties() = 0;
};

}
}