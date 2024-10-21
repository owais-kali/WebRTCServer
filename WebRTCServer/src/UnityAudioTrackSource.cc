#pragma clang diagnostic ignored "-Wunused-private-field"
#include "pch.h"

#include <common_audio/include/audio_util.h>
#include <rtc_base/ref_counted_object.h>

#include "UnityAudioTrackSource.h"

namespace unity
{
    namespace webrtc
    {

        rtc::scoped_refptr<UnityAudioTrackSource> UnityAudioTrackSource::Create()
        {
            rtc::scoped_refptr<UnityAudioTrackSource> source(new rtc::RefCountedObject<UnityAudioTrackSource>());
            return source;
        }

        rtc::scoped_refptr<UnityAudioTrackSource> UnityAudioTrackSource::Create(const cricket::AudioOptions& audio_options)
        {
            rtc::scoped_refptr<UnityAudioTrackSource> source(
                new rtc::RefCountedObject<UnityAudioTrackSource>(audio_options));
            return source;
        }

        void UnityAudioTrackSource::AddSink(AudioTrackSinkInterface* sink)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            _arrSink.push_back(sink);
        }

        void UnityAudioTrackSource::RemoveSink(AudioTrackSinkInterface* sink)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            auto i = std::find(_arrSink.begin(), _arrSink.end(), sink);
            if (i != _arrSink.end())
                _arrSink.erase(i);
        }
        
        //UINT timer = 0;
        void UnityAudioTrackSource::PushAudioData(
            std::vector<std::int16_t>& pAudioData, int nSampleRate, size_t nNumChannels, size_t nNumFrames)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            
            //timer += 480;
            for (auto sink : _arrSink)
                 sink->OnData(pAudioData.data(), 16, nSampleRate, 2, 480);
        }

        UnityAudioTrackSource::UnityAudioTrackSource() { }
        UnityAudioTrackSource::UnityAudioTrackSource(const cricket::AudioOptions& audio_options)
            : _options(audio_options)
        {
        }

        UnityAudioTrackSource::~UnityAudioTrackSource() { }

    } // end namespace webrtc
} // end namespace unity
