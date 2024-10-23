#include <api/video/video_frame.h>
#include "VideoFrameAdapter.h"

namespace unity
{
	namespace webrtc
	{
		VideoFrameAdapter::ScaledBuffer::ScaledBuffer(rtc::scoped_refptr<VideoFrameAdapter> parent, int width, int height)
			: parent_(parent)
			, width_(width)
			, height_(height)
		{
		}

		VideoFrameAdapter::ScaledBuffer::~ScaledBuffer()
		{
		}

		VideoFrameBuffer::Type VideoFrameAdapter::ScaledBuffer::type() const
		{
			return parent_->type();
		}

		rtc::scoped_refptr<webrtc::I420BufferInterface> VideoFrameAdapter::ScaledBuffer::ToI420()
		{
			return parent_->GetOrCreateFrameBufferForSize(Size(width_, height_))->ToI420();
		}

		const webrtc::I420BufferInterface* VideoFrameAdapter::ScaledBuffer::GetI420() const
		{
			return parent_->GetOrCreateFrameBufferForSize(Size(width_, height_))->GetI420();
		}

		rtc::scoped_refptr<webrtc::VideoFrameBuffer> VideoFrameAdapter::ScaledBuffer::GetMappedFrameBuffer(rtc::ArrayView<webrtc::VideoFrameBuffer::Type> types)
		{
			return rtc::scoped_refptr<webrtc::VideoFrameBuffer>();
		}

		rtc::scoped_refptr<webrtc::VideoFrameBuffer> VideoFrameAdapter::ScaledBuffer::CropAndScale(int offset_x, int offset_y, int crop_width, int crop_height, int scaled_width, int scaled_height)
		{
			return rtc::scoped_refptr<webrtc::VideoFrameBuffer>();
		}

		VideoFrameAdapter::VideoFrameAdapter(rtc::scoped_refptr<VideoFrame> frame)
			: frame_(std::move(frame))
			, size_(frame_->size())
		{
		}

		::webrtc::VideoFrame VideoFrameAdapter::CreateVideoFrame(rtc::scoped_refptr<VideoFrame> frame)
		{
			rtc::scoped_refptr<VideoFrameAdapter> adapter(new rtc::RefCountedObject<VideoFrameAdapter>(std::move(frame)));

			return ::webrtc::VideoFrame::Builder().set_video_frame_buffer(adapter).build();
		}

		VideoFrameBuffer::Type VideoFrameAdapter::type() const
		{
			return VideoFrameBuffer::Type::kNative;
		}

		const I420BufferInterface* VideoFrameAdapter::GetI420() const
		{
			return nullptr;
		}

		rtc::scoped_refptr<I420BufferInterface> VideoFrameAdapter::ToI420()
		{
			return rtc::scoped_refptr<I420BufferInterface>();
		}

		rtc::scoped_refptr<webrtc::VideoFrameBuffer> VideoFrameAdapter::CropAndScale(int offset_x, int offset_y, int crop_width, int crop_height, int scaled_width, int scaled_height)
		{
			return rtc::scoped_refptr<webrtc::VideoFrameBuffer>();
		}

		rtc::scoped_refptr<webrtc::VideoFrameBuffer> VideoFrameAdapter::GetOrCreateFrameBufferForSize(const Size& size)
		{
			std::unique_lock<std::mutex> guard(scaleLock_);

			for (auto scaledI420buffer : scaledI40Buffers_)
			{
				Size bufferSize(scaledI420buffer->width(), scaledI420buffer->height());
				if (size == bufferSize)
				{
					return scaledI420buffer;
				}
			}
			auto buffer = VideoFrameBuffer::CropAndScale(0, 0, width(), height(), size.width(), size.height());
			scaledI40Buffers_.push_back(buffer);
			return buffer;
		}

		rtc::scoped_refptr<I420BufferInterface> VideoFrameAdapter::ConvertToVideoFrameBuffer(rtc::scoped_refptr<VideoFrame> video_frame) const
		{
			return rtc::scoped_refptr<I420BufferInterface>();
		}
	}
}