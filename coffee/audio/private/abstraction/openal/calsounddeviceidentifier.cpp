#include <abstraction/openal/calsounddeviceidentifier.h>

namespace Coffee{
namespace CAudio{
namespace COpenAL{

CALSoundDeviceIdentifier::CALSoundDeviceIdentifier(const szptr &idx, cstring sid):
    i_idx(idx),
    s_id(sid)
{
}
szptr CALSoundDeviceIdentifier::deviceIndex() const
{
    return i_idx;
}
cstring CALSoundDeviceIdentifier::stringIdentifier() const
{
    return s_id;
}

}
}
}
