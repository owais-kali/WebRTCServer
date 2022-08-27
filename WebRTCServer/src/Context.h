#pragma once
#include "PeerConnectionObject.h"

namespace webrtc {

enum class RTCSdpType;

class Context {
  PeerConnectionObject* CreatePeerConnection(
      const webrtc::PeerConnectionInterface::RTCConfiguration& config);
};
extern RTCSdpType ConvertSdpType(webrtc::SdpType type);
}  // namespace webrtc
