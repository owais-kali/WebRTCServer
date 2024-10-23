#include <api/video_codecs/video_codec.h>
#include <media/engine/internal_decoder_factory.h>
#include <modules/video_coding/include/video_error_codes.h>

#include "UnityVideoDecoderFactory.h"


namespace unity
{
	namespace webrtc
	{
		std::vector<webrtc::SdpVideoFormat> UnityVideoDecoderFactory::GetSupportedFormats() const
		{
			auto formats = std::vector<webrtc::SdpVideoFormat>();
			SdpVideoFormat format1("H264");
			format1.name = "H264";
			format1.parameters.emplace("implementation_name", "NvCodec");
			format1.parameters.emplace("level-asymmetry-allowed", "1");
			format1.parameters.emplace("packetization-mode", "1");
			format1.parameters.emplace("profile-level-id", "42e033");

			formats.push_back(format1);
			return formats;
		}

		std::unique_ptr<webrtc::VideoDecoder> UnityVideoDecoderFactory::CreateVideoDecoder(const webrtc::SdpVideoFormat& format)
		{
			return std::unique_ptr<webrtc::VideoDecoder>();
		}

		UnityVideoDecoderFactory::UnityVideoDecoderFactory(IGraphicsDevice* gfxDevice, ProfilerMarkerFactory* profiler)
		{
		}

		UnityVideoDecoderFactory::~UnityVideoDecoderFactory()
		{
		}
	}
}
