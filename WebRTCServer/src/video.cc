#include "video.h"
#include <chrono>
#include <thread>
#include "UnityVideoTrackSource.h"

video::video(void* video_track)
:video_track(video_track)
{
}

video::~video()
{
}

void video::Start(){
    unity::webrtc::UnityVideoTrackSource* track = (unity::webrtc::UnityVideoTrackSource*)video_track;
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        track->CaptureVideoFrame();
    }
    
}