#ifndef COFFEE_FFMEDIA_FFPLAYER_H
#define COFFEE_FFMEDIA_FFPLAYER_H

#include "cffmedia.h"

namespace Coffee{
namespace FFMedia{

class CFFPlayer
{
public:
    CFFPlayer(Resource& vsource)
    {
        FFInit(nullptr,false);
        m_player = CreatePlayer(vsource);
        m_vclock = nullptr;
        CASSERT(m_player);
    }
    ~CFFPlayer()
    {
        FreeDecodeContext(m_dcontext);
        FreePlayer(m_player);
    }

    FFVideoPlayer* player()
    {
        return m_player;
    }
    CFFVideoDescriptor& descriptor()
    {
        return m_descriptor;
    }
    FFDecodeContext* decodeContext()
    {
        return m_dcontext;
    }
    CFFVideoTarget& videoTarget()
    {
        return m_target;
    }

    bool startDecoder()
    {
        return DecodeFrame(m_player,m_dcontext,&m_target);
    }
    void createDecoder()
    {
        m_dcontext = CreateDecodeContext(m_player,m_descriptor);
    }
    bool decodeFrame()
    {
        return DecodeFrame(m_player,m_dcontext,&m_target);
    }
    void awaitFrames()
    {
        m_target.v.updated = false;
        while(!m_target.v.updated&&DecodeFrame(m_player,m_dcontext,&m_target));
    }

    void startVideoTimer()
    {
        m_vclock = new CElapsedTimer;
        m_vclock->start();
        last_pts = 0;
        m_video_delay = 0;
        last_timestamp = 0;
    }
    void stopVideoTimer()
    {
        if(!m_vclock)
            return;
        delete m_vclock;
        m_vclock = nullptr;
    }
    bool isDisplayTime()
    {
        bool status = true;

        if(m_vclock)
	    status = m_vclock->elapsed()>=(uint64)last_timestamp;

        last_check_timer = status;
        return status;
    }
    bool checkTimer()
    {
        if(!m_vclock)
            startVideoTimer();

        bool status = false;
        if(m_target.v.pts)
        {
            m_video_delay = m_target.v.pts-last_pts;
            last_pts = m_target.v.pts;
            status = true;
        }
        last_timestamp = m_vclock->elapsed()+m_video_delay;

        return status;
    }
    bool checkTimerLast()
    {
        return last_check_timer;
    }
    bool finished()
    {
        return DecodeIsEOS(m_dcontext);
    }

private:
    //FFMPEG components
    CFFVideoTarget m_target;
    FFVideoPlayer* m_player;
    CFFVideoDescriptor m_descriptor;
    FFDecodeContext* m_dcontext;

    //Timing
    CElapsedTimer* m_vclock;
    int64 m_video_delay;
    int64 last_pts;
    int64 last_timestamp;
    bool last_check_timer;
};

}
}

#endif
