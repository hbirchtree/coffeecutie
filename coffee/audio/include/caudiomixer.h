#ifndef COFFEE_AUDIO_MIXER_H
#define COFFEE_AUDIO_MIXER_H

#include <coffee/core/CBase>
#include "openal/copenal.h"
#include "caudio.h"

namespace Coffee{
namespace CAudio{
namespace CSoundMixer{

using namespace COpenAL;

struct CSoundDescriptor
{
    enum Flags : uint8
    {
        Loops = 0x1,
    };

    Flags flags;
    scalar gain;
    scalar pitch;

    scalar maxDist;
    scalar refDist;

    scalar rolloff;

    CVec3 position;
    CVec3 velocity;
    CVec3 direction;
};

struct CSoundByte
{
    CSoundDescriptor desc;
    CAudioSample sample;
    uint64 pts; //usecs
    uint64 length; //msecs
    uint8 track;
};

class CAudioMixer
{
public:
    CAudioMixer()
    {
        m_quit_flag.store(false);
        m_running.store(false);
        m_al_clock = coffee_fun_alloc_timer_micro();

        std::function<void()> audio_kickstart = [=]()
        {
            m_al_context = coffee_audio_context_create();

            if(!m_al_context)
                m_quit_flag.store(true);

            m_running.store(true);

            uint64 start_pts = m_al_clock->elapsed();
            uint64 curr_pts = 0;

            while(!m_quit_flag.load())
            {
                curr_pts = m_al_clock->elapsed();

                //Process newly added audio bytes

                m_queue_mutex.lock();
                for(const CSoundByte& sb : m_queued_sounds)
                {
                    _amix_soundbyte sbi;
                    sbi.size = coffee_audio_sample_get_datasize(sb.sample.fmt,
                                                                sb.sample.fmt.samples);
                    sbi.sbyte = sb;

                    //Calculate start PTS and end PTS
                    sbi.playing = false;
                    sbi.start_pts = curr_pts-start_pts+sb.pts;
                    sbi.end_pts = sbi.start_pts+sb.length*1000;

                    //Create a buffer
                    CALBuffer* buffer = new CALBuffer;
                    coffee_audio_alloc(buffer);
                    coffee_audio_buffer_data(buffer,&sb.sample);
                    m_al_buffers.push_back(buffer);
                    sbi.buffer = buffer;

                    //Create a source
                    CALSource* src = new CALSource;
                    coffee_audio_alloc(src);
                    coffee_audio_source_seti(src,CSourceProperty::Looping,
                                             (sb.desc.flags) ? 1 : 0);
                    coffee_audio_source_setf(src,CSourceProperty::Gain,
                                             sb.desc.gain);
                    coffee_audio_source_setf(src,CSourceProperty::Pitch,
                                             sb.desc.pitch);
                    coffee_audio_source_setf(src,CSourceProperty::MaxDist,
                                             sb.desc.maxDist);
                    coffee_audio_source_setf(src,CSourceProperty::ReferenceDistance,
                                             sb.desc.refDist);
                    coffee_audio_source_setf(src,CSourceProperty::RolloffFactor,
                                             sb.desc.rolloff);

                    m_al_sources.push_back(src);
                    sbi.source = src;

                    m_internal_queue.push_back(sbi);
                }
                m_queue_mutex.unlock();

                //Process what is going to be sent to speakers
                curr_pts = m_al_clock->elapsed();
                for(const _amix_soundbyte& mix : m_internal_queue)
                {
                    //If
                    if(curr_pts >= mix.start_pts && curr_pts < mix.end_pts)
                    {
                        if(!mix.playing)
                        {
                            coffee_audio_source_queue_buffers(
                                        mix.source,1,&mix.buffer);
                        }
                    }else if(curr_pts > mix.end_pts)
                    {
                        coffee_audio_free(mix.buffer);
                        coffee_audio_free(mix.source);
                    }
                }
            }

            for(CALBuffer* buff : m_al_buffers)
                coffee_audio_free(buff);
            for(CALSource* src : m_al_sources)
                coffee_audio_free(src);
            coffee_audio_context_destroy(m_al_context);
        };
        m_audio_thread = CThreading::runAsync(audio_kickstart);
    }
    ~CAudioMixer()
    {
        m_quit_flag.store(true);
        m_audio_thread.get();
        coffee_fun_free(m_al_clock);
    }
    void addQueue(const CSoundByte& soundbyte)
    {
        m_queue_mutex.lock();
        m_queued_sounds.push_back(soundbyte);
        m_queue_mutex.unlock();
    }

private:
    std::future<void> m_audio_thread;

    //Foreign data
    std::mutex m_queue_mutex;
    std::vector<CSoundByte> m_queued_sounds;

    //Internal data
    CALContext* m_al_context;
    CALListener m_al_listener;
    CElapsedTimerMicro* m_al_clock;

    std::mutex m_observer_access;
    std::atomic_bool m_quit_flag;
    std::atomic_bool m_running;

    std::vector<CALBuffer*> m_al_buffers;
    std::vector<CALSource*> m_al_sources;

    struct _amix_soundbyte
    {
        CSoundByte sbyte;
        CALBuffer* buffer;
        CALSource* source;
        szptr offset;
        szptr size;

        uint64 start_pts;
        uint64 end_pts;

        bool playing;
    };

    std::vector<_amix_soundbyte> m_internal_queue;
};

}
}
}

#endif
