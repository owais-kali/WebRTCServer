#include "CapturerTrackSource.h"
#include <iostream>
namespace webrtc {
rtc::scoped_refptr<CapturerTrackSource> CapturerTrackSource::Create() {
    const size_t kWidth = 640;
    const size_t kHeight = 480;
    const size_t kFps = 30;
    std::unique_ptr<webrtc::test::VcmCapturer> capturer;
    std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
        webrtc::VideoCaptureFactory::CreateDeviceInfo());
    if (!info) {
      return nullptr;
    }
    int num_devices = info->NumberOfDevices();
    RTC_LOG(LS_INFO) << "nb video devices:" << num_devices;
    std::list<std::string> videoDeviceList;
    for (int i = 0; i < num_devices; ++i)
    {
        const uint32_t kSize = 256;
        char name[kSize] = {0};
        char id[kSize] = {0};
        if (info->GetDeviceName(i, name, kSize, id, kSize) != -1)
        {
            RTC_LOG(LS_INFO) << "video device name:" << name << " id:" << id;
            std::string devname;
            auto it = std::find(videoDeviceList.begin(), videoDeviceList.end(), name);
            if (it == videoDeviceList.end()) {
                devname = name;
            } else {
                devname = "videocap://";
                devname += std::to_string(i);
            }
            videoDeviceList.push_back(devname);
        }
    }
    /*for (int i = 0; i < num_devices; ++i) {
      capturer = absl::WrapUnique(
          webrtc::test::VcmCapturer::Create(kWidth, kHeight, kFps, i));
      if (capturer) {
        return rtc::make_ref_counted<CapturerTrackSource>(std::move(capturer));
      }
    }*/

    return nullptr;
  }

  rtc::VideoSourceInterface<webrtc::VideoFrame>* CapturerTrackSource::source() {
    return capturer_.get();
  }
}