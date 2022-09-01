#pragma once
#include <iostream>

#include "pch.h"

namespace webrtc {

namespace webrtc = ::webrtc;

class Context;
class PeerConnectionObject;
class PeerConnectionInterface;

enum class RTCSdpType { Offer, PrAnswer, Answer, Rollback };

struct RTCSessionDescription {
  RTCSdpType type;
  const char* sdp;
};

// Callback Delegates
using DelegateCreateSDSuccess = void (*)(PeerConnectionObject*,
                                         RTCSdpType,
                                         const char*);

using DelegateCreateSDFailure = void (*)(PeerConnectionObject*,
                                         webrtc::RTCErrorType,
                                         const char*);
using DelegateLocalSdpReady = void (*)(PeerConnectionObject*,
                                       const char*,
                                       const char*);
using DelegateIceCandidate = void (*)(PeerConnectionObject*,
                                      const char*,
                                      const char*,
                                      const int);
using DelegateOnIceConnectionChange =
    void (*)(PeerConnectionObject*,
             webrtc::PeerConnectionInterface::IceConnectionState);
using DelegateOnIceGatheringChange =
    void (*)(PeerConnectionObject*,
             webrtc::PeerConnectionInterface::IceGatheringState);
using DelegateOnConnectionStateChange =
    void (*)(PeerConnectionObject*,
             webrtc::PeerConnectionInterface::PeerConnectionState);
using DelegateOnDataChannel = void (*)(PeerConnectionObject*,
                                       DataChannelInterface*);
using DelegateOnRenegotiationNeeded = void (*)(PeerConnectionObject*);
using DelegateOnTrack = void (*)(PeerConnectionObject*,
                                 webrtc::RtpTransceiverInterface*);
using DelegateOnRemoveTrack = void (*)(PeerConnectionObject*,
                                       webrtc::RtpReceiverInterface*);
////////////////////////////////////

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
  void AddTracks(Context* context);

  void PeerConnectionRegisterCallbackCreateSD(
      PeerConnectionObject* obj,
      DelegateCreateSDSuccess onSuccess,
      DelegateCreateSDFailure onFailure);

  void PeerConnectionRegisterOnIceCandidate(PeerConnectionObject* obj,
                                            DelegateIceCandidate callback);

  void PeerConnectionCreateOffer(PeerConnectionObject* obj,
                                 const RTCOfferAnswerOptions* options);
  void PeerConnectionCreateAnswer(PeerConnectionObject* obj,
                                  const RTCOfferAnswerOptions* options);

  RTCErrorType PeerConnectionSetLocalDescription(
      Context* context,
      PeerConnectionObject* obj,
      const RTCSessionDescription* desc,
      std::string& error);

  RTCErrorType PeerConnectionSetRemoteDescription(
      Context* context,
      PeerConnectionObject* obj,
      const RTCSessionDescription* desc,
      char* error[]);

  PeerConnectionInterface::SignalingState PeerConnectionSignalingState(
      PeerConnectionObject* obj);

  char* ConvertString(const std::string str);

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
