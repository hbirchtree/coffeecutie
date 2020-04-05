#include <coffee/audio/abstraction/openal/calsounddeviceidentifier.h>

namespace Coffee {
namespace CAudio {
namespace COpenAL {

CALSoundDeviceIdentifier::CALSoundDeviceIdentifier(szptr idx, cstring sid) :
    i_idx(idx), s_id(sid)
{
}

CALSoundDeviceIdentifier::~CALSoundDeviceIdentifier()
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

} // namespace COpenAL
} // namespace CAudio
} // namespace Coffee
