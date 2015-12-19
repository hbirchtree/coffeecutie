#ifndef COFFEE_AUDIO_OPENAL_OPENAL_AUDIO_ABSTRACTION_H
#define COFFEE_AUDIO_OPENAL_OPENAL_AUDIO_ABSTRACTION_H

#include "../caudiomixer.h"
#include "copenal.h"

namespace Coffee{
namespace CAudio{
namespace COpenAL{

using namespace CSoundAbstraction;

class CALSoundFormat : public CSoundFormat
{
    uint32 i_samrate;
    uint16 i_channels;
    uint8 i_bitd;

public:
    CALSoundFormat();

    uint32 sampleSize();
    uint32 samplerate();
    uint16 channels();
    uint8 bitDepth();
    void setSamplerate(const uint32 &smrt);
    void setChannels(const uint16 &chans);
    void setBitDepth(const uint8 &bitd);
};

class CALSoundDeviceIdentifier : public CSoundDeviceIdentifier
{
    szptr i_idx;
    cstring s_id;
public:
    CALSoundDeviceIdentifier(const szptr& idx, cstring sid);

    szptr deviceIndex() const;
    cstring stringIdentifier() const;
};

class CALSoundStream : public CSoundStream
{
public:
    const CSoundDevice &device();
    CSoundBuffer &buffer();
    void setSoundBuffer(CSoundBuffer &dest);
    bool isInputStream();
};

class CALSoundMixer : public CSoundMixer
{
public:
    uint64 createTrack();
    CSoundTrack &soundtrack(const uint64 &track);
};

class CALSoundDevice : public CSoundDevice
{
    CALContext* m_ctxt = nullptr;
    bool b_input = false;
    CALSoundMixer m_mixer;
    CALSoundFormat m_format;
    CALSoundStream m_inputstream;

public:
    CALSoundDevice(const CSoundDeviceIdentifier &dev, bool input);

    CSoundMixer &mixer();
    CSoundFormat &outputFormat();
    bool isCaptureDevice();
    CSoundStream &captureStreamer();
};

class CALSoundManager : public CSoundManager
{
    uint32 d_idx = 0;

    uint32 odevices = 0;
    uint32 idevices = 0;
    cstring* a_idevices = nullptr;
    cstring* a_odevices = nullptr;
    std::vector<CALSoundDeviceIdentifier> v_idevices;
    std::vector<CALSoundDeviceIdentifier> v_odevices;

public:
    CALSoundManager();

    CSoundDeviceIdentifier &defaultSoundDevice();
    uint32 numberSoundDevices();
    uint32 numberSoundInputDevices();
    CSoundDeviceIdentifier &soundDevice(const szptr &devEnum);
    CSoundDeviceIdentifier &soundInputDevice(const szptr &devEnum);
    CSoundDevice *createDevice(const CSoundDeviceIdentifier &id);
    CSoundDevice *createInputDevice(const CSoundDeviceIdentifier &id);
};

}
}
}

#endif
