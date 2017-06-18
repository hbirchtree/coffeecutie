#ifndef COFFEE_AUDIO_MIXER_H
#define COFFEE_AUDIO_MIXER_H

#include <coffee/core/CObject>
#include <coffee/core/types/tdef/integertypes.h>
#include <coffee/core/types/vector_types.h>
#include <coffee/core/coffee_message_macros.h>

namespace Coffee{
namespace CAudio{
namespace CSoundAbstraction{

//ST = stream/track/source type, BT = buffer type
template<class ST,class BT>
class CSoundDevice;

struct CSoundTransform
{
    struct Transform
    {
        Vecf3 position;
        Vecf3 velocity;
    } transform;
};

struct CSoundProperty
{

    struct
    {
        scalar inner;
        scalar outer;

        scalar outer_gain;
    } cone;

    struct Transform : public CSoundTransform::Transform
    {
        Vecf3 direction;
    } transform;

    struct
    {
        scalar min;
        scalar base;
        scalar max;
    } gain;

    struct
    {
        scalar rolloff;
        scalar ref_dist;
        scalar max_dist;
    } acoustic;

    struct
    {
        scalar pitch;
    } warp;

    union
    {
        struct{
            /* Simple tickers, still properties */
            struct{
                bool relative:1;
                bool spatial:1;
                bool looping:1;
            };
            /* Because we might make a lot of calls to the audio API,
             *  we will minimize it by allowing partial application.
             */
            u16 active;
        };
        u32 flags;
    };

    enum ActiveProperty
    {
        Cone_Inner          = 0x1,
        Cone_Outer          = 0x2,
        Cone_Outer_Gain     = 0x4,
        XF_Position         = 0x8,
        XF_Velocity         = 0x10,
        XF_Direction        = 0x20,
        Gain_Min            = 0x40,
        Gain_Base           = 0x80,
        Gain_Max            = 0x100,
        Rolloff             = 0x200,
        Ref_Dist            = 0x400,
        Max_Dist            = 0x800,
        Pitch               = 0x1000,
        Relative            = 0x2000,
        Spatialized         = 0x4000,
        Looping             = 0x8000,

        ActiveProperty_All  = 0xFFFF,
        ActiveProperty_Num  = 16,
    };
};

struct CListenerProperty
{
    struct Transform : public CSoundTransform::Transform
    {
        struct {
            Vecf3 forward;
            Vecf3 up;
        } orientation;
    } transform;

    scalar gain;
};

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
     * \param samples Number of samples of provided
     */
    virtual void fillBuffer(c_cptr data, const szptr& samples) = 0;

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

    virtual CSoundProperty const* properties() = 0;
    virtual void assignProperties(CSoundProperty const* props) = 0;
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

    virtual void assignProperties(CSoundProperty const* props) = 0;
    virtual void assignProperties(CListenerProperty const*) = 0;
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

    virtual void startStream() = 0;
    virtual void stopStream() = 0;
    virtual void pauseStream() = 0;

    /*!
     * \brief Insert data into sound stream.
     *  Implementation decides how the data is queued.
     * \param data
     * \param fmt
     * \param samples
     */
    virtual void feedData(c_cptr data,const CSoundFormat& fmt,
                          const szptr& samples) = 0;
    /*!
     * \brief Insert sound buffer into stream.
     *  Will always queue after previous invocations.
     * \param buffer
     */
    virtual void feedBuffer(CSoundBuffer<ST,BT>& buffer) = 0;

    //TODO: Add PTS processing to sound streams
    virtual void feedDataTimed(c_cptr data, const CSoundFormat& fmt,
                               const szptr& samples, const uint64& pts)
    {
        C_UNUSED(data);
        C_UNUSED(fmt);
        C_UNUSED(samples);
        C_UNUSED(pts);
        C_STUBBED("Timed data streaming");
    }

    /*!
     * \brief If this is a capture stream, this is true and there
     *  will be no use of the soundbuffer.
     *  Any data will be collected with collectSamples() function.
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
class CSoundArbiter
{
public:
    CSoundArbiter()
    {
    }
    virtual ~CSoundArbiter()
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

/*!
 * \brief A simple namespace for all our audio API features
 */
template<typename ArbiterT,typename FormatT,typename StreamT,typename BufferT>
struct AudioAPI
{
    using Arbiter = ArbiterT;

    /* Device-mixer API */
    using Device = CSoundDevice<StreamT,BufferT>;
    using Mixer = CSoundMixer<StreamT,BufferT>;

    /* Audio track API */
    using Track = CSoundTrack<StreamT,BufferT>;
    using Sample = CSoundSample<StreamT,BufferT>;
    /* Audio stream API */
    using Stream = CSoundStream<StreamT,BufferT>;

    /* Basic buffers */
    using Buffer = CSoundBuffer<StreamT,BufferT>;
    using Format = FormatT;

    using Properties = CSoundProperty;
    using ListenProperties = CListenerProperty;
};

}
}
}

#endif
