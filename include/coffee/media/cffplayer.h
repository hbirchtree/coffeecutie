#ifndef COFFEE_FFMEDIA_FFPLAYER_H
#define COFFEE_FFMEDIA_FFPLAYER_H

#include "cffmedia.h"

namespace Coffee{
namespace CFFMedia{

class CFFPlayer
{
public:
    CFFPlayer(CResource& vsource)
    {
        coffee_ffmedia_init(nullptr,false);
        m_player = coffee_ffmedia_create_player(vsource);
        m_vclock = nullptr;
        CASSERT(m_player);
    }
    ~CFFPlayer()
    {
        coffee_ffmedia_free_decodecontext(m_dcontext);
        coffee_ffmedia_free_player(m_player);
    }

    CFFVideoPlayer* player()
    {
        return m_player;
    }
    CFFVideoDescriptor& descriptor()
    {
        return m_descriptor;
    }
    CFFDecodeContext* decodeContext()
    {
        return m_dcontext;
    }
    CFFVideoTarget& videoTarget()
    {
        return m_target;
    }

    bool startDecoder()
    {
        return coffee_ffmedia_decode_frame(m_player,m_dcontext,&m_target);
    }
    void createDecoder()
    {
        m_dcontext = coffee_ffmedia_create_decodecontext(m_player,m_descriptor);
    }
    bool decodeFrame()
    {
        return coffee_ffmedia_decode_frame(m_player,m_dcontext,&m_target);
    }
    void awaitFrames()
    {
        m_target.v.updated = false;
        while(!m_target.v.updated&&coffee_ffmedia_decode_frame(m_player,m_dcontext,&m_target));
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
        return coffee_ffmedia_decode_is_eos(m_dcontext);
    }

private:
    //FFMPEG components
    CFFVideoTarget m_target;
    CFFVideoPlayer* m_player;
    CFFVideoDescriptor m_descriptor;
    CFFDecodeContext* m_dcontext;

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
