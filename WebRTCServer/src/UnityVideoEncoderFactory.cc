#include "UnityVideoEncoderFactory.h"

#include <absl/strings/match.h>
#include <api/video/video_codec_constants.h>
#include <api/video/video_codec_type.h>
#include <api/video_codecs/video_encoder.h>
#include <common_video/h264/h264_bitstream_parser.h>
#include <common_video/h264/h264_common.h>
#include <media/base/media_constants.h>
#include <media/engine/internal_encoder_factory.h>
#include <modules/video_coding/include/video_codec_interface.h>
#include <modules/video_coding/include/video_error_codes.h>
#include <modules/video_coding/utility/simulcast_utility.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

#include "Logger.h"

struct FileData {
  uint8_t* data;  // Pointer to file data
  size_t length;  // Length of the file data
};

namespace unity {
namespace webrtc {
EncodedImageCallback* m_encodedCompleteCallback;

class UnityVideoEncoder : public VideoEncoder {
 public:
  H264BitstreamParser m_h264BitstreamParser;

  int fileNumber = 0;
  FileData lastfiledata;

  // Function to read the next file in sequence (e.g., 0.h264, 1.h264, etc.)
  FileData readNextFile(const std::string& directory) {
    FileData fileData = {nullptr, 0};

    // Create the file name using directory and fileNumber
    std::string fileName =
        directory + "/" + std::to_string(fileNumber) + ".h264";

    // Open the file in binary mode
    std::ifstream file(fileName,
                       std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
      std::cerr << "Failed to open file: " << fileName << std::endl;
      return fileData;  // Return empty fileData if the file cannot be opened
    }

    // Get the size of the file
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Allocate memory to store the file data
    fileData.data = new uint8_t[fileSize];
    fileData.length = fileSize;

    // Read the file data into memory
    if (!file.read(reinterpret_cast<char*>(fileData.data), fileSize)) {
      std::cerr << "Error reading file: " << fileName << std::endl;
      delete[] fileData.data;
      fileData.data = nullptr;
      fileData.length = 0;
    }

    // Close the file
    file.close();

    // Increment the fileNumber for the next call
    ++fileNumber;

    return fileData;
  }

  UnityVideoEncoder() {}
  ~UnityVideoEncoder() override {}
  int32_t InitEncode(const VideoCodec* codec_settings,
                     int32_t number_of_cores,
                     size_t max_payload_size) override {
    JLogPrint(LoggingSeverity::LS_INFO, "Init Encode");
    return WEBRTC_VIDEO_CODEC_OK;
  }
  int InitEncode(const VideoCodec* codec_settings,
                 const VideoEncoder::Settings& settings) override {
    JLogPrint(LoggingSeverity::LS_INFO, "Init Encode");
    return WEBRTC_VIDEO_CODEC_OK;
  }

