#ifndef COFFEE_AUDIO_MIXER_H
#define COFFEE_AUDIO_MIXER_H

#include <coffee/core/CBase>

namespace Coffee{
namespace CAudio{
namespace CSoundAbstraction{

class CSoundDevice;

class CSoundFormat
{
public:
    virtual uint32 sampleSize() = 0;
    virtual uint32 samplerate() = 0;
    virtual uint16 channels() = 0;
    virtual uint8 bitDepth() = 0;

    virtual void setSamplerate(const uint32& smrt) = 0;
    virtual void setChannels(const uint16& chans) = 0;
    virtual void setBitDepth(const uint8& bitd) = 0;
};

class CSoundBuffer : public CObject
{
public:
    virtual const CSoundDevice& device() = 0;

    virtual szptr size() const = 0;

    virtual CSoundFormat& format() = 0;
    virtual void setFormat(CSoundFormat& fmt) = 0;

    virtual void fillBuffer(c_cptr data, const szptr& size) = 0;
};

class CSoundSample : public CObject
{
public:
    virtual CSoundBuffer& buffer() = 0;
    virtual CSoundFormat& format() = 0;
    virtual uint64 samples() = 0;

    virtual uint64 pts() = 0;
    virtual void setPts() = 0;
};

/*!
 * \brief Responsible for queueing up sound samples and playing them back
 */
class CSoundTrack : public CObject
{
public:
    virtual const CSoundDevice& device() = 0;

    virtual void queueSample(const CSoundSample& sample) = 0;
    virtual void updateTrack(uint64 ts) = 0;
};

class CSoundStream : public CObject
{
public:
    virtual const CSoundDevice& device() = 0;

    virtual CSoundBuffer& buffer() = 0;
    virtual void setSoundBuffer(CSoundBuffer& dest) = 0;

    virtual bool isInputStream() = 0;
};

class CSoundMixer : public CObject
{
public:
    virtual uint64 createTrack() = 0;
    virtual CSoundTrack& soundtrack(const uint64& track) = 0;
};

class CSoundDeviceIdentifier
{
public:
    virtual szptr deviceIndex() const = 0;
    virtual cstring stringIdentifier() const
    {
        return nullptr;
    }
    virtual uint64 numericIdentifier() const
    {
        return 0;
    }
};

class CSoundDevice : public CObject
{
public:
    virtual CSoundMixer& mixer() = 0;
    virtual CSoundFormat& outputFormat() = 0;

    virtual bool isCaptureDevice() = 0;
    virtual CSoundStream& captureStreamer() = 0;
};

class CSoundManager
{
public:
    virtual CSoundDeviceIdentifier& defaultSoundDevice() = 0;

    virtual uint32 numberSoundDevices() = 0;
    virtual uint32 numberSoundInputDevices() = 0;

    virtual CSoundDeviceIdentifier& soundDevice(const szptr& devEnum) = 0;
    virtual CSoundDeviceIdentifier& soundInputDevice(const szptr& devEnum) = 0;

    virtual CSoundDevice* createDevice(const CSoundDeviceIdentifier& id) = 0;
    virtual CSoundDevice* createInputDevice(const CSoundDeviceIdentifier& id) = 0;
};

}
}
}

#endif
