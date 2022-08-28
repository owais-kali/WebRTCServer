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

    struct RTCOfferAnswerOptions
    {
        bool iceRestart;
        bool voiceActivityDetection;
    };

class WebRTCPlugin {
 public:
  PeerConnectionObject* _ContextCreatePeerConnection(
      Context* context,
      const PeerConnectionInterface::RTCConfiguration& config);

  PeerConnectionObject* ContextCreatePeerConnection(Context* context);

  void Print(){std::cout << "hello " << std::endl;}
};
}  // namespace webrtc