  int kVideoFrameKey_COUNT = 0;
  int32_t Encode(const VideoFrame& frame,
                 const std::vector<VideoFrameType>* frame_types) override {
    JLogPrint(LoggingSeverity::LS_INFO, "Encode");

    if (lastfiledata.data != nullptr) {
      delete lastfiledata.data;
    }

    lastfiledata = readNextFile("h264_frames");
    if (lastfiledata.length <= 0) {
      std::cout << "WEBRTC_VIDEO_CODEC_NO_OUTPUT: packet->av_packet->size <= 0"
                << std::endl;
      return WEBRTC_VIDEO_CODEC_NO_OUTPUT;
    }
    EncodedImage m_encodedImage;
    m_encodedImage._encodedWidth = 1920;
    m_encodedImage._encodedHeight = 1080;
    m_encodedImage.SetTimestamp(frame.timestamp());
    m_encodedImage.SetSimulcastIndex(0);
    m_encodedImage.ntp_time_ms_ = frame.ntp_time_ms();
    m_encodedImage.capture_time_ms_ = frame.render_time_ms();
    m_encodedImage.rotation_ = frame.rotation();
    m_encodedImage.content_type_ = VideoContentType::SCREENSHARE;
    m_encodedImage.timing_.flags = VideoSendTiming::kInvalid;
    m_encodedImage._frameType = VideoFrameType::kVideoFrameDelta;

    if (fileNumber == 1) {
      kVideoFrameKey_COUNT++;
      m_encodedImage._frameType = VideoFrameType::kVideoFrameKey;
      JLogPrint(LoggingSeverity::LS_INFO, "kVideoFrameKey %d",
                kVideoFrameKey_COUNT);
    }

    m_encodedImage.SetColorSpace(frame.color_space());

    m_encodedImage.SetEncodedData(EncodedImageBuffer::Create(
        (const uint8_t*)lastfiledata.data, lastfiledata.length));

    m_encodedImage.set_size(lastfiledata.length);

    // m_h264BitstreamParser.ParseBitstream(m_encodedImage);
    m_encodedImage.qp_ = 28;  // m_h264BitstreamParser.GetLastSliceQp().value_or(25);
    // JLogPrint(LoggingSeverity::LS_INFO,"m_encodedImage.qp_: %d",
    // m_encodedImage.qp_);
    CodecSpecificInfo codecInfo;
    codecInfo.codecType = kVideoCodecH264;
    codecInfo.codecSpecific.H264.packetization_mode =
        H264PacketizationMode::NonInterleaved;

    const auto result =
        m_encodedCompleteCallback->OnEncodedImage(m_encodedImage, &codecInfo);
    if (result.error != EncodedImageCallback::Result::OK) {
      // RTC_LOG(LS_ERROR) << "Encode m_encodedCompleteCallback failed " <<
      // result.error;
      JLogPrint(LoggingSeverity::LS_INFO,
                "Encode m_encodedCompleteCallback failed ");
      return WEBRTC_VIDEO_CODEC_ERROR;
    }
    return WEBRTC_VIDEO_CODEC_OK;
  }
  EncoderInfo GetEncoderInfo() const override {
    EncoderInfo info;
    info.scaling_settings.min_pixels_per_frame = 57600;
    info.requested_resolution_alignment = 1;
    info.apply_alignment_to_all_simulcast_layers = false;
    info.supports_native_handle = true;
    info.implementation_name = "NvCodec";
    info.has_trusted_rate_controller = false;
    info.is_hardware_accelerated = true;
    info.preferred_pixel_formats.push_back(VideoFrameBuffer::Type::kI420);

    return info;
  }

  void SetRates(const RateControlParameters& parameters) override {}

  void OnPacketLossRateUpdate(float packet_loss_rate) override {}
  void OnRttUpdate(int64_t rtt_ms) override {}
  void OnLossNotification(const LossNotification& loss_notification) override {}
  int32_t RegisterEncodeCompleteCallback(
      EncodedImageCallback* callback) override {
    m_encodedCompleteCallback = callback;
    return WEBRTC_VIDEO_CODEC_OK;
  }
  void SetFecControllerOverride(
      FecControllerOverride* fec_controller_override) override {}
  int32_t Release() override { return WEBRTC_VIDEO_CODEC_OK; }
};

std::vector<SdpVideoFormat> UnityVideoEncoderFactory::GetSupportedFormats()
    const {
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

webrtc::VideoEncoderFactory::CodecSupport
UnityVideoEncoderFactory::QueryCodecSupport(
    const SdpVideoFormat& format,
    absl::optional<std::string> scalability_mode) const {
  JLogPrint(LoggingSeverity::LS_INFO, "QueryCodecSupport");
  return CodecSupport();
}

std::unique_ptr<VideoEncoder> UnityVideoEncoderFactory::CreateVideoEncoder(
    const SdpVideoFormat& format) {
  JLogPrint(LoggingSeverity::LS_INFO, "CreateVideoEncoder");
  return std::make_unique<UnityVideoEncoder>();
}

UnityVideoEncoderFactory::UnityVideoEncoderFactory(
    IGraphicsDevice* gfxDevice)
    : factories_() {}

UnityVideoEncoderFactory::~UnityVideoEncoderFactory() {}
}  // namespace webrtc
}  // namespace unity