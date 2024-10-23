#include "UnityVideoTrackSource.h"
#include <system_wrappers/include/clock.h>
#include "VideoFrameAdapter.h"

namespace unity
{
	namespace webrtc
	{
		static std::unique_ptr<Clock> s_clock;
		rtc::scoped_refptr<UnityVideoTrackSource> UnityVideoTrackSource::Create(
			bool is_screencast, absl::optional<bool> needs_denoising, TaskQueueFactory* taskQueueFactory)
		{
			return rtc::make_ref_counted<UnityVideoTrackSource>(is_screencast, needs_denoising, taskQueueFactory);
		}

		UnityVideoTrackSource::UnityVideoTrackSource(bool is_screencast, absl::optional<bool> needs_denoising, TaskQueueFactory* taskQueueFactory)
			: AdaptedVideoTrackSource(/*required_alignment=*/1)
			, syncApplicationFramerate_(true)
		{
			if(s_clock.get() == nullptr) s_clock.reset(Clock::GetRealTimeClock());
		}

		UnityVideoTrackSource::~UnityVideoTrackSource()
		{
		}

		UnityVideoTrackSource::SourceState UnityVideoTrackSource::state() const
		{
			return SourceState();
		}

		bool UnityVideoTrackSource::remote() const
		{
			return false;
		}

		bool UnityVideoTrackSource::is_screencast() const
		{
			return false;
		}

		absl::optional<bool> UnityVideoTrackSource::needs_denoising() const
		{
			return absl::optional<bool>();
		}

		void UnityVideoTrackSource::OnFrameCaptured(rtc::scoped_refptr<VideoFrame> frame)
		{
		}

		void UnityVideoTrackSource::SetSyncApplicationFramerate(bool value)
		{
		}

		void UnityVideoTrackSource::OnUpdateVideoFrame()
		{
		}

		void UnityVideoTrackSource::CaptureVideoFrame()
		{
			Size size(1920,1080);
			frame_ = rtc::make_ref_counted<unity::webrtc::VideoFrame>(size, webrtc::TimeDelta::Micros(s_clock->CurrentTime().us()));
			const int orig_width = frame_->size().width();
			const int orig_height = frame_->size().height();
			const int64_t now_us = rtc::TimeMicros();

			FrameAdaptationParams frame_adaptation_params = ComputeAdaptationParams(orig_width, orig_height, now_us);
			if (frame_adaptation_params.should_drop_frame)
			{
				frame_ = nullptr;
				return;
			}

			const webrtc::TimeDelta timestamp = frame_->timestamp();
			rtc::scoped_refptr<VideoFrameAdapter> frame_adapter(
				new rtc::RefCountedObject<VideoFrameAdapter>(std::move(frame_)));

			::webrtc::VideoFrame::Builder builder = ::webrtc::VideoFrame::Builder()
				.set_video_frame_buffer(std::move(frame_adapter))
				.set_timestamp_us(timestamp.us());
			OnFrame(builder.build());
		}

		void UnityVideoTrackSource::GenerateKeyFrame()
		{
		}

		void UnityVideoTrackSource::SendFeedback()
		{
		}

		void UnityVideoTrackSource::RequestRefreshFrame()
		{
		}

		UnityVideoTrackSource::FrameAdaptationParams UnityVideoTrackSource::ComputeAdaptationParams(int width, int height, int64_t time_us)
		{
			return FrameAdaptationParams();
		}
	} // end namespace webrtc
} // end namespace unity
