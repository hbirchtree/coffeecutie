#ifndef COFFEE_GRAPHICS_APIS_HMD_API_H
#define COFFEE_GRAPHICS_APIS_HMD_API_H

#include <coffee/core/CTypes>
#include "gltypes.h"

namespace Coffee{
namespace HMD{

struct CHMD_Binding
{
    enum class Eye
    {
        Left,Right,
    };

    struct Device
    {
        CString identifier() const{}

        CMat4 view(Eye) const{}

        CVec3 radianVelocity() const{}
        CVec3 velocity() const{}

        bool isConnected() const{return false;}
    };

    struct Lens
    {
        _cbasic_tmnmatrix<bigscalar,3,2> distortion; /*Why MN-matrix? Allows scaling of all components in one function.*/
    };

    static bool InitializeBinding(){}
    static bool InitializeHMD(Device*){}

    static void Shutdown(){}

    static Device* GetDevice(int32 id){return nullptr;}
    static Device* GetDefaultDevice(){return nullptr;}
};

}
}

#endif
