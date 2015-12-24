#ifndef COFFEE_AUDIO_MIXER_H
#define COFFEE_AUDIO_MIXER_H

#include <coffee/core/CBase>

namespace Coffee{
namespace CAudio{
namespace CSoundAbstraction{

//ST = stream/track/source type, BT = buffer type
template<class ST,class BT>
class CSoundDevice;

class CSoundFormat
{
public:
    virtual ~CSoundFormat()
    {
    }
    /*!
     * \brief The size of one sample with this format
     * \return
     */
    virtual uint32 sampleSize() const = 0;
    /*!
     * \brief Samplerate/frequency of sound
     * \return
     */
    virtual uint32 samplerate() const = 0;
    /*!
     * \brief Number of channels in buffer
     * \return
     */
    virtual uint16 channels() const = 0;
    /*!
     * \brief Bit-depth of buffer data
     * \return
     */
    virtual uint8 bitDepth() const = 0;

    virtual void setSamplerate(const uint32& smrt) = 0;
    virtual void setChannels(const uint16& chans) = 0;
    virtual void setBitDepth(const uint8& bitd) = 0;
};

template<class ST,class BT>
class CSoundBuffer : public CObject
{
public:
    CSoundBuffer(CObject* parent):
        CObject(parent)
    {
    }
    virtual ~CSoundBuffer()
    {
    }

    /*!
     * \brief Get the owner sound device
     * \return
     */
    virtual const CSoundDevice<ST,BT>& device() = 0;
    /*!
     * \brief Get size of buffer
     * \return
     */
    virtual szptr size() const = 0;

    /*!
     * \brief Get current sound format of buffered data
     * \return
     */
    virtual CSoundFormat& format() = 0;
    /*!
     * \brief Set a new format for the data of the buffer
     * \param fmt
     */
    virtual void setFormat(CSoundFormat& fmt) = 0;

    /*!
     * \brief Insert data into the buffer
     * \param data
     * \param size
     */
    virtual void fillBuffer(c_cptr data, const szptr& size) = 0;

    virtual BT* object() = 0;
};
template<class ST,class BT>
class CSoundSample : public CObject
{
public:
    CSoundSample(CObject* parent):
        CObject(parent)
    {
    }
    virtual ~CSoundSample()
    {
    }
    /*!
     * \brief Get the owner sound device
     * \return
     */
    virtual const CSoundDevice<ST,BT>& device() = 0;

    virtual CSoundBuffer<ST,BT>& buffer() = 0;
    virtual CSoundFormat& format() = 0;
    virtual uint64 samples() const = 0;

    virtual uint64 pts() const = 0;
    virtual void setPts(const uint64& pts) = 0;
};
template<class ST,class BT>
/*!
 * \brief Responsible for queueing up sound samples and playing them back
 */
class CSoundTrack : public CObject
{
public:
    CSoundTrack(CObject* parent):
        CObject(parent)
    {
    }
    virtual ~CSoundTrack()
    {
    }

    virtual const CSoundDevice<ST,BT>& device() = 0;
    /*!
     * \brief Queue a sound sample for playback
     * \param sample
     */
    virtual void queueSample(CSoundSample<ST,BT>& sample) = 0;
    /*!
     * \brief Progress and garbage-collect the sound track
     * \param ts
     */
    virtual void updateTrack(uint64 ts) = 0;

    virtual ST* object() = 0;
};
template<class ST,class BT>
class CSoundStream : public CObject
{
public:
    CSoundStream(CObject* parent):
        CObject(parent)
    {
    }
    virtual ~CSoundStream()
    {
    }

    virtual const CSoundDevice<ST,BT>& device() = 0;

    /*!
     * \brief Insert data into sound stream. Implementation decides how the data is queued.
     * \param data
     * \param fmt
     * \param samples
     */
    virtual void feedData(c_cptr data,const CSoundFormat& fmt,const szptr& samples) = 0;
    /*!
     * \brief Insert sound buffer into stream. Will always queue after previous invocations.
     * \param buffer
     */
    virtual void feedBuffer(CSoundBuffer<ST,BT>& buffer) = 0;

    //TODO: Add PTS processing to sound streams
    virtual void feedDataTimed(c_cptr data, const CSoundFormat& fmt,
                               const szptr& samples, const uint64& pts)
    {
        C_STUBBED("Timed data streaming");
    }

    /*!
     * \brief If this is a capture stream, this is true and there will be no use of the soundbuffer. Any data will be collected with collectSamples() function.
     * \return
     */
    virtual bool isInputStream() const = 0;
    /*!
     * \brief Collect audio samples from capture device into pointer
     * \param data
     * \param max_samples
     * \return
     */
    virtual szptr collectSamples(c_ptr data, const szptr& max_samples) = 0;

    virtual ST* object() = 0;
};
template<class ST,class BT>
class CSoundMixer : public CObject
{
public:
    CSoundMixer(CObject* parent):
        CObject(parent)
    {
    }
    virtual ~CSoundMixer()
    {
    }

    virtual uint64 createTrack() = 0;
    virtual CSoundTrack<ST,BT>& soundtrack(const uint64& track) = 0;
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
template<class ST,class BT>
class CSoundDevice : public CObject
{
public:
    CSoundDevice(CObject* parent):
        CObject(parent)
    {
    }
    virtual ~CSoundDevice()
    {
    }

    virtual CSoundMixer<ST,BT>& mixer() = 0;
    virtual CSoundFormat& outputFormat() = 0;

    virtual bool isCaptureDevice() = 0;
    virtual CSoundStream<ST,BT>& captureStreamer() = 0;

    virtual CSoundBuffer<ST,BT>& genBuffer() = 0;
    virtual CSoundSample<ST,BT>& genSample(CSoundBuffer<ST,BT> &buf,
                                           CSoundFormat &fmt) = 0;
    virtual CSoundStream<ST,BT>& genStream(CSoundFormat& fmt) = 0;
};

template<class ST,class BT>
class CSoundManager
{
public:
    CSoundManager()
    {
    }
    virtual ~CSoundManager()
    {
    }

    virtual CSoundDeviceIdentifier& defaultSoundDevice() = 0;

    virtual uint32 numberSoundDevices() = 0;
    virtual uint32 numberSoundInputDevices() = 0;

    virtual CSoundDeviceIdentifier& soundDevice(const szptr& devEnum) = 0;
    virtual CSoundDeviceIdentifier& soundInputDevice(const szptr& devEnum) = 0;

    virtual CSoundDevice<ST,BT>* createDevice(const CSoundDeviceIdentifier& id) = 0;
    /*!
     * \brief Create a capture device
     * \param p_id Parent device, if applicable
     * \param id Capture device
     * \return
     */
    virtual CSoundDevice<ST,BT>* createInputDevice(
            const CSoundDeviceIdentifier& p_id,
            const CSoundDeviceIdentifier& id) = 0;
};

}
}
}

#endif
