#pragma once
#include "pch.h"
namespace webrtc {
using namespace webrtc;
class CapturerTrackSource : public webrtc::VideoTrackSource {
 public:
    static rtc::scoped_refptr<CapturerTrackSource> Create();

protected:
  explicit CapturerTrackSource(
      std::unique_ptr<webrtc::test::VcmCapturer> capturer)
      : VideoTrackSource(/*remote=*/false), capturer_(std::move(capturer)) {}

private:
  rtc::VideoSourceInterface<webrtc::VideoFrame>* source() override;
  std::unique_ptr<webrtc::test::VcmCapturer> capturer_;
};
}