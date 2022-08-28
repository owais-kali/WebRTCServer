#pragma once
#include <iostream>

#include "pch.h"
namespace webrtc {

namespace webrtc = ::webrtc;

class Context;
class PeerConnectionObject;
class PeerConnectionInterface;

enum class RTCSdpType;

enum class RTCSdpType { Offer, PrAnswer, Answer, Rollback };

struct RTCOfferAnswerOptions {
  bool iceRestart;
  bool voiceActivityDetection;
};

class WebRTCPlugin {
 public:
  PeerConnectionObject* _ContextCreatePeerConnection(
      Context* context,
      const PeerConnectionInterface::RTCConfiguration& config);

  PeerConnectionObject* ContextCreatePeerConnection(Context* context);
  void PeerConnectionCreateOffer(PeerConnectionObject* obj,
                                 const RTCOfferAnswerOptions* options);

std::string GetEnvVarOrDefault(const char* env_var_name,
                               const char* default_value) {
  std::string value;
  const char* env_var = getenv(env_var_name);
  if (env_var)
    value = env_var;

  if (value.empty())
    value = default_value;

  return value;
}
  std::string GetPeerConnectionString() {
  return GetEnvVarOrDefault("WEBRTC_CONNECT", "stun:stun.l.google.com:19302");
}
};
}  // namespace webrtc
