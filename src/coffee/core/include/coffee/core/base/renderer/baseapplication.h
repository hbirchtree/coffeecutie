#pragma once

#include <coffee/core/CDisplay>

namespace Coffee{
namespace Display{

class BaseApplication
{
protected:
    virtual const Properties &initialProperties() = 0;
};

}
}
